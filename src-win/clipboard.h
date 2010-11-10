#define WINDOWS_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0500
#define UNICODE


#include<windows.h>
#include<winuser.h>
#include<stdio.h>
//TESTING CODE
HWND hWnd;
HWND label;

DWORD x_pos;
DWORD y_pos;

LPRECT lRect;

#define TIMER_HIDE 1
