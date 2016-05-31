---
layout: post
title: C Tricks Modulo Bounds Checking
date: 2015-12-11 19:35:00
categories: 
---
Writing a circular buffer in a language such as C will often include code which looks like the following:

{% highlight c %}
#define BUF_MAX 10
int buf[BUF_MAX];
int buf_i = 0;

void insert(const int c)
{
    buf[buf_i] = c;

    buf_i += 1;
    if (buf_i > BUF_MAX)
        buf_i = 0;
} 
{% endhighlight %}
The mathematical modulo operator is usually used to bound the range of output from a random number generator. The same principle can be applied to our circular buffer for much more elegant code.

{% highlight c %}
void insert(const int c)
{
    buf[buf_i] = c;
    buf_i = (buf_i + 1) % BUF_MAX;
} 
{% endhighlight %}
