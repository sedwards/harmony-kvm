#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

int main(int argc, char **argv) {
    struct sockaddr_rc loc_addr = { 0 };
    int s, client, bytes_read;
    char buf[1024] = { 0 };

    // Create a socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // Bind the socket to a specific local Bluetooth adapter
    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = *BDADDR_ANY;  // Use the first available adapter
    loc_addr.rc_channel = 22;  // RFCOMM channel

    bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));

    // Listen for incoming connections
    listen(s, 1);

    // Accept the connection
    client = accept(s, (struct sockaddr *)0, (socklen_t *)0);

    // Read data from the client
    while (1) {
        memset(buf, 0, sizeof(buf));
        bytes_read = read(client, buf, sizeof(buf));
        printf("Received: %s", buf);
    }

    close(client);
    close(s);
    return 0;
}

