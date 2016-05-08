---
layout: post
title:  "Sudo is not just for root"
date:   2015-03-28 13:16:12
categories: 
---

Everyone knows that in the crudest sense, `sudo` lets you run a command as root, and to be fair that is probably the most useful thing it does. That being the case, such a user cannot be blamed for thinking that `su` stood for "Super User". However, as I found out recently, there is a little more to it than that.

So apparently, `su` actually stands for ["Substitute User"](https://en.wikipedia.org/wiki/Su_%28Unix%29), and actually it can be used to run a command as any user, not just root. Moreover, so can `sudo`.

Following a number of bad experiences with various IRC clients, the worst being the ones that could not understand that I want a different identity for each network. I do not want to log onto a serious network like Freenode with my silly Rizon nick for example.

The obvious solution is to run a separate IRC client under a separate Unix user for each network and my new found `sudo` knowledge helped greatly. 

First we need a new user.

{% highlight bash %}
useradd -m rizon
{% endhighlight %}

Next we need to explain the new kind of behaviour we would like to allow in the suders file (run `visudo` to modify it).
{% highlight bash %}
 %wheel ALL=(rizon) NOPASSWD:/usr/bin/weechat
{% endhighlight %}
What this says is that any user in the wheel group (`%wheel`), running on any host (`ALL`), may run a command as the user rizon (`(rizon)`), without supplying a password (`NOPASSWD:`) and the permitted command is weechat (`/usr/bin/weechat`).

There's one extra detail to be aware of and that is the invocation. We must also tell `sudo` on the command line that we wish to run the command as a user other than root.

{% highlight bash %}
sudo -u rizon /usr/bin/weechat                                    
{% endhighlight %}
