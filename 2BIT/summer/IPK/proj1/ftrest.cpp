/*
 * IPK - Computer Communications and Networks
 * Project 1
 * Adrian Toth (xtotha01)
 * Client/server application for file transfer via HTTP using RESTful API.
 * 19.3.2017
 *
 * CLIENT
 */

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <streambuf>
#include <algorithm>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define BUFFSIZE 1024

using namespace std;

bool is_number(const char* str) {
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

std::string current_dir_name() {
	std::string path;
	char str_rf[2048];
	getcwd(str_rf, sizeof(str_rf));
	path = str_rf;
	return path;
}

bool file_exists (const std::string& path) {
	struct stat s;
	if((stat(path.c_str(),&s)==0)&&(s.st_mode & S_IFREG )) {
		return true;
	}
	else {
		return false;
	}
}

bool dir_exists(const std::string& path) {
	struct stat s;
	if((stat(path.c_str(),&s)==0 )&&(s.st_mode & S_IFDIR)) {
		return true;
	}
	else {
		return false;
	}
}

off_t fileSize( const char* filePath ) {
    struct stat st;
	stat(filePath, &st);
	return st.st_size;
}

void check_arguments(int argc, char* argv[], std::string& command, std::string& http_command ,std::string& remote_path ,std::string& local_path) {
	if ((argc == 3) || (argc == 4)) {
		std::string argv1 = argv[1];
		std::string argv2 = argv[2];

		std::vector<std::string> cmd      = {"del",    "get", "put", "lst", "mkd", "rmd"   };
		std::vector<std::string> cmd_http = {"DELETE", "GET", "PUT", "GET", "PUT", "DELETE"};
		int i;
		for (i=0; i<6; i++) {
			if (argv1.compare(cmd[i])==0) {
				command = argv1; // LOAD COMMAND
				http_command = cmd_http[i];// LOAD COMMAND AS HTTP
				break;
			}
		}
		if ( (i==6) && !(argv2.compare(cmd[5])==0) ) { // WRONG COMMAND
			fprintf(stderr,"Error: Wrong command! Types of commands: \"del\", \"get\", \"put\", \"lst\", \"mkd\", \"rmd\".\n");
			exit(EXIT_FAILURE);
		}

		remote_path = argv2; // LOAD REMOTE-PATH

		// LOCAL-PATH necessary for PUT
		if (command.compare("put")==0 && argc!=4) {
			fprintf(stderr,"Error: LOCAL-PATH for put must be speciefied!\n");
			exit(EXIT_FAILURE);
		}

		if (argc==4) {
			if (command.compare("put")==0 || command.compare("get")==0) {
				local_path = argv[3]; // LOAD [LOCAL-PATH]
			}
			else {
				fprintf(stderr,"Error: LOCAL-PATH is entered with wrong COMMAND!\n");
				exit(EXIT_FAILURE);
			}
		}
	}
	else {
		fprintf(stderr,"Error: Wrong parameters! Usage: %s COMMAND REMOTE-PATH [LOCAL-PATH].\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	// REMOTE_PATH must start with HTTP://
	std::string http = remote_path.substr(0,7);
	if (http!="http://") { // MUST BE HTTP://....
		fprintf(stderr,"Error: Wrong REMOTE-PATH! Remote path does not start with \"http://\".\n");
		exit(EXIT_FAILURE);
	}

	return;
}

/*
 * This function will make a string which has the following type:
 * (GET|PUT|DELETE) /USER/PATH/foo/bar HTTP/1.1
 */
std::string get_http_header(int argc, std::string& command, std::string& http_command, std::string& remote_path, std::string& local_path, std::string& adress, int* port_number) {
	char x0[100] = {0}; // ADRESS = IP:PORT
	char x1[100] = {0}; // IP
	char x2[100] = {0}; // PORT
	char x3[100] = {0}; // USER
	char x4[1000] = {0}; // PATH
	std::string path;
	std::string user;
	std::string port;

	// REMOTE_PATH replace whitespace with %20
	std::size_t position;
	while((position=remote_path.find(" "))!=string::npos) {
		remote_path.replace(position,1,"%20");
	}

	if (3 == sscanf(remote_path.c_str(), "http://%[^/]/%[^/]/%s", x0, x3, x4)) {
		user = x3;
		path = x4;
		if (2 == sscanf(x0, "%[^:]:%s", x1, x2)) {
			adress   = x1;
			port = x2;
		}
		else {
			if (1 == sscanf(x0, "%s", x1)) {
				adress   = x1;
				port = "6677";
			}
			else {
				fprintf(stderr,"Error: Wrong REMOTE-PATH! Remote path has wrong \"loclahost:port\" section!\n");
				exit(EXIT_FAILURE);
			}
		}
	}
	else {
		fprintf(stderr,"Error: Wrong REMOTE-PATH! Remote path has wrong format!\n");
		exit(EXIT_FAILURE);
	}

	// PORT must be an integer
	if (!(is_number(port.c_str()))) {
		fprintf(stderr,"Error: Wrong PORT! Port must be a decimal number.\n");
		exit(EXIT_FAILURE);
	}
	else {
		*port_number = atoi(port.c_str());
	}


	if (argc==4) {
		if (command.compare("put")==0) {
			if (!(file_exists(local_path))) {
				fprintf(stderr,"Error: \"%s\" wrong file!\n", local_path.c_str());
				exit(EXIT_FAILURE);
			}
		}
		/*
		if (command.compare("get")==0) {
			if (!(dir_exists(local_path))) {
				fprintf(stderr,"Error: \"%s\" wrong directory!\n", local_path.c_str());
				exit(EXIT_FAILURE);
			}
		}*/
	}

	// create ?type=[file|folder]
	std::string type;
	if (command.compare("put")==0 || command.compare("get")==0 || command.compare("del")==0) {
		type = "?type=file";
	}
	else {
		type = "?type=folder";
	}

	return (http_command+" /"+user+"/"+path+type+" HTTP/1.1");
}

std::string get_date() {
	std::string date="Date: ";
	char str[30];
	time_t now = time(NULL);
	strftime(str, 30, "%a, %d %b %Y %H:%M:%S %Z", localtime(&now));
	date += str;
	return date;
}

std::string get_type(std::string &file_path) {
	std::string content_type  = "Content-Type: ";
	std::string shell_command = "file -b --mime-type \""+file_path+"\"";
	FILE* shell_output;
	std::string shell_string;
	shell_output = popen(shell_command.c_str(), "r");
	int i=0; char buffer[100];
	while (fgets(buffer, sizeof(buffer)-1, shell_output)) {
		shell_string += buffer;
		i++;
	}
	pclose(shell_output);
	std::size_t found = shell_string.find_last_of("\n");
	shell_string = shell_string.substr(0,found);
	content_type += shell_string;
	return content_type;
}

std::string get_length(std::string &file_path) {
	return ("Content-Length: "+to_string(fileSize(file_path.c_str())));
}

std::string scan_header(std::string& str) {
	// the only one meaningful information is the code (200/400/404)
	std::string s = str;
	std::string delimiter = "\r\n";
	size_t pos = s.find(delimiter);
	s.erase(pos,s.size()-pos);
	delimiter=" ";
	pos=s.find(delimiter);
	s.erase(0,pos+1);
	pos=s.find(delimiter);
	s.erase(pos, s.size()-pos);
	return s;
}

void analyze(bool* write,bool* print, FILE** file,std::string& command,int argc,std::string& remote_path,std::string& local_path) {

	if (command.compare("get")==0 && argc==4) {
		//std::size_t pos=(remote_path.rfind("/")+1);
		//std::string file_name = remote_path.substr(pos,(remote_path.size()-pos));
		std::string file_path = local_path;
		*file=fopen(file_path.c_str(), "w");
		if (*file==NULL) {
			fprintf(stderr, "Error: get remote-path local-path, fopen() failed to make file in local-path\n");
			exit(EXIT_FAILURE);
		}
		else {
			*write=true;
		}
		return;
	}
	if (command.compare("get")==0 && argc==3) {
		std::size_t pos=(remote_path.rfind("/")+1);
		std::string file_name = remote_path.substr(pos,(remote_path.size()-pos));
		std::string file_path = current_dir_name()+"/"+file_name;
		*file=fopen(file_path.c_str(), "w");
		if (*file==NULL) {
			fprintf(stderr, "Error: get remote-path, fopen() failed to make file in current working directory\n");
			exit(EXIT_FAILURE);
		}
		else {
			*write=true;
		}
		return;
	}
	else if (command.compare("lst")==0) {
		*print=true;
		return;
	}
}

int main (int argc, char* argv[]) {

	std::string SENDING_DATA; // string which will be sent as data to the server

	std::string command;      // COMMAND      : get, put, del, lst, mkd, rmd
	std::string http_command; // HTTP COMMAND : GET, PUT, DELETE
	std::string remote_path;  // REMOTE-PATH
	std::string local_path;   // [LOCAL-PATH]

	check_arguments(argc, argv, command, http_command, remote_path, local_path);

	std::string adress;  // ADRESS of host
	int port_number;     // PORT   of host

	std::string head=get_http_header(argc, command, http_command, remote_path, local_path, adress, &port_number);
	std::string date=get_date();
	std::string accept="Accept: text/plain";
	std::string encoding="Accept-Encoding: identity";

	// this section is only for COMMAND "put"
	if (command.compare("put")==0) {

		std::string type=get_type(local_path);
		std::string length=get_length(local_path);

		// read the file content into string
		std::ifstream ifs(local_path.c_str());
		std::string file_data((std::istreambuf_iterator<char>(ifs)),(std::istreambuf_iterator<char>()));

		SENDING_DATA = head+"\r\n"+date+"\r\n"+accept+"\r\n"+encoding+"\r\n"+type+"\r\n"+length+"\r\n\r\n"+file_data;
	}
	else if (command.compare("mkd")==0) {
		std::string type="Content-Type: text/plain";
		std::string length="Content-Length: 0";
		SENDING_DATA = head+"\r\n"+date+"\r\n"+accept+"\r\n"+encoding+"\r\n"+type+"\r\n"+length+"\r\n\r\n";
	}
	else {
		SENDING_DATA = head+"\r\n"+date+"\r\n"+accept+"\r\n"+encoding+"\r\n\r\n";
	}

/*
#############################################################################################
*/
	const char *server_hostname;
	server_hostname = adress.c_str();
	struct hostent *server;


	// existence of HOST
	if ((server = gethostbyname(server_hostname)) == NULL) {
		fprintf(stderr,"Error: No such HOST as \"%s\".\n", server_hostname);
		exit(EXIT_FAILURE);
	}


	struct sockaddr_in server_address;


	// finding the IP adress of server and initialization of structure server_adress
	bzero((char *) &server_address, sizeof(server_address));
	server_address.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&server_address.sin_addr.s_addr, server->h_length);
	server_address.sin_port = htons(port_number);


	int client_socket;


	// CREATE SOCKET
	if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
		perror("Error: Failed to create socket!");
		exit(EXIT_FAILURE);
	}

	// FAILED TO CONNECT
	if (connect(client_socket, (const struct sockaddr *) &server_address, sizeof(server_address)) != 0)	{
		perror("Error: Failed to connect");
		exit(EXIT_FAILURE);
	}

	//printf("\n===SEDING-DATA===\n%s===END===\n\n",SENDING_DATA.c_str());


	// SEND TO SERVER
	// retval = send(socket, string, string_length, flag);
	int bytestx;
	bytestx = send(client_socket, SENDING_DATA.c_str(), SENDING_DATA.length(), 0);
	if (bytestx < 0) {
		fprintf(stderr,"Error in send to server!\n\"send()\" failed!\n");
		exit(EXIT_FAILURE);
	}

/*
#############################################################################################
*/
	int bytesrx;
	std::string data;
	char buff[BUFFSIZE];
	bool first_msg=false;
	std::string code;
	int ret_value;
	bool write=false;
	bool print=false;
	FILE* file=NULL;

	while (1) {
		bzero(buff, BUFFSIZE);
		bytesrx = recv(client_socket, buff, BUFFSIZE-1, 0);
		if (bytesrx < 0) {
			fprintf(stderr,"Error in receive from server!\n\"recv()\" failed!\n");
			exit(EXIT_FAILURE);
		}
		if (bytesrx == 0) break;
		data.append(buff, bytesrx);

		if (first_msg==false) { // first data contain http header with commands
			first_msg=true;
			std::string http_header = data.substr(0, data.find("\r\n\r\n")); http_header += "\r\n"; // cut off http header in correct format for scan_header
			code = scan_header(http_header); // get commands what we have to accomplish
			data.erase(0, http_header.size()+2);
			std::string answer = data.substr(0, data.find("\r\n"));
			data.erase(0, answer.size()+2);

			if (code.compare("200")==0) ret_value=200;
			else if (code.compare("400")==0) ret_value=400;
			else ret_value=404;

			if (ret_value!=200) {
				cerr << answer << endl;
				exit(EXIT_FAILURE);
			}

			analyze(&write, &print, &file, command, argc, remote_path, local_path);
		}

		if (write==true) {
			fwrite(data.c_str(), sizeof(char), data.size(), file);
		}

		if (print==true) {
			fprintf(stdout,"%s", data.c_str());
		}

		data="";
		if (bytesrx < BUFFSIZE-1) break;
	}
	if (file!=NULL) {
		fclose(file);
	}
	close(client_socket);
	return 0;
}
