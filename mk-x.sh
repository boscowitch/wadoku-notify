#!/bin/bash
valac -o wadoku-x clip-x.vala db.c notify-libnotify.vala --disable-warnings --pkg libnotify --pkg gtk+-2.0 --pkg sqlite3 && ./wadoku-X
