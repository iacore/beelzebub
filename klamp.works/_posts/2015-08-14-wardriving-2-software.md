---
layout: post
title:  Wardriving Part 2 - Software
date: 2015-08-14 20:34:32
categories: 
---
Part 1 of this series contains an overview of a practical hardware configuration for wardriving. This part will cover the software component of that setup.

Operating System
===
Something Linux based would be the most popular choice. I went with Raspbian but Kali would be a smart choice since it comes with many of the required pieces preinstalled and configured.

Kismet
===
Kismet is a wireless network detector and sniffer with built in GPS support. Exactly the tool for the job. If you were smart and installed Kali then Kismet should already be avaialable. Unfortunantly Raspbian does not package Kismet for the Raspberry Pi so it must be compiled manually. This is a simple matter of downloading the [source code](https://kismetwireless.net/download.shtml) and running through the typical `./configure`, `make`, `make install` process.

The Kismet configuration file will be located in either `/etc/kismet.conf` or `/usr/local/etc/kismet.conf` depending on how it was installed. Some of the more pertinent configuration options are as follows:

    # ncsource=interface:options 
    ncsource=wlan0 

    gps=true
    gpstype=gpsd
    gpshost=localhost:2947
    gpsreconnect=true

GPS
===
Install the `gpsd` package and start the daemon with the following command:

    gpsd -S2947 /dev/gps0

The `gpsmon` tool from `gpsd-clients` can but used to manually verify that GPS is working. Remember that the GPS hardware requires line of sight with the satillites. If it is not working and you are indoors no where near a window then that may be part of the problem.

Bringing it all togther
===
A simple init script can be used to initialise the various componants at boot time.

    #!/bin/sh
    RETVAL=0
    prog="start_psniff"

    start() {

        ifconfig wlan0 up
        airmon-ng check kill
        killall gpsd
        killall -9 gpsd
        /usr/sbin/gpsd -S2947 /dev/gps0
        /usr/local/bin/kismet_server -s --daemonize -p /home/pi/ --homedir=/home/pi/
    }

    case "$1" in
            start)
                    start
                    ;;
            *)
                    echo $"Usage: $0 {start}"
                    RETVAL=1
    esac
    exit $RETVAL

Call it something like `/etc/init.d/start_psniff.sh` and then put something like the following line in `/etc/inittab`.

    2:23:respawn:/home/miku/start_psniff.sh &

Now whenever the Pi powers on it should start capturing data on nearby wireless networks along with GPS coordinates.

The next part of this series covers analysis of the results.

