#include "../inc/client.hpp"

Client::Client(int sockfd, sockaddr_in clientAddr) {
  this->sockfd = sockfd;
  this->clientAddr = clientAddr;
  this->hostInfo = string(inet_ntoa(this->clientAddr.sin_addr));
  cout << "Client connected" << endl;
  cout << "IP: " << inet_ntoa(clientAddr.sin_addr) << endl;
  cout << "Port: " << ntohs(clientAddr.sin_port) << endl;
  cout << "Socket: " << sockfd << endl;
  cout << "-------------------" << endl;
}

Client::Client(const Client &client) {
  if (this != &client)
  {
      this->sockfd = client.sockfd;
      this->nickname = client.nickname;
      this->username = client.username;
      this->password = client.password;
      this->isAuthed = client.isAuthed;
      this->hostInfo = client.hostInfo;
      this->realname = client.realname;
      this->clientAddr = client.clientAddr;
      this->isOp = client.isOp;
  }
}

Client &Client::operator=(const Client &client) {
  if (this != &client)
  {
    this->sockfd = client.sockfd;
    this->nickname = client.nickname;
    this->username = client.username;
    this->password = client.password;
    this->isAuthed = client.isAuthed;
    this->hostInfo = client.hostInfo;
    this->realname = client.realname;
    this->clientAddr = client.clientAddr;
    this->isOp = client.isOp;
  }
  return *this;
}

Client::~Client() {}
