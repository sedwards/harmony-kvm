#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>

int main(int argc, char *argv[]) {
    struct sockaddr_l2 local_addr = { 0 };
    struct sockaddr_l2 remote_addr = { 0 };
    int s, status;
    char buffer[1024];

    // Create an L2CAP socket
    s = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
    if (s == -1) {
        perror("Socket error");
        return 1;
    }

    // Set up the local address
    local_addr.l2_family = AF_BLUETOOTH;
    local_addr.l2_psm = htobs(0x1001); // Replace with your PSM
    bacpy(&local_addr.l2_bdaddr, BDADDR_ANY);

    status = bind(s, (struct sockaddr*)&local_addr, sizeof(local_addr));
    if (status == -1) {
        perror("Bind error");
        close(s);
        return 1;
    }

    // Set up the remote address
    remote_addr.l2_family = AF_BLUETOOTH;
    remote_addr.l2_psm = htobs(0x1001); // Replace with the server's PSM
    str2ba("3C:22:FB:9A:01:4F", &remote_addr.l2_bdaddr); // Replace with the server's MAC address

#if 0
3C:22:FB:9A:01:4F
B0:35:9F:0D:90:BB
EC:2C:E2:F0:2F:46
70:F0:87:3F:AB:84
64:A2:F9:DD:77:A3
48:4B:AA:EF:4E:DF
#endif

    // Connect to the remote server
    status = connect(s, (struct sockaddr*)&remote_addr, sizeof(remote_addr));
    if (status == -1) {
        perror("Connect error");
        close(s);
        return 1;
    }

    printf("Connected to the server\n");

    // Now you can send and receive data on the socket
    strcpy(buffer, "Hello from the client!");
    status = send(s, buffer, strlen(buffer), 0);
    if (status == -1) {
        perror("Send error");
    } else {
        printf("Data sent: %s\n", buffer);
    }

    // Receive data from the server
    status = recv(s, buffer, sizeof(buffer), 0);
    if (status == -1) {
        perror("Receive error");
    } else {
        buffer[status] = '\0';
        printf("Received data: %s\n", buffer);
    }

    // Close the socket
    close(s);

    return 0;
}

