#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include<fstream>
#include<string>
#include <winsock2.h>

void print_node_data(char *buff)
{
    std::cout<<"\n.....................................\n";
    int i=0;
    while(buff[i]!='\0')
    {
        if(buff[i]==',')
        {
            std::cout<<"\n";
        }
        else if(buff[i]==':')
        {
            std::cout<<" : ";
        }
        else
        {
            std::cout<<buff[i];
        }
        i++;
    }
    std::cout<<"\n.....................................\n";
}
void print_node(unsigned int nodeId,int serverSocket)
{
    char buff[255];
    std::string cypher="INFO ";
    cypher+=std::to_string(nodeId);
    send(serverSocket,(char*)cypher.data(), cypher.size(), 0);
    recv(serverSocket,buff,sizeof(buff), 0);        //garbage reciever;
    buff[recv(serverSocket,buff,sizeof(buff), 0)]='\0';
    print_node_data(buff);
    
}
void client_program()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return;
    }
    //.....................................................
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket <0) {
        std::cerr << "Error creating server socket." << std::endl;
        return;
    }
    //....................................................
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8000); // Port number (you can choose any available port)
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    //.............................
    if (connect(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Error listening server" << std::endl;
        return;
    }
    std::cout << "Successfully Connected\n";
    while (1)
    {
        char buff[255];
        {
            std::cout<<"Which database you wanna work with : ";
            std::cin>>*(int*)buff;
            if(*(int*)buff==-1)
            {
                send(serverSocket,buff,sizeof(int), 0);
                return;
            }
            send(serverSocket,buff, sizeof(int), 0);
            
            std::string input;
            std::cout<<"START WRITING CYPHER \n";    
            std::cin.clear();
            std::getline(std::cin, input);   
            while (true)
            {
                std::cin.clear();
                std::getline(std::cin, input);   
                if(input =="-1")
                {
                   send(serverSocket,"C", 1, 0);
                   break; 
                }
                send(serverSocket,(char*)input.data(), input.size(), 0);
                recv(serverSocket,buff,sizeof(buff), 0);
                if(*buff=='C')
                {
                    buff[recv(serverSocket,buff,sizeof(buff), 0)]='\0';
                    print_node_data(buff);
                }
                else
                {
                    unsigned int numOfNodesRecieved = recv(serverSocket,buff,sizeof(buff), 0)/4;
                    if(*(int*)buff==-1)
                        continue;
                    for(int i=0;i<numOfNodesRecieved;i++)
                    {
                        print_node(*((int*)&buff+i),serverSocket);
                    }
                }
            }
            
        }

    }
    
}
int main()
{
    client_program();
    std::cout << "Disconnected with NEO server";   
    WSACleanup();
}


