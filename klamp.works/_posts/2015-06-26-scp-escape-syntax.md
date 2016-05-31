---
layout: post
title:  "SCP Escape Syntax"
date: 2015-06-26 20:04:14
categories: 
---
Secure copying files with shell metacharacters embedded in their names involves slightly more arcane escaping syntax than usual. Imagine the following file:

{% highlight bash %}
$ scp yuki@192.168.0.61:hello world .
scp: hello: No such file or directory
cp: cannot stat 'world': No such file or directory
{% endhighlight %}

Well duh, you've got to escape that space in the middle of the name.

{% highlight bash %}
$ scp yuki@192.168.0.61:hello\ world .
scp: hello: No such file or directory
scp: world: No such file or directory.
{% endhighlight %}

Hang on. Maybe quote it?

{% highlight bash %}
$ scp yuki@192.168.0.61:"hello world" .
scp: hello: No such file or directory
scp: world: No such file or directory
{% endhighlight %}

Well single quotes are the strongest type of quote.

{% highlight bash %}
$ scp yuki@192.168.0.61:'hello world' .
scp: hello: No such file or directory
scp: world: No such file or directory
{% endhighlight %}

Boo. 

It turns out that the filename needs to be either escaped AND quoted or simply quoted twice. 

Any of the following will work.
{% highlight bash %}
$ scp yuki@192.168.0.61:'hello\ world' .
$ scp yuki@192.168.0.61:"hello\ world" .
$ scp yuki@192.168.0.61:"'hello world'" .
$ scp yuki@192.168.0.61:'"hello world"' .
$ scp yuki@192.168.0.61:"\"hello world\"" .
{% endhighlight %}
