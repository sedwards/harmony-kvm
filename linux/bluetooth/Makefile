CC=gcc
CFLAGS=`pkg-config --libs --cflags gio-2.0 gobject-2.0 cairo glib-2.0 gtk+-3.0` -fPIC -Wall -Wno-deprecated-declarations -g -I. -I./include -I./include/gdk -I./include/public
CFLAGS_CAIRO=`pkg-config --libs --cflags cairo` -fPIC -Wall -g -I. -I./include

all: bluetooth

LE_SRC = \
        bluetooth.o

LE_OBJS = $(patsubst %.c, %.o, $(LE_SRC))

LE_DEP = ${LE_SRC:.c=.d}

#### Broadway Standalone

bluetooth : $(LE_OBJS)
	$(CC) -o $@ $(LE_OBJS) `pkg-config --libs --cflags dbus-1 gio-2.0 gobject-2.0 glib-2.0 gtk+-3.0` -fPIC -Wall -g -lbluetooth -uuid

###
### Cbluetoothan target, other useful targets
###

cbluetoothan:
	rm -f *.o bluetooth bluetooth


