#include "../../inc/IRC.hpp"

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
            sendMyOperationOthers(*channel, sender, sender.getIDENTITY()+"PRIVMSG "+target + " " + msg);
        else
            sendMsg(sender.getSockfd(), "NOTICE "+ sender.getNickname()+" :No such channel");
    }
}
