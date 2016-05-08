---
layout: post
title:  Monitoring Keystrokes in X11
date: 2015-08-17 21:52:47
categories: 
---
After experimenting with alternative operating systems many annoying little issues with Windows may suddenly become apparent. For example, have you ever noticed how Windows requires the user to explicitly switch to a background window before interacting with it? You can't just hover your mouse over an unfocused window and start spinning the scroll wheel like on Linux. Lame right? Well...

As it turns out, this vigorous isolation of windows is a vastly superior design decision. X11 on its own has no such isolation; every window has access to the input of every other window. Lets reiterate that more clearly, when you visit your bank's website and type in your password, those characters are being sent to every window on your desktop - the majority of which are just politely pretending they didn't see anything. 

This immediately raises two questions: 

* Are you sure? 
* What can be done about it?

Indeed, if we are going to do something about it we need a test to make sure the problem is solved. A quick and dirty way of substantiating this claim is with a tool called [xinput](http://linux.die.net/man/1/xinput)

{% highlight bash %}
$ xinput list
...
    ↳ AT Translated Set 2 keyboard              id=10   [slave  keyboard (3)]
{% endhighlight %}
Look for the device called "AT Translated keyboard" or similar and use the `id` for the next command, in my case it was 10.

{% highlight bash %}
$ xinput test 10
{% endhighlight %}

Notice how it will capture every keystroke, even when a different window is focused. Shoot.

Fortunately, workarounds do exist. To protect a single valuable window, for example online banking, consider spinning up a new X11 session for it under a different Unix account. 

To isolate a single dubious or untrusted application, consider using some kind of remote desktop tool such as Xephyr, VNC or, my favourite, Xpra. Although performance may drastically degrade with these methods, that is the price of security.
