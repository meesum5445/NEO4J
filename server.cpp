#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include<iostream>
#include <sstream>
#include<string>
#include <cstdlib> 
#include<vector>
#include<stack>
#include<set>
#include <unordered_map>
#include <winsock2.h>
#include"NEO4J/NEO4J.h"


class CYPHER{
    NEO4J neo; 
    std::unordered_map<std::string,std::vector<unsigned int>>listofvariablesofnode; 
    int client=-1;
    std::vector<unsigned int>ANS;
public:
CYPHER (int cl,unsigned int db)
{
    neo=NEO4J(db);
    this->client=cl;
}
std::vector<unsigned int> vectorUnion(const std::vector<unsigned int>& vector1, const std::vector<unsigned int>& vector2) {
    // Create sets from vectors to automatically eliminate duplicates
    std::set<unsigned int> set1(vector1.begin(), vector1.end());
    std::set<unsigned int> set2(vector2.begin(), vector2.end());

    // Create a set to store the union of unique elements
    std::set<unsigned int> unionSet;

    // Perform union operation
    unionSet.insert(set1.begin(), set1.end());
    unionSet.insert(set2.begin(), set2.end());

    // Convert the set back to a vector
    std::vector<unsigned int> unionVector(unionSet.begin(), unionSet.end());

    return unionVector;
}
void send_vector_of_nodes(std::vector<unsigned int>nodes)
{
    char ch='I';
    send(this->client,(char*)&ch,1,0);
    unsigned int *nodestemp=new unsigned int[nodes.size()];
    
    for(int i=0;i<nodes.size();i++)
    {
        nodestemp[i]=nodes[i];
    }
    if(nodes.size()==0)
    {
        int a=-1;
        send(this->client,(char*)&a,sizeof(int),0);
    }
    else
    {
        send(this->client,(char*)nodestemp,nodes.size()*sizeof(int), 0);
    }  
}
void send_vector_of_characters(std::vector<char>characters)
{
    char ch='C';
    send(this->client,(char*)&ch,1,0);
    char *nodestemp=new char[characters.size()];
    
    for(int i=0;i<characters.size();i++)
    {
        nodestemp[i]=characters[i];
    }
    send(this->client,nodestemp, characters.size(), 0);
}
std::string getMode(const std::string &input) {
    int i=-1;
    while(input[i++]!=' ')
    {
        if(i==input.size())
        {
            i++;
            break;
        }
    }

    return input.substr(0,i-1);
}
std::string getCypher(const std::string &input)
{
    int i=0;
    while(input[i++]!=' ');
    return input.substr(i);
}
int selection(std::string a)
{
    if(a[0]=='(')
        return 1;
    else if(a[0]=='-')
        return 2;
    else
        return 3; 
}
void extract_attributes(std::string attributesCypher,std::vector<std::pair<std::string,std::string>>&attributes)
{
    int i=0;
    while(attributesCypher[i]!='}')
    {
        std::pair<std::string,std::string>temp;
        while(attributesCypher[++i]!=':')
        {
            temp.first+=attributesCypher[i];
        }
        while((++i) && attributesCypher[i]!=',' && attributesCypher[i]!='}')
        {
            temp.second+=attributesCypher[i];
        }
        attributes.push_back(temp);
    }
}
void extractinfofromcyphersegment(std::string segment,std::string &variable,std::string &family,std::vector<std::pair<std::string,std::string>>&attributes)
{
    int i=0;
    while(segment[i]!='(' && segment[i]!='[')i++;
    i++;
    //............................................................
    int start=i;
    int count=0;
    while(segment[i++]!=':')count++;
    variable=segment.substr(start,count);
    //............................................................
    start=i;
    count=0;
    while(segment[i]!=')' && segment[i]!=']' && segment[i]!='{')
    {
        i++;
        count++;
    }
    family=segment.substr(start,count);
    //............................................................
   
    if(segment[i]=='{')
    {
        start=i;
        count=1;
        while(segment[i]!='}')
        {
            i++;
            count++;
        }
        extract_attributes(segment.substr(start,count),attributes);
    }
}
bool isonlyvariable(std::string segment)
{
    int i=0;
    while(i<segment.size())
    {
        if(segment[i++]==':')
            return false;
    }
    return true;
}
void compute_segment(int MODE,std::string segment,std::stack<std::vector<unsigned int>>&computationStack)
{
    std::string variable;
    std::string family;
    std::vector<std::pair<std::string,std::string>>attributes; 
    if(!isonlyvariable(segment))
        extractinfofromcyphersegment(segment,variable,family,attributes);
    if(MODE == 1)
    {
        if(selection(segment)==1)
        {
            if(isonlyvariable(segment))
            {
                computationStack.push(listofvariablesofnode[segment.substr(1,segment.size()-2)]);
            }
            else
            {
                unsigned int temp=neo.CREATENODE(variable,family,attributes);
                if(variable.size()!=0)
                    listofvariablesofnode[variable]={temp};
                computationStack.push({temp});
            }
            
        }
        else if(selection(segment)==2)
        {
            auto temp1=computationStack.top();
            computationStack.pop();
            auto temp2=computationStack.top();
            computationStack.pop();
            neo.CREATERELATION_BETWEEN_LIST(temp1,temp2,family,attributes);  //here family is relation
            computationStack.push(temp1);
        }
        else 
        {
            auto temp1=computationStack.top();
            computationStack.pop();
            auto temp2=computationStack.top();
            computationStack.pop();
            neo.CREATERELATION_BETWEEN_LIST(temp2,temp1,family,attributes);  //here family is relation
            computationStack.push(temp2);
        }
    }
    else if(MODE ==2)
    {
         if(selection(segment)==1)
        {
            if(isonlyvariable(segment))
            {
                computationStack.push(listofvariablesofnode[segment.substr(1,segment.size()-2)]);
            }
            else
            {
                auto temp=neo.search(variable,family,attributes);
                if(variable.size()!=0)
                    listofvariablesofnode[variable]=temp;
                computationStack.push(temp);
            }
            
        }
        else if(selection(segment)==2)
        {
            auto temp1=computationStack.top();
            computationStack.pop();
            for(auto t2:temp1)
                std::cout<<t2<<",";
            std::cout<<"\n";
            auto temp2=computationStack.top();
            computationStack.pop(); 
            for(auto t2:temp1)
                std::cout<<t2<<",";
            std::cout<<"\n";
            computationStack.push(neo.As_related_to_Bs(temp1,temp2,family,attributes));  //here family is relation

        }
        else 
        {
            std::cout<<"3rdoption";
            auto temp1=computationStack.top();
            computationStack.pop();
             for(auto t2:temp1)
                std::cout<<t2<<",";
            std::cout<<"\n";
            auto temp2=computationStack.top();
            computationStack.pop(); 
            for(auto t2:temp2)
                std::cout<<t2<<",";
            std::cout<<"\n";
            computationStack.push(neo.Bs_related_by_As(temp2,temp1,family,attributes));  //here family is relation
        }
        
    }
    else if(MODE == 3)
    {
         if(selection(segment)==1)
        {
            if(isonlyvariable(segment))
            {
                computationStack.push(listofvariablesofnode[segment.substr(1,segment.size()-2)]);
            }
            else
            {
                auto temp=neo.search(variable,family,attributes);
                if(temp.size()!=0)
                {
                    computationStack.push(temp);
                    if(variable.size()!=0)
                        listofvariablesofnode[variable]=temp;
                }
                else
                {
                    unsigned int temp=neo.CREATENODE(variable,family,attributes);
                    computationStack.push({temp});
                    if(variable.size()!=0)
                        listofvariablesofnode[variable]={temp};

                }               
            }
            
        }
        else if(selection(segment)==2)
        {
            auto temp1=computationStack.top();
            computationStack.pop();
            auto temp2=computationStack.top();
            computationStack.pop(); 
            auto temp=neo.As_related_to_Bs(temp1,temp2,family,attributes);
            if(temp.size()!=0)
            {
                computationStack.push(temp);  //here family is relation
            }
            else
            {
                neo.CREATERELATION_BETWEEN_LIST(temp1,temp2,family,attributes);  //here family is relation
                computationStack.push(temp1);
            }
            
        }
        else 
        {
            auto temp1=computationStack.top();
            computationStack.pop();
            auto temp2=computationStack.top();
            computationStack.pop(); 
            auto temp=neo.Bs_related_by_As(temp2,temp1,family,attributes);
            if(temp.size()!=0)
            {
                computationStack.push(temp);  //here family is relation
            }
            else
            {
                neo.CREATERELATION_BETWEEN_LIST(temp2,temp1,family,attributes);  //here family is relation
                computationStack.push(temp2);
            }
           

        }
    }
    else if(MODE==4)
    {
        if(selection(segment)==1)
        {
            if(isonlyvariable(segment))
            {
                computationStack.push(listofvariablesofnode[segment.substr(1,segment.size()-2)]);
            }
            else
            {
                auto temp=neo.search(variable,family,attributes);
                if(variable.size()!=0)
                    listofvariablesofnode[variable]=temp;
                computationStack.push(temp);
            }
        }
        else if(selection(segment)==2)
        {
            auto temp1=computationStack.top();
            computationStack.pop();
            std::cout<<"\nTEMP1 : ";
            for(auto t2:temp1)
                std::cout<<t2<<",";
            std::cout<<"\n";
            auto temp2=computationStack.top();
            computationStack.pop(); 
            std::cout<<"\nTEMP2 : ";
            for(auto t2:temp2)
                std::cout<<t2<<",";
            std::cout<<"\n";
            this->neo.remove_edges(temp1,temp2,family,attributes);
            computationStack.push({});
        }
        else if(selection(segment)==3)
        {
            auto temp1=computationStack.top();
            computationStack.pop();
            auto temp2=computationStack.top();
            computationStack.pop(); 
            this->neo.remove_edges(temp2,temp1,family,attributes);
            computationStack.push({});
        }
    }
    
}
std::stack<std::string> convertcypherintostack(std::string cypher)
{
    std::stack<std::string>cypherPostFix;
    //......................................................................................
    for(int i=0;i<cypher.size();i++)
    {
        if(cypher[i]=='(')
        {
            int start=i;
            int count=2;
            while(cypher[++i]!=')')count++;
            cypherPostFix.push(cypher.substr(start,count));
        }
        else if(cypher[i]=='-')
        {
            int start=i;
            int count=2;
            while(cypher[++i]!='>')count++;
            
            auto temp=cypherPostFix.top();
            cypherPostFix.pop();
            cypherPostFix.push(cypher.substr(start,count));
            cypherPostFix.push(temp);
        }
        else if(cypher[i]=='<')
        {
            int start=i;
            i++;
            int count=3;
            while(cypher[++i]!='-')count++;
            auto temp=cypherPostFix.top();
            cypherPostFix.pop();
            cypherPostFix.push(cypher.substr(start,count));
            cypherPostFix.push(temp);
        }
    }
    return cypherPostFix;
}
std::vector<unsigned int> computecypher(int MODE,std::stack<std::string>cyphersegments)
{
    std::stack<std::vector<unsigned int>>computationStack;
    while(!cyphersegments.empty())
    {
        std::cout<<cyphersegments.top()<<",";
        compute_segment(MODE,cyphersegments.top(),computationStack);
        cyphersegments.pop();
    }

    listofvariablesofnode["ANS"]=computationStack.top();
    return this->ANS=computationStack.top();
} 

