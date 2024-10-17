#include "../../inc/IRC.hpp"

void IRC::who(string channelOrName, bool isChannel, Client &sender)
{
    bool found = false;
    if (isChannel)
    {
        list<Channel>::iterator channel = this->channels.begin();
        while (channel != this->channels.end())
        {
            if (channel->getName() == channelOrName)
            {
                found = true;
                break;
            }
            channel++;
        }
        if (found)
        {
            list<Client>::iterator client = channel->getClients().begin();
            while(client != channel->getClients().end())
            {
                sendMsg(sender.getSockfd(), ":server 352 " + sender.getNickname() + " " + channel->getName() + " " + client->getUsername() + " " + client->getHostInfo() + " " + "42Istanbul" + " " + client->getNickname() + " H :0 " + client->getRealname());
                client++;
            }
            sendMsg(sender.getSockfd(), ":server 315 " + sender.getNickname() + " " + channel->getName() + " :End of /WHO list");
        }
    }
    else
    {
        list<Channel>::iterator channel = this->channels.begin();
        list<Client>::iterator client;
        while (channel != this->channels.end())
        {
            found = false;
            client = channel->getClients().begin();
            if (client != channel->getClients().end())
            {
                if (client->getNickname() == channelOrName)
                {
                    sendMsg(sender.getSockfd(), ":server 352 " + sender.getNickname() + " " + channel->getName() + " " + client->getUsername() + " " + client->getHostInfo() + " " + "42Istanbul" + " " + client->getNickname() + " H :0 " + client->getRealname());
                    found = true;
                }
                client++;
            }
            if (found)
                sendMsg(sender.getSockfd(), ":server 315 " + sender.getNickname() + " " + channel->getName() + " :End of /WHO list");
            channel++;
        }
    }
}
