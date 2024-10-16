#include "../../inc/IRC.hpp"

void IRC::topic(Client &client, string channel, string topic)
{
    if (channel.empty() && topic.empty())
    {
        sendMsg(client.getSockfd(), "461 TOPIC :Not enough parameters");
        return;
    }
    Channel *chan = searchChannel(channel);
    if (chan == NULL)
    {
        sendMsg(client.getSockfd(), "403 " + channel + " :No such channel");
        return;
    }
    if (topic.empty())
    {
        sendMsg(client.getSockfd(), "332 " + channel + " :" + chan->getTopic());
        return;
    }
    string msg = "TOPIC " + channel + " :" + topic;
    sendMsg(client.getSockfd(), msg);
    sendMyOperationOthers(*chan, client, msg);
    chan->setTopic(topic);
}
