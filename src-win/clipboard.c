#include"clipboard.h"

static HWND old;
static HWND clipwin;
static NOTIFYICONDATA niData = {0};
static BOOL ignoreFirst=0;
static UINT WM_TASKBARCREATED = 0;
static HMENU con = 0;
enum { ID_OPTIONS, ID_SEP, ID_EXIT };

void init_db(const char* path);
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
	HBRUSH hBrushStatic = CreateSolidBrush(RGB(0,0,0));
	if(x==WM_TASKBARCREATED) {
		//explorer restarted, recreate systray icon
		Shell_NotifyIcon(NIM_ADD,&niData);
	} else
	switch(x) {
	case WM_COMMAND: { //handle popup menu
		int wID = LOWORD(y);
		switch(wID) {
			case ID_OPTIONS:
				notify("error","not yet implemented");
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
	case WM_CTLCOLORSTATIC: {
		SetTextColor((HDC) y, RGB(255,255,255));
		SetBkColor((HDC)y,RGB(0,0,0));
		return (LRESULT) hBrushStatic;
	}
	case WM_MOUSEMOVE: {
        ShowWindow(hWnd,SW_HIDE);
	}
	case WM_TIMER: {
	    switch(y)
	    {
	        case TIMER_HIDE: {
	            ShowWindow(hWnd,SW_HIDE);
	            KillTimer(hWnd,TIMER_HIDE);
                break;
	        }
	    }
	}
	/*case WM_SIZE: {

		break;
	}*/
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

typedef BOOL (WINAPI * SLWAProc)(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);

static SLWAProc pSLWA = NULL;

BOOL SetWindowTransparency (HWND hwnd)
{
   static short beenHere = FALSE;

   HMODULE  hUser32;

   if (!pSLWA && !beenHere)  {
      beenHere = TRUE;
      hUser32 = GetModuleHandle (L"user32.dll");

      pSLWA = (SLWAProc)GetProcAddress(hUser32, (char *)"SetLayeredWindowAttributes");

   }

   if (!pSLWA)
      return (FALSE);  // No support for translucent windows!

   SetWindowLong (hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);

   return (pSLWA(hwnd, 0, 200, 2));   // percent% alpha
}


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE b, LPSTR c, int d) {

	MSG        msg;

	WNDCLASSEX wc = {0};

	wc.cbSize        =  sizeof(WNDCLASSEX);
    	wc.style         =  CS_SAVEBITS;// | WS_CLIPCHILDREN | WS_CLIPSIBLINGS; // | CS_SAVEBITS;//| WS_CLIPCHILDREN | WS_CLIPSIBLINGS ;//| CS_SAVEBITS | 0x00020000; //0x00020000 > win xp only drop shadow
    	wc.lpfnWndProc   =  wndproc;
    	wc.cbClsExtra    =  0;
    	wc.cbWndExtra    =  0;
    	wc.hInstance     =  hInst;
    	wc.hCursor       =  LoadCursor(NULL,IDC_ARROW);
    	wc.hIcon         =  0;
    	wc.hbrBackground =  GetStockObject(BLACK_BRUSH);
    	wc.lpszClassName =  L"wadoku_notify";
    	wc.lpszMenuName  =  NULL;
    	wc.hIconSm       =  0;


	if(!RegisterClassEx(&wc))
	    return 0;


    //get desktop resolution
    LPRECT drect = (RECT*) malloc(sizeof(RECT));
    HWND desktop = GetDesktopWindow();
    GetWindowRect(desktop, drect);

    //calculate x and y position cordinates for notification window
    x_pos = (drect->right-drect->left)-300;
    y_pos = (drect->bottom-drect->top)-250;

    free(drect);

	hWnd = CreateWindowEx(WS_EX_TOOLWINDOW,
                       L"wadoku_notify",
                       L"Wadoku Notify",
                       WS_POPUP,// | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, //| CS_SAVEBITS, //| 0x00020000, after win xp only for drop shadow
                       x_pos,
                       y_pos,
                       30,
                       20,
                       NULL,
                       NULL,
                       hInst,
                       NULL);
	if( hWnd == NULL)
		return 0;

	label = CreateWindow(L"static",L"",WS_CHILD | WS_VISIBLE | SS_LEFT,0,0,300,200,hWnd,(HMENU) 1,hInst,NULL);

	lRect = (LPRECT) malloc(sizeof(RECT));
	lRect->left = 0;
	lRect->right = 290;
	lRect->top = 0;
	lRect->bottom = 10;

	HDC dc = GetDC(label);

	SetWindowTransparency(hWnd);

    /*
    DrawTextEx(dc, strn,(int) wcslen(strn),lRect, DT_CALCRECT | DT_WORDBREAK ,NULL);
    ShowWindow(hWnd,SW_SHOW);
    SetWindowPos(label,NULL, 0, 0, lRect->right - lRect->left, lRect->bottom - lRect->top, SWP_NOMOVE|SWP_NOZORDER);
    SetWindowPos(hWnd,HWND_TOPMOST, x_pos, y_pos, lRect->right - lRect->left, lRect->bottom - lRect->top, SWP_SHOWWINDOW); //|SWP_NOMOVE
    */




    init_db(NULL,true);


    //Tray icon
	WNDCLASS classy = {0};
	LPCWSTR classname = L"wadoku_notify_clipper";
	classy.lpszClassName=classname;
	classy.hInstance=hInst;
	classy.lpfnWndProc=wndproc;
	ATOM x = RegisterClass(&classy);

	WM_TASKBARCREATED = RegisterWindowMessage(L"TaskbarCreated");

	clipwin = CreateWindow(classname,L"wadoku-notify-trayicon",WS_OVERLAPPEDWINDOW,0,0,0,0,0,0,hInst,0);
	old = SetClipboardViewer(clipwin);

	//start setup systray icon
    niData.cbSize = sizeof(NOTIFYICONDATA);
    niData.uID = 101;
    niData.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP;
    niData.hIcon =
        (HICON)LoadImage( hInst,
            L"MAINICON",
            IMAGE_ICON,
            GetSystemMetrics(SM_CXSMICON),
            GetSystemMetrics(SM_CYSMICON),
            LR_DEFAULTCOLOR);
    niData.hWnd = clipwin;
    niData.uCallbackMessage = WM_APP;
    Shell_NotifyIcon(NIM_ADD,&niData);
    //end setup systray icon

	//start popup menu creation
	con = CreatePopupMenu();
	AppendMenu(con,MF_STRING,ID_OPTIONS,L"&Options");
	AppendMenu(con,MF_MENUBARBREAK,ID_SEP,NULL);
	AppendMenu(con,MF_STRING,ID_EXIT,L"E&xit");
	//end popup menu creation



    //Start message event handling
	MSG Msg;
	while (GetMessage(&Msg,NULL,0,0)==TRUE) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	Shell_NotifyIcon(NIM_DELETE,&niData);

	ExitProcess(0);
}
