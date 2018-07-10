  /* Server code in C */
 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <map>
#include <chrono>
#include <errno.h>

#include "utilidad.h"

/*VARIABLES Y FUNCIONES*/

struct sockaddr_in stSockAddr;
int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

std::map<std::string, std::pair<std::string,int> > clients;
std::map<std::string, std::pair<std::string,int> >::iterator it2;

std::map<int, std::pair<std::string,std::string> > Table;
std::map<int, std::pair<std::string,std::string> >::iterator it;

void acceptClient(int ConnectFD);
bool write2(int ConnectFD,std::string prnt, std::string act,std::string remitente="?"); 
void read2(int ConnectFD);

/***********************/

bool find_nick(std::string st){ //find  a  nickname is equal to st

	for (it2 = clients.begin(); it2 != clients.end(); it2++)
		if(it2->first == st){
			return true;
		}
	return false;
}

std::string find_str(int id){//return nickanme found their number socket  
	for (it2 = clients.begin(); it2 != clients.end(); it2++)
		if((it2->second).second == id){
			return it2->first;
		}
	return "";
}

void read2(int ConnectFD){
	char buffer[5];
	int n;
	bool login=false;

	for (;;){
		bzero(buffer, 5);
		do{
			n = read(ConnectFD, buffer, 4);
			if(n==0){
				if(login){
					clients.erase(/*clients.find(*/find_str(ConnectFD)/*)*/);
				}
				close(ConnectFD);
				return;
			}
			int size_txt=atoi(buffer);
			bzero(buffer, 4);

			n = read(ConnectFD, buffer, 1);
			std::string action(buffer);
			bzero(buffer, 1);

			if (action == "U"){ //Protocolo for Users
				char m_User[size_txt+1];
				n = read(ConnectFD, m_User, size_txt);//User
				m_User[size_txt]=0;
				char size_passw[3];
				n = read(ConnectFD, size_passw, 2);
				size_passw[2]=0;
				char m_Passw[atoi(size_passw)+1];
				n = read(ConnectFD, m_Passw, atoi(size_passw));//Passw
				m_Passw[atoi(size_passw)]=0;
				if(clients.find(std::string(m_User))!=clients.end() && clients[std::string(m_User)].first==std::string(m_Passw)) {
					clients[std::string(m_User)]=std::make_pair(std::string(m_Passw),ConnectFD);
					std::cout << std::string(m_User) << " " << std::string(m_Passw) << "\n";
					std::vector<std::string> usersList;
					for (auto ite = clients.begin(); ite != clients.end(); ite++){
						std::cout << "usuario: " << ite->first << "\n";
						if((ite->second).second != -1 && (ite->second).second != ConnectFD){
							if(Table.find(clients[ite->first].second)!=Table.end()){
								usersList.push_back(ite->first);
								std::cout << "conectado\n";
							}
						}
					}
					std::string usersProtocol=fillZeros(usersList.size(),4);
					for (int i=0;i<usersList.size();i++){
						usersProtocol+=fillZeros(usersList[i].size(),2);
						usersProtocol+=usersList[i];
						usersProtocol+=fillZeros(Table[clients[usersList[i]].second].first.size(),2);
						usersProtocol+=Table[clients[usersList[i]].second].first;
						usersProtocol+=fillZeros(Table[clients[usersList[i]].second].second.size(),2);
						usersProtocol+=Table[clients[usersList[i]].second].second;
					}
					write(ConnectFD,usersProtocol.c_str(),usersProtocol.size());
					std::cout << usersProtocol << std::endl;
				} else {
					write(ConnectFD,"0000",4);
				}
			} else if (action == "F") {//Protocolo for Insert Row
				char m_routerIp[size_txt+1];
				n = read(ConnectFD, m_routerIp, size_txt);//routerIp
				m_routerIp[size_txt]=0;
				char size_port[3];
				n = read(ConnectFD, size_port, 2);
				size_port[2]=0;
				char m_routerPort[atoi(size_port)+1];
				n = read(ConnectFD, m_routerPort, size_txt);//routerPort
				m_routerPort[atoi(size_port)]=0;
				Table[clients[find_str(ConnectFD)].second]=make_pair(std::string(m_routerIp),std::string(m_routerPort));
			} else if (action == "E"){//protocol for End
				std::vector<std::string> V;
				for (auto it2=clients.begin();it2!=clients.end();it2++){
					if((it2->second).second==ConnectFD){
						V.push_back(it2->first);
					}
				}
				for (int i=0;i<V.size();i++){
					clients.erase(V[i]);
				}
				close(ConnectFD);
				return;
			} else {// this is can be better, you can do it =)
				std::cout << "error in action, msg bad\n";
			}

		} while (n == 0);
	}
}

void acceptClient(int ConnectFD) {
	if (0 > ConnectFD) {
		perror("error accept failed");
		close(SocketFD);
		exit(EXIT_FAILURE);
	}

	//write2(ConnectFD,"Welcome","C");
	std::thread(read2, ConnectFD).detach();
	std::this_thread::sleep_for(std::chrono::seconds(100));
}

bool startConnection(int port){
	stSockAddr.sin_port = htons(port);
	if(-1 == bind(SocketFD,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in))){
		/*perror("error bind failed");close(SocketFD);exit(EXIT_FAILURE);*/
		return false;
	}
	if(-1 == listen(SocketFD, 10)){
		/*perror("error listen failed");close(SocketFD);exit(EXIT_FAILURE);*/
		return false;
	}
	std::cout << "Port " << port << ": " << "connection success" << std::endl;
	return true;
}

int main(void){

	if(-1 == SocketFD){
		perror("can not create socket");
		exit(EXIT_FAILURE);
	}

	memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

	stSockAddr.sin_family = AF_INET;
	stSockAddr.sin_addr.s_addr = INADDR_ANY;
	for (int i=1100;startConnection(i)==false;i++){
		std::cout << "Port " << i << ": " << "bind or listen error" << std::endl;
	}
	std::cout << "¿preasignar contraseñas?(yes/no)" << std::endl;
	std::string preAsig;
	std::cin >> preAsig;
	if(preAsig=="yes"){
		std::cout << "Numero de Usuarios"<< std::endl;
		int n;
		std::cin >> n;
		for (int i=0;i<n;i++){
			std::cout << "Usuario contraseña"<< std::endl;
			std::string useraux, passaux;
			std::cin >> useraux >> passaux;
			clients[useraux]=std::make_pair(passaux,-1);
		}
	} else {
		for (int i=0;i<5;i++){
			clients["PC"+std::to_string(i)]=std::make_pair("pass",-1);
		}
	}
	while(true){
		int ConnectFD = accept(SocketFD, NULL, NULL);
		std::cout << "ConnectFD: " << ConnectFD << std::endl;
		std::thread(acceptClient,ConnectFD).detach();
	}

	shutdown(SocketFD, SHUT_RDWR);
	close(SocketFD);
	return 0;
}
