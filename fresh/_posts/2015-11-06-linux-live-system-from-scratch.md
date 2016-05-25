---
layout: post
title: Rolling your own Linux Live System from scratch Part 1
date: 2015-11-06 19:20:08
categories: 
---
A live system is an operating system configured to run from some removable media such as CD/DVD or USB Flash Drive or directly from main memory itself. The prupose is to run an operating system without permenantly modifying the internal harddrive of the machine.

This allows people to test Linux distributions without committing to actually installing it. It allows people to recover their original operating system in the case that it cannot boot or is infected with malware.

Some of the more interesting applications lie in forensics - e.g. examining a harddrive in its native environment without modifying it and anti-forensics - e.g. handling sensetive information without leaving a trail in permenant storage.

There are a number of frameworks and user friendly tools for quickly glueing together Live Systems. Some of the more natable ones I have used in the past are [Debian Live](http://live.debian.net/manual/stable/html/live-manual.en.html#3), [livecd-creator](https://fedoraproject.org/wiki/How_to_create_and_use_a_Live_CD?rd=FedoraLiveCD/LiveCDHowTo) and [remastersys](https://en.wikipedia.org/wiki/Remastersys). If all you want is a Live System, these tools are the way to go.

However, rolling your own Live System from scratch will help you learn a lot more about how they work and how Linux works in general. It is a worthy undertaking even if you later decide to use a tool or framework for your serious work.

For the rest of this guide I will use Gentoo because it is a simple and minimalistic distro with no "user friendly" aspirations which come with horrific levels of hidden complexity.

Overview 
---
On a high level the system is comprised of 3 fundemental parts.

*Root Filesystem Image* - We need a root filesystem where `/bin`, `/usr`, `/home`, `/etc`, etc. go. The key thing is that it either must be read-only to avoid permanent changes or it must be copied into main memory where any changes will be immediately lost when the power is cut. This guide will cover both methods.

*Initial Ram Filesystem* - The initramfs is a Linux specific concept. It is a mini root filesystem packed into a compressed archive (like a Zip file). When Linux boots, the kernel will mount this in memory and treat it like the root filesystem. From here the initramfs's job is to prepare the *real* root filesystem and switch to that. 

This is important for special cases where the Linux kernel cannot mount the real root filesystem directly because it is encrypted and the user must enter a password first or because some special kernel module must be loaded first because of special hardware or a special filesystem etc.

As it turns out, live systems are also such a special case. We want an initramfs which will either mount or copy our real Root Filesystem Image into memory and overlay a read-write layer of memory ontop before switching to it. Don't worry, this is easier to create than it sounds.

*Boot Loarder* - There is nothing about the bootloader that is specific to Live Systems so for the sake of simplicity I will only cover booting from USB.

Part I - Root Filesystem
---
The first step is very straight forward. Simply install your operating system of choice and configure it the way you want it to work. A virtual machine is a great way of doing this without interfering with your current setup.

The only thing that needs to be installed from a live system perspective is a tool to mount the root filesystem in RAM. There are a number of past and future projects working on this problem. 

A number of tools are available for this:

 * [AUFS](http://aufs.sourceforge.net/) - requires a kernel patch.
 * [unionfs-fuse](https://github.com/rpodgorny/unionfs-fuse) - userspace only (but slow).
 * [overlayfs](https://en.wikipedia.org/wiki/Overlayfs) - Not released yet.

#### AUFS

Gentoo Portage contains a convenient kernel ebuild called `sys-kernel/aufs-sources`. If you don't have any other kernel requirements (such as Grsecurity) then this is the most straight forward approach. 

Alternatively it is possible to use any kernel and use the `sys-fs/aufs3` or `sys-fs/aufs4` ebuilds (depending on your kernel version). With the `patch-kernel` use flag this ebuild will try to patch whichever kernel is pointed to by `/urs/src/kernel`. 

As a last resort you can always manually download and apply the AUFS patches to your kernel.

### unionfs-fuse

For this option, enable `fuse` inside your kernel and emerge the `sys-fs/unionfs-fuse` ebuild.

### Create Image

Once everything is set up and working the way you want (don't forget to set a transparent background for your terminal) or you are just ready for the first test, create a squashfs image of the filesystem.

If using a virtual machine, [this post]({{ site.baseurl }}/mounting-raw-images) on mounting virtual disk images may be helpful.

Emerge the `sys-fs/squashfs-tools` ebuild. I recommend using the xz compression algorithm for creating the smallest images so make sure the `xz` use flag is enabled.

It may also be a good idea to prepare an exclude file, especially if you are trying to create an image of the current root filesystem.

An exclude file is simply a list of files or directories to omit when creating the image.

<pre>
sys/
proc/
dev/
run/
usr/portage/
usr/src/
</pre>

{% highlight bash %}
$ mksquashfs /path/to/root-filesystem/ output.squashfs -comp xz -ef exclude
{% endhighlight %}

### Testing

Mount the root filesystem image with the following command (may need root).

{% highlight bash %}
mount -t squashfs output.squashfs /mnt
{% endhighlight %}

To test the union mounting use the following commands.

{% highlight bash %}
mount -t tmpfs none /mnt2

# AUFS
mount -t aufs -o br=/mnt2=rw:/mnt=ro aufs /mnt3

# unionfs-fuse
mount -t unionfs -o cdirs=/mnt2=rw:/mnt=ro /mnt3
{% endhighlight %}

In the next part we will examine how to create an initramfs to setup this root filesystem at boot time.

