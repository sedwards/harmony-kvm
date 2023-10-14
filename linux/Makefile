CC=gcc
CFLAGS=`pkg-config --libs --cflags gio-2.0 gobject-2.0 cairo glib-2.0 gtk+-3.0` -fPIC -Wall -Wno-deprecated-declarations -g -I. -I./include -I./include/gdk -I./include/public
CFLAGS_CAIRO=`pkg-config --libs --cflags cairo` -fPIC -Wall -g -I. -I./include

all: bluetooth

LE_SRC = \
        le.o

BT_SRC = \
        bluetooth.o \
	common.o

LE_OBJS = $(patsubst %.c, %.o, $(LE_SRC))
BT_OBJS = $(patsubst %.c, %.o, $(BT_SRC))

LE_DEP = ${LE_SRC:.c=.d}
BT_DEP = ${BT_SRC:.c=.d}

#### Broadway Standalone

bluetooth : $(BT_OBJS)
	$(CC) -o $@ $(BT_OBJS) `pkg-config --libs --cflags dbus-1 gio-2.0 gobject-2.0 glib-2.0 gtk+-3.0` -fPIC -Wall -g -lbluetooth -uuid

le : $(LE_OBJS)
	$(CC) -o $@ $(LE_OBJS) `pkg-config --libs --cflags dbus-1 gio-2.0 gobject-2.0 glib-2.0 gtk+-3.0` -fPIC -Wall -g -lbluetooth -uuid

###
### Clean target, other useful targets
###

clean:
	rm -f *.o bluetooth le


