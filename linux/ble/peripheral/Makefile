CC=gcc
CFLAGS=`pkg-config --libs --cflags gio-2.0 gobject-2.0 cairo glib-2.0 gtk+-3.0` -fPIC -Wall -Wno-deprecated-declarations -g -I. -I./include -I./include/gdk -I./include/public
CFLAGS_CAIRO=`pkg-config --libs --cflags cairo` -fPIC -Wall -g -I. -I./include

all: peripheral

L2CAPC_SRC = \
        peripheral.o

L2CAPC_OBJS = $(patsubst %.c, %.o, $(L2CAPC_SRC))

L2CAPC_DEP = ${L2CAPC_SRC:.c=.d}

#### Broadway Standalone

peripheral : $(L2CAPC_OBJS)
	$(CC) -o $@ $(L2CAPC_OBJS) `pkg-config --libs --cflags dbus-1 gio-2.0 gobject-2.0 glib-2.0 gtk+-3.0` -fPIC -Wall -g -lbluetooth -uuid

###
### Clean target, other useful targets
###

clean:
	rm -f *.o peripheral


