#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>

#define L2CAP_PSM 0x1001
#define BUFFER_SIZE 1024

void *bt_malloc(size_t size)
{
	return malloc(size);
}

char *batostr(const bdaddr_t *ba)
{
	char *str = bt_malloc(18);
	if (!str)
		return NULL;

	sprintf(str, "%2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X",
		ba->b[0], ba->b[1], ba->b[2],
		ba->b[3], ba->b[4], ba->b[5]);

	return str;
}


int main() {
    int server_socket, client_socket;
    struct sockaddr_l2 local_addr = { 0 };
    struct sockaddr_l2 remote_addr = { 0 };
    socklen_t opt = sizeof(remote_addr);
    char buffer[BUFFER_SIZE];

    // Create a socket
    server_socket = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);

    // Set up the local address
    local_addr.l2_family = AF_BLUETOOTH;
    local_addr.l2_psm = htobs(L2CAP_PSM);
    bacpy(&local_addr.l2_bdaddr, BDADDR_ANY);

    // Bind the socket
    bind(server_socket, (struct sockaddr*)&local_addr, sizeof(local_addr));

    // Listen for connections
    listen(server_socket, 1);
    printf("Listening for L2CAP connections...\n");

    // Accept an incoming connection
    client_socket = accept(server_socket, (struct sockaddr*)&remote_addr, &opt);

    if (client_socket < 0) {
        perror("Accept failed");
        close(server_socket);
        return 1;
    }

    printf("Accepted L2CAP connection from %s\n", batostr(&remote_addr.l2_bdaddr));

    // Receive data from the client
    ssize_t bytes_received;
    while (1) {
        bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received < 0) {
            perror("Receive failed");
            break;
        }
        if (bytes_received == 0) {
            printf("Client disconnected\n");
            break;
        }

        printf("Received: %.*s", (int)bytes_received, buffer);
    }

    // Close the sockets
    close(client_socket);
    close(server_socket);

    return 0;
}

