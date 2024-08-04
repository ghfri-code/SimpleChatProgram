#ifndef UNICODE
#define UNICODE
#endif


#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "Parameters.h"



SOCKET serverSocket;		//This is the socket we will connect to.
SOCKET createSocket(int port);
fd_set master;				//File descriptor storing all the sockets.

char username[100];
int Ports[100];
char PortsStr[1000] = "";
BOOL recvThreadRunning;
BOOL is_nameset = FALSE;
char listenerIPAddress[15];
int listenerPort;
SOCKET listeningSocket;

SOCKET NodeServerSocket[100];		//This is the socket we will connect to.
int numClients = 0;

BOOL initWinsockClient();
void threadRecvInClients(int k);

void mainClient();
void createAndConnectSocket(int port);
BOOL initWinsockClient();
void setPortList(char stri[4096]);
void setServerPort();
SOCKET createSocket(int _port);
void fnRunServer(int port);
void getInputMessage(char buf[MaxBufferSize], SOCKET sock);
void createOwnServer() ;
DWORD WINAPI chatClientThread(void* data);
DWORD WINAPI chatServerThread(void* data);
void sendToAll(char msgSent[MaxStrLen]);
void setIPStr();
void addPort(char strmyServerPort[]);
int main(){
    createOwnServer();
    while (TRUE) {
        getchar();
    }
    //thread.join();
    return 0;
}


void createOwnServer() {
    HANDLE threadChatServer = CreateThread(NULL, 0, chatServerThread, NULL, 0, NULL);

}
DWORD WINAPI chatClientThread(void* data) {
    int _serverSocket = *(int*)(data);
    threadRecvInClients(_serverSocket);
    return 0;
}
DWORD WINAPI chatServerThread(void* data) {
    fnRunServer(mainServerPort);
    return 0;
}

//########## Client ===========================
BOOL initWinsockClient() {

    WSADATA data;
    WORD ver = MAKEWORD(2, 2);
    int wsResult = WSAStartup(ver, &data);
    if (wsResult != 0) {
        printf("\nError: can't start Winsock.\n");
        return FALSE;
    }
    return TRUE;
}
void createAndConnectSocket(int _port) {
    //printf_s("port %d added\n",_port);
    NodeServerSocket[numClients]  = socket(AF_INET, SOCK_STREAM, 0);
    if (NodeServerSocket[numClients]  == INVALID_SOCKET) {
        printf("\nError: can't create socket.\n");
        WSACleanup();
        //return -1;
    }

    struct sockaddr_in _hint;
    //Specify data for hint structure.
    _hint.sin_family = AF_INET;
    _hint.sin_port = htons(_port);

    int connResult = connect(NodeServerSocket[numClients], (struct sockaddr*)&_hint, sizeof(_hint));
    if (connResult == SOCKET_ERROR) {
        printf("\nError: can't connect to server.");
        closesocket(NodeServerSocket[numClients]);
        WSACleanup();
        return;
    }
    int k = numClients++; void * u =(void *) &k;

    CreateThread(NULL, 0, chatClientThread,u , 0, NULL);
    Sleep(100);

}

void threadRecvInClients(int k) {

    while(TRUE) {

        char buf[4096];
        char stri[4096];
        ZeroMemory(buf, 4096);
        int bytesReceived = recv(NodeServerSocket[k], buf, 4096, 0);
        if (bytesReceived > 0) {
            strncpy_s(stri, bytesReceived+1, buf,5000);
        }
    }
}


void setServerPort() {
    for (int k = 0; k < 100; k++)
        Ports[k] = 0;
}
//============ SERVER =========================
void fnRunServer(int port) {
    strcpy(listenerIPAddress, serverIP);
    listenerPort = port;
    WSADATA data;
    WORD ver = MAKEWORD(2, 2);
    int wsInit = WSAStartup(ver, &data);

    char buf[MaxBufferSize];		//Create the buffer to receive the data from the clients.
    listeningSocket = createSocket(listenerPort);		//Create the listening socket for the server.

    while (TRUE) {
        FD_ZERO(&master);			//Empty file file descriptor.
        FD_SET(listeningSocket, &master);		//Add listening socket to file descriptor.
        while (TRUE) {
            fd_set copy = master;	//Create new file descriptor bc the file descriptor gets destroyed every time.
            int socketCount = select(0, &copy, NULL, NULL, NULL);				//Select() determines status of sockets & returns the sockets doing "work".

            SOCKET sock = copy.fd_array[0];				//Loop through all the sockets in the file descriptor, identified as "active".
            if (sock == listeningSocket) {				//Case 1: accept new connection.
                SOCKET client = accept(listeningSocket, NULL, NULL);		//Accept incoming connection & identify it as a new client.
                FD_SET(client, &master);		//Add new connection to list of sockets.

            }
            else {										//Case 2: receive a msg.
                getInputMessage(buf, sock);
            }
        }
    }
}
void getInputMessage(char buf[MaxBufferSize], SOCKET sock) {
    ZeroMemory(buf, MaxBufferSize);	 //Clear the buffer before receiving data.
    int bytesReceived = recv(sock, buf, MaxBufferSize, 0);	//Receive data into buf & put it into bytesReceived.
    //Send msg to other clients & not listening socket.
    for (int i = 0; i < master.fd_count; i++) {			//Loop through the sockets.
        SOCKET outSock = master.fd_array[i];
        if (outSock != listeningSocket) {
            char strport[100] = "";
            strncpy(strport, buf, bytesReceived);
            addPort(strport);
            send(outSock, PortsStr, strlen(PortsStr) + 1, 0);	//Notify the client that the msg was delivered.


            if (outSock == sock) {
                        //Notify the client that the msg was delivered.
            }
            else {
                //send(outSock, buf, bytesReceived, 0);
            }
        }
    }
    char str_in[MaxStrLen];
    strncpy(str_in, buf, bytesReceived+1);			//Log the message on the server side.
    printf(str_in);
}
SOCKET createSocket(int _port) {
    SOCKET listeningSocket = socket(AF_INET, SOCK_STREAM, 0);	//AF_INET = IPv4.

    struct sockaddr_in hint;		//Structure used to bind IP address & port to specific socket.
    hint.sin_family = AF_INET;		//Tell hint that we are IPv4 addresses.
    hint.sin_port = htons(_port);	//Tell hint what port we are using.
    int bindCheck = bind(listeningSocket, (struct sockaddr*)&hint, sizeof(hint));	//Bind listeningSocket to the hint structure. We're telling it what IP address family & port to use.
    int listenCheck = listen(listeningSocket, SOMAXCONN);	//Tell the socket is for listening.
    printf("server socket created:  %d\n", _port);
    return listeningSocket;
}

void sendToAll(char msgSent[MaxStrLen]) {

    for (int i = 0; i < master.fd_count; i++) {			//Loop through the sockets.
        SOCKET outSock = master.fd_array[i];
            send(outSock, msgSent, strlen(msgSent) + 1, 0);
            printf("\n%s",msgSent);

    }

}

void setIPStr()
{

    char str0[10] = "";
    strcpy(PortsStr, "\nPortList:");
    for (int k = 0; k < numClients; k++) {
        sprintf(str0, "%d,", Ports[k]);
        strcat(PortsStr, str0);
    }

}

void addPort(char strmyServerPort[]) {
    int port = atoi(strmyServerPort);


    for (int k = 0; k < numClients; k++)
        if (Ports[k] == port)
            return;

    Ports[numClients] = port;
    numClients++;
    printf("\n new port : %d \n", port);
    setIPStr();
}
