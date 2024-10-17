#pragma once

#include "client.hpp"
#include "libs.hpp"
#include <cstddef>
#include <list>

class Channel {
private:
  string name;
  string topic;
  string pass;
  list<Client> clients;
  list<Client> invitedClients;
  list<Client> invisibleClients;
  map<string, string> banList;
  list<int> modfd;
  bool TopicChangeByOp;
  bool OnlyMembersCanMsg;
  bool OnlyInviteMode;
  size_t  maxClient;
  Channel();

public:
  Channel(string name, string pass);
  ~Channel();
  void addInvisibleClient(Client &client);
  Client *findInvisibleClient(string nickName);
  void removeInvisibleClient(Client &client);
  void addClient(Client &client);
  void addBanList(string nickName);
  void removeBanList(string hostInfo);
  bool isBanned(string hostInfo);
  void removeClient(Client &client);
  void setModfd(int sockfd);
  void setOnlyInviteMode(bool flag)      { this->OnlyInviteMode =  flag; };
  int searchClientFdByNick(string nick);
  int searchClientFdByUser(string user);
  Client *findClient(string nickName);
  Client *findInvitedClient(string nickName);
  void    addInvitedClient(Client &client);
  void    removeInvitedClient(Client &client);
  // Get-Set
  size_t getMaxClientCount()            { return this->maxClient; }
  size_t getOnlyInviteMode()            { return this->OnlyInviteMode; }
  string getName()                      { return this->name; }
  string getPass()                      { return this->pass; }
  string getTopic()                     { return this->topic; }
  list<Client> &getClients()            { return this->clients; }
  map<string, string> &getBanList()     { return this->banList; }
  list<int> getModFd()                  { return this->modfd; }
  bool getTopicChangeByOp()             { return this->TopicChangeByOp; };
  void removeModFd(int socketFd)        { this->modfd.remove(socketFd); }
  bool getOnlyMembersCanMsg()           { return this->OnlyMembersCanMsg; };
  string getClientsNames();
  bool isOp(int socketFd);
  //---------------------
  void setMaxClientCount(int clientCount)    { this->maxClient = clientCount; }
  void setName(string name)                 { this->name = name; }
  void setPass(string pass)                 { this->pass = pass; }
  void setTopic(string topic)               { this->topic = topic; }
  void onlyOpSetsTopic(bool flag)           { this->TopicChangeByOp = flag; };
  void setOnlyMembersCanMsg(bool flag)      { this->OnlyMembersCanMsg = flag; };
  void setRemovePass()                      { this->pass.clear(); };

};

void sendMsg(int targetFd, string msg);
void sendAllClientMsg(map<int, Client> &clientList, string msg);
