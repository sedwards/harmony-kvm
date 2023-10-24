#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>

int client; // Shared client socket
pthread_mutex_t client_mutex;

// Function to send a message to the client
void sendHelloMessage() {
    const char *response = "Hello from the server";
    pthread_mutex_lock(&client_mutex);
    write(client, response, strlen(response));
    pthread_mutex_unlock(&client_mutex);
}

// Thread function to handle sending "Hello" messages every 60 seconds
void *sendHelloThread(void *arg) {
    while (1) {
        sleep(30);
        sendHelloMessage();
    }
    return NULL;
}

// Thread function to handle incoming client messages
void *clientThread(void *arg) {
    char buf[1024] = { 0 };
    int bytes_read;

    while (1) {
        pthread_mutex_lock(&client_mutex);
        bytes_read = read(client, buf, sizeof(buf));
        pthread_mutex_unlock(&client_mutex);

        if (bytes_read > 0) {
            printf("received [%s]\n", buf);
        }

	for (ssize_t i = 0; i < bytes_read; i++) {
          printf("%02X ", buf[i]);
        }

    }
    return NULL;
}

int main(int argc, char **argv) {
    struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
    char buf[1024] = { 0 };
    int s;
    socklen_t opt = sizeof(rem_addr);
    pthread_t tid_send, tid_client;

    char mac_addr[] = "b0-35-9f-0d-90-bb";

    pthread_mutex_init(&client_mutex, NULL);

    // Allocate socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // Bind socket to port 1 of the first available local Bluetooth adapter
    loc_addr.rc_family = AF_BLUETOOTH;
    str2ba(mac_addr, &loc_addr.rc_bdaddr);
    loc_addr.rc_channel = (uint8_t) 1;
    bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));

    // Put the socket into listening mode
    listen(s, 1);

    while (1) {
        // Accept one connection
        client = accept(s, (struct sockaddr *)&rem_addr, &opt);

	ba2str(&rem_addr.rc_bdaddr, buf);
        fprintf(stderr, "accepted connection from %s\n", buf);

	// Create a new thread to handle the client's incoming messages
        if (pthread_create(&tid_client, NULL, clientThread, NULL) != 0) {
            perror("pthread_create");
        }

        // Create a new thread to handle sending "Hello" messages
        if (pthread_create(&tid_send, NULL, sendHelloThread, NULL) != 0) {
            perror("pthread_create");
        }
    }

    close(s);
    pthread_mutex_destroy(&client_mutex);
    return 0;
}

