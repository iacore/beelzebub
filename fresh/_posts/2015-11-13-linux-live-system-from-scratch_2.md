---
layout: post
title: Rolling your own Linux Live System from scratch Part 2
date: 2015-11-13 19:10:08
categories: 
---

Initramfs
---
An initial RAM file system is a mini root filesystem zipped up into a compressed archive. It is loaded by the kernel at boot time and its purpose is to prepare the real root filesystem for use.

An initramfs is useful for situations where extra kernel modules most be loaded, or a hard disk partition must be decrypted before the real root filesystem is usable. In our case we want to mount the root filesystem in RAM, and union mount it with tmpfs so we can make non-permanent changes to it.

We start by building a skeleton of the directories our initramfs needs.

{% highlight bash %}
$ mkdir -p work/initramfs/{bin,sbin,etc,proc,dev,sys}
$ cd work
{% endhighlight %}

Busybox contains all the common userspace tools we'll need to prepare our root filesystem. Your favorite distribution's package manager may have a static version of Busybox, otherwise compile one yourself. If you have a Gentoo system, then emerging with the `static` USE flag will also do the trick. The reason we want a static version is so we can drop the Busybox binary into the `work/initramfs/bin/` directory and not worry about also including any dynamic libraries it might need, including `libc`.

To check if a binary is static or not, run the `file` or `ldd` commands on it.
{% highlight bash %}
$ file /bin/busybox 
/bin/busybox: ELF 64-bit LSB executable, x86-64, version 1 (GNU/Linux), statically linked, for GNU/Linux 2.6.32, stripped
$ ldd /bin/busybox 
        not a dynamic executable
{% endhighlight %}

{% highlight bash %}
$ cp /bin/busybox initramfs/bin/
{% endhighlight %}

Init
---
Our initramfs needs an init process to do its job. Though when we thing of init daemons we tend to think of behamoths such as Systemd and OpenRC in reality they can be as simple as a single shell script.

