#include "../../inc/IRC.hpp"

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
                    if (!result.empty() && searchClientByNick(result) == -1 &&
                        result.find('@') == string::npos && result.find('!')  == string::npos && result.find(':') == string::npos)
                        sender.setNickname(result);
                    else
                        sendMsg(sender.getSockfd(), ": malformed nick");
                }
                else if (token == "USER")
                {
                    iss >> result;
                    if (!result.empty() && searchClientByUser(result) == -1 &&
                        result.find('@') == string::npos && result.find('!')  == string::npos && result.find(':') == string::npos)
                        sender.setUsername(result);
                    else
                        sendMsg(sender.getSockfd(), ": malformed user");
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
                    string target, msg;
                    iss >> target;
                    if (iss.peek() != EOF)
                        std::getline(iss >> std::ws, msg);
                    privmsg(sender,target, msg);
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
                    quit(sender);
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
                    std::string channel, title;
                    iss >> channel;
                    // Eğer hala veriler kaldıysa, başlığı (title) al
                    if (iss.peek() != EOF)  // peek() ile veri olup olmadığını kontrol ediyoruz
                        std::getline(iss >> std::ws, title);  // Tüm kalan satırı başlık olarak alıyoruz
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
                    string target, msg;
                    iss >> target;
                    if (iss.peek() != EOF)
                        std::getline(iss >> std::ws, msg);
                    notice(sender, target, msg);
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
