CC=gcc
CFLAGS=`pkg-config --libs --cflags gio-2.0 gobject-2.0 cairo glib-2.0 gtk+-3.0` -fPIC -Wall -Wno-deprecated-declarations -g -I. -I./include -I./include/gdk -I./include/public
CFLAGS_CAIRO=`pkg-config --libs --cflags cairo` -fPIC -Wall -g -I. -I./include

all: discover

DISCOVER_SRC = \
        discover.o

DISCOVER_OBJS = $(patsubst %.c, %.o, $(DISCOVER_SRC))

DISCOVER_DEP = ${DISCOVER_SRC:.c=.d}

#### Broadway Standalone

discover : $(DISCOVER_OBJS)
	$(CC) -o $@ $(DISCOVER_OBJS) `pkg-config --libs --cflags dbus-1 gio-2.0 gobject-2.0 glib-2.0 gtk+-3.0` -fPIC -Wall -g -lbluetooth -uuid

###
### Cdiscoveran target, other useful targets
###

clean :
	rm -f *.o discover 


