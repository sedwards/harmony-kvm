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
    uuid_t svc_uuid, rfcomm_uuid;
    sdp_list_t *l2cap_list = 0, *rfcomm_list = 0, *root_list = 0;
    uint8_t rfcomm_channel = 1; // RFCOMM channel to listen on

    char buf[1024] = { 0 };

    // Create a socket
    int s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // Set up the server information
    struct sockaddr_rc loc_addr = { 0 };
    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = *BDADDR_ANY; // Use the first available Bluetooth adapter
    loc_addr.rc_channel = (uint8_t)rfcomm_channel;

    // Bind the socket
    bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));
#if 0
    // Register the service
    sdp_session_t *session = 0;
    sdp_record_t *record = sdp_record_alloc();

    // Service class and profile
    sdp_uuid128_create(&svc_uuid, PUBLIC_BROWSE_GROUP);
    sdp_set_service_id(record, svc_uuid);

    // Create a new list for the service record
    root_list = sdp_list_append(0, record);

    // Create the service record for RFCOMM
    sdp_uuid16_create(&rfcomm_uuid, 0x2A37);
    rfcomm_list = sdp_list_append(0, &rfcomm_uuid);
    sdp_list_append(rfcomm_list, sdp_data_alloc(SDP_UINT8, &rfcomm_channel));
    sdp_list_append(root_list, sdp_data_alloc(SDP_SEQ_UNSPEC, rfcomm_list));

    // Create a new list for L2CAP
    l2cap_list = sdp_list_append(0, sdp_data_alloc(SDP_UUID16, L2CAP_UUID));
    sdp_list_append(root_list, sdp_data_alloc(SDP_SEQ_UNSPEC, l2cap_list));

    // Attach the service record to the session
    session = sdp_connect(&loc_addr, &loc_addr, SDP_RETRY_IF_BUSY);
    sdp_record_register(session, root_list, 0);
#endif
    // Put the server into listening mode
    listen(s, 1);

    // Accept an incoming connection
    socklen_t opt = sizeof(loc_addr);
    int client = accept(s, (struct sockaddr *)&loc_addr, &opt);

    // Receive data from the client
    while (1) {
        memset(buf, 0, sizeof(buf));
        int bytes_read = recv(client, buf, sizeof(buf), 0);
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

