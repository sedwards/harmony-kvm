#include <stdio.h>
#include <stdlib.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/hci_lib.h>
#include <glib.h>
#include <uuid/uuid.h>

#define SERVICE_UUID "F47AC10B-58CC-4372-A567-0E02B2C3D479"

//3D6CC32E-09A2-469D-BB70-C572F43ED547`

//These capitalized UUIDs can be used in your Bluetooth applications." // Replace with your own UUID

int hci_le_set_advertising_data(int dd, uint8_t* data, uint8_t length, int to)
{
  struct hci_request rq;
  le_set_advertising_data_cp data_cp;
  uint8_t status;

  memset(&data_cp, 0, sizeof(data_cp));
  data_cp.length = length;
  memcpy(&data_cp.data, data, sizeof(data_cp.data));

  memset(&rq, 0, sizeof(rq));
  rq.ogf = OGF_LE_CTL;
  rq.ocf = OCF_LE_SET_ADVERTISING_DATA;
  rq.cparam = &data_cp;
  rq.clen = LE_SET_ADVERTISING_DATA_CP_SIZE;
  rq.rparam = &status;
  rq.rlen = 1;

  if (hci_send_req(dd, &rq, to) < 0)
    return -1;

  if (status) {
    errno = EIO;
    return -1;
  }

  return 0;
}

int main() {
    int dev_id = hci_get_route(NULL);
    if (dev_id < 0) {
        perror("Bluetooth adapter not found");
        return 1;
    }

    int sock = hci_open_dev(dev_id);
    if (sock < 0) {
        perror("Can't open socket");
        return 1;
    }

    // Set up advertising parameters
    le_set_advertising_data(sock, 0, NULL);
    le_set_advertising_parameters(sock, 0, 0x1000, 0x1000, 0, 0, 0, 0, 0, 0);

    // Set up service data
    uint8_t service_data[] = {
        0x02, 0x01, 0x06, // Flags
        0x03, 0x03, 0x10, 0x18 // Service UUID
    };
    le_set_advertising_data(sock, sizeof(service_data), service_data);

    // Set up the advertising UUID
    uuid_t my_uuid;
    uuid_parse(SERVICE_UUID, my_uuid);
    if (hci_le_set_advertise_enable(sock, 1, 0) < 0) {
        perror("Failed to enable advertising");
        return 1;
    }

    if (hci_le_add_advertising_data(sock, my_uuid, 0x11, NULL) < 0) {
        perror("Failed to add advertising data");
        return 1;
    }

    if (hci_le_set_advertise_enable(sock, 1, 0) < 0) {
        perror("Failed to enable advertising");
        return 1;
    }

    printf("Advertising service with UUID %s\n", SERVICE_UUID);
    usleep(3000000); // Advertise for 3 seconds

    // Disable advertising
    if (hci_le_set_advertise_enable(sock, 0, 0) < 0) {
        perror("Failed to disable advertising");
    }

    close(sock);
    return 0;
}

