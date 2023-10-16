#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <sys/socket.h>

#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

int main(int argc, char **argv) {
    struct sockaddr_rc loc_addr = { 0 };
    int s, client, bytes_read;
    char buf[1024] = { 0 };
    int port = 1; // RFCOMM channel to listen on

    // Create a socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // Set up the server information
    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = *BDADDR_ANY; // Use the first available Bluetooth adapter
    loc_addr.rc_channel = (uint8_t)port;

    // Bind the socket
    bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));

    // Put the server into listening mode
    listen(s, 1);

    // Create a unique service UUID for your RFCOMM service
    //uuid_t svc_uuid;
    //sdp_uuid128_create(&svc_uuid, &YOUR_SERVICE_UUID); // Replace YOUR_SERVICE_UUID with your UUID

uuid_t svc_uuid;
// Define your UUID in 128-bit format
uint8_t service_uuid[] = {0x00, 0x00, 0x2A, 0x37, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB};
sdp_uuid128_create(&svc_uuid, service_uuid);

    // Initialize the service record
    sdp_record_t record = { 0 };
    sdp_set_service_id(&record, svc_uuid);
    sdp_list_t *service_class_list = 0;
    sdp_list_t *channel_list = 0;
    sdp_data_t *channel = 0;

    service_class_list = sdp_list_append(0, &svc_uuid);
    sdp_set_service_classes(&record, service_class_list);

    channel = sdp_data_alloc(SDP_UINT8, &port);
    channel_list = sdp_list_append(0, channel);
    //sdp_set_rfcomm_channel(&record, port);
    sdp_set_access_protos(&record, channel_list);

    // Register the service record
    sdp_session_t *session = 0;
    session = sdp_connect(&loc_addr, BDADDR_ANY, SDP_RETRY_IF_BUSY);
    sdp_record_register(session, &record, 0);

    // Accept an incoming connection
    socklen_t opt = sizeof(loc_addr);
    client = accept(s, (struct sockaddr *)&loc_addr, &opt);

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

