#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <winsock2.h>
#include <stdio.h>
#include <winsock2.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "Parameters.h"
#include "fileReadWite.h"
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <winsock2.h>


#define PORT 3490
#define IP "127.0.0.1"
#define BACKLOG 10

const int winsockversion = 2;

int numListeners = 0;

SOCKET serverSocket;		//This is the socket we will connect to.
SOCKET createSocket(int port);
fd_set master;				//File descriptor storing all the sockets.

char username[100];
int Ports[100];
int UsersPort[100];
BOOL recvThreadRunning;
BOOL is_nameset = FALSE;
char listenerIPAddress[15];
int listenerPort;
SOCKET listeningSocket;

SOCKET NodeServerSocket[100];		//This is the socket we will connect to.
int numClients = 0;


struct CLIENT_INFO
{
    SOCKET hClientSocket ;
    struct sockaddr_in clientAddr ;
} ;

char szServerIPAddr[ ] = "127.0.0.1" ;     // Put here the IP address of the server
const char sendFileSignal[10] = "SendFile";            // clients to talk with the server;

BOOL InitWinSock2_0( ) ;
BOOL WINAPI ClientThread( LPVOID lpData ) ;


BOOL initWinsockClient();

void threadRecvInClients(int k);


void mainClient();

void createAndConnectSocket(int port);
BOOL initWinsockClient();

void setPortList(char stri[4096]);
void addPort(int port);
void dispPortList();
void setServerPort();
SOCKET createSocket(int _port);

void fnRunServer(int port);
void getInputMessage(char buf[MaxBufferSize], SOCKET sock);

void createOwnServer() ;
DWORD WINAPI chatClientThread(void* data);
DWORD WINAPI chatServerThread(void* data);

void sendToAll(char msgSent[MaxStrLen]);

void sendMsgToMainServer(char txt[MaxStrLen]);
void sendToAllUsers(char pBuffer[300]);

int mainServer( );

void dispRecivedBuffer(char stri[300]);
void fnTypedString(char str[300]);
BOOL startsWith(const char *pre, const char *str);
BOOL addListinerPort(int port);
void sendToUser(int k ,char pBuffer[10000]);

int main(){

    setServerPort();

    createOwnServer();

    mainClient();

    while (TRUE) {
        char str[264];

        (!is_nameset) ? printf("\nEnter your Name : ") : printf("\n>>");
        gets(str);
        (!is_nameset)?  strcpy(username, str): fnTypedString(str);

        is_nameset = TRUE;
    }
    return 0;
}
//===Display==========
void dispRecivedBuffer(char stri[300]){
    printf_s("\b\b\b\b\b%s\n",stri);

}
void fnTypedString(char str[300]){
    char str1[1000];
    if(strcmp(str,"portlist")==0||strcmp(str,"PORTLIST")==0){
        dispPortList();
        return;
    }
    if(strcmp(str,"adduser")==0||strcmp(str,"addUser")==0||strcmp(str,"ADDUSER")==0){
        dispPortList();
        printf("\nEnter User Port:");
        int port;
        scanf ("%d",&port);
        addPort(port);
        return;
    }
    if(strcmp(str,"sendfile")==0||strcmp(str,"Sendfile")==0||strcmp(str,"SendFile")==0){
        printf("\nEnter File Name Like: c://1.txt :\n");
        char filename[1000];
        gets (filename);

        char FileBufer[10000];

        FILE * stream;
        stream = fopen(filename, "rb");
        fseek(stream, 0, SEEK_END);
        long fileLen=ftell(stream);
        fseek(stream, 0, SEEK_SET);

        fread(FileBufer,1, fileLen+1, stream);
        fclose(stream);
        printf("%s",FileBufer);

        for(int k=0;k<numListeners;k++){
            send(UsersPort[k], sendFileSignal, strlen(sendFileSignal)+1, 0 );
            //sleep(200);
            send(UsersPort[k], FileBufer, sizeof (FileBufer)+1, 0 );
        }

        printf("\nsending File %s",filename);
        //dispPortList();
        return;
    }

    sprintf(str1, "\n%s : %s", username, str);
    printf_s(str1);
    sendToAllUsers(str1);


};

//==============
BOOL startsWith(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
            lenstr = strlen(str);
    return lenstr < lenpre ? FALSE : memcmp(pre, str, lenpre) == 0;
}