    public: 
    void run_cypher(std::string cypher)
    {
        std::cout<<"COMMAND TO BE PROCESSED "<<cypher<<"\n";
            if(cypher =="-1")
                return;
            if(getMode(cypher)=="READNODECSV")
            {
                   neo.readCSVNodes(getCypher(cypher));
                   send_vector_of_nodes({});
            }
            else if(getMode(cypher)=="READRELATIONCSV")
            {
                   neo.readCSVrelations(getCypher(cypher));
                   send_vector_of_nodes({});
            }
            else if(getMode(cypher)=="CREATE")
            {
                send_vector_of_nodes(computecypher(1,convertcypherintostack(getCypher(cypher))));
            }
            else if(getMode(cypher)=="MATCH")
            {
                send_vector_of_nodes(computecypher(2,convertcypherintostack(getCypher(cypher))));
            }
            else if(getMode(cypher)=="MERGE")
            {
                send_vector_of_nodes(computecypher(3,convertcypherintostack(getCypher(cypher))));
            }
            else if(getMode(cypher)=="REMOVENODE")
            {
                neo.delete_nodes(computecypher(2,convertcypherintostack(getCypher(cypher))));
                send_vector_of_nodes({});
            }
            else if(getMode(cypher)=="REMOVEEDGE")
            {
                computecypher(4,convertcypherintostack(getCypher(cypher)));
                std::cout<<"\nREMOVAL DONE SUCCESSFULLY\n";
                send_vector_of_nodes({});
            }
            else if(getMode(cypher)=="INFO")
            {
                send_vector_of_characters(this->neo.get_attributes_of(stoi(getCypher(cypher))));
            }
            else if(getMode(cypher)=="ANS")
            {
                send_vector_of_nodes(this->ANS);
            }
            else if(getMode(cypher)=="VARIABLE")
            {
                this->listofvariablesofnode[getMode(getCypher(cypher))]=listofvariablesofnode[getCypher(getCypher(cypher))];
                send_vector_of_nodes({});
            }
            else if(getMode(cypher)=="OR")
            {
                auto v1 =listofvariablesofnode[getMode(getCypher(getCypher(cypher)))];
                auto v2 =listofvariablesofnode[getCypher(getCypher(getCypher(cypher)))];
                this->listofvariablesofnode[getMode(getCypher(cypher))]=vectorUnion(v1,v2);
                send_vector_of_nodes({});
            }
            else
            {
                send_vector_of_nodes(listofvariablesofnode[getMode(cypher)]);
            }
        
    }
};
void dealwithclient(int clientSocket)
{
    std::cout<<"STARTED TO WORK WITH : "<<clientSocket;
    while(1)
    {
        char buff[255];
        recv(clientSocket,buff, sizeof(unsigned int), 0);
        if(*(int*)buff==-1)
        {
            //close(clientSocket);
            return;
        }
        CYPHER cp(clientSocket,*(int*)buff);
        //client connected with database
        while (1)
        {
            buff[recv(clientSocket, buff, sizeof(buff), 0)] = '\0';
            std::string cypher(buff); 
            if(cypher=="C")
            {
                break;
            }
            cp.run_cypher(cypher);
        } 
    }
}
DWORD WINAPI dealwithclientThreadWrapper(LPVOID lpParam) {
    dealwithclient(*(int*)lpParam);
    return 0;
}

int main()
{ 
    // CYPHER cp;
    // cp.run_cypher();
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return 1;
    }
    //.....................................................
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating server socket." << std::endl;
        return 1;
    }
    //....................................................
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8000); // Port number (you can choose any available port)
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    int serverAddressSize= sizeof(serverAddr);
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr))==-1) {
        std::cerr << "Error binding socket." << std::endl;
        return 1;
    }
    //.............................
    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Error listening for connections." << std::endl;
        return 1;
    }

    while (1)
    {
        Sleep(1000);
        std::cout << "Server is listening for connections..." << std::endl;
        int clientListeningSocket = accept(serverSocket, (sockaddr*)&serverAddr, &serverAddressSize);
        if (clientListeningSocket < 0) {
            continue;
        }
        std::cout << "Connection with client is successful..." << std::endl;
        CreateThread(NULL, 0, dealwithclientThreadWrapper, &clientListeningSocket, 0, NULL);
       
    }
    std::cout << "Hello World!\n";
    WSACleanup();
}