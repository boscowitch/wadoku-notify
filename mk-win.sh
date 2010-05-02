#!/bin/bash
i386-mingw32-windres icon.rc -O coff -o icon.res
i386-mingw32-gcc -mwindows -o bin/wadoku.exe src-win/notify.c src/db.c src-win/clipboard.c icon.res src/sqlite3.c -lgdi32 && open -a "Windows Explorer.app" wadoku.exe
