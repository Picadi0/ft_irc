#include "../../inc/IRC.hpp"
#include "../../inc/channel.hpp"

void IRC::privmsg(Client &sender,string target, string msg)
{
    Client *targetClient;
    if (target[0] != '#') {
        if ((targetClient = findClient(target)))
            sendMsg(targetClient->getSockfd(), sender.getIDENTITY()+"PRIVMSG "+target+" "+msg); }
    else
    {
        Channel *channel = findChannel(target);
        if (channel)
        {
             if (channel->getOnlyMembersCanMsg() == true) {
                int targetSockfd = channel->searchClientFdByNick(sender.getNickname());
                if (targetSockfd == -1) {
                    sendMsg(sender.getSockfd(), "NOTICE " + sender.getNickname() + " :You cannot send messages to this channel because you are not a member.");
                    return;
                }
            }
            sendMyOperationOthers(*channel, sender, sender.getIDENTITY()+"PRIVMSG "+target + " " + msg);
        }
        else
            sendMsg(sender.getSockfd(), "NOTICE "+ sender.getNickname()+" :No such channel");
    }
}
