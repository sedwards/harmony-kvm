#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include <glib.h>
#include <gio/gio.h>

// Connecting too
// 3C:22:FB:9A:01:4F

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <device_address>\n", argv[0]);
        return 1;
    }

    const char *device_address = argv[1];
    bdaddr_t bdaddr;
    str2ba(device_address, &bdaddr);

    // Create an RFCOMM socket
    int sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if (sock < 0) {
        perror("Failed to create RFCOMM socket");
        return 1;
    }

    // Specify the Bluetooth device to connect to
    struct sockaddr_rc addr = { 0 };
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = 1;  // RFCOMM channel, adjust as needed
    bacpy(&addr.rc_bdaddr, &bdaddr);

    // Connect to the remote Bluetooth device
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Failed to connect");
        close(sock);
        return 1;
    }

    // Send data to the remote device
    char data[] = "Hello, Bluetooth!";
    ssize_t bytes_written = write(sock, data, sizeof(data));
    if (bytes_written < 0) {
        perror("Failed to send data");
    } else {
        printf("Data sent: %s\n", data);
    }

    // Continuously read and print data
    char buffer[1024];  // Adjust buffer size as needed
    while (1) {
        // Read data from the RFCOMM socket
        ssize_t bytes_read = read(sock, buffer, sizeof(buffer));
        if (bytes_read < 0) {
            perror("Failed to read data");
            break;
        } else if (bytes_read == 0) {
            // Connection closed by the remote device
            break;
        } else {
            // Print the received data
            printf("Received data: ");
            for (ssize_t i = 0; i < bytes_read; i++) {
                printf("%02X ", buffer[i]);
            }
            printf("\n");
        }
    }

    close(sock);

    return 0;
}

