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
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>

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
    "The battery level is less than %d percent.\n"
    "You might want to plug in your computer.";
const std::string low_title = "Battery Critical";
const std::string low_message =
    "The battery level is less than <b>%d percent</b>.\n"
    "Plug in your computer now.";
// End config section

int main(void) {
    int capacity;
    std::string status;

    while(true) {
        std::this_thread::sleep_for(SLEEP_TIME);

        // Ensure that the files exist
        std::ifstream status_file(status_path);
        if(!status_file.good()) {
            std::cerr << "no status file" << std::endl;
            exit(1);
        }
        status_file >> status;
        status_file.close();

        if(status == "Discharging") {
            std::ifstream capacity_file(capacity_path);
            if(!capacity_file.good()) {
                std::cerr << "no capacity file" << std::endl;
                exit(1);
            }
            capacity_file >> capacity;
            capacity_file.close();
            if(low < capacity && capacity < high) {
                if(notify_init("batteryd")) {
                    std::ostringstream message;
                    message << "Battery level is less than " <<
                        capacity << " percent." << std::endl <<
                        "You might want to plug in your computer.";
                    NotifyNotification *notification =
                        notify_notification_new(high_title.c_str(),
                                message.str().c_str(), nullptr);
                    notify_notification_set_urgency(notification,
                                                    NOTIFY_URGENCY_NORMAL);
                    notify_notification_show(notification, nullptr);
                    g_object_unref(notification);
                    notify_uninit();
                }
            } else if(critical < capacity && capacity < low) {
                if(notify_init("batteryd")) {
                    std::ostringstream message;
                    message << "Battery level is less than <b>" <<
                        capacity << " percent</b>." << std::endl <<
                        "Plug in your computer now.";
                    NotifyNotification *notification =
                        notify_notification_new(low_title.c_str(),
                                message.str().c_str(), nullptr);
                    notify_notification_set_urgency(notification,
                                                    NOTIFY_URGENCY_CRITICAL);
                    notify_notification_show(notification, nullptr);
                    g_object_unref(notification);
                    notify_uninit();
                }
                std::cout << "\a";
            } else if (capacity < critical) {
                std::system("/usr/bin/systemctl suspend");
            }
        }
    }
}
