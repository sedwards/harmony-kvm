#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

int main() {
    inquiry_info *ii = NULL;
    int max_rsp = 255; // Maximum number of devices to discover
    int num_rsp;
    int dev_id, sock;
    char addr[19] = "XX:XX:XX:XX:XX:XX"; // Replace with the peripheral's MAC address

    // Prepare a "Hello" message
    char hello_msg[] = "Hello";

    dev_id = hci_get_route(NULL);
    sock = hci_open_dev(dev_id);

    if (dev_id < 0 || sock < 0) {
        perror("Cannot open Bluetooth socket");
        exit(1);
    }

    ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));

    num_rsp = hci_inquiry(dev_id, 8, max_rsp, NULL, &ii, IREQ_CACHE_FLUSH);

    if (num_rsp < 0) {
        perror("Bluetooth inquiry failed");
    }

    for (int i = 0; i < num_rsp; i++) {
        ba2str(&(ii + i)->bdaddr, addr);

        if (strcmp(addr, "XX:XX:XX:XX:XX:XX") == 0) { // Replace with the target peripheral's MAC address
            // Establish an RFCOMM socket connection and send the "Hello" message
            int s, status;
            struct sockaddr_rc addr = { 0 };
            char dest[18] = "XX:XX:XX:XX:XX:XX"; // Replace with the peripheral's MAC address

            s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

            addr.rc_family = AF_BLUETOOTH;
            addr.rc_channel = (uint8_t)1; // RFCOMM channel to use
            str2ba(dest, &addr.rc_bdaddr);

            status = connect(s, (struct sockaddr*)&addr, sizeof(addr));
            if (status < 0) {
                perror("Connection failed");
            }

            // Send the "Hello" message
            send(s, hello_msg, sizeof(hello_msg), 0);

            close(s);
        }
    }

    free(ii);
    close(sock);

    return 0;
}

