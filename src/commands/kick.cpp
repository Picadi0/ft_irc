#include "../../inc/IRC.hpp"
#include "../../inc/libs.hpp"
#include <cstddef>

int Channel::searchClientFdByNick(string nick) {
    std::list<Client>::iterator it = clients.begin();
    while (it != clients.end()) {
        if (it->getNickname() == nick) {
            return it->getSockfd(); // Kullanıcının soket numarasını döndür
        }
        ++it;
    }
    return -1; // Kullanıcı bulunamadı
}
int Channel::searchClientFdByUser(string user) {
    std::list<Client>::iterator it = clients.begin();
    while (it != clients.end()) {
        if (it->getUsername() == user) {
            return it->getSockfd(); // Kullanıcının soket numarasını döndür
        }
        ++it;
    }
    return -1; // Kullanıcı bulunamadı
}

void IRC::KickUser(Client &sender, const std::string &channelName, const std::string &targetNick)
{
    // Kanalı bul
    std::list<Channel>::iterator channel = this->channels.begin();
    bool channelFound = false;
    while (channel != this->channels.end())
    {
        if (channel->getName() == channelName)
        {
            channelFound = true;
            break;
        }
        ++channel;
    }
    // Eğer kanal bulunamazsa, hata mesajı gönder
    if (!channelFound)
    {
        sendMsg(sender.getSockfd(), "403 " + channelName + " :No such channel");
        return;
    }
    // Kullanıcı moderatör mü? (Soket numarası üzerinden kontrol yapılıyor)
    if (!channel->isOp(sender.getSockfd()))
    {
        sendMsg(sender.getSockfd(), "482 " + channelName + " :You're not channel operator");
        return;
    }
    // Hedef kullanıcı kanalda mı?
    int targetSockfd = channel->searchClientFdByNick(targetNick);
    Client *client = channel->findClient(targetNick);
    if (targetSockfd == -1)
    {
        sendMsg(sender.getSockfd(), "441 " + targetNick + " " + channelName + " :They aren't on that channel");
        return;
    }
    // Kanaldaki tüm kullanıcılara KICK mesajı gönder
    std::string kickMessage = ":" + sender.getNickname() + " KICK " + channelName + " " + targetNick;
    sendMsg(sender.getSockfd(), kickMessage);
    sendMyOperationOthers(*channel, sender, kickMessage);
    // Kullanıcıyı kanaldan çıkar
    channel->removeClient(*client);
}
