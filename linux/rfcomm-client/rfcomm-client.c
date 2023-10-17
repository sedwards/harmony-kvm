#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

int main(int argc, char **argv)
{
    struct sockaddr_rc addr = { 0 };
    int s, status;
    char dest[18] = "3C:22:FB:9A:01:4F"; // Mac

    // allocate a socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // set the connection parameters (who to connect to)
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t)1;
    str2ba(dest, &addr.rc_bdaddr);

    // connect to the server
    status = connect(s, (struct sockaddr *)&addr, sizeof(addr));

    if (status < 0) {
        perror("Connection error");
        return 1;
    }

    // send a message
    if (status == 0) {
        status = write(s, "hello!", 6);
        if (status < 0) {
            perror("Write error");
        } else {
            printf("Message sent: hello!\n");
        }
    } else {
        perror("Connection error");
    }

    // Read and print messages from the server
    char buffer[1024];
    int bytesRead;
    while (1) {
        bytesRead = read(s, buffer, sizeof(buffer));
        if (bytesRead <= 0) {
            // The server may have closed the connection, or an error occurred
            break;
        }
        printf("Received: %.*s", bytesRead, buffer);
    }

    close(s);
    return 0;
}

