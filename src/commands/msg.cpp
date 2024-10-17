#include "../../inc/IRC.hpp"

void sendMsg(int targetFd, string msg)
{
    //cout << FG_CYAN<< "{LOG}{Send}[" << targetFd << "]"<< FG_WHITE <<(msg + "\r\n").c_str();
    if (send(targetFd, (msg + "\r\n").c_str(), msg.length() + 2, 0) == -1)
        std::runtime_error("Send error");
}

void sendAllClientMsg(map<int, Client> &clientList, string msg) {
  std::map<int, Client>::iterator us = clientList.begin();
  while (us != clientList.end()) {
    send(us->second.getSockfd(), (msg + "\r\n").c_str(), msg.length() + 2, 0);
    us++;
  }
}
