#include "../../inc/IRC.hpp"
#include <_types/_nl_item.h>
#include <algorithm>
#include <string>
#include <sys/socket.h>

void IRC::sendMyOperationOthers(Channel &channel, Client &sender, string opmsg)
{
    list<Client>::iterator client = channel.getClients().begin();
    while(client != channel.getClients().end())
    {
        if (client->getSockfd() != sender.getSockfd())
            sendMsg(client->getSockfd(), opmsg);
        client++;
    }
}

void IRC::sendMyJoinOthers(Channel &channel, Client &sender)
{
    list<Client>::iterator client = channel.getClients().begin();
    while(client != channel.getClients().end())
    {
        if (client->getSockfd() != sender.getSockfd())
        {
            sendMsg(client->getSockfd(), sender.getIDENTITY() + " JOIN " + channel.getName());
            sendMsg(client->getSockfd(), RPL_TOPIC(sender.getNickname(), channel.getName(), "42"));
        }
        client++;
    }
}

void IRC::getUsersInChannel(Channel &channel, Client &sender)
{
    list<Client>::iterator client = channel.getClients().begin();
    while(client != channel.getClients().end())
    {
        if (client->getSockfd() != sender.getSockfd())
        {
            sendMsg(sender.getSockfd(), client->getIDENTITY() + " JOIN " + channel.getName());
            sendMsg(sender.getSockfd(), RPL_TOPIC(client->getNickname(), channel.getName(), "42"));
            if (find(channel.getModFd().begin(), channel.getModFd().end(), client->getSockfd()) != channel.getModFd().end())
                sendMsg(sender.getSockfd(), "MODE " + channel.getName() + " +o " + client->getNickname());

        }
        client++;
    }
}

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

