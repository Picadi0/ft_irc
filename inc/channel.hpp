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
  int removeClient(int sockfd);
  void setModfd(int sockfd);
  int searchClient(string nick);

  // Get-Set
  string getName() { return this->name; }
  string getPass() { return this->pass; }
  string getTopic() { return this->topic; }
  list<Client> getClients() { return this->clients; }
  //---------------------
  void setName(string name) { this->name = name; }
  void setPass(string pass) { this->pass = pass; }
  void setTopic(string topic) { this->topic = topic; }
};

void sendMsg(int targetFd, string msg);
void sendAllClientMsg(map<int, Client> &clientList, string msg);
