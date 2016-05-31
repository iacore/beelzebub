---
layout: post
title:  Can your Python do this? Mutual recursion in Perl
date: 2015-07-31 20:27:52
categories: 
---
I'd like to start out by disclaiming that I'm neither a Python person or a Perl person, I just thought that those who would be most interested in this post would get a kick out of the provocative title.

I'm one of those odd people who learned Scheme before doing any serious TCL, Perl, Python, Ruby etc. which gives me an interesting perspective on things. At CSR, Perl and Python are the only two languages permitted for internal (non-test related) script development. 

For a new script I tried Python first. I quickly realised that the problem, which included some parsing, could elegantly be solved with mutual recursion. 

Great, but one problem. Python does not support [tail call optimisation](https://en.wikipedia.org/wiki/Tail_call) and [never will](http://neopythonic.blogspot.com/2009/04/tail-recursion-elimination.html). So naturally I dumped Python and used Perl instead.

I can't disclose the code I wrote for my company so lets just use the old odd/even example to demonstrate. 

{% highlight perl %}
sub is_even {
    my ($acc) = @_;

    if ($acc == 0) {
        return 1;
    } else {
        @_ = ($acc - 1);
        goto &is_odd;
    }
}

sub is_odd {
    my ($acc) = @_;

    if ($acc == 0) {
        return 0;
    } else {
        @_ = ($acc - 1);
        goto &is_even;
    }
}
{% endhighlight %}

As you've probably guessed, indeed Perl implements Tail Call Optimisation through the [goto](http://perldoc.perl.org/functions/goto.html) function. 

As I stated earlier I'm not a big fan of either Python or Perl but I'm beginning to think that Perl is as least a bit fun. 