Be warned however, Bash is way too big to squeeze inside Busybox; what we will be working with is called [Ash](https://en.wikipedia.org/wiki/Almquist_shell). It is similar enough to [Dash](http://linux.die.net/man/1/dash) that we can use their [documentation](https://wiki.ubuntu.com/DashAsBinSh).

Below is a minimal init script which will do some basic setup and then drop you into an interactive shell.

{% highlight bash %}
#!/bin/busybox sh

# Prevent kernel messages from popping onto the screen.
echo 0 > /proc/sys/kernel/printk

# Wait for kernel messages to subside.
sleep 2
clear

# Unpack Busybox utilities i.e. `mount` will not be available under `/bin/mount`.
/bin/busybox --install -s

mount -t proc none /proc
mount -t sysfs none /sys

#Create device nodes
mknod /dev/tty c 5 0
mdev -s

exec sh
{% endhighlight %}

Generating the Archive
---
The initamfs must be in a specific format in order for the kernel to recognise it.

{% highlight bash %}
$ cd work/initramfs
$ find . | cpio -H newc -o > ../initramfs.cpio
$ cd ..
$ cat initramfs.cpio | gzip > initramfs.igz
{% endhighlight %}

Testing
---
Now it is simply a matter of copying it to the boot partition and making adjustments to grub. Under grub legacy, the following addition to `/boot/grub/grub.conf` will allow us to test our new live system.

{% highlight bash %}
title My Live System
kernel /bzImage
initrd /initramfs.igz
{% endhighlight %}

However, testing on bare metal involves waiting for a physical machine to boot and moving USB drives around and can be unnecessarily time consuming. An alternative could be to test inside a virtual machine instead.

Below is an example Xen configuration.

{% highlight bash %}
kernel = "bzImage"
ramdisk = "initramfs.igz"

disk = ['']
vif = ['']

memory = 1024
name = "my-live-system"
vcpus = 1
maxvcpus = 1
{% endhighlight %}

As an added sneaky trick, we can attach our squashfs root file system image to the virtual machine by pretending it is some kind of read only virtual harddrive. This also works under Qemu-KVM, however in this case it may to necessary to tell Qemu that the squashfs image is a DVD.

{% highlight bash %}
disk = [
    'format=raw, vdev=xvda, access=r, target=/home/me/output.squashfs'
]
{% endhighlight %}

A Note on Keymaps
---
Like most people, if you're not from the U.S. you will probably find a localized keymap useful, especially if you plan to type in passwords. Fortunately this is a simple process. Busybox conatins two applets called `dumpkmap` and `loadkmap` we we can use to generate a special file describing our system's current keyboard layout and later load that file when our initramfs runs.

To generate the keymap file, run the following commands on a machine with the desired keyboard layout.

{% highlight bash %}
$ busybox dumpkmap > etc/keymap
{% endhighlight %}

In the `init` script, add the following lines to load the keymap again.
{% highlight bash %}
mkdir -p /dev/vc
ln -s /dev/console /dev/vc/0
loadkmap < /etc/keymap
{% endhighlight %}

Finishing up
---
So far we have an initramfs which will dump us into an interactive shell session. This is useful because it will allow us to manually run the commands we would like and gain immediate feedback on any problems.

First we need to figure out where the squashfs image is. If we're running this from a USB drive on bare metal then we need to mount the USB drive (something like `/dev/sdb1`). If we're running virtualised and added the squashfs image itself as a virtual block device (something like `/dev/xvda`) then we can treat that `/dev/` block device as the squashfs image file itself.

{% highlight bash %}
$ mkdir -p /{ro,mnt}
$ mount -t ext4 /dev/sdb1 /mnt
$ mount -t squashfs /mnt/output.squashfs /ro
{% endhighlight %}

or an Xen based example

{% highlight bash %}
$ mkdir -p /{ro,mnt}
$ mount -t squashfs /dev/xvda /ro
{% endhighlight %}

In the [previous]() post we saw how to union mount the squashfs image.

{% highlight bash %}
$ mkdir -p /{rw,un}
$ mount -t tmpfs none /rw
$ mount -t aufs -o br=/rw=rw:/ro=ro aufs /un
$ ls /un
{% endhighlight %}

If everything went well then `/un` should now contain what appears to be a read write copy of your root filesystem image except any changes will not persist after power down.

The final thing to do is to `switch_root` and "boot into" the new root filesystem.

{% highlight bash %}
$ #System will be inoperable without these directories, best make sure.
$ mkdir -p /mnt/run/ /mnt/dev/ /mnt/sys /mnt/proc /mnt/var/log/
$ mount -t devtmpfs none /mnt/dev
$ 
$ umount /sys /proc
$ exec switch_root /un /sbin/init
{% endhighlight %}

Once you've got this process working manually it is simply a matter of adding the commands to the init script. One final note: if you'd prefer the rootfilesystem image to be entirely resident in RAM (i.e. you'd like to pull the USB drive out of the machine after it has finished booting), this is simply a matter of `cp`ing the squashfs image from the USB drive before mounting it.

{% highlight bash %}
#!/bin/busybox sh

# Prevent kernel messages from popping onto the screen.
echo 0 > /proc/sys/kernel/printk

# Wait for kernel messages to subside.
sleep 2
clear

# Unpack Busybox utilities i.e. `mount` will not be available under `/bin/mount`.
/bin/busybox --install -s

mount -t proc none /proc
mount -t sysfs none /sys

#Create device nodes
mknod /dev/tty c 5 0
mdev -s

mkdir -p /dev/vc
ln -s /dev/console /dev/vc/0
loadkmap < /etc/keymap

mkdir -p /{ro,mnt}
mount -t ext4 /dev/sdb1 /mnt

mount -t squashfs /mnt/output.squashfs /ro

# To eject the USB drive after booting.
#cp /mnt/output.squashfs /
#mount -t squashfs /output.squashfs /ro

#for Xen
#mount -t squashfs /dev/xvda /ro

mkdir -p /{rw,un}
mount -t tmpfs none /rw
mount -t aufs -o br=/rw=rw:/ro=ro aufs /un

#System will be inoperable without these directories, best make sure.
mkdir -p /mnt/run/ /mnt/dev/ /mnt/sys /mnt/proc /mnt/var/log/
mount -t devtmpfs none /mnt/dev

umount /sys /proc
exec switch_root /un /sbin/init

echo "Something went wrong."
exec sh
{% endhighlight %}

Finding the Squashfs Image
---
One final, final note. If you have a stable environment i.e. you're not plugging and unplugging USB drives or internal storage devices left right and centre then you can probably get away with hardcoding something like `/dev/sdb1` into the init script.

However, a more robust approach would be to identify the device based on its UUID.

Running the `blkid` utility (may require root) will display a list of block devices and their Unique Universal IDentifier.

{% highlight bash %}
$ sudo blkid
/dev/sda: UUID="f81d4fae-7dec-11d0-a765-00a0c91e6bf6" TYPE="crypto_LUKS"
{% endhighlight %}

With a little `sed` it is possible to extract the `/dev/sdX` portion of the blkid output.

{% highlight bash %}
# Given a disk UUID, return the dev name i.e.
# f81d4fae-7dec-11d0-a765-00a0c91e6bf6 --> /dev/sdb3
find_mnt()
{
    res=$(echo $1 | sed -n '{s/\([0-9a-f]\{8\}-[0-9a-f]\{4\}\-[0-9a-f]\{4\}\-[0-9a-f]\{4\}-[0-9a-f]\{12\}\)/\1/p}')

    if [ -z "$res" ]
    then
    return
    fi

    blkid | sed -n "/$1/s/\([^:]\+\).*/\1/p"
}
{% endhighlight %}

Note the use of POSIX style regular expression syntax, this is because Busybox does not support <s>sensible</s> alternative flavours in order to keep its size small.
