/*
 * IPK - Computer Communications and Networks
 * Project 1
 * Adrian Toth (xtotha01)
 * Client/server application for file transfer via HTTP using RESTful API.
 * 19.3.2017
 *
 * SERVER
 */

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFSIZE 1024 // buffer size

using namespace std;

/*
 * Function is called
 */
void wrong_arguments(const char * x) {
	fprintf(stderr,"Error: Wrong parameters! Usage: %s [-r ROOT-FOLDER] [-p PORT].\n", x);
	exit(EXIT_FAILURE);
}

/*
 * check if the file-path (absolute) represents an existing file
 */
bool file_exists (const std::string& path) {
	struct stat s;
	if((stat(path.c_str(),&s)==0)&&(s.st_mode & S_IFREG )) {
		return true;
	}
	else {
		return false;
	}
}

/*
 * check if the directory-path (absolute) represents an existing file
 */
bool dir_exists(const std::string& path) {
	struct stat s;
	if((stat(path.c_str(),&s)==0 )&&(s.st_mode & S_IFDIR)) {
		return true;
	}
	else {
		return false;
	}
}

/*
 * check if the enterd path represents file or directory which exists
 */
bool file_or_dir_exists(const std::string& path) {
	return (file_exists(path) || dir_exists(path));
}

/*
 * same as "pwd" in shell, return the current working directory absolute path
 */
std::string current_dir_name() {
	std::string path;
	char str_rf[2048];
	getcwd(str_rf, sizeof(str_rf));
	path = str_rf;
	return path;
}

/*
 * check the -r ROOT-DIR if we have permissions
 */
void check_directory(std::string &root_folder) {
	DIR* dir = opendir(root_folder.c_str());
	if (dir) {
	    closedir(dir);
	}
	else if (ENOENT == errno) {
	    fprintf(stderr,"Error: Directory does not exist!\n");
		exit(EXIT_FAILURE);
	}
	else {
	    fprintf(stderr,"Error: Cannot check if directory exists! Opendir() failed.\n");
		exit(EXIT_FAILURE);
	}
}

/*
 * if string represents number than it returns true, false if not
 */
bool is_number(const char *str) {
	//return !s.empty() && std::find_if(s.begin(), s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
	if (!*str)
    	return false;
	while (*str) {
		if (!isdigit(*str))
			return false;
		else
			++str;
	}
	return true;
}

/*
 * ftrestd main argument checker, it check everything -r ROOT-DIR -p PORT and setup variables
 */
void check_arguments(int argc, const char * argv[], std::string &root_folder, std::string &port, bool *r, bool *p) {
	if (argc == 1) {
		port = "6677";
		root_folder = current_dir_name();
	}
	else if (argc == 3) {
		if (strcmp(argv[1], "-r") == 0) {
			*r=true;
			root_folder = argv[2];
			port = "6677";
		}
		else if (strcmp(argv[1], "-p") == 0) {
			*p=true;
			port = argv[2];
			root_folder = current_dir_name();
		}
		else {
			wrong_arguments(argv[0]);
		}
	}
	else if (argc == 5) {
		if ((strcmp(argv[1],"-r") == 0) && (strcmp(argv[3],"-p") == 0)) {
			*r = true;
			*p = true;
			root_folder = argv[2];
			port = argv[4];
		}
		else if ((strcmp(argv[1],"-p") == 0) && (strcmp(argv[3],"-r") == 0)) {
			*r = true;
			*p = true;
			root_folder = argv[4];
			port = argv[2];
		}
		else {
			wrong_arguments(argv[0]);
		}
	}
	else {
		wrong_arguments(argv[0]);
	}

	if (*r==true) {
		check_directory(root_folder);
	}

	if (*p==true) {
		if (is_number(port.c_str())==false) {
			fprintf(stderr,"Error: Port must be a decimal number!\n");
			exit(EXIT_FAILURE);
		}
	}
}

/*
 * return time in string for http head
 */
std::string get_date() {
	std::string date="Date: ";
	char str[30];
	time_t now = time(NULL);
	strftime(str, 30, "%a, %d %b %Y %H:%M:%S %Z", localtime(&now));
	date += str;
	return date;
}

