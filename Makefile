ifndef ${DESTDIR}
DESTDIR=/
endif

ifneq ($(SBOX_DPKG_INST_ARCH),)
N900_MODE="SBOX"
endif

ifeq ($(OSSO_PRODUCT_NAME),N900)
N900_MODE="NATIVE"
endif

all:wadoku-notify-linux
win:wadoku-notify-win
mac:wadoku-notify-mac

wadoku-notify-linux:
ifneq ($(N900_MODE),)
	valac -o bin/wadoku-notify src-linux/clipboard.vala src/db.c src/sqlite3.c src-linux/notify-libnotify.vala --pkg libnotify --pkg gtk+-2.0 --pkg posix -D N900
else
	valac -o bin/wadoku-notify src-linux/clipboard.vala src/db.c src-linux/notify-libnotify.vala --pkg libnotify --pkg gtk+-2.0 --pkg posix -X -ldl -X `pkg-config sqlite3 --libs`
endif
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
	mkdir -p ${DESTDIR}/usr/share
	rm -f ${DESTDIR}/usr/bin/wadoku-notify
	mkdir -p ${DESTDIR}/usr/share/pixmaps
ifneq ($(N900_MODE),)
	mkdir -p ${DESTDIR}/opt/wadoku-notify
	mkdir -p ${DESTDIR}/usr/share/applications/hildon
	cp bin/wadoku-notify ${DESTDIR}/opt/wadoku-notify/
	cp bin/wadoku.png ${DESTDIR}/opt/wadoku-notify/
	cp data/wadoku.sqlite3 ${DESTDIR}/opt/wadoku-notify/
	cp data/wadoku-notify.desktop ${DESTDIR}/usr/share/applications/hildon/
	ln -sf /opt/wadoku-notify/wadoku-notify ${DESTDIR}/usr/bin/wadoku-notify
	ln -sf /opt/wadoku-notify/wadoku.png ${DESTDIR}/usr/share/pixmaps/wadoku-notify.png
else
	mkdir -p ${DESTDIR}/usr/share/wadoku-notify
	mkdir -p ${DESTDIR}/usr/lib/wadoku-notify
	mkdir -p ${DESTDIR}/usr/share/applications
	cp bin/wadoku-notify ${DESTDIR}/usr/lib/wadoku-notify/
	cp bin/wadoku.png ${DESTDIR}/usr/share/wadoku-notify/
	cp data/wadoku.sqlite3 ${DESTDIR}/usr/share/wadoku-notify/
	cp data/wadoku-notify.desktop ${DESTDIR}/usr/share/applications/
	ln -sf /usr/share/wadoku-notify/wadoku.sqlite3 ${DESTDIR}/usr/lib/wadoku-notify/wadoku.sqlite3
	ln -sf /usr/share/wadoku-notify/wadoku.png ${DESTDIR}/usr/lib/wadoku-notify/wadoku.png
	ln -sf /usr/lib/wadoku-notify/wadoku-notify ${DESTDIR}/usr/bin/wadoku-notify
	ln -sf /usr/share/wadoku-notify/wadoku.png ${DESTDIR}/usr/share/pixmaps/wadoku-notify.png
endif

clean:
	rm -f bin/*
