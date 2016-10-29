#Makefile

CC=gcc

XFCE_FLAGS=`pkg-config --cflags --libs libxfce4panel-2.0`
GTK_FLAGS=`pkg-config --cflags --libs gtk+-3.0`

NAME=battery_plugin


makeplugin: battery_plugin.c
	$(CC) -Wall -shared -o lib${NAME}.so -fPIC ${NAME}.c ${XFCE_FLAGS} ${GTK_FLAGS}
