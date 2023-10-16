#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

int main() {
    const char* target_mac_address = "3C:22:FB:9A:01:4F";
    const char* service_uuid = "3C8DC645-7BB0-4F18-8A2A-1413E75D63F6";
    struct sockaddr_rc addr = { 0 };
    int s, status;
    char buf[1024];

    // Create a socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t)strtol(service_uuid, NULL, 16);
    str2ba(target_mac_address, &addr.rc_bdaddr);

    // Connect to the remote device
    status = connect(s, (struct sockaddr*)&addr, sizeof(addr));
    if (status < 0) {
        perror("Failed to connect");
        close(s);
        return 1;
    }

    // Send data to the remote device
    char* data_to_send = "Hello, Bluetooth Device!";
    status = write(s, data_to_send, strlen(data_to_send));
    if (status < 0) {
        perror("Failed to send data");
    } else {
        printf("Sent data: %s\n", data_to_send);
    }

    // Receive data from the remote device
    status = read(s, buf, sizeof(buf));
    if (status < 0) {
        perror("Failed to receive data");
    } else {
        buf[status] = '\0';
        printf("Received data: %s\n", buf);
    }

    // Close the socket
    close(s);
    return 0;
}

