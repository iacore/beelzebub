---
layout: post
title:  "Simplyfying the Tor Browser"
date:   2015-06-12 20:16:12
categories: 
---

The Tor Browser Bundle contains a lot of unnecessary features and complexity. I appreciate that the developers are trying their best to make the application as idiot-proof as possible but whenever designing a system to be smarter than the user there is always a significantly detrimental side effect of making the system very cumbersome or even completely unusable for a user who's use case even slightly differs from what the developers envisioned.

Overview
---
The Tor Browser Bundle can be downloaded as a tar ball. The idea is you uncompress it, type `./start-tor-browser` and away you go. A Tor Browser window pops up, starts and connects to a bundled Tor binary and includes nifty features such as a "new identity" button. Moreover, it contains a failsafe to avoid launching the browser if the Tor SOCKS proxy port is not reachable. 

In my case I don't really need any of that. I want to start my own system Tor client as root, so it run under its own unique Unix user account. Then I want to apply firewall rules to only allow this user account to connect to the outside internet, effectively blocking all non-Tor network leaks. As example of such firewall rules can be found here.

The problem is that the Tor Browser won't let me do any of this. I can remove the bundled Tor client but then the browser will hang on startup telling me it is waiting for Tor to start. This is the case even when my system Tor client is listening on the port the Tor Browser is expecting (9150). If I look in the Browser directory I see shared library objects and shell scripts. I do not see anything that looks like a configuration file. There is a 400 line shell script called start-tor-browser but it does not give me any indication of what I should do differently.

It very much seems to me that there is way too much emphasis on making assumptions on behalf of the unsophisticated user and next to no concern for keeping the system simple, modular and extendible. Of course it might just be me who is too stupid to understand what actually is a simple system.

Cutting the Bloat
---
The local Tor client lives in a directory called `Browser/TorBrowser/Tor/`. Delete it. But before you do note that this directory contains shared objects which may be required by the browser application too. In particular on Debian systems the system `libstdc++.so.6` may be too old for the Tor Browser.

{% highlight bash %}
mv Browser/TorBrowser/Tor/libstdc++.so.6 Browser/
rm -r Browser/TorBrowser/Tor/
rm -r Browser/TorBrowser/Docs/
{% endhighlight %}

How do we get rid of this `Waiting for Tor to start...` junk? This appears to be the work of a browser extension called tor-launcher. Delete it.

{% highlight bash %}
TorBrowser/Data/Browser/profile.default/extensions/tor-launcher@torproject.org.xpi
{% endhighlight %}

To launch the Tor Browser without the wrapper script there is a `firefox` ELF executable in the Browser directory. Note that many shared libraries it depends on are in the same directory so we need to supply this path in the LD_LIBRARY_PATH environmental variable.

{% highlight bash %}
LD_LIBRARY_PATH="Browser/"  ./Browser/firefox
{% endhighlight %}

Last but not least make sure you change the SOCKS proxy port through the standard Firefox GUI `edit->preferences->advanced->settings...`. You probably want to change port `9150` to `9050` or whatever is in your `torrc` file.

I hope this helps anyone in a similar position bypass some of the unnecessary complexity coupled with the Tor Browser Bundle.

