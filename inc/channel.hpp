#pragma once

#include "client.hpp"
#include "libs.hpp"
#include <list>

class Channel {
private:
  string name;
  string topic;
  string pass;
  list<Client> clients;
  list<int> modfd;
  bool TopicChangeByOp;
  bool OnlyMembersCanMsg;
  bool noExternalMsg; // Kanal dışı mesajları engellemek için
  Channel();

public:
  Channel(string name, string pass);
  ~Channel();

  int addModfd(int sockfd);
  void addClient(Client &client);
  void removeClient(Client &client);
  void setModfd(int sockfd);
  int searchClientFdByNick(string nick);
  int searchClientFdByUser(string user);
  Client *findClient(string nickName);
  // Get-Set
  string getName()               { return this->name; }
  string getPass()               { return this->pass; }
  string getTopic()              { return this->topic; }
  list<Client> &getClients()     { return this->clients; }
  list<int> getModFd()           { return this->modfd; }
  bool getTopicChangeByOp()      { return this->TopicChangeByOp; };
  void removeModFd(int socketFd) { this->modfd.remove(socketFd); }
  bool getOnlyMembersCanMsg()    { return this->OnlyMembersCanMsg; };
  string getClientsNames();
  bool isOp(int socketFd);

  //---------------------
  void setName(string name)         { this->name = name; }
  void setPass(string pass)         { this->pass = pass; }
  void setTopic(string topic)       { this->topic = topic; }
  void onlyOpSetsTopic(bool flag)   { this->TopicChangeByOp = flag; };
  void setOnlyMembersCanMsg(bool flag)   { this->OnlyMembersCanMsg = flag; };
  void setRemovePass() { this->pass.clear(); };

};

void sendMsg(int targetFd, string msg);
void sendAllClientMsg(map<int, Client> &clientList, string msg);
