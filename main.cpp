#include <windows.h>
#include <winsock.h>
#include <cstdio>
#include <iostream>
#include <csignal>
#include <string>
#include "threadsafe_queue.cpp"
#include "json.hpp"

using json = nlohmann::json;

//DECLARATIONS
//error trapping signals
#define SIGINT 2
#define SIGKILL 9
#define SIGQUIT 3
// SOCKETS
SOCKET sock,client;

void s_handle(int s)
{
    if(sock)
        closesocket(sock);
    if(client)
        closesocket(client);
    WSACleanup();
    Sleep(1000);
    std::cout<<"EXIT SIGNAL :"<<s;
    exit(0);
}


void s_cl(const char *a, int x)
{
    std::cout<<a;
    s_handle(x+1000);
}


int main()
{
    SetConsoleTitle("Simple Message Queue Client");

    //Declarations
    int res;
    std::string rcv;
    char ip[15];
    WSADATA data;
    threadsafe_queue<std::string> sharedQueue{};

    signal(SIGINT,s_handle);
    signal(SIGKILL,s_handle);
    signal(SIGQUIT,s_handle);

    std::cout<<"\nEnter IP to connect to: ";
    gets(ip);

    sockaddr_in ser{};
    sockaddr addr{};


    ser.sin_family=AF_INET;
    ser.sin_port=htons(5005);                    //Set the port
    ser.sin_addr.s_addr=inet_addr(ip);      //Set the address we want to connect to

    memcpy(&addr,&ser,sizeof(SOCKADDR_IN));

    res = WSAStartup(MAKEWORD(1,1),&data);      //Start Winsock
    std::cout<<"\n\nWSAStartup"
        <<"\nVersion: "<<data.wVersion
        <<"\nDescription: "<<data.szDescription
        <<"\nStatus: "<<data.szSystemStatus<<std::endl;

    if(res != 0)
        s_cl("WSAStarup failed",WSAGetLastError());

    sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);       //Create the socket
    if(sock==INVALID_SOCKET )
        s_cl("Invalid Socket ",WSAGetLastError());
    else if(sock==SOCKET_ERROR)
        s_cl("Socket Error)",WSAGetLastError());
    else
        std::cout<<"Socket Established"<<std::endl;

    res=connect(sock,&addr,sizeof(addr));               //Connect to the server
    if(res !=0 )
    {
        s_cl("SERVER UNAVAILABLE",res);
    }
    else
    {
        std::cout<<"\nConnected to Server: ";
        memcpy(&ser,&addr,sizeof(SOCKADDR));
    }

    char ReceivedData[100] = "";
    int ret;

    while(true)
    {
        std::string line_in;
        json j;
        std::string dataToSend;
        bool waitingOnServer = false;

        std::cout<<"\nEnter Message Operation: (g)et or (s)end\n";
        std::getline(std::cin,line_in);

        if(line_in == "g"){
            std::cout<<"Requesting message from server\n";

            j["messageType"] = "get";
            dataToSend = j.dump();
            waitingOnServer = true;
        }
        else if(line_in == "s"){
            std::cout<<"\nEnter message to send ->\n";
            std::getline(std::cin, line_in);

            j["messageType"] = "send";
            j["data"] = line_in;

            dataToSend = j.dump();

            waitingOnServer = true;
        }
        else{
            std::cout<<"Invalid command\n"<<std::endl;
        }

        Sleep(5);
        res = send(sock,dataToSend.c_str(),dataToSend.length() + 1,0);

        if(res==0)
        {
            //0==other side terminated conn
            printf("\nSERVER terminated connection\n");
            Sleep(40);
            closesocket(client);
            client = 0;
            break;
        }
        else if(res==SOCKET_ERROR)
        {
            //-1 == send error
            printf("Socket error\n");
            Sleep(40);
            s_handle(res);
            break;
        }

        if(waitingOnServer) {
            ret = recv(sock, ReceivedData, sizeof(ReceivedData), 0);
            if (ret > 0) {
                std::cout << "Response from Server:\n" << std::endl;
                std::cout << std::endl << ReceivedData;
                strcpy(ReceivedData, "");
            }
        }

        waitingOnServer = false;
    }

    closesocket(client);
    WSACleanup();
}