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
#define BUFFSIZE    1024
#define CLIENTQUEUE 5


//简单的SOCKET服务器程序
//非阻塞的方式

int test02()
{
    int fd_Connected[CLIENTQUEUE];
    int currentConnectedCount;
    int sock,length;
    struct sockaddr_in server;
    char buf[BUFFSIZE];
    int rval,len;
    unsigned long sockBlockFlag = 1;     //设置非阻塞时使用


    printf("the server is beginning!\n");

    WSADATA wsaData;
    int iResult = WSAStartup( MAKEWORD(1, 2), &wsaData );
    if( iResult != NO_ERROR )
    {
        printf( "Error at WSAStartup()\n" );
    }

    printf("1 load the lib success!\n");

    sock = socket(AF_INET,SOCK_STREAM,0);
    if (sock <0)
    {
        WSACleanup();
        perror("opening stream socket");
        exit(1);
    }

    printf("2 socket create success!\n");

    //设置sock为非阻塞
    //fcntl(sock,F_GETFL,0);
    ioctlsocket(sock,FIONBIO,&sockBlockFlag);
    printf("  set the socket is noblocking!\n");


    server.sin_family = AF_INET;
    server.sin_port = htons(SERVERPORT);
    server.sin_addr.s_addr=htonl(INADDR_ANY);

    memset(&(server.sin_zero),0,8);  //等价于 bzero(&(server.sin_zero),8);


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

    printf("3 bind success ! the server info: %s %d \n" ,inet_ntoa(server.sin_addr), ntohs(server.sin_port));

    if (listen(sock,10) == SOCKET_ERROR)
    {
        perror("listen the socket");
        exit(1);
    }

    printf("4 listen is success !\n");


    int x = 1;

    struct timeval tv;
    tv.tv_sec=20;
    tv.tv_usec=0;
    fd_set writefds;
    FD_ZERO(&writefds);
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sock, &readfds);
    len = sizeof(struct sockaddr);
    do
    {
        //readfds = writefds;
        int selectResult = select(0,&readfds,&writefds,NULL,&tv);
        if (selectResult >0)
        {
            //read the data
            for(int i=0;i<readfds.fd_count;i++)
            {
                if (readfds.fd_array[i] == sock)
                {
                    if (readfds.fd_count < FD_SETSIZE)
                    {
                        struct sockaddr_in clientAddr;
                        SOCKET clientSocket = accept(sock,(struct sockaddr *)&clientAddr,&len);
                        if (clientSocket == INVALID_SOCKET)
                        {
                            printf("  accept failed: %d\n", WSAGetLastError());
                            printf("  client ip is :%s \n",inet_ntoa(clientAddr.sin_addr));
                            continue;
                        }
                        else
                        {
                            FD_SET(clientSocket,&readfds);
                            printf("5 accept  successed ! clientSocket number is %d \n",clientSocket);
                            printf("  client ip is :%s \n",inet_ntoa(clientAddr.sin_addr));
                        }
                    }
                    else
                    {
                        printf("  connected number is more ! the max number is %d" ,FD_SETSIZE);
                        continue;
                    }
                }
                else if (readfds.fd_array[i] != 0 )
                {
                    //接收数据
                    do
                    {

                        memset(buf,0,BUFFSIZE);
                        rval = recv(readfds.fd_array[i],buf,BUFFSIZE,0);
                        if (rval == SOCKET_ERROR)
                        {
                            if (WSAGetLastError() != WSAEWOULDBLOCK)
                            {
                                printf("  client socket【%d】is closed by exception!",readfds.fd_array[i]);
                                printf("  or recv() Failed,Error=【%d】\n",WSAGetLastError());
                                closesocket(readfds.fd_array[i]);
                                FD_CLR(readfds.fd_array[i],&readfds);
                            }
                            else
                            {
                                rval = 1;
                            }
                        }
                        else if (rval >0)
                        {
                            printf("receive the clientsocket 【%d】data：%s \n",readfds.fd_array[i],buf);
                        }
                        else
                        {
                            printf( "  client socket【%d】will close!\n" , readfds.fd_array[i]);
                            closesocket(readfds.fd_array[i]);
                            FD_CLR(readfds.fd_array[i],&readfds);
                        }
                    }while(rval > 0);
                    //发送数据
                    //send()

                }
            }
        }
        //超时
        else if ( selectResult == 0 )
        {
            printf( "第【%d】次超时\n" , x);
            if  (x< 3)
            {
                x++;
                continue ;
            }
            else
            {
                printf( "超过等待限制，退出程序\n" );
                break ;
            }
        }
        //失败
        else
        {
            printf( "select() Failed,Error=【%d】\n" , WSAGetLastError());
            break ;
        }
    }while(TRUE);
    closesocket(sock);
    printf("close the application!");
    WSACleanup();
    return 0;
}
