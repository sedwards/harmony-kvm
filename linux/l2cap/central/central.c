#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>

#define ATT_CID 4

int main(int argc, char *argv[]) {
    int sock = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
    if (sock == -1) {
        perror("socket");
        return 1;
    }

    struct sockaddr_l2 local_addr = { 0 };
    struct sockaddr_l2 remote_addr = { 0 };
    int bytes_read;
    char buf[1024];

    local_addr.l2_family = AF_BLUETOOTH;
    local_addr.l2_bdaddr_type = BDADDR_LE_PUBLIC;
    str2ba("3C:22:FB:9A:01:4F", &local_addr.l2_bdaddr);

    local_addr.l2_psm = htobs(ATT_CID);

    if (bind(sock, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
        perror("bind");
        close(sock);
        return 1;
    }

    listen(sock, 1);

    socklen_t opt = sizeof(remote_addr);
    int client = accept(sock, (struct sockaddr*)&remote_addr, &opt);

    if (client == -1) {
        perror("accept");
        close(sock);
        return 1;
    }

    ba2str(&remote_addr.l2_bdaddr, buf);
    printf("Accepted connection from %s\n", buf);

    bytes_read = recv(client, buf, sizeof(buf), 0);

    if (bytes_read > 0) {
        buf[bytes_read] = '\0';
        printf("Received: %s\n", buf);
    }

    close(client);
    close(sock);

    return 0;
}

