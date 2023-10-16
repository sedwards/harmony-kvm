#include <stdio.h>
#include <glib.h>
#include <gio/gio.h>

GMainLoop* loop;
GDBusConnection *conn;

#define MY_SERVICE_UUID "12345678-1234-5678-1234-567812345678"
#define MY_CHARACTERISTIC_UUID "87654321-4321-4321-4321-210987654321"

GVariant *on_read(const gchar *interface_name,
                   const gchar *property_name,
                   gpointer user_data)
{
    GVariant *ret = g_variant_new("(ay)", 2, (guchar*)"Hello, BLE!");

    return ret;
}

void on_bus_acquired (GDBusConnection *connection,
                      const gchar     *name,
                      gpointer         user_data)
{
    GError *error = NULL;

    g_dbus_connection_register_object(connection,
        "/com/example/MyApp",
        "org.bluez.GattCharacteristic1",
        G_DBUS_PROPERTY_INFO,
        on_read,
        NULL,
        NULL,
        NULL);

    if (error != NULL) {
        g_printerr("Error registering GATT characteristic: %s\n", error->message);
        g_error_free(error);
        return;
    }

    g_print("GATT service registered\n");
}

void on_name_acquired (GDBusConnection *connection,
                      const gchar     *name,
                      gpointer         user_data)
{
    g_print("Acquired bus name: %s\n", name);
}

void on_name_lost (GDBusConnection *connection,
                  const gchar     *name,
                  gpointer         user_data)
{
    g_print("Lost bus name: %s\n", name);
    g_main_loop_quit(loop);
}

int main (int argc, char *argv[])
{
    GError *error = NULL;

    loop = g_main_loop_new(NULL, FALSE);
    conn = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);

    if (error != NULL) {
        g_printerr("Failed to connect to system bus: %s\n", error->message);
        g_error_free(error);
        return 1;
    }

    guint id = g_bus_own_name(G_BUS_TYPE_SYSTEM,
                "com.example.MyApp",
                G_BUS_NAME_OWNER_FLAGS_NONE,
                on_bus_acquired,
                on_name_acquired,
                on_name_lost,
                NULL,
                NULL);

    g_print("Starting GATT service on DBus...\n");
    g_main_loop_run(loop);

    g_bus_unown_name(id);
    g_object_unref(conn);

    return 0;
}

