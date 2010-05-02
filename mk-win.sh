#!/bin/bash
i386-mingw32-windres icon.rc -O coff -o icon.res
i386-mingw32-gcc -mwindows -o wadoku.exe notify-win.c db.c clip-win.c icon.res sqlite3.c -lgdi32 && open -a "Windows Explorer.app" wadoku.exe