#pragma once

#include "channel.hpp"
#include "client.hpp"

#define BACKLOG 10       // how many pending connections queue will hold
#define MAXCLIENTS 10    // maximum number of clients
#define BUFFER_SIZE 4096 // buffer size

class IRC {
private:
  int port;
  int sockfd;
  int maxfd;
  int newClientfd;
  int itsMine;
  int maxClients;
  fd_set masterfd;
  fd_set readfd;
  string password;
  sockaddr_in serverAddr;
  sockaddr_in clientAddr;
  map<int, Client> clients;
  list<Channel> channels;

  IRC();

public:
  // methods
  void start();
  void newClientAdd();
  void handleClient(int sockfd);
  void CommandHandler(Client &client, string cmd);
  void JoinChannel(Client &client, string channelName, string channelPwd);
  void KickUser(Client &client, const std::string &channelName, const std::string &targetUser);
  void privmsg(string targetfd, string msg, int sender);
  void part(Client &client, string channelName);
  void quit(Client &client);
  void checkChannelEmpty();
  // constructors
  IRC(int port, string password);
  IRC(const IRC &irc);
  ~IRC() { close(sockfd); }
};
