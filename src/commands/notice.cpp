#include "../../inc/IRC.hpp"

void IRC::notice(Client &sender, string target, string msg)
{
    if (target[0] != '#') {
        if (searchClient(target)) {
            sendMsg(sender.getSockfd(), "NOTICE "+target + " " + msg); } }
    else
    {
        Channel *channel = searchChannel(target);
        if (channel)
        {
            sendMsg(sender.getSockfd(), "NOTICE " + target + " " + msg);
            sendMyOperationOthers(*channel, sender, "NOTICE "+target + " " + msg);
        }
        else
            sendMsg(sender.getSockfd(), "No such channel");
    }
}
