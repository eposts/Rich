
/*
 *TCP Stream Socket Server
 *这个例子说明如何实现一个Winsock TCP stream socket服务器。
 *它接收从客户端发过来的消息并且发送一条消息到客户端。 
*/
#include <windows.h> 
#include <winsock.h>
#include "resource.h"
#include "PROCESS.H"

#define PORTNUM               5000    // 端口号
#define MAX_PENDING_CONNECTS  4       // 等待连接队列的最大长度

#define	WM_MESSAGE_REVED	(WM_USER+0)			//接收到数据包消息
#define	WM_MESSAGE_SENDED	(WM_USER+1)			//发送出到数据包消息


//主线程向负责接收和发送的线程传递参数
typedef	struct
{
	HWND	hwnd;					//主线程的窗口句柄
	BOOL	bContinue;				//状态指示参数	
	SOCKET	WinSocket;				//监听socket
	SOCKADDR_IN local_sin,			//本地socket地址   
              accept_sin;			//接收连接的socket地址
}
PARAMS,*PPARAMS;

char szServerA[100];                // ASCII string 
TCHAR szServerW[100];               // Unicode string
TCHAR szError[100];                 // Error message string
TCHAR		szSendBuffer[100];		// 线程间共享缓冲池
SOCKET ClientSock = INVALID_SOCKET; // 服务器和客户端通信的Socket

BOOL WINAPI MainDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
void	NetworkThread(PVOID	pvoid);

int WINAPI WinMain (
               HINSTANCE hInstance,    // 实例句柄
              HINSTANCE hPrevInstance,// NULL
              LPTSTR lpCmdLine,       // 用于运行程序的命令行
              int nCmdShow)           // 程序的最初的显示方式
{
  
	WNDCLASS wc;

   GetClassInfo( NULL, WC_DIALOG, &wc );          //  Get dialog box class info

   wc.hInstance = hInstance;
   wc.lpszClassName = "Client";                 //  so's we can superclass it
   wc.lpfnWndProc = DefDlgProc;               //  all this for an icon...sheesh
   wc.hIcon = LoadIcon( hInstance, MAKEINTRESOURCE( 100 ) );
   wc.cbWndExtra = DLGWINDOWEXTRA;

   if( ! RegisterClass( &wc ) )                  //  Quit if registration fails
      return( FALSE );

   //hInst = hInstance;               //  Now just start the main dialog and exit
   DialogBox( hInstance, MAKEINTRESOURCE(ID_CLIENT), NULL, MainDlgProc );
 
  return TRUE;
}

// === Main Dialog Box ========================================================