int mainServer( )
{
    if ( ! InitWinSock2_0( ) )
    {
        printf("Unable to Initialize Windows Socket environment %d ", WSAGetLastError( ) );
        return -1 ;
    }

    SOCKET hServerSocket ;

    hServerSocket = socket(
                AF_INET,        // The address family. AF_INET specifies TCP/IP
                SOCK_STREAM,    // Protocol type. SOCK_STREM specified TCP
                0               // Protoco Name. Should be 0 for AF_INET address family
                ) ;
    if ( hServerSocket == INVALID_SOCKET )
    {
        printf("\nUnable to create Server socket") ;
        // Cleanup the environment initialized by WSAStartup()
        WSACleanup( ) ;
        return -1 ;
    }

    // Create the structure describing various Server parameters
    struct sockaddr_in serverAddr ;

    serverAddr . sin_family = AF_INET ;     // The address family. MUST be AF_INET
    serverAddr . sin_addr . s_addr = inet_addr( szServerIPAddr ) ;
    serverAddr . sin_port = htons( myServerPort ) ;

    // Bind the Server socket to the address & port
    if ( bind( hServerSocket, ( struct sockaddr * ) &serverAddr, sizeof( serverAddr ) ) == SOCKET_ERROR )
    {
        printf("\nUnable to bind to %s %d" , szServerIPAddr , myServerPort ) ;
        // Free the socket and cleanup the environment initialized by WSAStartup()
        closesocket( hServerSocket ) ;
        WSACleanup( ) ;
        return -1 ;
    }

    // Put the Server socket in listen state so that it can wait for client connections
    if ( listen( hServerSocket, SOMAXCONN ) == SOCKET_ERROR )
    {
        printf("\nUnable to put server in listen state" );
        // Free the socket and cleanup the environment initialized by WSAStartup()
        closesocket( hServerSocket ) ;
        WSACleanup( ) ;
        return -1 ;
    }

    // Start the infinite loop
    while ( TRUE )
    {
        // As the socket is in listen mode there is a connection request pending.
        // Calling accept( ) will succeed and return the socket for the request.
        SOCKET hClientSocket ;
        struct sockaddr_in clientAddr ;
        int nSize = sizeof( clientAddr ) ;

        hClientSocket = accept( hServerSocket, ( struct sockaddr *) &clientAddr, &nSize ) ;
        if ( hClientSocket == INVALID_SOCKET )
        {
            printf("accept( ) failed") ;
        }
        else
        {
            HANDLE hClientThread ;
            struct CLIENT_INFO clientInfo ;
            DWORD dwThreadId ;

            clientInfo . clientAddr = clientAddr ;
            clientInfo . hClientSocket = hClientSocket ;

            // Start the client thread

            hClientThread = CreateThread( NULL, 0,
                                          ( LPTHREAD_START_ROUTINE ) ClientThread,
                                          ( LPVOID ) &clientInfo, 0, &dwThreadId ) ;
            if ( hClientThread == NULL )
            {
                printf("Unable to create client thread") ;
            }
            else
            {
                CloseHandle( hClientThread ) ;
            }
        }
    }

    closesocket( hServerSocket ) ;
    WSACleanup( ) ;
    return 0 ;
}