/*
 * cut out main information from vector of http rows
 */
std::vector<std::string> scan_rows(std::vector<std::string> &array) {
	std::string first_row = array[0];

	if (first_row.find("HTTP/1.1")==std::string::npos) {
		fprintf(stderr,"Error: HTTP header format is not supported (it must be in HTTP/1.1)!\n");
		exit(EXIT_FAILURE);
	}

	std::string command;
	if (first_row.find("PUT ")!=std::string::npos) {
	   command="PUT";
	}
	else if (first_row.find("GET ")!=std::string::npos) {
	    command="GET";
	}
	else if (first_row.find("DELETE ")!=std::string::npos) {
	    command="DELETE";
	}
	else {
		fprintf(stderr,"Error: HTTP header does not contain command (PUT/GET/DELETE)!\n");
		exit(EXIT_FAILURE);
	}

	std::string type;
	if (first_row.find("?type=file ")!=std::string::npos) {
	    type="file";
	}
	else if (first_row.find("?type=folder ")!=std::string::npos) {
	    type="folder";
	}
	else {
		fprintf(stderr,"Error: HTTP header does not contain type (file/folder)!\n");
		exit(EXIT_FAILURE);
	}

	std::string path;
	std::string end="?type=";
	end.append(type);
	path=first_row.substr(command.length()+1,first_row.size());
	path.erase(path.find(end),path.size()-path.find(end));

	std::size_t position;
	while((position=path.find("%20"))!=string::npos) {
		path.replace(position,3," ");
	}

	std::vector<std::string> cmds;
	cmds.push_back(command);
	cmds.push_back(type);
	cmds.push_back(path);

	return cmds;
}

/*
 * parse the http header to vector of string (rows)
 */
std::vector<std::string> scan_header(std::string& str) {

	std::vector<std::string> array;

	std::string s = str;
	std::string delimiter = "\r\n";

	size_t pos = 0;
	std::string token;
	while ((pos = s.find(delimiter)) != std::string::npos) {
	    token = s.substr(0, pos);
	    array.push_back(token);
	    s.erase(0, pos + delimiter.length());
	}

	std::vector<std::string> cmds;
	cmds = scan_rows(array);
	return cmds;
}

/*
 * this function control the response, scan header return command|type|path into this function which provide analysis of these parameters and decide how to make response
 */
