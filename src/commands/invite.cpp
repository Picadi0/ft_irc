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
    // Kanalı bul
    std::list<Channel>::iterator it = this->channels.begin();
    bool channelFound = false;
   // int i = 0;

    while (it != this->channels.end())
    {
        /*cout << i++ << endl;
        cout << "getname" << it->getName() << endl;
        cout << "chaneelName" <<  channelName << endl;
        */
        if (it->getName() == channelName)
        {
            channelFound = true;
            break;
        }
        it++;
    }

    // Eğer kanal bulunamazsa, hata mesajı gönder
    if (!channelFound)
    {
        sendMsg(sender.getSockfd(), "403 " + channelName + " :No such channel");
        return;
    }

    // Kullanıcı moderatör mü? (Soket numarası üzerinden kontrol yapılıyor :)
    if (!it->isOp(sender.getSockfd()))
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

    Client &targetClient = this->clients[targetSockfd];
    // buraya koşul koyucazz
    // Hedef kullanıcı zaten kanalda mı?
    if (it->searchClientFdByNick(targetNick) && targetClient.getInvited())
    {
        sendMsg(sender.getSockfd(), "443 " + targetNick + " " + channelName + " :is already on channel / is already invited");
        return;
    }
     targetClient.setInvited(true); // Davet

    // Kullanıcıya davet gönder
    std::string inviteMessage = ":" + sender.getNickname() + " INVITE " + targetNick + " :" + channelName;
    sendMsg(targetSockfd, inviteMessage);

    // Daveti gönderen kullanıcıya onay mesajı
    sendMsg(sender.getSockfd(), "341 " + sender.getNickname() + " " + targetNick + " " + channelName);
}