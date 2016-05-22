---
layout: post
title: Mounting Raw Images
date: 2015-09-25 20:13:56
categories: 
permalink: mounting-raw-images
---
Mounting raw disk images is one of those fiddly things that needs doing every now and then but it can be easy to forget the precise details of how to deal with different disk partitions.

Lets take raw virtual machine image as an example `/var/lib/libvirt/images/Debian-8.img`.

A naive attempt to mount will fail because it is not a single filesystem but is composed of several partitions of filesystems.

{% highlight bash %}
$ mount /var/lib/libvirt/images/Debian-8.img
mount: /dev/loop1 is write-protected, mounting read-only
mount: wrong fs type, bad option, bad superblock on /dev/loop1,
       missing codepage or helper program, or other error

       In some cases useful info is found in syslog - try
       dmesg | tail or so.
{% endhighlight %}

As a first port of call lets try typing [man mount](http://bash.org/?15970) and Read The [Fun Manual](http://linux.die.net/man/8/mount). Notice that there is an `offset` option mentioned:

>This type of mount knows about four options, namely loop, offset, sizelimit and encryption, that are really options to losetup(8). (These options can be used in addition to those specific to the filesystem type.) 

And if we look at [losetup](http://linux.die.net/man/8/losetup):

>-o, --offset offset<br>
>the data start is moved offset bytes into the specified file or device 

So how do we figure out which value we need for each partition? Lets try running `fdisk` and pressing `p` at the prompt in order to print the partition table and then `q` to quit.

{% highlight bash %}
$ fdisk '/var/lib/libvirt/images/Debian-8.img'

Welcome to fdisk (util-linux 2.25.2).
Changes will remain in memory only, until you decide to write them.
Be careful before using the write command.


Command (m for help): p
Disk /var/lib/libvirt/images/Debian-8.img: 8 GiB, 8589934592 bytes, 16777216 sectors
Units: sectors of 1 * 512 = 512 bytes
Sector size (logical/physical): 512 bytes / 512 bytes
I/O size (minimum/optimal): 512 bytes / 512 bytes
Disklabel type: dos
Disk identifier: 0x000a0716

Device                                Boot   Start      End  Sectors  Size Id Type
/var/lib/libvirt/images/Debian-8.img1 *       2048  1026047  1024000  500M 83 Linux
/var/lib/libvirt/images/Debian-8.img2      1026048  2703359  1677312  819M 82 Linux sw
/var/lib/libvirt/images/Debian-8.img3      2703360 16777215 14073856  6.7G 83 Linux


Command (m for help): q
{% endhighlight %}

Great so the first partition is at offset `2048`? Lets give it a try.

{% highlight bash %}
$ mount /var/lib/libvirt/images/Debian-8.img -o offset=2048
mount: /dev/loop1 is write-protected, mounting read-only
mount: wrong fs type, bad option, bad superblock on /dev/loop1,
       missing codepage or helper program, or other error

       In some cases useful info is found in syslog - try
       dmesg | tail or so.
{% endhighlight %}

No dice. Maybe we should read the fdisk output more carefully.

> Units: sectors of 1 * 512 = 512 bytes

So `fdsik` is giving us `sectors` but `mount` wants `bytes`. That means we need to do some maths.

{% highlight bash %}
$ mount '/var/lib/libvirt/images/Debian-8.img' /mnt -o offset=$(expr 2048 \* 512)
{% endhighlight %}
[Silence is golden](http://www.linfo.org/rule_of_silence.html).
