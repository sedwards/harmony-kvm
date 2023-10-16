#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/hci_event.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

int main(int argc, char** argv) {
    int dev_id = hci_get_route(NULL);
    int sock;
    struct hci_dev_info dev_info;
    bdaddr_t bdaddr_any = {{0, 0, 0, 0, 0, 0}};
    bdaddr_t bdaddr_local = {{0, 0, 0, 0xff, 0xff, 0xff}};
    struct hci_version ver;
    struct hci_conn_info_req *cr;
    int addr, opt;
    struct sockaddr_l2 loc_addr = { 0 };
    struct sockaddr_l2 rem_addr = { 0 };
    socklen_t optlen = sizeof(opt);
    int s, client, bytes_read;
    char buf[1024] = { 0 };

    // Set up HCI socket
    sock = hci_open_dev(dev_id);
    if (dev_id < 0 || sock < 0) {
        perror("Error opening socket");
        exit(1);
    }

    // Check HCI version
    if (hci_read_local_version(sock, &ver, 0) < 0) {
        perror("Can't get version info");
        exit(1);
    }

    // Set up local Bluetooth adapter
    bacpy(&loc_addr.l2_bdaddr, BDADDR_ANY);
    loc_addr.l2_family = AF_BLUETOOTH;
    loc_addr.l2_psm = htobs(0x1001);

    // Bind the socket
    if (bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    // Put the server into listening mode
    if (listen(s, 1) < 0) {
        perror("Listen failed");
        exit(1);
    }

    // Accept an incoming connection
    socklen_t opt = sizeof(rem_addr);
    client = accept(s, (struct sockaddr *)&rem_addr, &opt);

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

