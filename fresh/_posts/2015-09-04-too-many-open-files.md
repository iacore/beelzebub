---
layout: post
title:  Too Many Open Files
date: 2015-09-04 20:05:10
categories: 
---
Any prolific bittorrent user is likely to sooner or later encounter a client crash† due to "too many open files".

    Java IOException: Too many open files. 

Most operating systems place limits on the resources a single process can utilise. [file descriptors](https://en.wikipedia.org/wiki/File_descriptor) are one such resource. Linux enforces these constrains based on the contents of the `/etc/security/limits.conf` file. The [man page](http://linux.die.net/man/5/limits.conf) contains details of the file syntax.

Below is an example line which will extend the resource limit for the `i2psvc` user. 

    i2psvc          -     nofile         100000

Seeding more than 100000 files seems excessive to me anyway.

---
† Yeah okay maybe I should use a better client.
