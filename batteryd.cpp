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


#include <libnotify/notify.h>
#include <cstdlib> 
#include <fstream> 
#include <unistd.h>
#include <sstream> 
#include <iostream>
#include <sys/stat.h>

using namespace std;

//Begin config section feel free to change these values :)
const int repeater = 60;
const int high = 20;
const int low = 10;
const int critical = 5;
const char* statuspath = "/sys/class/power_supply/BAT0/status";
const char* capacitypath = "/sys/class/power_supply/BAT0/capacity";

const string high_title = "Battery Low";
const string high_message = "The battery level is less than %d percent.\n"
                            "You might want to plug in your computer.";
const string low_title = "Battery Critical";
const string low_message = "The battery level is less than <b>%d percent</b>.\n"
                           "Plug in your computer now.";
//End config section



int main(void)
{
  string scapacity;
  int icapacity;
  string status;
  ifstream infile1;
  ifstream infile2;

  while(true)
  {
    sleep(repeater);

    //Lines for filecheck
    struct stat buffer;
    if( (stat (statuspath, &buffer) == 0) == 0)
    {
      cerr << "no status file" << endl;
      return 1;
    }
    
    if( (stat (capacitypath, &buffer) == 0) == 0)
    {
      cerr << "no capacity file" << endl;
      return 1;
    }
    //End Filecheck 

    infile1.open(statuspath);
    getline(infile1,status);
    infile1.close();
    if(status.compare("Discharging") == 0) {
      infile2.open(capacitypath);
      getline(infile2,scapacity);
      infile2.close();
      stringstream convert(scapacity);
      convert >> icapacity;
      if(icapacity < high && icapacity > low) {
        if(notify_init("batteryd")) {
          ostringstream message;
          message << "Battery level is less than <b>" <<
              icapacity << " percent." << endl <<
              "You might want to plug in your computer.";
          NotifyNotification *notification =
              notify_notification_new(high_title.c_str(),
                                      message.str().c_str(), NULL);
          notify_notification_set_urgency(notification, NOTIFY_URGENCY_NORMAL);
          notify_notification_show(notification, NULL);
          g_object_unref(notification);
          notify_uninit();
        }
      } else if(icapacity < low && icapacity > critical) {
        if(notify_init("batteryd")) {
          ostringstream message;
          message << "Battery level is less than <b>" <<
              icapacity << " percent</b>." << endl <<
              "Plug in your computer now.";
          NotifyNotification *notification =
              notify_notification_new(low_title.c_str(), message.str().c_str(),
                                      NULL);
          notify_notification_set_urgency(notification, NOTIFY_URGENCY_CRITICAL);
          notify_notification_show(notification, NULL);
          g_object_unref(notification);
          notify_uninit();
        }
        cout << "\a";
      } else if (icapacity < critical) {
          system("/usr/bin/systemctl suspend");
      }
    }
  }
  return 0;
}
