/*
 * TCP Stream Socket Client
 * 这个例子说明了如何实现一个Winsock的客户端。
 * 它做的工作非常简单，负责发送一条消息到服务器，并且接收从服务器的返回信息。
 * 如果指定服务器名为"localhost"，这个程序和TCP Stream Socket Server这个例子运行在同一台机器上。
 * 也可以通过指定其他服务器名来在不同的机器上工作。
*/
#include <windows.h> 
#include <winsock.h>

#define PORTNUM         5000          // 端口号
#define HOSTNAME        "localhost"   // 服务器名字
                                      // 根据服务器的不同，更改上面的名字
                                      
int WINAPI WinMain (
              HINSTANCE hInstance,    // 实例句柄
              HINSTANCE hPrevInstance,// NULL
              LPTSTR lpCmdLine,       // 用于运行程序的命令行
              int nCmdShow)           // 程序的最初的显示方式
{
  int index = 0,                      // 索引
      iReturn;                        // recv函数的返回值
  char szClientA[100];                // ASCII码串
  TCHAR szClientW[100];               // Unicode串
  TCHAR szError[100];                 // 出错消息串

  SOCKET ServerSock = INVALID_SOCKET; // 服务器的socket
  SOCKADDR_IN destination_sin;        // 服务器的socket地址
  PHOSTENT phostent = NULL;           // 指向服务器的HOSTENT结构
  WSADATA WSAData;                    // 包含了Winsocket执行后的细节

  // 初始化Winsocket. 
  if (WSAStartup (MAKEWORD(1,1), &WSAData) != 0) 
  {
    wsprintf (szError, TEXT("WSAStartup failed. Error: %d"), 
              WSAGetLastError ());
    MessageBox (NULL, szError, TEXT("Error"), MB_OK);
    return FALSE;
  }

  // 创建一个绑定服务器的TCP/IP socket
  if ((ServerSock = socket (AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
  {
    wsprintf (szError, TEXT("Allocating socket failed. Error: %d"), 
              WSAGetLastError ());
    MessageBox (NULL, szError, TEXT("Error"), MB_OK);
    return FALSE;
  }

  // 把服务器的socket的地址信息填入
  destination_sin.sin_family = AF_INET;

  // 通过主机名取得主机信息
  if ((phostent = gethostbyname (HOSTNAME)) == NULL) 
  {
    wsprintf (szError, TEXT("Unable to get the host name. Error: %d"), 
              WSAGetLastError ());
    MessageBox (NULL, szError, TEXT("Error"), MB_OK);
    closesocket (ServerSock);
    return FALSE;
  }

  // 分配socket IP地址
  memcpy ((char FAR *)&(destination_sin.sin_addr), 
          phostent->h_addr, 
          phostent->h_length);

  // 转化端口字段为大终点
  destination_sin.sin_port = htons (PORTNUM);      

  // 建立一个到服务器socket的连接.
  if (connect (ServerSock, 
               (PSOCKADDR) &destination_sin, 
               sizeof (destination_sin)) == SOCKET_ERROR) 
  {
    wsprintf (szError, 
              TEXT("Connecting to the server failed. Error: %d"),
              WSAGetLastError ());
    MessageBox (NULL, szError, TEXT("Error"), MB_OK);
    closesocket (ServerSock);
    return FALSE;
  }

  // 向服务器发送信息
  if (send (ServerSock, "To Server.", strlen ("To Server.") + 1, 0)
           == SOCKET_ERROR) 
  {
    wsprintf (szError, 
              TEXT("Sending data to the server failed. Error: %d"),
              WSAGetLastError ());
    MessageBox (NULL, szError, TEXT("Error"), MB_OK);
  }

  // 禁止服务器接收信息
  shutdown (ServerSock, 0x01);

  for (;;)
  {
    // 从服务器接收信息
    iReturn = recv (ServerSock, szClientA, sizeof (szClientA), 0);

    // 如果接收到服务器信息，显示之
    if (iReturn == SOCKET_ERROR)
    {
      wsprintf (szError, TEXT("No data is received, recv failed.")
                TEXT(" Error: %d"), WSAGetLastError ());
      MessageBox (NULL, szError, TEXT("Client"), MB_OK);
      break;
    }
    else if (iReturn == 0)
    {
      MessageBox (NULL, TEXT("Finished receiving data"), TEXT("Client"),
                  MB_OK);
      break;
    }
    else
    {
      // 把ASCII码字符串转换成Unicode码字符串
      for (index = 0; index <= sizeof (szClientA); index++)
        szClientW[index] = szClientA[index];

      // 显示从服务器接收到的字符串
      MessageBox (NULL, szClientW, TEXT("Received From Server"), MB_OK);
    }
  }

  // 禁止服务器发送信息
  shutdown (ServerSock, 0x00);

  // 关闭服务器socket.
  closesocket (ServerSock);

  WSACleanup ();

  return TRUE;
}
