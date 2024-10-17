#include "../../inc/IRC.hpp"

void IRC::JoinChannel(Client &sender, string channelName, string channelPwd)
{
    Channel *channel = findChannel(channelName);
    bool join = channel != NULL;
    string joinopmsg = sender.getIDENTITY() + " JOIN " + channelName;
    if (join)
    {
        if (!channel->findClient(sender.getNickname()))
        {
            if (channel->getPass() == channelPwd || channel->findInvitedClient(sender.getNickname()))
            {
                cout << sender.getNickname() << " joining channel" << endl;
                channel->addClient(sender);
                sendMsg(sender.getSockfd(), sender.getIDENTITY() + " JOIN " + channel->getName());
                sendMsg(sender.getSockfd(), RPL_TOPIC(sender.getNickname(), channelName, channel->getTopic()));
                sendMyJoinOthers(*channel, sender);
                getUsersInChannel(*channel, sender);
                if (channel->findInvitedClient(sender.getNickname()))
                    channel->removeInvitedClient(sender);
                //sendAllClientMsg(clients, "331 : " + client.getNickname() + channelName + ":No topic is set");
            }
            else
            {
                cout << "475 : Failed to join the " + channelName + " bad password." << endl;
                sendMsg(sender.getSockfd(), "475 : Failed to join the " + channelName + " bad password.");
            }
        }
        else
        {
            sendMsg(sender.getSockfd(), "INFO : You are already in this channel");
            return;
        }
    }
    else
    {
        cout << sender.getNickname() << " creating channel" << endl;
        Channel create(channelName,channelPwd);
        create.setName(channelName);
        create.setPass(channelPwd);
        create.setModfd(sender.getSockfd());
        create.addClient(sender);
        create.setTopic("42 Default Topic");
        create.setOnlyMembersCanMsg(false);
        this->channels.push_back(create);
        sendMsg(sender.getSockfd(), joinopmsg);
        sendMsg(sender.getSockfd(), RPL_TOPIC(sender.getNickname(), channelName, create.getTopic()));
        sendMsg(sender.getSockfd(), "MODE " + channelName + " +o " + sender.getNickname());
        sendMsg(sender.getSockfd(), "MODE " + channelName + " +k " + channelPwd); // sifreyi yazdırmak ???
        //sendAllClientMsg(clients, "331 : " + client.getNickname() + channelName + ":No topic is set");
    }
}