#include "../../inc/IRC.hpp"
#include <string>

void IRC::listChannelsss(Client client ,string channelNames)
{
    list<Channel>::iterator channel = this->channels.begin();
    sendMsg(client.getSockfd(), client.getIDENTITY() + " 321 " + client.getNickname() + " Channel :Users  :Topic");
    if (channelNames.empty())
        while (channel != this->channels.end())
        {
            sendMsg(client.getSockfd(), client.getIDENTITY() + " 322 " + client.getNickname() + " : " + channel->getName() + " " + std::to_string(channel->getClients().size()) + " :" + channel->getTopic());
            channel++;
        }
    else
    {
        if (channel->getName() == channelNames)
            sendMsg(client.getSockfd(), client.getIDENTITY() + " 322 " + client.getNickname() + " : " + channel->getName() + " " + std::to_string(channel->getClients().size()) + " :" + channel->getTopic());
        else
            sendMsg(client.getSockfd(), client.getIDENTITY() + " 403 " + client.getNickname() + ": No such channel");
    }
    sendMsg(client.getSockfd(), client.getIDENTITY() + " 323 " + client.getNickname() + " :End of LIST");
}
