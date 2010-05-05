#define WINDOWS_LEAN_AND_MEAN
#include<windows.h>

void notify(const char* ctitle, const char* ctext) {
	WCHAR btitle[1024];
	WCHAR btext[1024];
	MultiByteToWideChar(CP_UTF8,0,ctitle,-1,btitle,1024);
	MultiByteToWideChar(CP_UTF8,0,ctext,-1,btext,1024);
	MessageBoxW(0,btext,btitle,MB_ICONINFORMATION|MB_OK);
}
