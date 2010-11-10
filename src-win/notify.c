#include"clipboard.h"


void notify(const char* ctitle, const char* ctext) {
	WCHAR btitle[1024];
	WCHAR btext[1024];
	MultiByteToWideChar(CP_UTF8,0,ctitle,-1,btitle,1024);
	MultiByteToWideChar(CP_UTF8,0,ctext,-1,btext,1024);
	//MessageBoxW(0,btext,btitle,MB_ICONINFORMATION|MB_OK);

	LPWSTR content;// = _stprintf("%s\n%s", btitle,btext);
	swprintf(content, L"%s\n%s", btitle, btext);
	SetWindowText(label, content);

	/*lRect->left = 0;
	lRect->right = 290;
	lRect->top = 0;
	lRect->bottom = 10;*/
	HDC dc = GetDC(label);

	DrawTextEx(dc, content,(int) wcslen(content),lRect, DT_CALCRECT | DT_WORDBREAK ,NULL);

	ShowWindow(hWnd,SW_SHOW);
    SetWindowPos(label,NULL, 0, 0, lRect->right - lRect->left, lRect->bottom - lRect->top, SWP_NOMOVE|SWP_NOZORDER);
    SetWindowPos(hWnd,HWND_TOPMOST, x_pos, y_pos, lRect->right - lRect->left, lRect->bottom - lRect->top, SWP_SHOWWINDOW|SWP_NOZORDER); //|SWP_NOMOVE
	SetTimer(hWnd,TIMER_HIDE,5000,NULL);
}
