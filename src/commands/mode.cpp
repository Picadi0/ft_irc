#include "../../inc/IRC.hpp"

void IRC::modecmd(string targetChannel, string mode, string param, Client &sender)
{
    if (!targetChannel.empty())
    {
        if (targetChannel[0] == '#')//target is channel
        {
            Channel *channel = findChannel(targetChannel);
            if (channel != NULL && channel->isOp(sender.getSockfd()) == true)
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
                    if(mode == "+n")
                        channel->setOnlyMembersCanMsg(true);
                    else
                        channel->setOnlyMembersCanMsg(false);
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
                else if (mode == "+b")//Belirtilen kullanıcıyı banlar
                {
                }
                else if (mode == "+i")//Davetli kullanıcı istisnası (invite-only kanallarda).
                {
                }
                else if (mode == "+e")//Ban istisnası (banned kullanıcıları engel dışı tutar).
                {
                }
                else if (mode == "+l")//Kanaldaki maksimum kullanıcı sayısını sınırlar.
                {
                }
                else if (mode.empty())
                {
                    //hangi modsa ona göre bilgi çekicek
                }
                else {
                    // invalid channel operator
                }
                //else if (mode == "+m")//Kanalı moderated yapar (sadece ses hakkı olanlar mesaj gönderebilir).
                //else if (mode == "+v")//Bir kullanıcıya ses hakkı verir (moderated modda konuşabilir).
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
            sendMsg(sender.getSockfd(), "476 " + targetChannel + " :Invalid channel name");
            return;
        }
    }
    else
    {
        sendMsg(sender.getSockfd(), "472 " + mode + " :is unknown mode char to me");
        return;
    }
}