void analyze(std::vector<std::string> &array, int* rettype, std::string& message, std::string& data_send_back, std::string& cwd, std::string& command, FILE** file ) {

	std::string path = array[2];
	std::string abs_path = cwd+path;

	std::string user = path;
	user.erase(0,1);
	std::size_t user_pos=(user.find("/"));
	user=user.substr(0,user_pos);
	std::string user_dir=cwd+"/"+user;
	if (!dir_exists(user_dir)) {
		*rettype=404;
		message="User Account Not Found";
		return;
	}

	// get ======================================================================================
	if ( array[0].compare("GET")==0 && array[1].compare("file")==0 ) {
		if (!file_or_dir_exists(abs_path)) {
			*rettype=404;
			message="File not found.";
			return;
		}
		else if (dir_exists(abs_path)) {
			*rettype=400;
			message="Not a file.";
			return;
		}

		FILE* f=fopen(abs_path.c_str(),"rb");
		if (f!=NULL) {
			fclose(f);
		}
		else {
			*rettype=400;
			message="Unknown error.";
			return;
		}

		std::ifstream ifs(abs_path.c_str());
		std::string content((std::istreambuf_iterator<char>(ifs)),(std::istreambuf_iterator<char>()));
		data_send_back.append(content.data(), content.size());
		*rettype=200;
		message="OK";
		command="get";
		return;
	}

	// put ======================================================================================
	else if ( array[0].compare("PUT")==0 && array[1].compare("file")==0 ) {
		std::string file_name;
		std::string dir_name;
		std::size_t pos=(abs_path.rfind("/")+1);
		file_name = abs_path.substr(pos,(abs_path.size()-pos));
		dir_name  = abs_path.substr(0,pos);
		if (!file_or_dir_exists(dir_name)) {
			*rettype=404;
			message="Directory not found.";
			return;
		}
		else if (dir_exists(abs_path)) {
			*rettype=400;
			message="Already exists.";
			return;
		}
		else if (file_exists(abs_path)) {
			*rettype=400;
			message="Already exists.";
			return;
		}
		*file=fopen(abs_path.c_str(), "w");
		if (*file==NULL) {
			*rettype=400;
			message="Unknown error.";
			return;
		}
		else {
			*rettype=200;
			message="OK";
			command="put";
			return;
		}
	}

	// del ======================================================================================
	else if ( array[0].compare("DELETE")==0 && array[1].compare("file")==0 ) {
		std::string file_name;
		std::string dir_name;
		std::size_t pos=(abs_path.rfind("/")+1);
		file_name = abs_path.substr(pos,(abs_path.size()-pos));
		dir_name  = abs_path.substr(0,pos);
		if (!file_or_dir_exists(dir_name)) {
			*rettype=404;
			message="Directory not found.";
			return;
		}
		else if (dir_exists(abs_path)) {
			*rettype=400;
			message="Not a file.";
			return;
		}
		else if (!file_exists(abs_path)) {
			*rettype=404;
			message="File not found.";
			return;
		}
		if (remove(abs_path.c_str())==0) {
			*rettype=200;
			message="OK";
			command="del";
			return;
		}
		else {
			*rettype=400;
			message="Unknown error.";
			return;
		}
	}

	// lst ======================================================================================
	else if ( array[0].compare("GET")==0 && array[1].compare("folder")==0 ) {
		if (!file_or_dir_exists(abs_path)) {
			*rettype=404;
			message="Directory not found.";
			return;
		}
		else if (file_exists(abs_path)) {
			*rettype=400;
			message="Not a directory.";
			return;
		}
		DIR *dir;
		struct dirent *ent;
		if ( (dir = opendir(abs_path.c_str())) != NULL) {
			while ((ent = readdir (dir)) != NULL) {
				if (strcmp(ent->d_name,".")==0) continue;
				if (strcmp(ent->d_name,"..")==0) continue;
				data_send_back.append(ent->d_name);
				data_send_back.append("\n");
			}
			closedir (dir);
			*rettype=200;
			message="OK";
			command="lst";
			return;
		}
		else {
			*rettype=400;
			message="Unknown error.";
			return;
		}
	}

	// mkd ======================================================================================
	else if ( array[0].compare("PUT")==0 && array[1].compare("folder")==0 ) {
		std::string name;
		std::string dir_name;
		std::size_t pos=(abs_path.rfind("/")+1);
		name      = abs_path.substr(pos,(abs_path.size()-pos));
		dir_name  = abs_path.substr(0,pos);
		if (!file_or_dir_exists(dir_name)) {
			*rettype=404;
			message="Directory not found.";
			return;
		}
		else if (dir_exists(abs_path) || file_exists(abs_path)) {
			*rettype=400;
			message="Already exists.";
			return;
		}
		if (mkdir(abs_path.c_str(), 0700)==0) {
			*rettype=200;
			message="OK";
			command="mkd";
			return;
		}
		else {
			*rettype=400;
			message="Unknown error.";
			return;
		}
	}

	// rmd ======================================================================================
	else if ( array[0].compare("DELETE")==0 && array[1].compare("folder")==0 ) {
		if (!file_or_dir_exists(abs_path)) {
			*rettype=404;
			message="Directory not found.";
			return;
		}
		else if (file_exists(abs_path)) {
			*rettype=400;
			message="Not a directory.";
			return;
		}
		if (rmdir(abs_path.c_str())==0) {
			*rettype=200;
			message="OK";
			command="rmd";
			return;
		}
		else {
			*rettype=400;
			message="Directory not empty.";
			return;
		}
		return;
	}
}

/*
 * put strings together (information representing answer)
 */