BOOL WINAPI MainDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{   
	static 	SOCKADDR_IN local_sin,              // 本地socket地址
              accept_sin;             // 接收连接对象的地址

	static SOCKADDR_IN destination_sin;        // 服务器的socket地址
	static PHOSTENT phostent = NULL;           // 指向服务器的HOSTENT结构
	static WSADATA WSAData;                    // 包含了Winsocket执行后的细节
	static PARAMS	params;
	static SOCKET WinSocket = INVALID_SOCKET;  // Window socket
		 
   switch( msg )
   {

   case WM_INITDIALOG:						//初始化对话框消息
	   {
   		// 初始化Winsocket. 
		  if (WSAStartup (MAKEWORD(1,1), &WSAData) != 0) 
		  {
			wsprintf (szError, TEXT("WSAStartup failed. Error: %d"), 
					  WSAGetLastError ());
			MessageBox (NULL, szError, TEXT("Error"), MB_OK);
			return FALSE;
		  }

		  //创建一个TCP/IP socket, WinSocket.
		  if ((WinSocket = socket (AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) 
		  {
			wsprintf (szError, TEXT("Allocating socket failed. Error: %d"), 
					  WSAGetLastError ());
			MessageBox (NULL, szError, TEXT("Error"), MB_OK);
			return FALSE;
		  }

		  // 填写本地socket地址信息
		  local_sin.sin_family = AF_INET;
		  local_sin.sin_port = htons (PORTNUM);  
		  local_sin.sin_addr.s_addr = htonl (INADDR_ANY);

		  // 把本地地址绑定到WinSocket.
		  if (bind (WinSocket, 
					(struct sockaddr *) &local_sin, 
					sizeof (local_sin)) == SOCKET_ERROR) 
		  {
			wsprintf (szError, TEXT("Binding socket failed. Error: %d"), 
					  WSAGetLastError ());
			MessageBox (NULL, szError, TEXT("Error"), MB_OK);
			closesocket (WinSocket);
			return FALSE;
		  }

		  // 建立一个socket监听到来的连接
		  if (listen (WinSocket, MAX_PENDING_CONNECTS) == SOCKET_ERROR) 
		  {
			wsprintf (szError, 
					  TEXT("Listening to the client failed. Error: %d"),
					  WSAGetLastError ());
			MessageBox (NULL, szError, TEXT("Error"), MB_OK);
			closesocket (WinSocket);
			return FALSE;
		  }
		  params.hwnd = hDlg;
		  params.bContinue = TRUE;
		  params.WinSocket = WinSocket;
		  params.accept_sin =  accept_sin ;
		  params.local_sin = local_sin ;
				
		  _beginthread(NetworkThread,0,&params);
  
	   }
		 
   	case WM_PAINT:              //  对话框绘制消息
		{
		PAINTSTRUCT ps;
		HDC hDC;
		RECT Rect;

		hDC = BeginPaint( hDlg, &ps );
		GetClientRect( hDlg, &Rect );
		FillRect( hDC, &Rect, GetProp( hDlg, "BRUSH" ) );
		EndPaint( hDlg, &ps );
		return	0;
		}

	case	WM_MESSAGE_REVED:		//辅助线程接收到消息
		{
		//把接收到的消息在对话框中显示
		SetWindowText( GetDlgItem(hDlg,IDC_EDIT1), szServerW );
		return	0;
		}

    case	WM_COMMAND:				//处理从控件传递来的消息
		{
	
		if(wParam == IDC_SEND )
		{
			//如果用户点击发送，从输入框中取得文本
			GetWindowText(GetDlgItem(hDlg,IDC_EDIT),
				  szSendBuffer,GetWindowTextLength(GetDlgItem(hDlg,IDC_EDIT)));

			//发送文本
			if (send (ClientSock, szSendBuffer, strlen (szSendBuffer) + 1, 0)
					   == SOCKET_ERROR) 
			{
				wsprintf (szError, 
						  TEXT("Sending data to the server failed. Error: %d"),
						  WSAGetLastError ());
				MessageBox (NULL, szError, TEXT("Error"), MB_OK);
			}

		 }
		else if(wParam == IDCANCEL)
		{
			EndDialog( hDlg, TRUE ); 
		}
		return( TRUE );
		}
	
   }
   return FALSE;
}   
                      

//辅助线程，负责socket通信
void	NetworkThread(PVOID	pvoid)
{
	volatile	PPARAMS	pparams;
	int index = 0,                      // 索引
			  iReturn;                  

	int accept_sin_len;                 // accept_sin的长度



	pparams = (PPARAMS)pvoid;

	accept_sin_len = sizeof (pparams->accept_sin);

	// 接收来自发送到WinSocket的连接请求
	ClientSock = accept (pparams->WinSocket, 
					   (struct sockaddr *) &(pparams->accept_sin), 
					   (int *) &accept_sin_len);

	// 停止接收客户端的连接请求
	closesocket (pparams->WinSocket);
	if (ClientSock == INVALID_SOCKET) 
	{
		wsprintf (szError, TEXT("Accepting connection with client failed.")
				  TEXT(" Error: %d"), WSAGetLastError ());
		MessageBox (NULL, szError, TEXT("Error"), MB_OK);

	  }

	for (;;)
	  {
		// 从客户端接收数据
		iReturn = recv (ClientSock, szServerA, sizeof (szServerA), 0);

		// 如果有数据，就显示之
		if (iReturn == SOCKET_ERROR)
		{
		  wsprintf (szError, TEXT("No data is received, recv failed.")
					TEXT(" Error: %d"), WSAGetLastError ());
		  MessageBox (NULL, szError, TEXT("Server"), MB_OK);
		  break;
		}
		else if (iReturn == 0)
		{
		  MessageBox (NULL, TEXT("Finished receiving data"), TEXT("Server"),
					  MB_OK);
		  break;
		}
		else
		{
		  // 把ASCII码字符串转换成Unicode码字符串
		  for (index = 0; index <= sizeof (szServerA); index++)
			szServerW[index] = szServerA[index];

	
		  // 显示从服务器接收到的字符串
		  // MessageBox (NULL, szClientW, TEXT("Received From Server"), MB_OK);
		  SendMessage(pparams->hwnd,WM_MESSAGE_REVED,0,0);
		}
	  } 

	  // 发送一条消息到客户端
	

	  // 禁止ClientSock的接收和发送端口.
	  shutdown (ClientSock, 0x02);

	  // 关闭ClientSock.
	  closesocket (ClientSock);

	  WSACleanup ();


}
