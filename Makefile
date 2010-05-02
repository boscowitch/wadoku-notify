all:wadoku-notify


wadoku-notify:
	valac -o bin/wadoku-notify src-linux/clipboard.vala src/db.c src-linux/notify-libnotify.vala --disable-warnings --pkg libnotify --pkg gtk+-2.0 --pkg sqlite3
