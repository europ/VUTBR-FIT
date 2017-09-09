/*
 * IPK - Computer Communications and Networks
 * 2. Project - Chat client
 * Adrian Toth - xtotha01
 * 8.4.2017
 */

#include <iostream>
#include <thread>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

#define BUFFSIZE 16384
using namespace std;
volatile sig_atomic_t flag = 0; // IF pressed(control + C) THEN flag=1 ELSE flag=0

// print error message to stderr and exit the program with exitcode
void error(const char* message, int exitcode) {
	fprintf(stderr,"%s",message);
	exit(exitcode);
}

// check arguments and setup variables
void check_args(int argc, char* argv[], std::string& ipaddress, std::string& username) {
	if (argc == 5 && strcmp(argv[1], "-i") == 0 && strcmp(argv[3], "-u") == 0) {
		ipaddress = argv[2];
		username = argv[4];
	}
	else error("Wrong arguments!\nUsage: ./chat_client -i IP_address -u username.\n",1);
}

void handler(int s) {
	flag=1;
	s=s; // cheat because of unused parameter s
}

// this thread is checking if client was kill via SIGINT
void t1_body(int client_socket, std::string username) {

	std::string msg = username+" logged out\r\n"; // logout message

	signal(SIGINT, handler);

	while(1) {
		if (flag!=0) { // ^C == SIGINT detected
			send(client_socket, msg.c_str(), msg.size(), 0); // send logout message
			close(client_socket); // close connection
			exit(0);
		}
	}
}

// this thread is a printer for received messages
void t2_body(int client_socket) {

	char buffer[BUFFSIZE];
	int recval;

	while (1) {
		bzero(buffer, BUFFSIZE); // reset buffer
		recval=recv(client_socket, buffer, BUFFSIZE-1, 0); // receive message
		fprintf(stdout,"%s",buffer); // print message
		if (recval==0) { // unexpected server shut down => shut down client
			close(client_socket);
			exit(1);
		}
	}
}

int main(int argc, char* argv[]) {

	std::string ipaddress, username;

	check_args(argc, argv, ipaddress, username); // check args, initialize ipaddress and username

	// get host address from DNS server
	struct hostent *server;
	if ((server = gethostbyname(ipaddress.c_str())) == NULL) error("Wrong IP address!\nNo such host.\n",1); // find ip in network

	int portnumber = 21011; // port, value is explicitly defined in task

	// initialize server_address structure
	struct sockaddr_in server_address;
	bzero((char *) &server_address, sizeof(server_address));
	server_address.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&server_address.sin_addr.s_addr, server->h_length);
	server_address.sin_port = htons(portnumber);

	// create socket
	int client_socket;
	if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0) error("Could not create socket!\n",1);

	// connect
	if (connect(client_socket, (const struct sockaddr *) &server_address, sizeof(server_address)) != 0) error("Could not connect!\n",1);

	// send login
	std::string msg = username+" logged in\r\n";
	if ( send(client_socket, msg.c_str(), msg.size(), 0) < 0) {
		close(client_socket);
		error("Could not send first message!\n",1);
	}

	// run thread to catch ^C sigint
	thread t1(t1_body, client_socket, username);

	// run thread for printing incoming messages
	thread t2(t2_body, client_socket);

	// read user message and send
	std::string line="";
	while(1) {
		std::getline(std::cin, line);
		if (line.empty()) continue; // do not send empty message
		msg = username+": "+line+"\r\n";
		send(client_socket, msg.c_str(), msg.size(), 0);
		line=msg="";
	}

	return 0;
}