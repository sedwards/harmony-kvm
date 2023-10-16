#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus.h>

int main(int argc, char **argv) {
    DBusError err;
    dbus_error_init(&err);

    DBusConnection *conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);

    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Connection Error (%s)\n", err.message);
        dbus_error_free(&err);
    }

    // Create a new D-Bus object manager and service
    const char *service_name = "com.example.MyApp";
    const char *object_path = "/com/example/MyApp";

    if (!dbus_bus_request_name(conn, service_name, 0, &err)) {
        fprintf(stderr, "Name Error (%s)\n", err.message);
        dbus_error_free(&err);
    }

    // Create and register a GATT service
    const char *gatt_service_path = "/com/example/MyApp/service001";
    dbus_uint16_t gatt_handle = 1;

    dbus_uint16_t primary_service_uuid = 0x1800; // Use the Generic Access Service UUID

    dbus_uint16_t *uuids = (dbus_uint16_t *)malloc(sizeof(dbus_uint16_t) * 2);
    uuids[0] = primary_service_uuid;
    uuids[1] = 0; // Null-terminated array of UUIDs

    if (!dbus_object_manager_server_export_gatt_service(object_path, gatt_service_path, gatt_handle, uuids)) {
        fprintf(stderr, "Failed to export GATT service\n");
    } else {
        printf("GATT service registered\n");
    }

    // Create and register a GATT characteristic
    const char *characteristic_path = "/com/example/MyApp/service001/char001";
    dbus_uint16_t characteristic_handle = 2;

    dbus_uint16_t characteristic_uuid = 0x2A00; // Use the Device Name characteristic UUID
    dbus_uint16_t permissions = 0x01; // Readable
    //dbus_uint8_t permissions = 0x01; // Readable

    if (!dbus_object_manager_server_export_gatt_characteristic(gatt_service_path, characteristic_path, characteristic_handle, characteristic_uuid, permissions)) {
        fprintf(stderr, "Failed to export GATT characteristic\n");
    } else {
        printf("GATT characteristic registered\n");
    }

    // Continue handling D-Bus events

    return 0;
}
/*
 * n the provided example, the primary_service_uuid and characteristic_uuid are represented as 16-bit UUIDs (Universally Unique Identifiers) in their abbreviated format. These UUIDs are standardized and defined by the Bluetooth SIG (Special Interest Group) for commonly used services and characteristics.

UUIDs are typically 128 bits long and expressed as a hexadecimal number. However, for common services and characteristics, it's common to use the abbreviated 16-bit form, which is shorter and easier to work with in code. The full 128-bit UUID can be reconstructed from the abbreviated 16-bit form by adding the Bluetooth base UUID (00000000-0000-1000-8000-00805F9B34FB) as a prefix.

Here's how you can convert a 16-bit UUID to its full 128-bit form:

Take the abbreviated 16-bit UUID, e.g., 0x1800 for primary_service_uuid.
Add the Bluetooth base UUID as a prefix: 00000000-0000-1000-8000-00805F9B34FB.
Combine the two parts to form the full 128-bit UUID: 00001800-0000-1000-8000-00805F9B34FB.
So, in the case of primary_service_uuid, 0x1800 represents the Generic Access Service UUID with the full 128-bit form 00001800-0000-1000-8000-00805F9B34FB. Similarly, 0x2A00 for characteristic_uuid is the abbreviated form of the Device Name characteristic with the full 128-bit form 00002A00-0000-1000-8000-00805F9B34FB.

These abbreviated UUIDs are used for common services and characteristics to make the code more concise. If you need to work with a specific 128-bit UUID, you can use the full form directly. The full 128-bit UUID is unique and defines a specific Bluetooth service or characteristic.

 */
