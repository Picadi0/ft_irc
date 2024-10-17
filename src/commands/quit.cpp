#include "../../inc/IRC.hpp"

void IRC::quit(Client &client)
{
    int saveSockfd;
    sendAllClientMsg(this->clients, client.getIDENTITY() + " QUIT Quit: ");
    cout << FG_RED << "{LOG}[" << sockfd << "] "
         << (client.getNickname().empty() ? "client" : client.getNickname())
         << " is disconnected from IRC Server" <<  RESET << endl
         << RESET;
    close(client.getSockfd());
    FD_CLR(client.getSockfd(), &masterfd);
    saveSockfd = client.getSockfd();
    this->clients.erase(client.getSockfd());
    checkChannelEmpty();
    transferOnOpLeave(saveSockfd);
}