std::string make_response(int ret_value,std::string& ret_message,std::string& ret_data) {

	std::string content;
	content=ret_message.data();
	content.append("\r\n",2);
	content.append(ret_data.data(),ret_data.size());

	std::string head;
	switch (ret_value) {
		case 200:
			head.append("HTTP/1.1 200 OK");
			break;
		case 400:
			head.append("HTTP/1.1 400 Bad Request");
			break;
		default:
			head.append("HTTP/1.1 404 Not Found");
			break;
	}

	std::string date=get_date();
	std::string type="Content-Type: text/plain";
	std::string length="Content-Length: "+to_string(content.size());
	std::string encoding="Content-Encoding: identity";

	return (head+"\r\n"+date+"\r\n"+type+"\r\n"+length+"\r\n"+encoding+"\r\n\r\n"+content);
}

/*
 * main function which call the rest of functions
 */
int main (int argc, const char * argv[]) {

	// [-r ROOT-FOLDER]
	bool r=false;
	std::string root_folder;

	//  [-p PORT]
	bool p=false;
	std::string port;

	check_arguments(argc, argv, root_folder, port, &r, &p);

	int port_number=atoi(port.c_str());

/*
#############################################################################################
*/

	int rc;
	int welcome_socket;
	struct sockaddr_in6 sa;
	struct sockaddr_in6 sa_client;

	socklen_t sa_client_len=sizeof(sa_client);
	if ((welcome_socket = socket(PF_INET6, SOCK_STREAM, 0)) < 0) {
		perror("ERROR: Failed to create socket!");
		exit(EXIT_FAILURE);
	}

	memset(&sa,0,sizeof(sa));
	sa.sin6_family = AF_INET6;
	sa.sin6_addr = in6addr_any;
	sa.sin6_port = htons(port_number);

	if ((rc = bind(welcome_socket, (struct sockaddr*)&sa, sizeof(sa))) < 0) {
		perror("ERROR: bind");
		exit(EXIT_FAILURE);
	}
	if ((listen(welcome_socket, 20)) < 0) {
		perror("ERROR: listen");
		exit(EXIT_FAILURE);
	}

	int res;
	char buff[BUFFSIZE];

	bool first_msg;
	int ret_value;
	FILE* pFile = NULL;
	std::string ret_message="";
	std::string ret_data="";
	std::string data="";
	std::string command="";
	std::vector<std::string> commands;

	while(1) {

		int comm_socket = accept(welcome_socket, (struct sockaddr*)&sa_client, &sa_client_len);
		if (comm_socket > 0) {
			first_msg=false;
			ret_message="";
			ret_data="";
			data="";
			command="";

			while(1) {

				bzero(buff, BUFFSIZE);
				res = recv(comm_socket, buff, BUFFSIZE-1, 0);
				if (res < 0) {
					fprintf(stderr,"Error in receive from client!\n\"recv()\" failed!\n");
					exit(EXIT_FAILURE);
				}
				if (res == 0) break;

				data.append(buff, res); // make std::string type buffer

				if (first_msg==false) { // first data contain http header with commands
					first_msg=true;
					std::string http_header = data.substr(0, data.find("\r\n\r\n")); http_header += "\r\n"; // cut off http header in correct format for scan_header
					commands = scan_header(http_header); // get commands what we have to accomplish
					data = data.erase(0, http_header.size()+2);
					analyze(commands, &ret_value, ret_message, ret_data, root_folder, command, &pFile);
				}

				if (ret_value==200) {
					if (command.compare("put")==0) {
						fwrite(data.c_str(), sizeof(char), data.size(), pFile);
					}
				}

				data="";
				if (res < BUFFSIZE-1) break;

			} // END OF INNER WHILE

			if (pFile!=NULL) {
				fclose(pFile);
				pFile=NULL;
			}
		}
		else {
			fprintf(stderr,"Error: comm_socket <= 0\n");
			exit(EXIT_FAILURE);
		}


		std::string response="";
		response = make_response(ret_value, ret_message, ret_data);
		int snd;
		snd = send(comm_socket, response.c_str(), response.size(), 0);
		if (snd < 0) {
			fprintf(stderr,"Error in send to client! \"send()\" failed!\n");
			exit(EXIT_FAILURE);
		}
		close(comm_socket);
	}
}
