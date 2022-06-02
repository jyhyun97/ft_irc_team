#ifndef DEFINE_HPP
#define DEFINE_HPP

// REPLY
#define RPL_WELCOME "001"
#define RPL_NAMREPLY "353"
#define RPL_ENDOFNAMES "366"

// ERROR
#define ERR_NOSUCHNICK "401" //"<nickname> :No such nick/channel"
#define ERR_NOSUCHCHANNEL "403"
#define ERR_UNKNOWNCOMMAND "421"
#define ERR_ERRONEUSNICKNAME "432" 
#define ERR_NICKNAMEINUSE "433"
#define ERR_USERNOTINCHANNEL	"441" //"<nick> <channel> :They aren't on that channel"
#define ERR_NOTONCHANNEL "442"
#define ERR_NOTREGISTERED "451"
#define ERR_NEEDMOREPARAMS "461"
#define ERR_ALREADYREGISTRED "462"
#define ERR_PASSWDMISMATCH "464"
#define ERR_CHANOPRIVSNEEDED	"482" //"<channel> :You're not channel operator"

#endif
