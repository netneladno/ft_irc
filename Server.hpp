#ifndef SERVER_HPP
#define SERVER_HPP

# include <iostream>
# include <sstream>
# include <iomanip>
# include <string>
# include <stdexcept>
# include <vector>
# include <set>
# include <map>

# include <sys/socket.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <fcntl.h>
# include <unistd.h>
# include <poll.h>
#include <algorithm>

# include "Error_Reply.hpp"

# define LOCALHOST "127.0.0.1"
#define MAX_CONNECTION	1024

#define NICK_VALIDSET		"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890-_[]{}\'|"
#define CHANNEL_VALIDSET	"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890-_[]{}\'| "

class Command;
class Channel;
class Server;
class Client;


class Client {
private:
	int						_sockFd;
	int						_port;
	std::string				_id;
	std::string				_nickname;
    std::string             _username;
	std::string				_host;
    std::string				_servername;
    std::string				_realname;
	std::string				_message;
	std::string				_awayMessage;
    bool    _passValid;
    bool    _registered;
	bool	_isInvisible;

public:
	Client(int socketFd) : _sockFd(socketFd), _nickname(""), _passValid(false), _registered(false), _isInvisible(false) {}
	~Client();

    int			getSockFd() const;
    int			getPort() const;
    std::string getNick() const;
	std::string getUsername() const;
	std::string	getHost() const;
	std::string getRealname() const;
	std::string getMessage() const;
    bool 		getRegisterStatus() const;
	bool 		getPassStatus() const;
	bool 		getInvisibleStatus() const;
    bool 		checkUserStatus() const;

    void setNick(const std::string &name);
	void setUserName (const std::string &name);
	void setHostName (const std::string &name);
	void setServerName (const std::string &name);
	void setRealName (const std::string &name);
	void setPassStatus();
	void setRegisterStatus();
	void setInvisibleStatus(bool status);

};

class Channel {
private:
	const std::string 		_name;
	std::string 			_key;
	std::string 			_topic;
	long int				_maxUsers;
	std::vector<Client *>	_users;
	std::set<Client *>		_operators;
//	std::vector<Client *>	_banned;
//	bool 					_inviteOnlyFlag;
	bool 					_keyFlag;
	bool 					_topicOperOnly;
	bool 					_maxUsersFlag;

	Channel() {}

public:
	Channel(const std::string& channel_name, Client &user);
	Channel(const std::string& channel_name, const std::string& key, Client &user);

	/*
	 *  GETTERS
	 */
	std::string getChannelName() const;
	std::string getTopic() const;
	std::string getKey() const;
	int 		getNumUsers() const;
	long int	getMaxUsers() const;
	bool 		getKeyStatus() const;
	bool 		getTopicOperatorsOnly() const;
//	bool 		getInviteOnlyFlag() const;
	bool 		getMaxUsersFlag() const;


	/*
	 *  SETTERS
	 */
	void setTopic (const std::string &topic);
	void setTopicOperOnly(bool status);
//	void setInviteOnlyFlag (bool status);
	void setMaxUsers (long int num);
	void setKey(std::string &password);
	void setKeyFlag(bool status);
	void setMaxUsersFlag(bool status);


	/*
	 * OTHER FUNCTIONS
	 */
	void addUser(Client &user);
	void addOperator(Client &user);

	void deleteOperator(Client &user);
	void deleteUser(Client &client);

	std::string sendUserList(bool printInvisibleUsers);
	bool isOperator(Client *client);
	bool isChannelUser (Client *client);

//	std::string printChannelUsers(); //todo delete
	void sendMsgToChan(const std::string &message, Client *client);
};


#include <string>
#include <iostream>
#include <sys/poll.h>
#include <vector>


class Server{
private:
	int _socketFd;
	const std::string *_host;
	const std::string &_port;
	const std::string &_password;
	std::vector<pollfd> _fds;
	std::vector<Client *> _users;
	std::vector<Channel *> _channels;
	std::set<Client *> _operators;
	std::map<std::string, std::string> _operator_login;
	int _maxNumberOfChannels;

	typedef void(Server::*command)(std::vector<std::string> &args, Client &user);

	std::vector<command> commands;
	std::vector<std::string> commandsName;

public:
	Server(const std::string *host, const std::string &port, const std::string &password);
	virtual ~Server();

	void start();
	void init();
	void acceptProcess();
	std::string recvMessage(int fd);


    void parser(Client *client, std::string msg);
	bool isServerOperator(Client *client);


    /*
     * Commands
     */
    void passExec(Client &client, std::vector<std::string> &args);
	void userExec(Client &client, std::vector<std::string> &args);
	void nickExec(Client &client, std::vector<std::string> &args);

	void joinExec(Client &client, std::vector<std::string> &args);
	void listExec(Client &client, std::vector<std::string> &args);
	void topicExec(Client &client, std::vector<std::string> &args);

	void privmsgExec(Client &client, std::vector<std::string> &args);
	void modeExec(Client &client, std::vector<std::string> &args);
	void pingExec(Client &client, std::vector<std::string> &args);

	void partExec(Client &client, std::vector<std::string> &args);
	void kickExec(Client &client, std::vector<std::string> &args);
	void namesExec(Client &client, std::vector<std::string> &args);

	void operExec(Client &client, std::vector<std::string> &args);
	void quitExec(Client &client, std::vector<std::string> &args);
	void killExec(Client &client, std::vector<std::string> &args);

	/*
	 * Server Utils
	 */
	Client *findClient(const std::string &clientNick);
	Client *findClientbyFd(int fd);
	Channel *findChannel(const std::string &channelName);
	void sendTopic(Client &client, const std::string& channelName);
	void sendUsers(Client &client,Channel &channel);
	void setChannelModes(Client &client, std::vector<std::string> &args);
	void setUserModes(Client &client, std::vector<std::string> &args);
	void informOfNewOperator(Client &client);
	void removeClient(Client *client);
	void removeOperator(Client *client);
	void leaveChannel(Client &client, Channel *channel);



};

void sendMessage(const std::string &msg, int socket_fd);
int checkValidChannelName(const std::string &name);
std:: vector<std::string> split(const std::string& line, const std::string& delimiter);
std:: vector<std::string> split_args(const std::string& line);
std::string	intToString(long int num);
void sendmotd(Client &client);

#endif
