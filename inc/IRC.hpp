#pragma once

#include "channel.hpp"
#include "client.hpp"
#include <cstddef>
#include <string>
#include <utility>

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
  map<int, string> socketBuff;

  IRC();

public:
  // methods
  void start();
  void newClientAdd();
  void handleClient(int sockfd);
  void CommandHandler(Client &client, string cmd);
  void addbuff(int socketfd, const string& str) { socketBuff[socketfd] += str;};
  string getbuff(int socketfd);
  void clearbuff(int socketfd);
  void JoinChannel(Client &client, string channelName, string channelPwd);
  void KickUser(Client &client, const std::string &channelName, const std::string &targetUser);
  void InviteUser(Client &client, const std::string &channelName, const std::string &targetNick);
  int findClientByNickname(const std::string &nickname);
  void privmsg(Client &client, string target, string msg);
  void part(Client &client, string channelName);
  void quit(Client &client);
  void cleanBufferOnLeave(Client &client);
  void checkChannelEmpty();
  void transferOnOpLeave(int sockfd);
  int searchClientByNick(string nick);
  int searchClientByUser(string user);
  void getUsersInChannel(Channel &channel, Client &Exclude);
  void sendMyJoinOthers(Channel &channel, Client &sender);
  void who(string channelName, bool isChannel, Client &sender);
  void sendMyOperationOthers(Channel &channel, Client &sender, string opmsg);
  void sendUsersInChannel(Channel &channel);
  void listChannelsss(Client client, string channelNames);
  void topic(Client &client, string channelName, string topic);
  Channel *findChannel(string channelName);
  Client *findClient(string nick);
  void modecmd(string target, string mode, string param, Client &sender);
  void notice(Client &sender, string target, string msg);
  // constructors
  IRC(int port, string password);
  IRC(const IRC &irc);
  ~IRC() { close(sockfd); }
};
