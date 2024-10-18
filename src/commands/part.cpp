#include "../../inc/IRC.hpp"

void IRC::part(Client &sender, string channelName)
{
    sendMsg(sender.getSockfd(), sender.getIDENTITY() + " PART " + channelName);
    list<Channel>::iterator channel = this->channels.begin();
    while (channel != this->channels.end())
    {
        if (channel->getName() == channelName && channel->findClient(sender.getNickname()))
        {
            channel->removeClient(sender);
            sendMyOperationOthers(*channel, sender,sender.getIDENTITY() + " PART " + channelName);
            cout << FG_RED << "{LOG}[" << sockfd << "] "
                 << (sender.getNickname().empty() ? "client" : sender.getNickname())
                 << " is disconnected from " << channelName <<  RESET << endl
                 << RESET;
            return;
        }
        channel++;
    }
}
