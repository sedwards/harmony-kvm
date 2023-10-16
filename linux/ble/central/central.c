#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>
#include <uuid/uuid.h>
//#include <bluetooth/gatt.h>

#define HCIGETCONNINFO  0x80204208

void print_advertising_data(uint8_t *data, size_t size) {
    for (size_t i = 0; i < size; ) {
        uint8_t field_length = data[i];
        if (field_length == 0 || i + field_length >= size) {
            break;
        }
        printf("  Type 0x%02X: ", data[i + 1]);
        for (size_t j = 0; j < field_length - 1; j++) {
            printf("%02X ", data[i + j + 2]);
        }
        printf("\n");
        i += field_length + 1;
    }
}

int hci_create_connection(int dd, const bdaddr_t *bdaddr, uint16_t ptype, uint16_t clkoffset, uint8_t rswitch, uint16_t *handle, int to);


static void notification_handler(uint16_t handle, uint8_t *data, uint16_t size, void *user_data) {
    printf("Received data: %.*s\n", size, data);
}

int main() {
    inquiry_info *ii = NULL;
    int max_rsp = 255; // Maximum number of devices to discover
    int num_rsp;
    int dev_id, sock;

    dev_id = hci_get_route(NULL);
    sock = hci_open_dev(dev_id);

    if (dev_id < 0 || sock < 0) {
        perror("Cannot open Bluetooth socket");
        exit(1);
    }

    ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));

    num_rsp = hci_inquiry(dev_id, 8, max_rsp, NULL, &ii, IREQ_CACHE_FLUSH);

    if (num_rsp < 0) {
        perror("Bluetooth inquiry failed");
        exit(1);
    }

    for (int i = 0; i < num_rsp; i++) {
        char addr[19];
        ba2str(&(ii + i)->bdaddr, addr);
        printf("Discovered device: %s\n", addr);

        // Assuming you want to connect to a specific peripheral, replace the condition with the target MAC address.
      //  if (strcmp(addr, "3C:22:FB:9A:01:4F") == 0) {
            // Establish a connection to the peripheral
            int s;
            s = hci_open_dev(dev_id);

            if (s < 0) {
                perror("Cannot open device");
                continue;
            }

            struct hci_conn_info_req req;
            bdaddr_t bdaddr;

            str2ba("3C:22:FB:9A:01:4F", &bdaddr); // Replace with the peripheral's MAC address

            bacpy(&req.bdaddr, &bdaddr);
            if (ioctl(s, HCIGETCONNINFO, (unsigned long) &req) < 0) {
                close(s);
                perror("Connection error");
                continue;
            }

            // Connect to the GATT server on the peripheral
            uint16_t handle;
            uint8_t notification[] = { 0x01, 0x00 };

            if (hci_le_set_scan_parameters(s, 0x01, 0x00, 0x10, 0x10, 0x00, 1000) < 0) {
                close(s);
                perror("Set scan parameters failed");
                continue;
            }

            if (hci_le_set_scan_enable(s, 0x01, 0, 1000) < 0) {
                close(s);
                perror("Enable scan failed");
                continue;
            }

            le_set_scan_enable_cp scan_cp;
            uint8_t enable = 0x01;
            scan_cp.enable = enable;
            scan_cp.filter_dup = 0x00;

            if (hci_send_cmd(s, 0x08, 0x000c, sizeof(scan_cp), &scan_cp) < 0) {
                close(s);
                perror("Scan command failed");
                continue;
            }

            evt_le_meta_event *meta;
            while (1) {
                uint8_t buf[HCI_MAX_EVENT_SIZE];
                int len = read(s, buf, sizeof(buf));
                meta = (evt_le_meta_event*)(buf + (1 + HCI_EVENT_HDR_SIZE));
                if (meta->subevent != 0x02) {
                    continue;
                }

                le_advertising_info *info;
                uint8_t *ptr = meta->data;

                if (!len) {
                    break;
                }

                info = (le_advertising_info *)ptr;
                print_advertising_data(info->data, info->length);

                if (info->evt_type != 0x02 && info->bdaddr_type != 0x01) {
                    continue;
                }

                if (bacmp(&info->bdaddr, &bdaddr) == 0) {
                    // Found the target peripheral, try to connect and exchange data
                    struct sockaddr_l2 addr = { 0 };
                    struct hci_filter nf, of;
                    socklen_t olen;
                    int n, opt;

                    printf("Found the target peripheral: %s\n", addr);

                    hci_close_dev(s);
                    s = hci_open_dev(dev_id);

                    if (s < 0) {
                        perror("Cannot open device");
                        break;
                    }
#if 0
                    if (hci_create_conn(s, &bdaddr, htobs(0x0800), htobs(0x0010), htobs(0x0038), htobs(0x000F), 0, htobs(0x0000), &handle, 0) < 0) {
                        perror("Connect failed");
                        break;
                    }
#endif
                    // Add GATT characteristics handling
                    // Replace MY_CHARACTERISTIC_UUID with the actual UUID
                    char* MY_CHARACTERISTIC_UUID = "1B10E3C4-E651-4E4D-ACFE-387D2F7A18BC";

                    int le = hci_open_dev(dev_id);
                    int err = 0;
                    uint16_t svc, chr, data_len;
#if 0
                    err = ble_connect(le, addr);
                    if (err < 0) {
                        printf("Error connecting to peripheral: %d\n", err);
                        close(s);
                        break;
                    }

                    // Discover services and characteristics
                    err = ble_discoverservice(le, addr, 1, &svc, &data_len);
                    if (err < 0) {
                        printf("Error discovering services: %d\n", err);
                        close(s);
                        break;
                    }

                    err = ble_discovercharacteristic(le, addr, svc, MY_CHARACTERISTIC_UUID, &chr, &data_len);
                    if (err < 0) {
                        printf("Error discovering characteristics: %d\n", err);
                        close(s);
                        break;
                    }

                    // Write and Read data to the characteristic
                    uint8_t value[] = "Hello";
                    err = ble_writewithresponse(le, addr, chr, value, sizeof(value), &data_len);
                    if (err < 0) {
                        printf("Error writing data: %d\n", err);
                        close(s);
                        break;
                    }

                    // Read data
                    err = ble_read(le, addr, chr, &data_len);
                    if (err < 0) {
                        printf("Error reading data: %d\n", err);
                        close(s);
                        break;
                    }

                    char data[data_len];
                    err = ble_read(le, addr, chr, &data_len, data);
                    if (err < 0) {
                        printf("Error reading data: %d\n", err);
                        close(s);
                        break;
                    }

                    printf("Received: %.*s\n", data_len, data);
#endif
                    close(s);
                    break;
                }
//            }
        }
    }

    free(ii);
    hci_close_dev(sock);

    return 0;
}



int hci_create_connection(int dd, const bdaddr_t *bdaddr, uint16_t ptype,
                                uint16_t clkoffset, uint8_t rswitch,
                                uint16_t *handle, int to)
{
        evt_conn_complete rp;
        create_conn_cp cp;
        struct hci_request rq;

        memset(&cp, 0, sizeof(cp));
        bacpy(&cp.bdaddr, bdaddr);
        cp.pkt_type       = ptype;
        cp.pscan_rep_mode = 0x02;
        cp.clock_offset   = clkoffset;
        cp.role_switch    = rswitch;

        memset(&rq, 0, sizeof(rq));
        rq.ogf    = OGF_LINK_CTL;
        rq.ocf    = OCF_CREATE_CONN;
        rq.event  = EVT_CONN_COMPLETE;
        rq.cparam = &cp;
        rq.clen   = CREATE_CONN_CP_SIZE;
        rq.rparam = &rp;
        rq.rlen   = EVT_CONN_COMPLETE_SIZE;

        if (hci_send_req(dd, &rq, to) < 0)
                return -1;

        if (rp.status) {
                errno = EIO;
                return -1;
        }

        *handle = rp.handle;
        return 0;
}

