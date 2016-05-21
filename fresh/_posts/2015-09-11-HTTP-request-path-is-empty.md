---
layout: post
title: "Ruby: HTTP request path is empty"
date: 2015-09-11 20:07:16
categories: 
---
If you've ever tried to grab webpages with Ruby you may have come across the runtine error message `HTTP request path is empty`. My acute pyschocosmic intuition tells me that you may have code similar to the following.

{% highlight ruby %}
require 'net/http'
uri = URI.parse("http://bing.com")

Net::HTTP::Proxy('192.168.100.1', 4444).start(uri.host, uri.port) { |http|
    puts http.get(uri.path).body
}
{% endhighlight %}

The `path` piece of the message is a small clue; the error pertains the supplied URL. It seems that `http://bing.com` may not be interpreted as a complete URL. How do I know? I [Read The Fun Manual](https://tools.ietf.org/html/rfc2616#page-19).

According to the RFC a valid URL looks like this: 

      http_URL = "http:" "//" host [ ":" port ] [ abs_path [ "?" query ]]

What are we missing? The `abs_path` part. It is easy to forget this because modern web browsers are so idiot proof we can simply supply a domain name such as `bing.com` and the rest will be taken care of for us. 

The solution is to emulate our web browsers and manually append a `/` to the end of the URL. If you're familiar with Unix you'll notice that this symbol signifies the root of the filesystem.

{% highlight ruby %}
# Don't forget the '/' at the end!.
uri = URI.parse("http://bing.com/")
{% endhighlight %}
