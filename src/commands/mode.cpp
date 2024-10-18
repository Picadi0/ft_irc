#include "../../inc/IRC.hpp"
#include <exception>
#include <unistd.h>

void IRC::modecmd(string targetChannel, string mode, string param, Client &sender)
{
    if (!targetChannel.empty())
    {
        if (targetChannel[0] == '#')//target is channel
        {
            Channel *channel = findChannel(targetChannel);
            if (channel != NULL)
            {
                if (mode == "b" && param.empty())
                {
                    if (channel->getBanList().size() <= 0)
                        sendMsg(sender.getSockfd(), sender.getIDENTITY() + " 368 " + sender.getNickname() + " :End of channel ban list");
                    else
                    {
                        map<string, string>::iterator banned = channel->getBanList().begin();
                        while (banned != channel->getBanList().end())
                        {
                            sendMsg(sender.getSockfd(),sender.getIDENTITY() + " 367 " + sender.getNickname() + " :" + channel->getName() + " " + banned->second + "!*@ " + banned->first);
                            banned++;
                        }
                        sendMsg(sender.getSockfd(), sender.getIDENTITY() + " 368 " + sender.getNickname() + " :End of channel ban list");
                    }
                }
                if (channel->isOp(sender.getSockfd()) == true)
                {
                    if (mode == "+o" || mode == "-o")//Bir kullanıcıyı kanal operatörü yapar
                    {
                        if (channel->findClient(param) != NULL && channel->findClient(param)->getSockfd() != sender.getSockfd())
                        {
                            if (mode == "+o")
                                channel->setModfd(channel->findClient(param)->getSockfd());
                            else
                                channel->removeModFd(channel->findClient(param)->getSockfd());
                            sendMsg(sender.getSockfd(), "MODE " + channel->getName() + " " + mode + " " + channel->findClient(param)->getNickname());
                            sendMyOperationOthers(*channel, sender,"MODE " + channel->getName() + " " + mode + " " + channel->findClient(param)->getNickname());
                        }
                        else
                        {
                            sendMsg(sender.getSockfd(), "401 " + param + " :No such nick");
                            return;
                        }
                    }
                    else if (mode == "+t" || mode == "-t")//Sadece operatörlerin kanal konusunu değiştirebilmesini sağlar.
                    {
                        if (mode == "+t")
                            channel->onlyOpSetsTopic(true);
                        else
                            channel->onlyOpSetsTopic(false);
                        sendMsg(sender.getSockfd(), "MODE " + channel->getName() + " " + mode);
                    }
                    else if (mode == "+n" || mode == "-n")//Kanalda olmayan kullanıcıların mesaj gönderememesini sağlar.
                    {
                        channel->setOnlyMembersCanMsg(mode == "+n");
                        sendMsg(sender.getSockfd(), "MODE " + channel->getName() + " " + mode);
                    }
                    else if (mode == "+k" || mode == "-k")//Kanal için bir şifre belirler.
                    {
                        if(mode == "+k")
                        {
                            if (param.empty())//error empty password
                            {
                                sendMsg(sender.getSockfd(), "461 " + sender.getNickname() + " MODE +k :Not enough parameters");
                                return;
                            }
                            else
                            {
                                channel->setPass(param);
                                sendMsg(sender.getSockfd(), "MODE " + channel->getName() + " +k " + param);
                            }
                        }
                        else // Kanal şifresini kaldırma
                        {
                            channel->setRemovePass();
                            sendMsg(sender.getSockfd(), "MODE " + channel->getName() + " -k");
                        }
                    }
                    else if (mode == "+b" || mode == "-b")//Belirtilen kullanıcıyı banlar
                    {
                        if (!param.empty() )
                        {
                            if (mode == "+b")
                            {
                                Client *client = channel->findClient(param);
                                if (client && !channel->isBanned(client->getHostInfo()) && channel->findClient(param)->getSockfd() != sender.getSockfd())
                                {
                                    channel->addBanList(param);
                                    sendMsg(sender.getSockfd(), "MODE " + channel->getName() + " +b :" + param);
                                    sendMyOperationOthers(*channel, sender, "MODE " + channel->getName() + " +b :" + param);;
                                    KickUser(sender,channel->getName(), client->getNickname());
                                }
                                else
                                    sendMsg(sender.getSockfd(), "401 " + param + " :No such nick/already banned or don't try to ban yourself :c");
                            }
                            else
                            {
                                channel->removeBanList(param);
                                sendMsg(sender.getSockfd(), "MODE " + channel->getName() + " -b :" + param);
                                sendMyOperationOthers(*channel, sender, "MODE " + channel->getName() + " +b :" + param);
                            }

                        }
                    }
                    else if (mode == "+i" || mode == "-i")//Davetli kullanıcı istisnası (invite-only kanallarda).
                    {
                        if (param.empty())
                        {
                            sendMsg(sender.getSockfd(), "MODE " + channel->getName() + " " + mode + " ");
                            sendMyOperationOthers(*channel, sender, "MODE " + channel->getName() + " " + mode + " ");
                            channel->setOnlyInviteMode(mode == "+i");
                        }
                        else
                            sendMsg(sender.getSockfd(), "401 " + mode + " : malformed MODE -> " + param);
                    }
                    else if (mode == "+l")//Kanaldaki maksimum kullanıcı sayısını sınırlar.
                    {
                        if (param.empty())
                        {
                            channel->setMaxClientCount(0);
                            sendMsg(sender.getSockfd(), "MODE " + channel->getName() + " " + mode + " 0");
                            sendMyOperationOthers(*channel, sender, "MODE " + channel->getName() + " " + mode + " 0");
                        }
                        else
                        {
                            size_t setTo;
                            try
                            {
                                setTo = std::stoul(param);
                                if (setTo < 0)
                                {
                                    sendMsg(sender.getSockfd(), "401 " + param + " : malformed size for Channel -> " + param);
                                    return;
                                }
                                channel->setMaxClientCount(setTo);
                                sendMsg(sender.getSockfd(), "MODE " + channel->getName() + " " + mode + " " + param);
                                sendMyOperationOthers(*channel, sender, "MODE " + channel->getName() + " " + mode + " " + param);
                            }
                            catch (std::exception &e)
                            {
                                sendMsg(sender.getSockfd(), "401 " + param + " : malformed size for Channel -> " + param);
                            }
                        }
                    }
                    else {
                        sendMsg(sender.getSockfd(), "401 Error : Unknown " + mode);
                    }
                }
            }
            else if (channel == NULL)
            {
                sendMsg(sender.getSockfd(), "403 " + targetChannel + " :No such channel");
                return;
            }
            else
            {
                sendMsg(sender.getSockfd(), "482 " + targetChannel + " :You're not channel operator");
                return;
            }
        }
        else
        {
            if (mode == "+i" || mode == "-i")
            {
                if (sender.getNickname() == targetChannel && param.empty())
                {
                    sender.setInvisible(mode == "+i");
                }
                else
                   sendMsg(sender.getSockfd(), "401 Error : malformed invisible format MODE <nick> <+i/-i>");
            }
            else
            {
                sendMsg(sender.getSockfd(), "476 " + targetChannel + " :Invalid channel name");
                return;

            }
        }
    }
    else
    {
        sendMsg(sender.getSockfd(), "472 " + mode + " :is unknown mode char to me");
        return;
    }
}
