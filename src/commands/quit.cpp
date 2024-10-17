#include "../../inc/IRC.hpp"
#include <cstddef>

void IRC::cleanBufferOnLeave(Client &client)
{
    list<Channel>::iterator channel = this->channels.begin();
    while (channel != this->channels.end())
    {
        if (channel->findClient(client.getNickname()))
            channel->removeClient(client);
        if (channel->findInvitedClient(client.getNickname()))
            channel->removeInvitedClient(client);
        if (channel->findInvisibleClient(client.getNickname()))
            channel->removeInvisibleClient(client);
        channel++;
    }
}

void IRC::quit(Client &client)
{
    int saveSockfd;
    sendAllClientMsg(this->clients, client.getIDENTITY() + " QUIT Quit: ");
    cout << FG_RED << "{LOG}[" << sockfd << "] "
         << (client.getNickname().empty() ? "client" : client.getNickname())
         << " is disconnected from IRC Server" <<  RESET << endl
         << RESET;
    saveSockfd = client.getSockfd();
    close(client.getSockfd());
    FD_CLR(client.getSockfd(), &masterfd);
    cleanBufferOnLeave(client);
    this->clients.erase(client.getSockfd());
    checkChannelEmpty();
    transferOnOpLeave(saveSockfd);
}
