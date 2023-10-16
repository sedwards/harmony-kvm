#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

int main(int argc, char **argv) {
    struct sockaddr_rc loc_addr = { 0 };
    int s, client, bytes_read;
    char buf[1024] = { 0 };
    uuid_t service_uuid = { 0 };
    int port = 1; // RFCOMM channel to listen on

    // Create a socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // Set up the server information
    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = *BDADDR_ANY; // Use the first available Bluetooth adapter
    loc_addr.rc_channel = (uint8_t)port;

    // Bind the socket
    bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));

    // Put the server into listening mode
    listen(s, 1);

    // Accept an incoming connection
    client = accept(s, (struct sockaddr *)&loc_addr, &sizeof(loc_addr));

    // Receive data from the client
    while (1) {
        memset(buf, 0, sizeof(buf));
        bytes_read = recv(client, buf, sizeof(buf), 0);
        if (bytes_read <= 0) {
            break;
        }
        printf("Received: %s", buf);
    }

    // Close the client and server sockets
    close(client);
    close(s);

    return 0;
}

