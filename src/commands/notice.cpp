#include "../../inc/IRC.hpp"

void IRC::notice(Client &sender, string target, string msg)
{
    Client *targetClient;
    cout << msg << endl;
    if (target[0] != '#') {
        if ((targetClient = findClient(target)))
            sendMsg(targetClient->getSockfd(), "NOTICE "+target + " :" + msg); }
    else
    {
        Channel *channel = findChannel(target);
        if (channel)
        {
            sendMsg(sender.getSockfd(), "NOTICE " + target + " :" + msg);
            sendMyOperationOthers(*channel, sender, "NOTICE "+target + " :" + msg);
        }
        else
            sendMsg(sender.getSockfd(), "No such channel");
    }
}