void IRC::JoinChannel(Client &sender, string channelName, string channelPwd)
{
    bool join = false;
    list<Channel>::iterator channel = this->channels.begin();
    string joinopmsg = sender.getIDENTITY() + " JOIN " + channelName;
    while (channel != this->channels.end())
    {
        if (channel->getName() == channelName)
        {
            join = true;
            break;
        }
        channel++;
    }
    if (join)
    {
        if (channel->getPass() == channelPwd)
        {
            cout << sender.getNickname() << " joining channel" << endl;
            channel->addClient(sender);
            sendMsg(sender.getSockfd(), sender.getIDENTITY() + " JOIN " + channel->getName());
            sendMsg(sender.getSockfd(), RPL_TOPIC(sender.getNickname(), channelName, channel->getTopic()));
            sendMyJoinOthers(*channel, sender);
            getUsersInChannel(*channel, sender);
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
        cout << sender.getNickname() << " creating channel" << endl;
        Channel create(channelName,channelPwd);
        create.setName(channelName);
        create.setModfd(sender.getSockfd());
        create.addClient(sender);
        create.setTopic("42 Default Topic");
        this->channels.push_back(create);
        sendMsg(sender.getSockfd(), joinopmsg);
        sendMsg(sender.getSockfd(), RPL_TOPIC(sender.getNickname(), channelName, create.getTopic()));
        sendMsg(sender.getSockfd(), "MODE " + channelName + " +o " + sender.getNickname());
        //sendAllClientMsg(clients, "331 : " + client.getNickname() + channelName + ":No topic is set");
    }
}

void IRC::part(Client &client, string channelName)
{
    sendAllClientMsg(this->clients, client.getIDENTITY() + " PART " + channelName);
    list<Channel>::iterator channel = this->channels.begin();
    while (channel != this->channels.end())
    {
        if (channel->getName() == channelName)
        {
            channel->removeClient(client);
            cout << FG_RED << "{LOG}[" << sockfd << "] "
                 << (client.getNickname().empty() ? "client" : client.getNickname())
                 << " is disconnected from " << channelName <<  RESET << endl
                 << RESET;
            return;
        }
        channel++;
    }
}

void IRC::quit(Client &client)
{
    int saveSockfd;
    sendAllClientMsg(this->clients, client.getIDENTITY() + " QUIT Quit: ");
    cout << FG_RED << "{LOG}[" << sockfd << "] "
         << (client.getNickname().empty() ? "client" : client.getNickname())
         << " is disconnected from IRC Server" <<  RESET << endl
         << RESET;
    close(client.getSockfd());
    FD_CLR(client.getSockfd(), &masterfd);
    saveSockfd = client.getSockfd();
    this->clients.erase(client.getSockfd());
    checkChannelEmpty();
    transferOnOpLeave(saveSockfd);
}

void IRC::privmsg(string target, string _msg, int sender)
{
    string msg = _msg.substr(1);
    if (target[0] == '#')
    {
        string channelName = target;
        list<Channel>::iterator itChannel = this->channels.begin();
        list<Client>::iterator itClients;
        while (itChannel != this->channels.end())
        {
            if (channelName == itChannel->getName())
            {
                itClients = itChannel->getClients().begin();
                while (itClients != itChannel->getClients().end())
                {
                    if (itClients->getSockfd() != sender)
                        sendMsg(itClients->getSockfd(), this->clients.find(sender)->second.getIDENTITY() + "PRIVMSG" + " " + target + " :" + msg);
                    itClients++;
                }
            }
            itChannel++;
        }
    }
}

void IRC::CommandHandler(Client &client, string cmd)
{
    std::istringstream iss(cmd);
    string token, result;
    while (iss >> token)
    {
        if (client.getPassword() != this->password)
        {
            if (token == "PASS")
            {
                iss >> result;
                if (result[0] == ':')
                    result.erase(0, 1);
                client.setPassword(result);
                if (client.getPassword() != this->password)
                    sendMsg(client.getSockfd(), ERR_PASSWMISMATCH);
                //else
                    //sendMsg(client.getSockfd(), ":GRANTED : Password is correct enter your USER&NICK");
            }
            else
                sendMsg(client.getSockfd(), ERR_UNKNOWNCOMMAND);
        }
        else
        {
            if (!client.getIsAuthed())
            {
                if (token == "NICK")
                {
                    iss >> result;
                    if (!result.empty() && searchClientByNick(result) == -1)
                        client.setNickname(result);
                    else
                        sendMsg(client.getSockfd(), ": NICK is empty / already taken");
                }
                else if (token == "USER")
                {
                    iss >> result;
                    if (!result.empty() && searchClientByUser(result) == -1)
                        client.setUsername(result);
                    else
                        sendMsg(client.getSockfd(), ": USER is empty / already taken");
                }
                else
                {
                    sendMsg(client.getSockfd(), token + " Command is not found. Please Enter your USER & NICK");
                }
                if (!client.getUsername().empty() && !client.getNickname().empty())
                {
                    sendMsg(client.getSockfd(), RPL_WELCOME(client.getNickname(),client.getUsername(),client.getHostInfo())));
                    client.setIsAuthed(true);
                    break;
                }
            }
            else
            {
                if (token == "PRIVMSG")
                {
                    string target;
                    iss >> target;
                    privmsg(target, iss.str().substr(iss.tellg()), client.getSockfd());
                    break;
                }
                else if (token == "JOIN" || token == "join")
                {
                    string channel, pwd;
                    iss >> channel >> pwd;
                    if (channel[0] == '#')
                        JoinChannel(client, channel, pwd);
                    else
                        sendMsg(client.getSockfd(), "Error: Channel name should be start with #");
                    break;
                }
                else if (token == "PART")
                {
                    string channel;
                    iss >> channel;
                    if (channel[0] == '#')
                        part(client, channel);
                    else
                        sendMsg(client.getSockfd(), "Error: Channel name should be start with #");
                    checkChannelEmpty();
                    transferOnOpLeave(client.getSockfd());
                    break;
                }
                else if (token == "QUIT")//serverden ayrılıyor
                {
                    quit(client);
                    checkChannelEmpty();
                    transferOnOpLeave(client.getSockfd());
                    break;
                }
                else if (token == "PING")
                {
                    sendMsg(client.getSockfd(), "PONG " + iss.str());
                    break;
                }
                else if (token == "WHO")
                {
                    string channel;
                    iss >> channel;
                    if (channel[0] == '#')
                        who(channel, true, client);
                    else
                        who(channel, false, client);
                    break;
                }
                else if (token == "TOPIC")
                {
                    string channel, title;
                    iss >> channel;
                    if (!iss.str().substr(iss.tellg()).empty())
                        title = iss.str().substr(iss.tellg());
                    topic(client, channel, title);
                    break;
                }
                else if (token == "KICK")
                {
                    string channel, targetUser;
                    iss >> channel >> targetUser;
                    if (channel[0] == '#')
                    {
                        KickUser(client, channel, targetUser);
                    }
                    else
                        sendMsg(client.getSockfd(), "Error: Channel name should start with #");
                    break;
                }
                else if (token == "LIST")
                {
                    string target;
                    iss >> target;
                    listChannelsss(client, target);
                    break;
                }
                else if (token == "INVITE")
                {
                    break;
                }
                else if (token == "MODE")//mod verir
                {
                    break;
                }
                else if (token == "NOTICE")
                {
                    break;
                }
                else
                {
                    sendMsg(client.getSockfd(), token + " Command is not found.");
                    break;
                }
            }
        }
    }
}