BOOL InitWinSock2_0( )
{
    WSADATA wsaData ;
    WORD wVersion = MAKEWORD( 2, 0 ) ;

    if ( ! WSAStartup( wVersion, &wsaData ) )
        return TRUE ;

    return FALSE ;
}
void sendToAllUsers(char pBuffer[300]){

    for(int k=0;k<numListeners;k++){

        send(UsersPort[k], pBuffer, strlen(pBuffer)+1, 0 );
    }
}
void sendToUser(int k ,char pBuffer[10000]){

    if (k>=numListeners)return;

    send(UsersPort[k], pBuffer, sizeof (pBuffer)+1, 0 );

}
BOOL addListinerPort(int port){
    int index = -1;

    for (int n=0;n<100;n++){
        if(UsersPort[n]==port)index = n;
    }
    if(index == -1){
        UsersPort[numListeners] = port;
        numListeners++;
        return TRUE;
    }

    return FALSE;
}
BOOL WINAPI ClientThread( LPVOID lpData )
{
    struct CLIENT_INFO *pClientInfo = (struct  CLIENT_INFO * ) lpData ;
    char szBuffer[ 1024 ] ;
    int nLength ;
    int _p = pClientInfo -> hClientSocket;
    if (!addListinerPort(_p))return -1;
    while ( 1 )
    {
        nLength = recv( pClientInfo -> hClientSocket, szBuffer, sizeof( szBuffer ), 0 ) ;
        if ( nLength > 0 )
        {
            szBuffer[ nLength ] = '\0' ;
            printf("\b\b\b\r\r\r\r\r\r\r\r%s\n>>", szBuffer) ;
            char welcome[200] ="\nYs";
            send(pClientInfo -> hClientSocket, welcome, sizeof(welcome)+1, 0);
            // Convert the string to upper case and send it back, if its not QUIT
            strupr( szBuffer ) ;
            if ( strcmp( szBuffer, "QUIT" ) == 0 )
            {
                closesocket( pClientInfo -> hClientSocket ) ;
                return TRUE ;
            }
            // send( ) may not be able to send the complete data in one go.
            // So try sending the data in multiple requests
            int nCntSend = 0 ;
            char *pBuffer = szBuffer ;

            while ( ( nCntSend = send( pClientInfo -> hClientSocket, pBuffer, nLength, 0 ) != nLength ) )
            {
                if ( nCntSend == -1 )
                {
                    break ;
                }
                if ( nCntSend == nLength )
                    break ;

                pBuffer += nCntSend ;
                nLength -= nCntSend ;
            }
        }
        else
        {
            //printf("Error reading the data from %s " , inet_ntoa( pClientInfo -> clientAddr . sin_addr ) );
        }
    }

    return TRUE ;
}
//=========================
void mainClient() {
    if (initWinsockClient()) {
        addPort(mainServerPort);
        char strmyserverport[1000];
        sprintf(strmyserverport, "%d", myServerPort);
        sendMsgToMainServer(strmyserverport);
    }
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
    mainServer();
    //fnRunServer(myServerPort);
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
    inet_pton(AF_INET, serverIP, &_hint.sin_addr);

    //If !initWinsock -> return false.
    int connResult = connect(NodeServerSocket[numClients], (struct sockaddr*)&_hint, sizeof(_hint));
    if (connResult == SOCKET_ERROR) {
        printf("\nError: can't connect to server.");
        closesocket(NodeServerSocket[numClients]);
        WSACleanup();
        return;
    }
    Ports[numClients] = _port;
    int k = numClients;numClients++; void * u =(void *) &k;

    CreateThread(NULL, 0, chatClientThread,u , 0, NULL);
    Sleep(100);

}
void sendMsgToMainServer(char txt[MaxStrLen]) {

    if ((sizeof(txt) != 0) && NodeServerSocket[0] != INVALID_SOCKET) {
        send(NodeServerSocket[0], txt, strlen(txt) + 1, 0);
    }

}
void threadRecvInClients(int k) {
    //printf("\n Line 355::add Port %d",k);
    BOOL isFile = FALSE;
    while(TRUE) {

        char buf[40960];
        char stri[4096];
        ZeroMemory(buf, 40960);
        int bytesReceived = recv(NodeServerSocket[k], buf, 4096, 0);
        if (bytesReceived > 0) {			//If client disconnects, bytesReceived = 0; if error, bytesReceived = -1;
            if (k == 0) {
                strncpy_s(stri, bytesReceived+1, buf,5000);
                setPortList(stri);
            }
            else {
                if(isFile){
                    isFile = FALSE;
                    time_t current_time;
                    struct tm * time_info;
                    char timeString[19];  // space for "HH:MM:SS\0"

                    time(&current_time);
                    time_info = localtime(&current_time);

                    strftime(timeString, sizeof(timeString), "%H_%M_%S", time_info);
                    printf("%s",timeString);
                    char wfilename[100] = "d://3.txt";
                    sprintf(wfilename,"D://%s_%s.txt",timeString,username);
                    writeTextFileFromBuffer(wfilename,buf,bytesReceived);
                }else{
                    strncpy_s(stri,  bytesReceived+1,buf,5009);
                    if(strcmp(stri,sendFileSignal)==0){
                        isFile = TRUE;
                        continue;
                    }
                    dispRecivedBuffer(stri);

                }
            }
        }
    }
}

