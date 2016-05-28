---
layout: post
title: Hooking Libc Functions Part 2
date: 2015-12-25 19:44:40
categories: 
---
In part one we explored a way of hooking `libc` functions with deliberately aliased symbol names. This time we will look at a more generic technique.

Consider the following code. 

`demo.c`
{% highlight c %}
#include <unistd.h>
#include <string.h>

int main()
{
    const char msg[] = "puppies\n";
    write(1, msg, sizeof(msg));
}
{% endhighlight %}

Similar to Part 1, we can hook the write function by creating our own shared library and using the `LD_PRELOAD` trick to load it before `libc`.

Lets start off simple and just shadow the original function. It won't do anything useful but it is a first step.

`mywrite1.c`
{% highlight c %}
    #include <sys/types.h>
    #include <stdio.h>

    ssize_t write(int fd, const void *buf, size_t count)
    {
        puts("Hooked `write` has been called!");
        return 0;
    }
{% endhighlight %}

We can compile and run the code as so.

{% highlight bash %}
$ gcc demo.c
$ ./a.out
puppies
$ gcc -shared -fPIC mywrite1.c -olibmywrite1.so -ldl
$ LD_PRELOAD=$(pwd)/libmywrite1.so ./a.out
Hooked `write` has been called!
{% endhighlight %}

That's cool. But how do we use the original `libc` `write` function from within our hooking function?

Fortunately there is a standard library function called `dlsym` which will find the address of a symbol at runtime. We can use this function to find the address of the *other* `write` function and then call it from within our hooking code.

`mywrite2.c`
{% highlight c %}
    #define _GNU_SOURCE
    #include <dlfcn.h>
    #include <sys/types.h>
    #include <stdio.h>

    ssize_t write(int fd, const void *buf, size_t count)
    {
        static ssize_t(*real_write)(int, const void*, size_t) = NULL;

        if (!real_write)
            real_write = dlsym(RTLD_NEXT, "write");

        const char puppies_suck[] = "kittens\n";
        return real_write(fd, puppies_suck, sizeof puppies_suck);
    }
{% endhighlight %}

This is slightly more complicated than it needs to be. We define the `real_write` function pointer as `static` so that if this hooking function is called multiple times, the symbol lookup is done only once. 

The function call to `dlsym` with the `RTLD_NEXT` argument is how we acquire the address of the real `write` function. Unfortunately `RTLD_NEXT` is considered a `GNU` extension to the language so we must define `_GNU_SOURCE` at the top.

{% highlight bash %}
$ gcc demo.c
$ ./a.out
puppies
$ gcc -shared -fPIC mywrite2.c -olibmywrite2.so -ldl
$ LD_PRELOAD=$(pwd)/libmywrite2.so ./a.out
kittens
{% endhighlight %}
