---
layout: post
title:  "Xen Console Woes with Paravirtualised Guest"
date:   2015-06-05 20:16:12
categories: 
---

I have ran into this problem numerous times now, the problem of Linux going completely silent half way through the boot sequence for no apparent reason. The machine has not crashed because I can still SSH in from across the network. But the serial console is completely dead. This has happened to me a number of times both when connected to a real device running embedded Linux through a physical serial cable and when connecting to a paravirtualised Linux guest running inside Xen through Xen's virtual serial console.

{% highlight bash %}
 * Bringing up network interface lo ...
 [ ok ]
 * Mounting misc binary format filesystem ...
 [ ok ]
 * Loading custom binary format handlers ...
 [ ok ]
 * setting up tmpfiles.d entries ...
 [ ok ]
 * Initializing random number generator ...
 [ ok ]
INIT: Entering runlevel: 3
 * Bringing up interface eth0
 *   192.168.0.27 ...
 [ ok ]
 *   Adding routes
 *     default gw 192.168.0.1 ...
 [ ok ]
 * Starting sshd ...
 [ ok ]
 * Starting local ...
 [ ok ]
{% endhighlight %}

The solution is to make sure that the getty deamon is spawning correctly and with suitable parameters set in `/etc/inittab`; specifically baud rate and the serial device name. 

Below is an example for Xen where `9600` is the baud rate and `hvc0` is the name of the virtual serial port i.e. `/dev/hvc0`.
{% highlight bash %}
h0:12345:respawn:/sbin/agetty 9600 hvc0 screen
{% endhighlight %}

Chances are that if your login screen is not appearing at all then this line is either missing from `/etc/inittab` or the baud rate or serial port name are missing.

If you are interested in learning what the rest of the line means consult the relevant man pages: `inittab` and `agetty`.
