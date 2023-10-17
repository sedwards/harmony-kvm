CC=gcc
CFLAGS=`pkg-config --libs --cflags gio-2.0 gobject-2.0 cairo glib-2.0 gtk+-3.0` -fPIC -Wall -Wno-deprecated-declarations -g -I. -I./include -I./include/gdk -I./include/public
CFLAGS_CAIRO=`pkg-config --libs --cflags cairo` -fPIC -Wall -g -I. -I./include

all: rfcomm-server 

RF_SRC = \
        rfcomm-server.o

RF_OBJS = $(patsubst %.c, %.o, $(RF_SRC))

RF_DEP = ${RF_SRC:.c=.d}

#### Broadway Standalone

rfcomm-server : $(RF_OBJS)
	$(CC) -o $@ $(RF_OBJS) `pkg-config --libs --cflags dbus-1 gio-2.0 gobject-2.0 glib-2.0 gtk+-3.0` -fPIC -Wall -g -lbluetooth -uuid

###
### Clean target, other useful targets
###

clean:
	rm -f *.o rfcomm-server


