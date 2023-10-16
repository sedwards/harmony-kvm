#include <stdio.h>
#include <stdlib.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

int main() {
    inquiry_info *ii = NULL;
    int max_rsp = 255; // Maximum number of devices to discover
    int num_rsp;

    int dev_id = hci_get_route(NULL); // Get the first available Bluetooth adapter
    int sock = hci_open_dev(dev_id);

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
        char addr[19] = { 0 };
        ba2str(&(ii + i)->bdaddr, addr);
        printf("Device Address: %s\n", addr);
        printf("Device Name: %s\n", (ii + i)->dev_class);
        printf("\n");
    }

    free(ii);
    close(sock);

    return 0;
}

