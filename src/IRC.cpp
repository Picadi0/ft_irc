#include "../inc/IRC.hpp"
#include "../inc/client.hpp"
#include <arpa/inet.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <ostream>
#include <sstream>
#include <string>
#include <sys/select.h>

IRC::IRC(int port, string password)
{
  this->port = port;
  this->password = password;
  this->maxClients = MAXCLIENTS;
  itsMine = 1;

  if ((this->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    throw runtime_error("Socket creation error");
  if (setsockopt(this->sockfd, SOL_SOCKET, SO_REUSEADDR, &this->itsMine,
                 sizeof(int)) < 0)
    throw runtime_error("Socket option [setsockopt] error");
  if (fcntl(this->sockfd, F_SETFL, O_NONBLOCK) == -1)
    throw runtime_error("Socket option [fcntl] error");
  cout << FG_GREEN << "Socket created..." << RESET << endl;

  this->serverAddr.sin_family = AF_INET;
  this->serverAddr.sin_addr.s_addr = INADDR_ANY;
  this->serverAddr.sin_port = htons(this->port);
  memset(&(this->serverAddr.sin_zero), '\0', 8);
  if (bind(this->sockfd, (struct sockaddr *)&this->serverAddr,
           sizeof(this->serverAddr)) < 0)
    throw runtime_error("Bind error");
  cout << FG_GREEN << "Binded to port " << this->port << RESET << endl;
  if (listen(this->sockfd, BACKLOG) < 0)
    throw runtime_error("Listen error");
  FD_SET(this->sockfd, &this->masterfd);
  this->maxfd = this->sockfd;
  cout << FG_GREEN << "Listening on port " << this->port << RESET << "\n"
       << endl;
};

IRC::IRC(const IRC &irc)
{
  if (this != &irc)
  {
    this->port = irc.port;
    this->password = irc.password;
  }
};

void IRC::start()
{
  while (true)
  {
    this->readfd = this->masterfd;
    int selectStatus = select(this->maxfd + 1, &this->readfd, NULL, NULL, NULL);
    switch (selectStatus)
    {
    case (0):
    {
      cout << FG_YELLOW << "Select Timeout..." << RESET << endl;
      break;
    }
    case (-1):
      throw runtime_error("Select error");
    default:
    {
      for (int i = 0; i <= this->maxfd; i++)
      {
        if (FD_ISSET(i, &this->readfd))
        {
          if (i == this->sockfd)
            newClientAdd();
          else
            handleClient(i);
        }
      }
      break;
    }
    }
  }
}

void IRC::newClientAdd()
{
  int clientLen = sizeof(this->clientAddr);
  if ((this->newClientfd = accept(this->sockfd, (struct sockaddr *)&this->clientAddr, (socklen_t *)&clientLen)) == -1)
    throw runtime_error("Accept error");
  else
  {
    FD_SET(this->newClientfd, &this->masterfd);
    if (this->newClientfd > this->maxfd)
      this->maxfd = this->newClientfd;
    Client newClient(this->newClientfd, this->clientAddr);
    this->clients.insert(pair<int, Client>(this->newClientfd, newClient));
  }
}

static void handleClientQuit(int sockfd, fd_set &masterfd, map<int, Client> &clients)
{
  Client &client = clients.find(sockfd)->second;
  cout << FG_RED << "{LOG}[" << sockfd << "] "
       << (client.getNickname().empty() ? "client" : client.getNickname())
       << " is Disconnected" <<  RESET << endl
       << RESET;
  close(sockfd);
  FD_CLR(sockfd, &masterfd);
  clients.erase(sockfd);
}

void IRC::checkChannelEmpty()
{
    list<Channel>::iterator it = this->channels.begin();
    while (it != this->channels.end())
    {
        if (it->getClients().size() == 0)
        {
            cout << FG_RED << "{LOG}"
            "Destroyed the channel " << it->getName() << RESET << endl;
            this->channels.erase(it);
            break;
        }
        it++;
    }
}
void IRC::transferOnOpLeave(int sockfd)
{
    list<Channel>::iterator channel = this->channels.begin();
    list<Client>::iterator client;
    list<Client>::iterator trueClient;
    while (channel != this->channels.end())
    {
        if (find(channel->getModFd().begin(), channel->getModFd().end(), sockfd) != channel->getModFd().end())
        {
            channel->removeModFd(sockfd);
            if (channel->getModFd().size() == 0)
            {
                int lowestSockFd = 2147483647;
                client = channel->getClients().begin();
                while (client != channel->getClients().end())
                {
                    if (client->getSockfd() < lowestSockFd)
                    {
                        trueClient = client;
                        lowestSockFd = client->getSockfd();
                    }
                    client++;
                }
                if (lowestSockFd != 2147483647)
                {
                    channel->setModfd(lowestSockFd);
                    sendMyOperationOthers(*channel, *trueClient, "MODE " + channel->getName() + " +o " + trueClient->getNickname());
                    sendMsg(trueClient->getSockfd(), "MODE " + channel->getName() + " +o " + trueClient->getNickname());
                }
            }
        }
        channel++;
    }
}

void IRC::handleClient(int sockfd)
{
  Client &client = this->clients.find(sockfd)->second;
  char buff[BUFFER_SIZE] = {0};
  int nbytes;
  if ((nbytes = recv(client.getSockfd(), buff, sizeof(buff) - 1, 0)) <= 0)
  {
    if (nbytes != 0)
      cout << FG_RED + client.getUsername() + " its Quit" + RESET;
    handleClientQuit(sockfd, this->masterfd, this->clients);
    checkChannelEmpty();
    transferOnOpLeave(sockfd);
  }
  else
  {
    buff[nbytes] = '\0';
    CommandHandler(client, buff);
    cout << FG_CYAN << "{LOG}[" << sockfd << "] "
         << (client.getNickname().empty() ? "Client" : client.getNickname())
         << " : " << FG_WHITE << buff << endl
         << RESET;
  }
}

int IRC::searchClientByNick(string nick)
{
    map<int, Client>::iterator it = this->clients.begin();
    while (it != clients.end()) {
        if (it->second.getNickname() == nick)
        {
            return it->second.getSockfd();
        }
        ++it;
    }
    return -1;
}

int IRC::searchClientByUser(string user)
{
    map<int, Client>::iterator it = this->clients.begin();
    while (it != clients.end()) {
        if (it->second.getUsername() == user)
        {
            return it->second.getSockfd();
        }
        ++it;
    }
    return -1;
}

Channel *IRC::searchChannel(string name)
{
    list<Channel>::iterator it = this->channels.begin();
    while (it != this->channels.end())
    {
        if (it->getName() == name)
            return &(*it);
        it++;
    }
    return NULL;
}
