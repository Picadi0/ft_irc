#include "../../inc/IRC.hpp"
#include "../../inc/libs.hpp"

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

void IRC::KickUser(Client &client, const std::string &channelName, const std::string &targetNick)
{
    // Kanalı bul
    std::list<Channel>::iterator it = this->channels.begin();
    bool channelFound = false;

    while (it != this->channels.end())
    {
        if (it->getName() == channelName)
        {
            channelFound = true;
            break;
        }
        ++it;
    }

    // Eğer kanal bulunamazsa, hata mesajı gönder
    if (!channelFound)
    {
        sendMsg(client.getSockfd(), "403 " + channelName + " :No such channel");
        return;
    }

    // Kullanıcı moderatör mü? (Soket numarası üzerinden kontrol yapılıyor)
    if (std::find(it->getModFd().begin(), it->getModFd().end(), client.getSockfd()) == it->getModFd().end())
    {
        sendMsg(client.getSockfd(), "482 " + channelName + " :You're not channel operator");
        return;
    }

    // Hedef kullanıcı kanalda mı?
    int targetSockfd = it->searchClientFdByNick(targetNick);
    if (targetSockfd == -1)
    {
        sendMsg(client.getSockfd(), "441 " + targetNick + " " + channelName + " :They aren't on that channel");
        return;
    }

    // Kullanıcıyı kanaldan çıkar
    it->removeClient(client);

    // Kanaldaki tüm kullanıcılara KICK mesajı gönder
    std::string kickMessage = ":" + client.getNickname() + " KICK " + channelName + " " + targetNick;

        sendAllClientMsg(clients, kickMessage);

    // Hedef kullanıcıya özel mesaj gönder
    sendMsg(targetSockfd, kickMessage);
}
