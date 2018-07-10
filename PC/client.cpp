#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <arpa/inet.h>
#include <stdio.h>
#include <thread>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <map>
#include <chrono>
#include <errno.h>

#include "mcs.h"

std::map<std::string,MainConnection> Users;

struct sockaddr_in stSockAddr;
int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

std::string m_User;
std::string m_routerPort;
std::string m_routerIp;

std::string fillZeros(int aux_size,int nroBytes){
        std::string aux = std::to_string(aux_size);
        int dif = nroBytes - int(aux.size());
        for (int i = 0; i < dif; i++)
                aux = "0" + aux;
        return aux;
}

void Cl_write(/*int  SocketFD*/) {
        std::cout << "enter option:\n";
        std::string opt;
        std::cin >> opt;
        while(opt!="E"){
                if (opt=="U"){//obtener usuarios
                } if (opt=="T"){//test
                        std::string otherUser,ip,port;
                        std::cout<<"enter nickname to chat: ";
                        //std::cin.ignore();
                        //getline(std::cin, otherUser);
                        std::cin >> otherUser;
                        std::cout<<"enter ip: ";
                        //getline(std::cin, ip);
                        std::cin >> ip;
                        std::cout<<"enter port: ";
                        //getline(std::cin, port);
                        std::cin >> port;
                        Users[otherUser]=MainConnection(ip.c_str(),stoi(port));
                } else if(opt=="L"){//invitacion a otro usuario
                        std::string otherUser,msg;
                        std::cout<<"enter nickname to chat: ";
                        //std::cin.ignore();
                        //getline(std::cin, otherUser);
                        std::cin >> otherUser;
                        int SocketFD=Users[otherUser].socketFD;
                        msg=fillZeros(m_User.size(),4)+
                                "L"+
                                m_User+
                                fillZeros(m_routerIp.size(),2)+
                                m_routerIp+
                                fillZeros(m_routerPort.size(),2)+
                                m_routerPort;
                        std::cout << msg << std::endl;
                        write(SocketFD, msg.c_str(),msg.size());
                } else if (opt=="M"){//enviar mensaje a usuario
                        std::string otherUser,msg;
                        std::cout<<"enter nickname to chat: ";
                        //std::cin.ignore();
                        //getline(std::cin, otherUser);
                        std::cin >> otherUser;
                        int SocketFD=Users[otherUser].socketFD;
                        std::cout<<"enter message: ";
                        //getline(std::cin, msg);
                        std::cin >> msg;
                        msg=fillZeros(msg.size(),4)+
                                "R"+
                                msg;
                        write(SocketFD, msg.c_str(),msg.size());
                }
                std::cout << "enter option:\n";
                std::cin >> opt;
        }
}

void Se_read(int ConnectFD){
        char buffer[5];
        int n;
        for (;;){
                bzero(buffer, 5);
                do{
                        n = read(ConnectFD, buffer, 4);
                        if(n==0){
                                close(ConnectFD);
                                return;
                        }
                        int size_txt=atoi(buffer);
                        bzero(buffer, 4);
                        n = read(ConnectFD, buffer, 1);
                        std::string action(buffer);
                        bzero(buffer, 1);
                        if(action == "L") {
                                n = read(ConnectFD, buffer, size_txt);
                                std::string user(buffer);//User
                                std::cout << user << std:: endl;
                                bzero(buffer,size_txt);
                                n = read(ConnectFD, buffer, 2);
                                std::string size_ipNat(buffer);
                                bzero(buffer,2);
                                char bufIpNat[stoi(size_ipNat)+1];
                                n = read(ConnectFD, bufIpNat, stoi(size_ipNat));
                                bufIpNat[stoi(size_ipNat)]=0;
                                std::string ipNat(bufIpNat);//ipNat
                                std::cout << ipNat << std:: endl;
                                n = read(ConnectFD, buffer, 2);
                                std::string size_portNat(buffer);
                                bzero(buffer,2);
                                char bufPortNat[stoi(size_portNat)+1];
                                n = read(ConnectFD, bufPortNat, stoi(size_portNat));
                                bufPortNat[stoi(size_portNat)]=0;
                                std::string portNat(bufPortNat);//portNat
                                std::cout << portNat << std:: endl;
                                Users[user]=MainConnection(ipNat.c_str(), stoi(portNat));
                        } else if(action == "R") {
                                n = read(ConnectFD, buffer, size_txt);
                                std::cout << std::string(buffer) << std::endl;
                                bzero(buffer,size_txt);
                        } else if (action == "E") {
                                close(ConnectFD);
                                return;
                        }
                } while (n==0);
        }
}

void acceptClient(int ConnectFD) {
        if (0 > ConnectFD) {
                perror("error accept failed");
                close(SocketFD);
                exit(EXIT_FAILURE);
        }
        //write2(ConnectFD,"Welcome","C");                               
        std::thread(Se_read, ConnectFD).detach();
        std::this_thread::sleep_for(std::chrono::seconds(100));
}

void openPortNat(int routerPort, int pcPort){
        system(std::string("/home/paralelos/CCR/nat.sh "+std::to_string(routerPort)+ " " + std::to_string(pcPort)).c_str());
        //m_User="pc1";
        m_routerPort=std::to_string(routerPort);
        system("/home/paralelos/CCR/gateway.sh");
        std::ifstream publicIp;
        publicIp.open("gateway.txt");
        getline(publicIp, m_routerIp);
}

int ConnectAndGetPort(/*std::string user, std::string passwd*/){
        std::cout << "User and passwd:\n";
        std::string user, passwd;
        std::cin >> user >> passwd;
        m_User=user;
        int maxPort= 1200;
        return maxPort+1;
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
        /*maximo puerto en tabla de usuarios del servidor*/
        openPortNat(ConnectAndGetPort(),port);
        std::thread(Cl_write).detach();
        return true;
}

int main(void){
        if(-1 == SocketFD){
                exit(EXIT_FAILURE);
        }
        memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
        stSockAddr.sin_family = AF_INET;
        stSockAddr.sin_addr.s_addr = INADDR_ANY;
        for (int i=1200;startConnection(i)==false;i++){
                std::cout << "Port " << i << ": " << "bind or listen error" << std::endl;
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
