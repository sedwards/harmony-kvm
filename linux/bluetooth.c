#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
//#include <bluetooth/gatt.h>

#include <glib.h>
#include <gio/gio.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <device_address> <characteristic_uuid>\n", argv[0]);
        return 1;
    }

    const char *device_address = argv[1];
    const char *characteristic_uuid = argv[2];

    bdaddr_t bdaddr;
    str2ba(device_address, &bdaddr);
    // Open the Bluetooth HCI socket and initialize the GATT connection.
    int hci_device_id = hci_get_route(NULL);
    int hci_socket = hci_open_dev(hci_device_id);

    if (hci_socket < 0) {
        perror("Failed to open HCI socket");
       return 1;
    }
    uint16_t handle;
    if (hci_le_create_conn(
        hci_socket,
        0x18,             // interval
        0x18,             // window
        0,                // initiator_filter
        0,                // peer_bdaddr_type
        bdaddr,           // peer_bdaddr
        0x01,             // own_bdaddr_type (usually 0x01 for public)
        0x0006,           // min_interval (adjust as needed)
        0x0006,           // max_interval (adjust as needed)
        0x0000,           // latency
        0x00a0,           // supervision_timeout (e.g., 100ms)
        0x0000,           // min_ce_length
        0x0000,           // max_ce_length
        &handle,
        1000             // to (timeout in milliseconds)
    ) < 0) {

        perror("Failed to create LE connection");
        return 1;
    }

    struct hci_conn_info_req conn_info;
    socklen_t len = sizeof(conn_info);
    int result = ioctl(hci_socket, HCIGETCONNINFO, &conn_info);
    if (result < 0) {
       perror("Failed to get connection info");
       return 1;
    }

    // Use the GATT D-Bus API to read from the characteristic.
    GDBusConnection *connection;
    GDBusMessage *message, *reply;
    GError *error = NULL;

    connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, NULL);
    if (!connection) {
        fprintf(stderr, "Failed to connect to the D-Bus system bus.\n");
        return 1;
    }

    char device_name[248];  // Assuming a maximum device name length of 248 characters
    device_name[0] = '\0';  // Initialize the name buffer

    // Get the device name
    if (hci_read_remote_name(hci_socket, &bdaddr, sizeof(device_name), device_name, 0) < 0) {
      perror("Failed to get device name");
      return 1;
    }

    // Build the D-Bus message to read from the characteristic.
    message = g_dbus_message_new_method_call(
    device_name,
    "/org/bluez/hci0/dev_XX_XX_XX_XX_XX_XX/serviceXX",
    "org.bluez.GattCharacteristic1",
    "ReadValue"
    );

    g_dbus_message_set_body(message, g_variant_new("(a{sv})", NULL));

    reply = g_dbus_connection_send_message_with_reply_sync(
        connection,
        message,
        G_DBUS_SEND_MESSAGE_FLAGS_NONE,
        -1,
        NULL,
        NULL,
        &error
    );

    if (error) {
        fprintf(stderr, "Error reading characteristic: %s\n", error->message);
        g_error_free(error);
        return 1;
    }

    GVariant *value;
    g_variant_get(reply, "(@ay)", &value);

    // Convert the received value to a string.
    const guint8 *data;
    gsize len_data;
    data = g_variant_get_fixed_array(value, &len_data, sizeof(guint8));

    printf("Received data: ");
    for (gsize i = 0; i < len_data; i++) {
        printf("%02X ", data[i]);
    }
    printf("\n");

    g_variant_unref(reply);
    g_object_unref(connection);

    // Close the Bluetooth HCI socket.
    close(hci_socket);

    return 0;
}

