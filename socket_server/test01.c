#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <netdb.h>
#endif

#define TRUE 1
#define SERVERPORT  8802
#define BUFFSIZE    50

//简单的SOCKET服务器程序

int test01()
{
    int sock,length;
    struct sockaddr_in server;
    struct sockaddr tcpaddr;
    int msgsock;
    char buf[BUFFSIZE];
    int rval,len;

    printf("load socke32!\n");

    WSADATA wsaData;
    int iResult = WSAStartup( MAKEWORD(2, 2), &wsaData );
    if( iResult != NO_ERROR )
    {
        printf( "Error at WSAStartup()\n" );
    }



    sock = socket(AF_INET,SOCK_STREAM,0);

    if (sock <0)
    {
        WSACleanup();
        perror("opening stream socket");
        exit(1);
    }
    else
    {
        printf("socket success!");
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVERPORT);
    server.sin_addr.s_addr=htonl(INADDR_ANY);


    if (bind(sock,(struct sockaddr *)&server,sizeof(server)) <0)
    {
        perror("binding stream socket");
        exit(1);
    }

    length = sizeof(server);
    if (getsockname(sock,(struct sockaddr *)&server,&length) <0)
    {
        perror("getting socket name");
        exit(1);
    }

    printf("socket port #%d\n",ntohs(server.sin_port));

    listen(sock,10);

    len = sizeof(struct sockaddr);

    do
    {
        msgsock = accept(sock,(struct sockaddr *)&tcpaddr,(int *)&len);
        printf("get the client request ! \n");
        if (msgsock == -1)
            perror("accept");
        else do
        {
            memset(buf,0,sizeof(buf));
            if((rval =recv(msgsock,buf,BUFFSIZE,0)) <0 )
                perror("reading stream message");
            if(rval == 0)
                printf("ending connetion \n");
            else
			{
				printf("----get the bytes number is %d ----\n",rval);
				printf("%s\n",buf);
			}
        } while (rval != 0);
        closesocket(msgsock);
    }while(TRUE);

    WSACleanup();
    return 0;
}
