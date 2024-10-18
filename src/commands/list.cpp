#include "../../inc/IRC.hpp"
#include <string>

void IRC::listChannelsss(Client sender ,string channelNames)
{
    list<Channel>::iterator channel = this->channels.begin();
    sendMsg(sender.getSockfd(), sender.getIDENTITY() + " 321 " + sender.getNickname() + " Channel :Users  :Topic");
    if (channelNames.empty())
        while (channel != this->channels.end())
        {
            sendMsg(sender.getSockfd(), ":" + sender.getHostInfo() +" 322 " + sender.getNickname() + " " + channel->getName() + " " + (channel->getMaxClientCount() == 0 ? "99" : std::to_string(channel->getMaxClientCount())) + "/" + std::to_string(channel->getClients().size()) + " :" + channel->getTopic());
            channel++;
        }
    else
    {
        if (channel->getName() == channelNames)
            sendMsg(sender.getSockfd(), ":" + sender.getHostInfo() +" 322 " + sender.getNickname() + " " + channel->getName() + " " + (channel->getMaxClientCount() == 0 ? "99" : std::to_string(channel->getMaxClientCount())) + "/" + std::to_string(channel->getClients().size()) + " :" + channel->getTopic());
        else
            sendMsg(sender.getSockfd(), sender.getIDENTITY() + " 403 " + sender.getNickname() + ": No such channel");
    }
    sendMsg(sender.getSockfd(), sender.getIDENTITY() + " 323 " + sender.getNickname() + " :End of LIST");
}
