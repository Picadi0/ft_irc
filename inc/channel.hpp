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

  // Get-Set
  string getName() { return this->name; }
  string getPass() { return this->pass; }
  string getTopic() { return this->topic; }
  string getClientsNames();
  list<Client> &getClients() { return this->clients; }
  list<int> getModFd() { return this->modfd; }
  void removeModFd(int socketFd) { this->modfd.remove(socketFd); }

  //---------------------
  void setName(string name) { this->name = name; }
  void setPass(string pass) { this->pass = pass; }
  void setTopic(string topic) { this->topic = topic; }
};

void sendMsg(int targetFd, string msg);
void sendAllClientMsg(map<int, Client> &clientList, string msg);
