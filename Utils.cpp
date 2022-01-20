#include "Server.hpp"


void sendMessage(const std::string &msg, int socket_fd) {
	std::cout << "To fd " << socket_fd << ": \"" << msg << "\"\n";
	send(socket_fd, msg.c_str(), msg.length(), 0);
}


std:: vector<std::string> split(const std::string& line, const std::string& delimiter) {
	std::vector<std::string> args;
	char* tmp;
	tmp = std::strtok(const_cast<char *>(line.c_str()), const_cast<char *>(delimiter.c_str()));

	while (tmp != NULL) {
		args.push_back(tmp);
		tmp = strtok (NULL, const_cast<char *>(delimiter.c_str()));
	}
	return args;
}


std:: vector<std::string> split_args(const std::string& line) {
	if (line.length() > 510)
		throw "Line is too long\n";

	std::vector<std::string> args;
	size_t i = 0;
	while (i < line.length()) {
		if (line[i] == ' ')
			i++;
		else {
			std::string tmp;
			size_t begin = i;
			if (line[i] == ':') {
				i = line.length();
				begin++;
			}
			else
				i = line.find(' ', i);
			tmp.assign(line, begin, i - begin);
			args.push_back(tmp);
		}
	}
	return args;
}

//Channels names are strings (beginning with a '&', '#', '+' or '!' character) of length up to 50 characters.
//Channel names are case insensitive. The only restriction on a channel name is that it SHALL NOT contain any spaces,
//a control G (ASCII 7) or a comma (',' which is used as a list item separator by the protocol).
//A colon (':') is used as a delimiter for the channel mask.
// Channels with '&' as prefix are local to the server where they are created.
int checkValidChannelName(const std::string &name) {
	if (name.length() > 50 ||
			(name[0] != '&' && name[0] != '#' && name[0] != '+' && name[0] != '!'))
		return 0;
	for (int i = 0; i < name.length(); i++) {
		if (isspace(name[i]) || iscntrl(name[i]) || name[i] == ',')
			return 0;
	}
	return 1;
}

Client *Server::findClient(const std::string &clientNick){
	std::vector<Client*>::iterator it= _clients.begin();
	std::vector<Client*>::iterator ite= _clients.end();
	for(; it !=ite; it++){
		if ((*it)->getNick() == clientNick)
			return (*it);
	}
	return NULL;
}

Channel *Server::findChannel(const std::string &channelName){
	std::vector<Channel*>::iterator it= _channels.begin();
	std::vector<Channel*>::iterator ite= _channels.end();
	for(; it !=ite; it++){
		if ((*it)->getChannelName() == channelName)
			return (*it);
	}
	return NULL;
}



