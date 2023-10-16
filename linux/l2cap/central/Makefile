CC=gcc
CFLAGS=`pkg-config --libs --cflags gio-2.0 gobject-2.0 cairo glib-2.0 gtk+-3.0` -fPIC -Wall -Wno-deprecated-declarations -g -I. -I./include -I./include/gdk -I./include/public
CFLAGS_CAIRO=`pkg-config --libs --cflags cairo` -fPIC -Wall -g -I. -I./include

all: central

CENTRAL_SRC = \
        central.o

CENTRAL_OBJS = $(patsubst %.c, %.o, $(CENTRAL_SRC))

CENTRAL_DEP = ${CENTRAL_SRC:.c=.d}

#### Broadway Standalone

central : $(CENTRAL_OBJS)
	$(CC) -o $@ $(CENTRAL_OBJS) `pkg-config --libs --cflags dbus-1 gio-2.0 gobject-2.0 glib-2.0 gtk+-3.0` -fPIC -Wall -g -lbluetooth -uuid

###
### Clean target, other useful targets
###

clean:
	rm -f *.o central


