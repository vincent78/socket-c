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
//返回执行的确认值
/*
    2011-11-10  vincent

    创建服务器监听的SOCKET
    当ACCEPT客户端的请求后。
    建立新的线程
    在新的线程中对客户端的SOCKET进行非阻塞访问
    注册客户端的信息，接收客户端的数据
    按客户端的请求进行数据处理
    将处理结果返回客户端
    注消客户端本次操作
    释放子线程
*/

int test04()
{

    return 0;
}
