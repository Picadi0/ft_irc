#pragma once

#include "libs.hpp"
#include <netinet/in.h>

class Client {
private:
  int sockfd;
  sockaddr_in clientAddr;
  string username;
  string nickname;
  string realname;
  string hostInfo;
  string password;
  bool isAuthed;
  bool isInvited;
  bool invisible;
  bool isOp;

public:
  Client();
  Client(int sockfd, sockaddr_in sockAddr);
  Client(const Client &client);
  Client &operator=(const Client &client);
  ~Client();

  string getIDENTITY()
  {
	return (":" + this->nickname + "!" + this->username + "@" + this->hostInfo + " ");
  }
  // get-set
  void setInvisible(bool flag) { this->invisible = flag; }
  void setUsername(string username) { this->username = username; }
  void setNickname(string nickname) { this->nickname = nickname; }
  void setRealname(string realname) { this->realname = realname; }
  void setHostInfo(string hostInfo) { this->hostInfo = hostInfo; }
  void setPassword(string password) { this->password = password; }
  void setIsAuthed(bool isAuthed) { this->isAuthed = isAuthed; }
  void setSockfd(int sock) { this->sockfd = sock; }
  void setClientAddr(sockaddr_in clientAddr) { this->clientAddr = clientAddr; }
  //-----------------------//
  bool isInvisible() { return this->invisible; }
  int getSockfd() { return this->sockfd; }
  sockaddr_in getClientAddr() { return this->clientAddr; }
  string getUsername() { return this->username; }
  string getNickname() { return this->nickname; }
  string getRealname() { return this->realname; }
  string getHostInfo() { return this->hostInfo; }
  string getPassword() { return this->password; }
  bool getIsAuthed() { return this->isAuthed; }
};
