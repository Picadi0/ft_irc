#include "../../inc/IRC.hpp"
#include <cstddef>

void IRC::topic(Client &sender, string channel, string topic)
{
    if (channel.empty() ||  topic.empty())
    {
        sendMsg(sender.getSockfd(), "461 TOPIC :Not enough parameters");
        return;
    }
    Channel *chan = findChannel(channel);
    if (chan == NULL)
    {
        sendMsg(sender.getSockfd(), "403 " + channel + " :No such channel");
        return;
    }
    if (chan->getTopicChangeByOp() && !chan->isOp(sender.getSockfd()))
    {
        sendMsg(sender.getSockfd(), "Error :Only operators can change topic");
        return;
    }
    if (chan->findClient(sender.getNickname()) != NULL)
    {
        sendMsg(sender.getSockfd(), "332 " + channel + " :" + (topic.empty() ? " 42 Default Topic" : topic));
    }
    sendMyOperationOthers(*chan, sender, RPL_TOPIC(sender.getNickname(), channel, (topic.empty() ? " 42 Default Topic" : topic)));
    chan->setTopic((topic.empty() ? " 42 Default Topic" : topic));
}
