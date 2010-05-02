#!/bin/bash
clang -Wno-unused-value -o wadoku-mac -framework Foundation -framework AppKit clip-mac.m db.c notify-mac.m sqlite3.c && ./wadoku-mac
