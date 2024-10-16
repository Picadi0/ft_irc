#include "../inc/channel.hpp"
#include "../inc/IRC.hpp"
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
}

Channel::~Channel() {}
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

void Channel::addClient(Client &client)
{
    std::list<Client>::iterator it = this->clients.begin();
    while (it != this->clients.end())
    {
        cout << "mal" << endl;
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
