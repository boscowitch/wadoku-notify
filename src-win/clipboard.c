#define WINDOWS_LEAN_AND_MEAN
#include<Windows.h>

static HWND old;
static HWND clipwin;
static NOTIFYICONDATA niData = {0};
static BOOL ignoreFirst=0;
static UINT WM_TASKBARCREATED = 0;
static HMENU con = 0;
enum { ID_OPTIONS, ID_SEP, ID_EXIT };

void lookup(const char* str);
void notify(const char* title,const char* text);

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
	if(x==WM_TASKBARCREATED) {
		//explorer restarted, recreate systray icon
		Shell_NotifyIcon(NIM_ADD,&niData);
	} else
	switch(x) {
	case WM_COMMAND: { //handle popup menu
		int wID = LOWORD(y);
		switch(wID) {
			case ID_OPTIONS:
				notify("error","not implemented");
				break;
			case ID_EXIT:
				PostQuitMessage(0);
				break;
			}
		break;
	}
	case WM_DRAWCLIPBOARD:
		clipBoardUpdated();
		SendMessage(old,x,y,z);
		break;
	case WM_CHANGECBCHAIN:
		if(old==(HWND)y)
			old=(HWND)z;
		else
			SendMessage(old,x,y,z);
		break;
	case WM_DESTROY:
		ChangeClipboardChain(w,old);
		break;
	case WM_APP: //handle systray messages
		switch(z) {
    	case WM_LBUTTONDOWN: {
        		POINT pt;
        		GetCursorPos(&pt);
        		SetForegroundWindow(w);
        		TrackPopupMenu(con,TPM_LEFTALIGN | TPM_LEFTBUTTON,pt.x,pt.y,0,w,NULL);
        		break;
        	}	
        case WM_RBUTTONDOWN: {
        		POINT pt;
        		GetCursorPos(&pt);
        		SetForegroundWindow(w);
        		TrackPopupMenu(con,TPM_LEFTALIGN | TPM_RIGHTBUTTON,pt.x,pt.y,0,w,NULL);
        		break;
        	}
        }
	default:
		return DefWindowProc(w,x,y,z);
	}
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE b, LPSTR c, int d) {
	WNDCLASS classy = {0};
	const char* classname = "wadoku_notify_clipper";
	classy.lpszClassName=classname;
	classy.hInstance=hInst;
	classy.lpfnWndProc=wndproc;
	ATOM x = RegisterClass(&classy);
	
	WM_TASKBARCREATED = RegisterWindowMessage("TaskbarCreated");
	
	clipwin = CreateWindow(classname,"aaa",WS_OVERLAPPEDWINDOW,0,0,0,0,0,0,hInst,0);
	old = SetClipboardViewer(clipwin);
	
	//start setup systray icon
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
	
	//start popup menu
	con = CreatePopupMenu();
	AppendMenu(con,MF_STRING,ID_OPTIONS,"&Options");
	AppendMenu(con,MF_MENUBARBREAK,ID_SEP,NULL);
	AppendMenu(con,MF_STRING,ID_EXIT,"E&xit");
	//end popup menu
	
	MSG Msg;
	while (GetMessage(&Msg,NULL,0,0)==TRUE) {
		DispatchMessage(&Msg);
	}
	Shell_NotifyIcon(NIM_DELETE,&niData);
	ExitProcess(0);
}
