/* Batteryd - A simple battery daemon for Archlinux
 *
 * Copyright (c) 2013 by Christian Rebischke <echo Q2hyaXMuUmViaXNjaGtlQGdtYWlsLmNvbQo= | base64 -d>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/
 *
 *====================================================================== 
 * Author: Christian Rebischke
 * Email : echo Q2hyaXMuUmViaXNjaGtlQGdtYWlsLmNvbQo= | base64 -d
 * Github: www.github.com/Shibumi
 *
 * How to compile:
 * g++ batteryd.cpp -o batteryd $(pkg-config --cflags --libs libnotify)
 *
 * vim: set ts=2 sts=2 sw=2 et
 */


#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>

#include <boost/format.hpp>
#include <gio/gio.h>
#include <libnotify/notify.h>

// Begin config section feel free to change these values :)
const std::chrono::seconds SLEEP_TIME(60);
const int high = 20;
const int low = 10;
const int critical = 5;
const std::string status_path = "/sys/class/power_supply/BAT0/status";
const std::string capacity_path = "/sys/class/power_supply/BAT0/capacity";

const std::string high_title = "Battery Low";
const std::string high_message =
    "The battery level is less than %1% percent.\n"
    "You might want to plug in your computer.";
const std::string low_title = "Battery Critical";
const std::string low_message =
    "The battery level is less than <b>%1% percent</b>.\n"
    "Plug in your computer now.";
// End config section

enum BatteryStatus { CHARGING, DISCHARGING };

/*
 * Suspend the computer by sending a message to logind.
 */
void suspend() {
    GError *error = nullptr;

    GDBusConnection *connection =
        g_bus_get_sync(
                G_BUS_TYPE_SYSTEM,
                nullptr,    /* cancellable */
                &error);

    GVariant *return_values =
        g_dbus_connection_call_sync(
                connection,
                "org.freedesktop.login1",
                "/org/freedesktop/login1",
                "org.freedesktop.login1.Manager",
                "Suspend",
                g_variant_new("(b)", "true"),
                NULL,
                G_DBUS_CALL_FLAGS_NONE,
                -1,
                NULL,
                &error);

    g_variant_unref(return_values);
    g_object_unref(connection);
}

void send_notification(std::string title, std::string message,
                       NotifyUrgency urgency) {
    NotifyNotification *notification =
        notify_notification_new(
                high_title.c_str(),
                message.c_str(),
                nullptr);

    notify_notification_set_urgency(notification, urgency);
    notify_notification_show(notification, nullptr);
    g_object_unref(notification);
    notify_uninit();
}

/*
 * Display a warning to the user on low battery.
 */
void emit_battery_warning(int capacity, bool critical) {
    using boost::format;
    using boost::str;

    if(notify_init("batteryd")) {
        if (critical) {
            std::string message = str(format(low_message) % capacity);
            send_notification(low_title, message, NOTIFY_URGENCY_CRITICAL);
        } else {
            std::string message = str(format(high_message) % capacity);
            send_notification(high_title, message, NOTIFY_URGENCY_NORMAL);
        }
    }
}

BatteryStatus read_battery_status() {
    std::string status;

    // Ensure that the file exists.
    std::ifstream status_file(status_path);
    if(!status_file.good()) {
        std::cerr << "no status file" << std::endl;
        exit(EXIT_FAILURE);
    }

    status_file >> status;
    status_file.close();

    return status == "Discharging" ? DISCHARGING : CHARGING;
}

int read_battery_capacity() {
    int capacity;

    std::ifstream capacity_file(capacity_path);
    if(!capacity_file.good()) {
        std::cerr << "no capacity file" << std::endl;
        exit(EXIT_FAILURE);
    }

    capacity_file >> capacity;
    capacity_file.close();

    return capacity;
}

int main(void) {
    while(true) {
        std::this_thread::sleep_for(SLEEP_TIME);

        BatteryStatus status = read_battery_status();

        if(status == DISCHARGING) {
            int capacity = read_battery_capacity();
            if(low < capacity && capacity < high) {
                emit_battery_warning(capacity, false);
            } else if(critical < capacity && capacity < low) {
                emit_battery_warning(capacity, true);
                std::cout << "\a" << std::endl;
            } else if (capacity < critical) {
                suspend();
            }
        }
    }
}
