#define WINDOWS_LEAN_AND_MEAN
#include<Windows.h>

static HWND old;
static HWND clipwin;
static NOTIFYICONDATA niData = {0};
static BOOL ignoreFirst=0;

void lookup(const char* str);

static void clipBoardUpdated() {
	if(OpenClipboard(clipwin)) {
		HANDLE x = GetClipboardData(CF_UNICODETEXT);
		if(x!=NULL) {
			LPCWSTR y = GlobalLock(x);
			char buf[1024] = {0};
			int res = WideCharToMultiByte(CP_UTF8,0,y,-1,buf,1024,NULL,NULL);
			GlobalUnlock((HGLOBAL)y);
			CloseClipboard();
			if(res) {
				if(ignoreFirst) {
					lookup(buf);
				} else {
					ignoreFirst=1;
				}
			}
		} else {
			CloseClipboard();
		}
	}
}

static long WINAPI wndproc (HWND w,UINT x,WPARAM y,LPARAM z) {
	if(x==WM_DRAWCLIPBOARD) {
		clipBoardUpdated();
		SendMessage(old,x,y,z);
	} else if(x==WM_CHANGECBCHAIN) {
		if(old==(HWND)y) {
			old=(HWND)z; 
		}
		else
		{
			SendMessage(old,x,y,z);
		}
	} else if(x==WM_DESTROY) {
		ChangeClipboardChain(w,old);
	} else if(x==WM_APP) { //handle systray messages
		switch(z)
        {
        case WM_LBUTTONDBLCLK:
       		Shell_NotifyIcon(NIM_DELETE,&niData);
            ExitProcess(0);
            break;
        case WM_RBUTTONDOWN:
        case WM_CONTEXTMENU:
            //ShowContextMenu(hWnd);
        	break;
        }
	}
	return DefWindowProc(w,x,y,z);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE b, LPSTR c, int d) {
	WNDCLASS classy = {0};
	const char* classname = "wadoku_notify_clipper";
	classy.lpszClassName=classname;
	classy.hInstance=hInst;
	classy.lpfnWndProc=wndproc;
	ATOM x = RegisterClass(&classy);
	
	clipwin = CreateWindow(classname,"aaa",WS_OVERLAPPEDWINDOW,0,0,0,0,0,0,hInst,0);
	old = SetClipboardViewer(clipwin);
	
	//setup systray icon
    niData.cbSize = sizeof(NOTIFYICONDATA);
    niData.uID = 101;
    niData.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP;
    niData.hIcon =
        (HICON)LoadImage( hInst,
            "MAINICON",
            IMAGE_ICON,
            GetSystemMetrics(SM_CXSMICON),
            GetSystemMetrics(SM_CYSMICON),
            LR_DEFAULTCOLOR);
    niData.hWnd = clipwin;
    niData.uCallbackMessage = WM_APP;
    Shell_NotifyIcon(NIM_ADD,&niData);
    //end setup systray icon
	
	MSG Msg;
	while (GetMessage(&Msg,NULL,0,0)==TRUE) {
		DispatchMessage(&Msg);
	}
	return 0;
}
