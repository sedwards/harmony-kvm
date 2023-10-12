#include <gio/gio.h>
#include <glib.h>

static GDBusProxy *proxy;
static GMainLoop *main_loop;

// Callback for handling incoming BLE notifications
static void handleNotification(GDBusProxy *proxy, GBytes *value, const char *sender, const char *object_path, const char *interface_name, const char *signal_name, GVariant *parameters, gpointer user_data) {
    gsize size;
    const guint8 *data = g_bytes_get_data(value, &size);

    // Process the received data (e.g., mouse or keyboard events)
    // You can implement event handling logic here

    // For example, print the received data
    g_print("Received notification: ");
    for (gsize i = 0; i < size; i++) {
        g_print("%02X ", data[i]);
    }
    g_print("\n");
}

int main(int argc, char *argv[]) {
    GError *error = NULL;

    // Initialize GLib and GIO
    g_type_init();

    // Initialize GIO
    g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, NULL, &error);
    if (error != NULL) {
        g_error("Error initializing GIO: %s\n", error->message);
        g_error_free(error);
        return 1;
    }

    // Create a D-Bus proxy for the macOS application
    proxy = g_dbus_proxy_new_for_bus_sync(
        G_BUS_TYPE_SYSTEM,
        G_DBUS_PROXY_FLAGS_NONE,
        NULL,
        "your.bluetooth.service",
        "your.bluetooth.service.path",
        "your.bluetooth.service.interface",
        NULL,
        &error
    );

    if (error != NULL) {
        g_error("Error creating D-Bus proxy: %s\n", error->message);
        g_error_free(error);
        return 1;
    }

    // Register the callback for BLE notifications
    g_signal_connect(proxy, "g-signal", G_CALLBACK(handleNotification), NULL);

    // Start the GIO main loop
    main_loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(main_loop);

    return 0;
}

