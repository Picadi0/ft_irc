#include "../inc/channel.hpp"
#include "../inc/IRC.hpp"
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <string>
Channel::Channel() {
  cout << FG_YELLOW << "   Channel Info\n"
       << "――――――――――――――――――――――׀\n"
       << " Name  ׀ " << this->getName() << "\n"
       << " Topic ׀ " << this->getTopic() << endl;
}

Channel::Channel(string name, string pass)
{
    this->name = name;
    this->pass = pass;
    this->maxClient = 0;
    this->OnlyInviteMode = false;
}

Channel::~Channel() {}

bool Channel::isOp(int socketFd)
{
    list<int>::iterator it = this->modfd.begin();
    while (it != this->modfd.end())
    {
        if (*it == socketFd)
            return true;
        it++;
    }
    return false;
}
string Channel::getClientsNames()
{
    string ret;
    list<Client>::iterator it = this->clients.begin();
    while(it != this->clients.end())
    {
        if (!it->getNickname().empty())
            ret += it->getNickname() + " ";
    }
    return ret;
}
Client *Channel::findClient(string nickName)
{
    list<Client>::iterator it = this->clients.begin();
    while (it != this->clients.end())
    {
        if (it->getNickname() == nickName)
            return &(*it);
        it++;
    }
    return NULL;
}
Client *Channel::findInvitedClient(string nickName)
{
    list<Client>::iterator it = this->invitedClients.begin();
    while (it != this->invitedClients.end())
    {
        if (it->getNickname() == nickName)
            return &(*it);
        it++;
    }
    return NULL;
}
void Channel::addInvitedClient(Client &client)
{
    std::list<Client>::iterator it = this->invitedClients.begin();
    while (it != this->invitedClients.end())
    {
        if (it->getSockfd() == client.getSockfd())
        {
            sendMsg(client.getSockfd(), "INFO : already invited");
            return;
        }
        it++;
    }
    this->invitedClients.push_back(client);
}
void Channel::removeInvitedClient(Client &client)
{
    std::list<Client>::iterator it = this->invitedClients.begin();
    while (it != this->invitedClients.end())
    {
        if (it->getSockfd() == client.getSockfd())
        {
            this->invitedClients.erase(it);
            return;
        }
        it++;
    }
}

void Channel::addClient(Client &client)
{
    std::list<Client>::iterator it = this->clients.begin();
    while (it != this->clients.end())
    {
        if (it->getSockfd() == client.getSockfd())
        {
            sendMsg(client.getSockfd(), "INFO : You are already in this channel");
            return;
        }
        it++;
    }
    this->clients.push_back(client);
}
void Channel::removeClient(Client &client)
{
    std::list<Client>::iterator it = this->clients.begin();
    while (it != this->clients.end())
    {
        if (it->getSockfd() == client.getSockfd())
        {
            this->clients.erase(it);
            return;
        }
        it++;
    }
}

void Channel::setModfd(int sockfd)
{
    this->modfd.push_back(sockfd);
}

bool Channel::isBanned(string hostInfo)
{
    map<string ,string>::iterator itBanlist = this->banList.begin();
    while (itBanlist != this->banList.end())
    {
        if (itBanlist->first == hostInfo)
            return true;
        itBanlist++;
    }
    return false;
}

void Channel::addBanList(string nickName)
{
    Client *client = findClient(nickName);
    if (client)
        this->banList.insert(pair<string, string>(client->getHostInfo(), nickName));
}

void Channel::removeBanList(string hostInfo)
{
    map<string ,string>::iterator itBanlist = this->banList.begin();
    while (itBanlist != this->banList.end())
    {
        if (itBanlist->first == hostInfo)
        {
            this->banList.erase(itBanlist->first);
            break;
        }
        itBanlist++;
    }
}
