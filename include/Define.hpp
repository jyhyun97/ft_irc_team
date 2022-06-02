#ifndef DEFINE_HPP
#define DEFINE_HPP

// numeric replies에 미리 띄어쓰기를 포함시켜 두면 편할까?
#define RPL_WELCOME "001"
#define RPL_NAMREPLY "353"
#define RPL_ENDOFNAMES "366"
#define ERR_PASSWDMISMATCH "464"


//nick
//ERR_NONICKNAMEGIVEN  431   ???
#define ERR_ERRONEUSNICKNAME	"432"
#define ERR_NICKNAMEINUSE		"433"
//ERR_NICKCOLLISION     434  다른 서버와 닉네임 충돌이므로 지금은 필요 없음

//privmsg
#define ERR_NOSUCHNICK "401" //"<nickname> :No such nick/channel"


//kick
#define ERR_NEEDMOREPARAMS "461" // "<command> :Not enough parameters"
// #define ERR_BADCHANMASK  476 "<channel> :Bad Channel Mask"
#define ERR_USERNOTINCHANNEL	"441" //"<nick> <channel> :They aren't on that channel"
#define ERR_NOSUCHCHANNEL		"403" //"<channel name> :No such channel"
#define ERR_CHANOPRIVSNEEDED	"482" //"<channel> :You're not channel operator"
#define ERR_NOTONCHANNEL		"442" //"<channel> :You're not on that channel"

#endif
