
#include "windows.h"
#include "winsock.h"
#include "resource.h"
#include "PROCESS.H"

#define	STATUS_SEND		0
#define	STATUS_REVE		1
#define	STATUS_DONE		2

#define	WM_MESSAGE_REVED	(WM_USER+0)
#define	WM_MESSAGE_SENDED	(WM_USER+1)

#define PORTNUM         5000          // 端口号
#define HOSTNAME        "localhost"   // 服务器名字
                                      // 根据服务器的不同，更改上面的名字


typedef	struct
{
	HWND	hwnd;
	BOOL	bContinue;
	SOCKET	ServerSock;
}
PARAMS,*PPARAMS;


// === Function Prototypes ====================================================

BOOL WINAPI MainDlgProc( HWND, UINT, WPARAM, LPARAM );
void	NetworkThread(PVOID	pvoid);

// === Global Variables =======================================================

HINSTANCE	hInst;
char		szClientA[100];                // ASCII码串
TCHAR		szClientW[100];               // Unicode串
TCHAR		szSendBuffer[100];
TCHAR szError[100];                 // 出错消息串


// === Application Entry Point ================================================

int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmd,
                      int nShow )
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

   hInst = hInstance;               //  Now just start the main dialog and exit
   DialogBox( hInstance, MAKEINTRESOURCE(ID_CLIENT), NULL, MainDlgProc );
   return( FALSE );
}

// === Main Dialog Box ========================================================

BOOL WINAPI MainDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{   
	static SOCKET ServerSock = INVALID_SOCKET; // 服务器的socket
	static SOCKADDR_IN destination_sin;        // 服务器的socket地址
	static PHOSTENT phostent = NULL;           // 指向服务器的HOSTENT结构
	static WSADATA WSAData;                    // 包含了Winsocket执行后的细节
	static PARAMS	params;

		 
   switch( msg )
   {
   case WM_INITDIALOG:
	   {
   	
		if (WSAStartup (MAKEWORD(1,1), &WSAData) != 0) 
		{
			wsprintf (szError, TEXT("WSAStartup failed. Error: %d"), 
					  WSAGetLastError ());
			MessageBox (NULL, szError, TEXT("Error"), MB_OK);
			return FALSE;
		 }
		 return	0;
	   }
		 
	    

	case WM_PAINT:              //  Paint dialog box background COLOR_BTNFACE
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

	case	WM_MESSAGE_REVED:
		{
		SetWindowText( GetDlgItem(hDlg,IDC_EDIT1), szClientW );
		return	0;
		}

    case	WM_COMMAND:
		{
		if( wParam == IDC_CONNECT )                          //  连接服务器
		{
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
			  params.hwnd = hDlg;
			  params.bContinue = TRUE;
			  params.ServerSock = ServerSock;
			  _beginthread(NetworkThread,0,&params);
		}
		
		else if(wParam == IDC_SEND )
		{
			GetWindowText(GetDlgItem(hDlg,IDC_EDIT),
				  szSendBuffer,GetWindowTextLength(GetDlgItem(hDlg,IDC_EDIT)));

			if (send (ServerSock, szSendBuffer, strlen (szSendBuffer) + 1, 0)
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
                                                   //  End of MainDlgProc()
void	NetworkThread(PVOID	pvoid)
{
	volatile	PPARAMS	pparams;
	int index = 0,                      // 索引
			  iReturn;                        // recv函数的返回值

	pparams = (PPARAMS)pvoid;

	for (;;)
	  {
		// 从服务器接收信息
		iReturn = recv (pparams->ServerSock, szClientA, sizeof (szClientA), 0);

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
		  // MessageBox (NULL, szClientW, TEXT("Received From Server"), MB_OK);
		  SendMessage(pparams->hwnd,WM_MESSAGE_REVED,0,0);

		}
	  }

	  // 禁止服务器发送信息
	  shutdown (pparams->ServerSock, 0x00);

	  // 关闭服务器socket.
	  closesocket (pparams->ServerSock);

	  WSACleanup ();



}






























































