#include "../../inc/IRC.hpp"

void IRC::who(list<Channel> channels, string channelName)
{
    for (list<Channel>::iterator it = channels.begin(); it != channels.end(); it++)
    {
        if (it->getName() == channelName)
        {
            cout << "Channel: " << it->getName() << endl;
            cout << "Users: " << endl;
            sendAllClientMsg(this->clients, "352 : "+ it->getName() + " : " + it->getUsers()x);
            return;
        }
    }
    cout << "Channel not found" << endl;
}
