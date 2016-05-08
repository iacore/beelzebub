---
layout: post
title:  "Securing SMPlayer with AppArmor"
date:   2015-03-28 13:16:12
categories: 
---
AppArmor is an easy to use Mandatory Access Control framework for Linux. In a nutshell it allows a user to monitor any given application for every file it attempts to read, write, execute, create, lock, etc. and allow or deny those actions on a file by file basis. Effectivly, it gives a user full control over what an application can do to a computer without looking at or rewriting the source code. This article is about my experiance writing a custom AppArmor profile for SMPlayer.

SMPlayer is a QT based frontend for various media players. I like it a lot but it is not without it's downsides. Like most media players it has a tendancy to make artibrarty outgoing network connections which is not cool at all. As it turns out, solving this problem provided a great opportunity to learn AppArmor.


Requires fucking auiditd


apparmor_parser: Unable to add "/usr/bin/smplayer".  Profile already exists
Use apparmor_parser -r /etc/apparmor.d/usr.bin.smplayer

Globbing directories does not imply access to the base directory.
/home/kokoro/Videos/ r,
/home/kokoro/Videos/** r,

 @{PROC}/[0-9]*/fd/ r,