void setPortList(char stri[4096]) {
    char delim = ',';
    char strNum[10] = "";
    int n = 0;
    int port;
    for (int k = 10; k < (int)strlen(stri); k++) {
        if ( (stri[k] == delim)||(k == (int)strlen(stri))) {
            port = atoi(strNum);
            addPort( port);
            n = 0;
            strcpy(strNum, "");
        }else {
            strNum[n]= stri[k];
            n++;
        }
    }

}
void addPort(int port) {
    for (int k = 0; k < numClients; k++)
        if (Ports[k] == port)
            return;
    if(port == myServerPort)return;


    Sleep(100);

    createAndConnectSocket(port);
    Sleep(200);
    //dispPortList();
}
void dispPortList(){
    printf("\n name:%s serverPort : %d",username,myServerPort);
    printf("\nPorts:\n");
    for(int k=0;k<numClients;k++){
        printf("%d,",Ports[k]);
    }
    printf("\nUserPort:\n");
    for(int k=0;k<numClients;k++){
        printf("%d,",UsersPort[k]);
    }


    printf("\n>>");
}
void setServerPort() {
    srand(time(NULL));   // Initialization, should only be called once.
    int r = 2000+(rand()% 45350);      // Returns a pseudo-random integer between 0 and RAND_MAX.
    myServerPort = r;
    for (int k = 0; k < 100; k++){
        Ports[k] = 0;
        UsersPort[k] = 0;
    }
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

            const PTIMEVAL up =NULL;
            select(0, &copy,(fd_set*)0, (fd_set*)0 ,up);				//Select() determines status of sockets & returns the sockets doing "work".

            SOCKET sock = copy.fd_array[0];				//Loop through all the sockets in the file descriptor, identified as "active".
            if (sock == listeningSocket) {				//Case 1: accept new connection.
                struct sockaddr *sa = NULL;
                int *al = NULL;
                SOCKET client = accept(listeningSocket,sa, al);		//Accept incoming connection & identify it as a new client.
                FD_SET(client, &master);		//Add new connection to list of sockets.
                char str0[100] = "HIIIIIII\n";
                getInputMessage(str0, sock);

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
    if(bytesReceived>0){
        char str_in[MaxStrLen];
        strncpy(str_in, buf, bytesReceived+1);			//Log the message on the server side.
        printf("----------%s\n>>",str_in);
        //Send msg to other clients & not listening socket.
        for (int i = 0; i < master.fd_count; i++) {			//Loop through the sockets.
            SOCKET outSock = master.fd_array[i];
            if (outSock != listeningSocket) {
                if (outSock == sock) {
                    //char msgSent[1000] = "\nHIII8989>>";
                    //send(outSock, msgSent, sizeof(msgSent) + 1, 0);
                }
                else {
                    send(outSock, buf, bytesReceived, 0);
                }
            }
        }

    }
}
SOCKET createSocket(int _port) {
    SOCKET listeningSocket = socket(AF_INET, SOCK_STREAM, 0);	//AF_INET = IPv4.

    struct sockaddr_in hint;		//Structure used to bind IP address & port to specific socket.
    hint.sin_family = AF_INET;		//Tell hint that we are IPv4 addresses.
    hint.sin_port = htons(_port);	//Tell hint what port we are using.
    inet_pton(AF_INET, serverIP, &hint.sin_addr); 	//Converts IP string to bytes & pass it to our hint. hint.sin_addr is the buffer.
    int bindCheck = bind(listeningSocket, (struct sockaddr*)&hint, sizeof(hint));	//Bind listeningSocket to the hint structure. We're telling it what IP address family & port to use.
    int listenCheck = listen(listeningSocket, SOMAXCONN);	//Tell the socket is for listening.
    //printf("server socket created:  %d\n", _port);
    return listeningSocket;
}

void sendToAll(char msgSent[MaxStrLen]) {
    char str1[100]="c:";
    char str0[100];
    BOOL isfile = strncmp(str1, msgSent,5)==0;

    for (int i = 0; i < master.fd_count; i++) {			//Loop through the sockets.
        SOCKET outSock = master.fd_array[i];
        if (outSock != listeningSocket) {
            send(outSock, msgSent, strlen(msgSent) + 1, 0);
        }
    }


    if( strlen(msgSent)>20000 ){
        if((msgSent[0]=='c'&&msgSent[1]==':')||(msgSent[0]=='d'&&msgSent[1]==':') ){
            printf("\n sending file : ");
            unsigned char fileBuffer[MaxFileSize];
            int filesize = setBufferFromFile(msgSent,fileBuffer);
            for (int i = 0; i < master.fd_count; i++) {			//Loop through the sockets.
                SOCKET outSock = master.fd_array[i];
                if (outSock != listeningSocket) {
                    if (filesize != 1){
                        char str[10]="file";
                        send(outSock, str, strlen(msgSent) + 1, 0);
                        Sleep(100);
                        send(outSock, msgSent, strlen(msgSent) + 1, 0);
                    }
                    printf("\nsending file %s ",msgSent);
                }
            }
        }
    }else{
        //        for (int i = 0; i < master.fd_count; i++) {
        //            SOCKET outSock = master.fd_array[i];
        //            if (outSock != listeningSocket) {
        //                send(outSock, msgSent, strlen(msgSent) + 1, 0);
        //            }
        //        }
    }
}












