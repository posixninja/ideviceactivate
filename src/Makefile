CFLAGS := -g -pthread -I/usr/local/include -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libxml2
LDFLAGS := -pthread -L/usr/local/lib -limobiledevice -lplist -lusbmuxd -lgthread-2.0 -lrt -lgnutls -ltasn1 -lxml2 -lglib-2.0 -lcurl

all:
	gcc -o ideviceactivate ideviceactivate.c activate.c $(CFLAGS) $(LDFLAGS)