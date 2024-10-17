#include "../../inc/IRC.hpp"
#include "../../inc/libs.hpp"

int IRC::findClientByNickname(const std::string &nickname)
{
    std::map<int, Client>::iterator it = this->clients.begin(); // Sunucudaki kullanıcılar listesi (map)
    while (it != this->clients.end())
    {
        if (it->second.getNickname() == nickname) // it->second ile Client nesnesine erişiyoruz
        {
            return it->first; // Kullanıcının soket numarasını döndür (map'teki key, sockfd)
        }
        ++it;
    }
    return -1; // Kullanıcı bulunamadı
}


void IRC::InviteUser(Client &sender, const std::string &channelName, const std::string &targetNick)
{
    Channel *channel = findChannel(channelName);
    bool channelFound = channel != NULL;

    // Eğer kanal bulunamazsa, hata mesajı gönder
    if (!channelFound)
    {
        sendMsg(sender.getSockfd(), "403 " + channelName + " :No such channel");
        return;
    }

    // Kullanıcı moderatör mü? (Soket numarası üzerinden kontrol yapılıyor :)
    if (!channel->isOp(sender.getSockfd()))
    {
        sendMsg(sender.getSockfd(), "482 " + channelName + " :You're not channel operator");
        return;
    }

    // Hedef kullanıcıyı sunucudaki kullanıcılar arasından bulalım .da

    int targetSockfd = findClientByNickname(targetNick);
    if (targetSockfd == -1)
    {
        sendMsg(sender.getSockfd(), "401 " + targetNick + " :No such nick/channel");
        return;
    }

    // Hedef kullanıcı zaten kanalda mı?
    if (channel->findClient(targetNick) || channel->findInvitedClient(targetNick))
    {
        sendMsg(sender.getSockfd(), "443 " + targetNick + " " + channelName + " :is already on channel / is already invited");
        return;
    }
    channel->addInvitedClient(*findClient(targetNick));

    // Kullanıcıya davet gönder
    std::string inviteMessage = ":" + sender.getNickname() + " INVITE " + targetNick + " :" + channelName;
    sendMsg(targetSockfd, inviteMessage);

    // Daveti gönderen kullanıcıya onay mesajı
    sendMsg(sender.getSockfd(), "341 " + sender.getNickname() + " " + targetNick + " " + channelName);
}
