
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

#define MAX_RESPONSE_SIZE 255
#define SERVICE_UUID "3C8DC645-7BB0-4F18-8A2A-1413E75D63F6"
#define UUID_16
#define CHAR_UUID

static uint128_t bluetooth_base_uuid = {
	.data = {	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
			0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB }
};

/*
 * converts a 128-bit uuid to a 16/32-bit one if possible
 * returns true if uuid contains a 16/32-bit UUID at exit
 */
int sdp_uuid128_to_uuid(uuid_t *uuid)
{
	uint128_t *b = &bluetooth_base_uuid;
	uint128_t *u = &uuid->value.uuid128;
	uint32_t data;
	unsigned int i;

	if (uuid->type != SDP_UUID128)
		return 1;

	for (i = 4; i < sizeof(b->data); i++)
		if (b->data[i] != u->data[i])
			return 0;

	memcpy(&data, u->data, 4);
	data = htonl(data);
	if (data <= 0xffff) {
		uuid->type = SDP_UUID16;
		uuid->value.uuid16 = (uint16_t) data;
	} else {
		uuid->type = SDP_UUID32;
		uuid->value.uuid32 = data;
	}
	return 1;
}

int main() {
    int sock;
    int status;
    int service_uuid16, *ptr_svc_uuid16;
    int service_uuid128;
    bdaddr_t target;
    sdp_list_t *response_list = NULL, *search_list, *attrid_list;
    uint32_t range = 0x0000ffff;

    uuid_t my_uuid;
    //service_uuid128 = uuid_parse(SERVICE_UUID, my_uuid);
    //service_uuid16 = sdp_uuid128_to_uuid(uuid_t *uuid);
    //service_uuid16 = sdp_uuid128_to_uuid(ptr_service_uuid128);

    // Set the target Bluetooth device address
    str2ba("3C:22:FB:9A:01:4F", &target); // Replace with the device's MAC address
    
    // Initialize the socket
    sock = hci_open_dev(hci_get_route(NULL));
    if (sock < 0) {
        perror("HCI device open failed");
        exit(1);
    }
    
    // Initialize the SDP session
    sdp_session_t *session = sdp_connect(BDADDR_ANY, &target, SDP_RETRY_IF_BUSY);
    if (!session) {
        perror("SDP connection failed");
        close(sock);
        exit(1);
    }

    // Set the UUID of the service you want to connect to
    //sdp_uuid16_create(&service_uuid, 0x180F); // Replace with your desired service UUID
    sdp_uuid16_create(&svc_uuid, "0x420G"); // Replace with your desired service UUID

    search_list = sdp_list_append(NULL, &svc_uuid);
    attrid_list = sdp_list_append(NULL, &range);

    // Perform the SDP service search
    status = sdp_service_search_attr_req(session, search_list, SDP_ATTR_REQ_RANGE, attrid_list, &response_list);
    if (status == 0) {
        printf("Service search was successful.\n");

        // Process the response_list to retrieve information about the service
        // ...

        // Close the SDP session
        sdp_close(session);
    } else {
        perror("Service search failed");
        sdp_close(session);
        close(sock);
        exit(1);
    }

    // Close the socket
    close(sock);

    return 0;
}

