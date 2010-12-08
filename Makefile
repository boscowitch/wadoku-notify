ifndef ${DESTDIR}
DESTDIR=/
endif

all:wadoku-notify-linux
win:wadoku-notify-win
mac:wadoku-notify-mac

wadoku-notify-linux:
	valac -o bin/wadoku-notify src-linux/clipboard.vala src/db.c src/sqlite3.c src-linux/notify-libnotify.vala --pkg libnotify --pkg gtk+-2.0 --pkg posix
	ln -sf ../data/wadoku.sqlite3 bin/wadoku.sqlite3
	ln -sf ../data/wadoku.png bin/wadoku.png
	
wadoku-notify-win:
	ln -sf ../src-win/icon.rc bin/icon.rc
	ln -sf ../data/wadoku.ico bin/wadoku.ico
	i486-mingw32-windres bin/icon.rc -O coff -o bin/icon.res
	i486-mingw32-gcc -mwindows -o bin/wadoku.exe src-win/notify.c src/db.c src-win/clipboard.c bin/icon.res src/sqlite3.c -lgdi32 -luser32
	ln -sf ../data/wadoku.sqlite3 bin/wadoku.sqlite3
	rm -f bin/icon.rc bin/icon.res bin/wadoku.ico
	
wadoku-notify-mac:
	clang -Wno-unused-value -o bin/wadoku-mac -framework Foundation -framework AppKit src-mac/clipboard.m src/db.c src-mac/notify.m src/sqlite3.c
	ln -sf ../data/wadoku.sqlite3 bin/wadoku.sqlite3
	ln -sf ../data/wadoku.png bin/wadoku.png
	
install:all
	mkdir -p ${DESTDIR}/usr/bin
	rm -f ${DESTDIR}/usr/bin/wadoku-notify
	mkdir -p ${DESTDIR}/usr/share
	mkdir -p ${DESTDIR}/usr/share/wadoku-notify
	cp bin/wadoku-notify ${DESTDIR}/usr/share/wadoku-notify/
	cp bin/wadoku.png ${DESTDIR}/usr/share/wadoku-notify/
	cp data/wadoku.sqlite3 ${DESTDIR}/usr/share/wadoku-notify/
	ln -s ${DESTDIR}/usr/share/wadoku-notify/wadoku-notify ${DESTDIR}/usr/bin/wadoku-notify

clean:
	rm -f bin/*
