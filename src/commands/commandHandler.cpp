#include "../../inc/IRC.hpp"
#include <_types/_nl_item.h>
#include <string>
#include <sys/socket.h>

void IRC::CommandHandler(Client &sender, string cmd)
{
    std::istringstream iss(cmd);
    string token, result;
    while (iss >> token)
    {
        if (sender.getPassword() != this->password)
        {
            if (token == "PASS")
            {
                iss >> result;
                if (result[0] == ':')
                    result.erase(0, 1);
                sender.setPassword(result);
                if (sender.getPassword() != this->password)
                    sendMsg(sender.getSockfd(), ERR_PASSWMISMATCH);
                //else
                    //sendMsg(client.getSockfd(), ":GRANTED : Password is correct enter your USER&NICK");
            }
            else
                sendMsg(sender.getSockfd(), ERR_UNKNOWNCOMMAND);
        }
        else
        {
            if (!sender.getIsAuthed())
            {
                if (token == "NICK")
                {
                    iss >> result;
                    if (!result.empty() && searchClientByNick(result) == -1)
                        sender.setNickname(result);
                    else
                        sendMsg(sender.getSockfd(), ": NICK is empty / already taken");
                }
                else if (token == "USER")
                {
                    iss >> result;
                    if (!result.empty() && searchClientByUser(result) == -1)
                        sender.setUsername(result);
                    else
                        sendMsg(sender.getSockfd(), ": USER is empty / already taken");
                }
                else
                {
                    sendMsg(sender.getSockfd(), token + " Command is not found. Please Enter your USER & NICK");
                }
                if (!sender.getUsername().empty() && !sender.getNickname().empty())
                {
                    sendMsg(sender.getSockfd(), RPL_WELCOME(sender.getNickname(),sender.getUsername(),sender.getHostInfo())));
                    sender.setIsAuthed(true);
                    break;
                }
            }
            else
            {
                if (token == "PRIVMSG")
                {
                    string target;
                    iss >> target;
                    privmsg(sender,target, iss.str().substr(iss.tellg()));
                    break;
                }
                else if (token == "JOIN" || token == "join")
                {
                    string channel, pwd;
                    iss >> channel >> pwd;
                    if (channel[0] == '#')
                        JoinChannel(sender, channel, pwd);
                    else
                        sendMsg(sender.getSockfd(), "Error: Channel name should be start with #");
                    break;
                }
                else if (token == "PART")
                {
                    string channel;
                    iss >> channel;
                    if (channel[0] == '#')
                        part(sender, channel);
                    else
                        sendMsg(sender.getSockfd(), "Error: Channel name should be start with #");
                    checkChannelEmpty();
                    transferOnOpLeave(sender.getSockfd());
                    break;
                }
                else if (token == "QUIT")
                {
                    int tmpfd = sender.getSockfd();
                    quit(sender);
                    checkChannelEmpty();
                    transferOnOpLeave(tmpfd);
                    break;
                }
                else if (token == "PING")
                {
                    sendMsg(sender.getSockfd(), "PONG " + iss.str());
                    break;
                }
                else if (token == "WHO")
                {
                    string channel;
                    iss >> channel;
                    if (channel[0] == '#')
                        who(channel, true, sender);
                    else
                        who(channel, false, sender);
                    break;
                }
                else if (token == "TOPIC")
                {
                    string channel, title;
                    iss >> channel;
                    if (!iss.str().substr(iss.tellg()).empty())
                        title = iss.str().substr(iss.tellg());
                    topic(sender, channel, title);
                    break;
                }
                else if (token == "KICK")
                {
                    string channel, targetUser;
                    iss >> channel >> targetUser;
                    if (channel[0] == '#')
                        KickUser(sender, channel, targetUser);
                    else
                        sendMsg(sender.getSockfd(), "Error: Channel name should start with #");
                    break;
                }
                else if (token == "LIST")
                {
                    string target;
                    iss >> target;
                    listChannelsss(sender, target);
                    break;
                }
                else if (token == "INVITE")
                {
                    std::string targetNick, channelName;
                    iss >> targetNick >> channelName;

                    if (targetNick.empty() || channelName.empty())
                        sendMsg(sender.getSockfd(), "461 INVITE :Not enough parameters");
                    else
                        InviteUser(sender, channelName, targetNick);
                    break;
                }
                else if (token == "MODE")//mod verir
                {
                    string target, mode, param;
                    iss >> target >> mode >> param;
                    modecmd(target, mode, param, sender);
                    break;
                }
                else if (token == "NOTICE")
                {
                    string target;
                    iss >> target;
                    notice(sender, target, iss.str().substr(iss.tellg()));
                    break;
                }
                else
                {
                    sendMsg(sender.getSockfd(), token + " Command is not found.");
                    break;
                }
            }
        }
    }
}
