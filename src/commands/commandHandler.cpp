#include "../../inc/IRC.hpp"
#include <_types/_nl_item.h>
#include <algorithm>
#include <cstddef>
#include <exception>
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

void IRC::part(Client &sender, string channelName)
{
    sendMsg(sender.getSockfd(), sender.getIDENTITY() + " PART " + channelName);
    list<Channel>::iterator channel = this->channels.begin();
    while (channel != this->channels.end())
    {
        if (channel->getName() == channelName)
        {
            channel->removeClient(sender);
            sendMyOperationOthers(*channel, sender,sender.getIDENTITY() + " PART " + channelName);
            cout << FG_RED << "{LOG}[" << sockfd << "] "
                 << (sender.getNickname().empty() ? "client" : sender.getNickname())
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

void IRC::modecmd(string targetChannel, string mode, string param, Client &sender)
{
    if (!targetChannel.empty())
    {
        if (targetChannel[0] == '#')//target is channel
        {
            Channel *channel = findChannel(targetChannel);
            if (channel != NULL && channel->isOp(sender.getSockfd()) == true)
            {
                if (mode == "+o" || mode == "-o")//Bir kullanıcıyı kanal operatörü yapar
                {
                    if (channel->findClient(param) != NULL && channel->findClient(param)->getSockfd() != sender.getSockfd())
                    {
                        if (mode == "+o")
                            channel->setModfd(channel->findClient(param)->getSockfd());
                        else
                            channel->removeModFd(channel->findClient(param)->getSockfd());
                        sendMsg(sender.getSockfd(), "MODE " + channel->getName() + " " + mode + " " + channel->findClient(param)->getNickname());
                        sendMyOperationOthers(*channel, sender,"MODE " + channel->getName() + " " + mode + " " + channel->findClient(param)->getNickname());
                    }
                }
                else if (mode == "+t" || mode == "-t")//Sadece operatörlerin kanal konusunu değiştirebilmesini sağlar.
                {
                    if (mode == "+t")
                        channel->onlyOpSetsTopic(true);
                    else
                        channel->onlyOpSetsTopic(false);
                }
                else if (mode == "+n")//Kanalda olmayan kullanıcıların mesaj gönderememesini sağlar.
                {
                }
                else if (mode == "+k")//Kanal için bir şifre belirler.
                {
                    if (param.empty())//error empty password
                    {
                    }
                    else
                    {
                    }
                }
                else if (mode == "+b")//Belirtilen kullanıcıyı banlar
                {
                }
                else if (mode == "+i")//Davetli kullanıcı istisnası (invite-only kanallarda).
                {
                }
                else if (mode == "+e")//Ban istisnası (banned kullanıcıları engel dışı tutar).
                {
                }
                else if (mode == "+l")//Kanaldaki maksimum kullanıcı sayısını sınırlar.
                {
                }
                //else if (mode == "+m")//Kanalı moderated yapar (sadece ses hakkı olanlar mesaj gönderebilir).
                //else if (mode == "+v")//Bir kullanıcıya ses hakkı verir (moderated modda konuşabilir).
            }
            else
            {
                //error channel not found
            }
        }
        else
        {
            //error syntax channel should start's with #
        }
    }
    else
    {
        //error target empty
    }
}

void IRC::CommandHandler(Client &sender, string cmd)
{
    std::istringstream iss(cmd);
    string token, result;
    while (iss >> token)
    {
        if (sender.getPassword() != this->password)
        {
            if (token == "PASS")
            {
                iss >> result;
                if (result[0] == ':')
                    result.erase(0, 1);
                sender.setPassword(result);
                if (sender.getPassword() != this->password)
                    sendMsg(sender.getSockfd(), ERR_PASSWMISMATCH);
                //else
                    //sendMsg(client.getSockfd(), ":GRANTED : Password is correct enter your USER&NICK");
            }
            else
                sendMsg(sender.getSockfd(), ERR_UNKNOWNCOMMAND);
        }
        else
        {
            if (!sender.getIsAuthed())
            {
                if (token == "NICK")
                {
                    iss >> result;
                    if (!result.empty() && searchClientByNick(result) == -1)
                        sender.setNickname(result);
                    else
                        sendMsg(sender.getSockfd(), ": NICK is empty / already taken");
                }
                else if (token == "USER")
                {
                    iss >> result;
                    if (!result.empty() && searchClientByUser(result) == -1)
                        sender.setUsername(result);
                    else
                        sendMsg(sender.getSockfd(), ": USER is empty / already taken");
                }
                else
                {
                    sendMsg(sender.getSockfd(), token + " Command is not found. Please Enter your USER & NICK");
                }
                if (!sender.getUsername().empty() && !sender.getNickname().empty())
                {
                    sendMsg(sender.getSockfd(), RPL_WELCOME(sender.getNickname(),sender.getUsername(),sender.getHostInfo())));
                    sender.setIsAuthed(true);
                    break;
                }
            }
            else
            {
                if (token == "PRIVMSG")
                {
                    string target;
                    iss >> target;
                    privmsg(sender,target, iss.str().substr(iss.tellg()));
                    break;
                }
                else if (token == "JOIN" || token == "join")
                {
                    string channel, pwd;
                    iss >> channel >> pwd;
                    if (channel[0] == '#')
                        JoinChannel(sender, channel, pwd);
                    else
                        sendMsg(sender.getSockfd(), "Error: Channel name should be start with #");
                    break;
                }
                else if (token == "PART")
                {
                    string channel;
                    iss >> channel;
                    if (channel[0] == '#')
                        part(sender, channel);
                    else
                        sendMsg(sender.getSockfd(), "Error: Channel name should be start with #");
                    checkChannelEmpty();
                    transferOnOpLeave(sender.getSockfd());
                    break;
                }
                else if (token == "QUIT")//serverden ayrılıyor
                {
                    int tmpfd = sender.getSockfd();
                    quit(sender);
                    checkChannelEmpty();
                    transferOnOpLeave(tmpfd);
                    break;
                }
                else if (token == "PING")
                {
                    sendMsg(sender.getSockfd(), "PONG " + iss.str());
                    break;
                }
                else if (token == "WHO")
                {
                    string channel;
                    iss >> channel;
                    if (channel[0] == '#')
                        who(channel, true, sender);
                    else
                        who(channel, false, sender);
                    break;
                }
                else if (token == "TOPIC")
                {
                    string channel, title;
                    iss >> channel;
                    if (!iss.str().substr(iss.tellg()).empty())
                        title = iss.str().substr(iss.tellg());
                    topic(sender, channel, title);
                    break;
                }
                else if (token == "KICK")
                {
                    string channel, targetUser;
                    iss >> channel >> targetUser;
                    if (channel[0] == '#')
                        KickUser(sender, channel, targetUser);
                    else
                        sendMsg(sender.getSockfd(), "Error: Channel name should start with #");
                    break;
                }
                else if (token == "LIST")
                {
                    string target;
                    iss >> target;
                    listChannelsss(sender, target);
                    break;
                }
                else if (token == "INVITE")
                {
                    break;
                }
                else if (token == "MODE")//mod verir
                {
                    string target, mode, param;
                    iss >> target >> mode >> param;
                    modecmd(target, mode, param, sender);
                    break;
                }
                else if (token == "NOTICE")
                {
                    string target;
                    iss >> target;
                    notice(sender, target, iss.str().substr(iss.tellg()));
                    break;
                }
                else
                {
                    sendMsg(sender.getSockfd(), token + " Command is not found.");
                    break;
                }
            }
        }
    }
}
