---
layout: post
title:  "Connecting to a Remote I2P Router"
date:   2015-05-15 20:16:12
categories: 
---

The Invisible Internet Project [(I2P)](https://geti2p.net/en/about/intro) is a Peer to Peer network providing anonymous and censorship resilient communication. This is a post on how to use connect to an I2P router running on a remote machine and why it may be desirable use I2P in this way.

Introduction
===========

I2P is a great network, has been around for over 10 years and has many strengths.

Unfortunately I2P has two rather stark deficiencies, neither of which are a problem with the system itself but nevertheless can have a large impact on usability and anonymity. 

The first issue is the slow startup time. It can take a number of minutes, even half and hour or more, to start up and become sufficiently integrated into the network and be in a position to do anything useful. 

The second issue is that by virtue of being a Peer to Peer network, all nodes need to publish their public IP address in order to participate in the network. This means that an advisory with an overview of the network can correlate an anonymous event such as the end of an IRC session with an IP address removing itself from the I2P network. Therefor regularly starting and stopping I2P, as you might Tor, can be damaging to your anonymity.

For these reasons it makes a lot of sense to run your I2P router 24/7 and connect to it remotely through your day to day machine.

A great way to do this securely if through SSH TCP forwarding.

Example SSH Tunnel Command
=================

{% highlight bash %}
ssh -fTNL 192.168.100.1:4444:127.0.0.1:4444 -L 192.168.100.1:7657:127.0.0.1:7657 i2p@192.168.0.8
{% endhighlight %}

There is a lot going on here so lets break it down.
Lets start with the arguments.

-f      Requests ssh to go to background just before command execution. We want this tunnel to stay open for as long as we want to interact with the router. Sending it to the background prevents locking up a terminal for the duration.

  -T      Disable pseudo-tty allocation.

  -N      Do not execute a remote command. Don't login essentially, just keep the tunnel open without having an interactive SSH session open as well.

  -L Specifies that the given port on the local (client) host is to be forwarded to the given host and port on the remote side.This is where the magic is. 

The syntax for -L is as follows.
{% highlight bash %}
    -L [bind_address:]port:host:hostport
{% endhighlight %}

In our example command above this was 
{% highlight bash %}
-L 192.168.100.1:4444:127.0.0.1:4444 
{% endhighlight %}

192.168.100.1 was the bind address and 4444 is the bind port, this means that to access I2P you would set your web browser's http proxy to 192.168.100.1:4444. Note that the bind address is optional. If omitted then this port will be available on all interfaces. I run my I2P web browser inside a virtual machine, by setting the explicit bind interface to 192.168.100.1 (the default gateway for the virtual machine) I can avoid giving the virtual machine the credentials necessary to build this SSH tunnel.

The second address and port number pair, in the example 127.0.0.1:4444, are the other side of the connection. Essentially, once our traffic has gone through the SSH tunnel and arrives on the other machine, how does it reach the I2P software? If I2P is running on that same remote machine then the address and port number pair 127.0.0.1:4444 is probably correct.

Next in the example command we specify another -L argument.

{% highlight bash %}
-L 192.168.100.1:7657:127.0.0.1:7657
{% endhighlight %}

This is exactly the same as the case we just covered except instead of forwarding port 4444 (I2P's HTTP proxy port) we forward port 7657 which is I2P's control interface port. If you only want to the I2P network and not mess with the router settings then only forward port 4444.

The final part of the command is the username and IP address combination of the machine running the I2P software. Note that i2p is a Unix username in this case, replace it with your own username.
{% highlight bash %}
i2p@192.168.0.8
{% endhighlight %}

Simplified Command
=================

A simplified version of this command without an explicit bind address is as follows.
{% highlight bash %}
ssh -fTNL 4444:127.0.0.1:4444 -L 7657:127.0.0.1:7657 i2p@192.168.0.8
{% endhighlight %}

