---
layout: post
title: Hooking Libc Functions Part 1
date: 2015-12-18 19:10:03
categories: 
---

Imagine the following C code:

{% highlight c %}
#include <unistd.h>
#include <string.h>

int main()
{
    const char a[] = "hello\n";
    write(1, a, sizeof(a));
}
{% endhighlight %}

It is simply compiled with GCC and lives as a binary executable `a.out`. How can we hook that libc `write` function in such a way that we can run our own code without the application breaking?

First lets list the symbols that `glibc` exports.

{% highlight bash %}
$ readelf -Ws /lib/libc.so.6 | grep -i write
    99: 00000000000746d0   495 FUNC    GLOBAL DEFAULT   11 _IO_wdo_write@@GLIBC_2.2.5
   168: 00000000000e7640    90 FUNC    WEAK   DEFAULT   11 __write@@GLIBC_2.2.5
   286: 000000000007a430    57 FUNC    GLOBAL DEFAULT   11 _IO_do_write@@GLIBC_2.2.5
   487: 00000000000f6340    36 FUNC    GLOBAL DEFAULT   11 process_vm_writev@@GLIBC_2.15
   489: 00000000000e5b70    96 FUNC    WEAK   DEFAULT   11 __pwrite64@@GLIBC_2.2.5
   843: 00000000000ed2d0    90 FUNC    WEAK   DEFAULT   11 writev@@GLIBC_2.2.5
  1239: 00000000000e5b70    96 FUNC    GLOBAL DEFAULT   11 __libc_pwrite@@GLIBC_PRIVATE
  1500: 00000000000ed3e0   166 FUNC    GLOBAL DEFAULT   11 pwritev@@GLIBC_2.10
  1552: 00000000000f59f0    55 FUNC    GLOBAL DEFAULT   11 eventfd_write@@GLIBC_2.7
  1567: 000000000006e4e0   494 FUNC    WEAK   DEFAULT   11 fwrite@@GLIBC_2.2.5
  1840: 00000000000ed3e0   166 FUNC    GLOBAL DEFAULT   11 pwritev64@@GLIBC_2.10
  1986: 00000000000793f0   164 FUNC    GLOBAL DEFAULT   11 _IO_file_write@@GLIBC_2.2.5
  2005: 000000000006e4e0   494 FUNC    GLOBAL DEFAULT   11 _IO_fwrite@@GLIBC_2.2.5
  2024: 00000000000e5b70    96 FUNC    WEAK   DEFAULT   11 pwrite@@GLIBC_2.2.5
  2082: 00000000000785b0   133 FUNC    GLOBAL DEFAULT   11 fwrite_unlocked@@GLIBC_2.2.5
  2091: 00000000000e5b70    96 FUNC    WEAK   DEFAULT   11 pwrite64@@GLIBC_2.2.5
  2138: 00000000000e7640    90 FUNC    WEAK   DEFAULT   11 write@@GLIBC_2.2.5

{% endhighlight %}

Notice how there is a `write` and a `__write` symbol? What's all that about? Let's look at the [source code](https://github.com/lattera/glibc/blob/master/io/write.c).

{% highlight c %}
ssize_t
__libc_write (int fd, const void *buf, size_t nbytes)
{
    if (nbytes == 0)
        return 0;
    if (fd < 0)
    {
        __set_errno (EBADF);
        return -1;
    }
    if (buf == NULL)
    {
        __set_errno (EINVAL);
        return -1;
    }

    __set_errno (ENOSYS);
    return -1;
}
libc_hidden_def (__libc_write)
stub_warning (write)

weak_alias (__libc_write, __write)
libc_hidden_weak (__write)
weak_alias (__libc_write, write)
{% endhighlight %}

Notice in particular the `weak_alias` macros near the bottom. It wouldn't be too much of a stretch to conclude from this that `write` and `__write`  will both do the same thing; although you're free to dig further into the code if you'd like.

Let's put this hypothosis to the test by creating a shared library.

{% highlight c %}
#include <stdio.h>
#include <unistd.h>

ssize_t write(int fd, const void *buf, size_t count)
{
        printf("Writing ... <%s>\n", buf);
        return __write(fd, buf, count);
}
{% endhighlight %}

We can compile this as a shared library like so.

{% highlight bash %}
$ gcc -fPIC -shared -o libhook.so hook.c
{% endhighlight %}

Unix-like operating systems include an environmental variable called [LD_PRELOAD](http://man7.org/linux/man-pages/man8/ld-linux.so.8.html#ENVIRONMENT) which allows us t specify a shared library which will be loaded ahead of all others, including `libc`. Using this method we can trick the application into using the `write` function we wrote instead of the official `libc` one.

{% highlight bash %}
$ LD_PRELOAD=$(pwd)/libhook.so ./a.out
Writing ... <hello
>
hello
{% endhighlight %}

This approach may seem flakly since we are relying on some illicit knowledge of glibc's internal workings however, the [official documentation](http://sourceware.org/glibc/wiki/Style_and_Conventions#Double-underscore_names_for_public_API_functions) states that this is actually part of the reason that double underscore functions exist in the library.

Unfortunantly not all libraries offer duplicate symbols in this way. Is there a another way to hook library functions? That's an answer we will answer in part 2. 
