#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include <gio/gio.h>
#include <glib.h>
#include <dbus/dbus.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>
#include "../bluez/client/gatt.h"

#define MAX_GATT_CHARACTERISTICS 100
#define ATT_CID			4

int main() {
    const char* target_mac_address = "B0:35:9F:0D:90:BB";
    const char* service_uuid = "3C8DC645-7BB0-4F18-8A2A-1413E75D63F6";

  int dev_id = hci_devid(target_mac_address);

  struct hci_dev_info *di = (struct hci_dev_info *)malloc(sizeof(struct hci_dev_info));
  int rtn = hci_devinfo(dev_id, di);
  printf("%s\n", di->name);


    //struct sockaddr_l2 addr = { 0 };
    int s; //, dev_id;
    char buffer[1024];
    // Set the BLE device ID (hciX)
    //dev_id = hci_devid(target_mac_address);
    if (dev_id < 0) {
        perror("Device not available");
        // return 1;
    }

    // Open HCI socket
    s = hci_open_dev(dev_id);
    if (s < 0) {
        perror("HCI device open failed");
        // return 1;
    }

    // Set up the GATT connection
    struct sockaddr_l2 l2_addr;
    bacpy(&l2_addr.l2_bdaddr, BDADDR_ANY);
    l2_addr.l2_family = AF_BLUETOOTH;
//    l2_addr.l2_psm = htobs(ATT_CID);
    l2_addr.l2_psm = htobs(0x001f);
    l2_addr.l2_cid = 0;
    l2_addr.l2_bdaddr_type = BDADDR_LE_PUBLIC;

    if (hci_le_set_scan_parameters(s, 0x01, 0x0010, 0x0010, 0x00, 0x00, 1000) < 0) {
        perror("Set scan parameters failed");
        close(s);
        return 1;
    }

    if (hci_le_set_scan_enable(s, 0x01, 1, 1000) < 0) {
        perror("Set scan enable failed");
        close(s);
        return 1;
    }

    evt_le_meta_event *meta;
    le_advertising_info *info;
    char addr[18];

    while (1) {
        int len = read(s, buffer, sizeof(buffer));
        meta = (evt_le_meta_event *)(buffer + (1 + HCI_EVENT_HDR_SIZE));
        if (meta->subevent != 0x02)
            continue;
        info = (le_advertising_info *)(meta->data + 1);
        ba2str(&info->bdaddr, addr);
        if (strcmp(addr, target_mac_address) == 0) {
            char* service_uuid_str = NULL;
            int service_uuid_len = 0;
            for (int i = 0; i < info->length; i++) {
                uint8_t field_len = info->data[i];
                uint8_t field_type = info->data[i + 1];
                if (field_type == 0x07) { // Complete List of 128-bit Service Class UUIDs
                    service_uuid_len = field_len - 1;
                    service_uuid_str = (char*)malloc(service_uuid_len * 2 + 1);
                    for (int j = 0; j < service_uuid_len; j++) {
                        sprintf(&service_uuid_str[j * 2], "%02X", info->data[i + 2 + j]);
                    }
                    service_uuid_str[service_uuid_len * 2] = '\0';
                }
                i += field_len;
            }
            if (service_uuid_str != NULL && strcmp(service_uuid_str, service_uuid) == 0) {
                // Service UUID match, connect to the device
                addr[17] = '\0'; // Null-terminate the address

                s = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
                l2_addr.l2_cid = htobs(0x0040);
                bacpy(&l2_addr.l2_bdaddr, &info->bdaddr);
                if (connect(s, (struct sockaddr*)&l2_addr, sizeof(l2_addr)) < 0) {
                    perror("Connect failed");
                    return 1;
                }
                printf("Connected to: %s\n", addr);
                free(service_uuid_str);
                close(s); // You can send/receive data using the socket s
                break;
            }
            if (service_uuid_str != NULL) {
                free(service_uuid_str);
            }
        }
    }

    // Close the HCI socket
    close(s);

    return 0;
}

/* 
 * #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sdp.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Bluetooth MAC Address>\n", argv[0]);
        return 1;
    }

    struct sockaddr_l2 addr = { 0 };  // Declaration of addr as a sockaddr_l2

    // Set up the address
    str2ba(argv[1], &addr.l2_bdaddr);

    // Rest of your code...

    return 0;
}
*/
