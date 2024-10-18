#include <exception>
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string>
using namespace std;
struct bot_info {
    string channel;
    string channelpwd;
    string serverPwd;
    string server_ip;
    int port;
};

string intToString(int number)
{
    stringstream ss;
    ss << number;
    return ss.str();
}

int connectToIRCServer(bot_info bot)
{
    cout << "Creating Socket" << endl;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        throw runtime_error("Socket error");
    cout << "Socket Created" << endl;
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(bot.port);
    inet_pton(AF_INET, bot.server_ip.c_str(), &serverAddr.sin_addr);
    if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        cout << "Error couldn't find the server" << bot.server_ip << endl;
        exit(2);
    }
    cout << "Connected !" << endl;
    return sockfd;
}

void sendToServer(int sockfd, const string &message)
{
    cout << "BOT: " << message << endl;
    send(sockfd, (message + "\r\n").c_str(), message.length() + 2, 0);
}

void joinChannel(int sockfd, bot_info bot)
{
    cout << "joining channel " << bot.channel << endl;
    srand(time(0));
    int randomNUM = rand() % 9999;
    string randomNick =  "BOT" + intToString(randomNUM);
    sendToServer(sockfd, "PASS " + bot.serverPwd);
    sleep(1);
    sendToServer(sockfd, "NICK " + randomNick );
    sleep(1);
    sendToServer(sockfd, "USER " + randomNick);
    sleep(1);
    sendToServer(sockfd, "JOIN " + bot.channel);
    cout << "joined channel " << bot.channel << endl;
}

void listenForMessages(int sockfd, bot_info bot)
{
    cout << "Listening messages " << endl;
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        if (recv(sockfd, buffer, sizeof(buffer) - 1, 0) <= 0)
        {
            cerr << "Lost connection" << endl;
            break;
        }
        string receivedMessage(buffer);
        cout << "Received: " << receivedMessage << endl;
        if (receivedMessage.find("sea") != string::npos)
            sendToServer(sockfd, "PRIVMSG " + bot.channel + " :\x03" "04" "as reis hosgeldin");
         if (receivedMessage.find("pls penis") != string::npos)
         {
            srand(time(0));
            int size = rand() % 31;
            string gex;
            if (size == 0)
                gex = "you don't have penis you have ()";
            if (size >= 1 && size <= 10)
                gex = "micro penis";
            if (size > 10)
                gex = "Champ penis";
            string strbuild(size, '=');
            string msg = "PRIVMSG " + (bot.channel) + " :\x03" "08" "8"   + strbuild + ":\x03" "06" "D" "\t"  ":\x03" "00" + gex + " " + (size == 0 ? "" : (intToString(size) + " cm"));
            sendToServer(sockfd, msg);
        }
    }
}

int main(int ac, char **av)
{
    if (ac < 5)
    {
        cout << "./bot <server_ip> <port> <serverPass> <#channel> <channelpwd>" << endl;
        return 2;
    }
    bot_info bot;
    try {
        bot.server_ip = av[1];
        bot.port = atoi(av[2]);
        bot.serverPwd = av[3];
        bot.channel = av[4][0] == '#' ? av[4] : ("#" + string(av[4]));
        bot.channelpwd = ac == 5 ? "" : string(av[5]);
    }
    catch (exception &e)
    {
        cout << "./bot <server_ip> <port> <serverPass> <#channel> <channelpwd>" << endl;
        return 2;
    }
    int sockfd = connectToIRCServer(bot);
    sleep(2);
    if (sockfd <= 0)
    {
        cout << "Socket Error" << endl;
        return 2;
    }
    joinChannel(sockfd, bot);
    listenForMessages(sockfd, bot);
    sendToServer(sockfd, "QUIT Quit: ");
    close(sockfd);
    return 0;
}

__attribute__((destructor))
void after_main() {
    std::cout << "main'den sonra çalışan fonksiyon\n";
}