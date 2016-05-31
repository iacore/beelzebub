---
layout: post
title: When Symbols Clash
date: 2015-10-23 20:06:16
categories: 
---
This is one of my favorite bugs so far in my short professional career. While working at [CSR](https://en.wikipedia.org/wiki/Cambridge_Silicon_Radio) I had a need to modify a customer facing tool. I am quite keen to note, as you'll see why later, that this tool was not at all the responsibility of my team. 

Being a good citizen I compiled on all platforms before submitting my changes, something I quickly found out that no one else did.

It turned out that the Linux version would not run at all; it would segfault at startup. I reverted my changes but the problem persisted. So the build was broken before I started? 

Actually yes. It had been broken for 5 years. I found the bug on the bug tracker. It contained a lengthy discussion but ultimately no one knew what was wrong or how to fix it. Now that is just catnip for engineers. Clearly I had no choice but to spend the next couple of hours digging into this.

I obviously cannot discuss the actual code I was working on but lets take a look at some example code. First of all, consider the following:

{% highlight c++ %}
#include <iostream>
#include <readline/readline.h>

int main()
{
    char *name = readline ("Enter your name: ");
    std::cout << "hello " << name << std::endl;
}
{% endhighlight %}
Make sure `readline` is installed (`apt-get install libreadline-dev` in Debian) and compile with `g++ example.cpp -lreadline -g` (yes we will need debug symbols later on ☺).

Ok so this trivial example works. Now imagine this is part of a larger program, and this larger program happens to have a global variable called `baudrate`. Guess what happens.

{% highlight c++ %}
#include <iostream>
#include <readline/readline.h>

int baudrate;

int main()
{
    char *name = readline ("Enter your name: ");
    std::cout << "hello " << name << std::endl;
}
{% endhighlight %}

    $ ./a.out 
    Segmentation fault

What the hell, right? Yes I'm rather excited too. Time to break out the debugger.

    $ gdb -q a.out 
    Reading symbols from a.out...done.
    (gdb) r
    Starting program: /home/nemu/a.out 

    Program received signal SIGSEGV, Segmentation fault.
    0x00000000006013d4 in baudrate ()
    (gdb) bt
    #0  0x00000000006013d4 in baudrate ()
    #1  0x00007ffff6daee5a in _nc_setupterm ()
    from /lib/x86_64-linux-gnu/libtinfo.so.5
    #2  0x00007ffff6daef73 in tgetent () from /lib/x86_64-linux-gnu/libtinfo.so.5
    #3  0x00007ffff7bc030c in _rl_init_terminal_io ()
    from /lib/x86_64-linux-gnu/libreadline.so.6
    #4  0x00007ffff7ba91a8 in rl_initialize ()
    from /lib/x86_64-linux-gnu/libreadline.so.6
    #5  0x00007ffff7ba9387 in readline ()
    from /lib/x86_64-linux-gnu/libreadline.so.6
    #6  0x0000000000400988 in main () at a.c:7
    (gdb) 

A sharp eyed reader may be able to spot the problem already.

Don't despair, I didn't see it at first either. My natural instinct drove me to take a peek at the offending code.

    (gdb) disassemble baudrate
    No function contains specified address.

Huh? Well, whatever...

    (gdb) disassemble 0x00000000006013d4
    Dump of assembler code for function baudrate:
    => 0x00000000006013d4 <+0>:     add    %al,(%rax)
       0x00000000006013d6 <+2>:     add    %al,(%rax)
    End of assembler dump.
    (gdb) 

Huh? Where's the [function prologue](https://en.wikipedia.org/wiki/Function_prologue)? What is this code even doing anyway? That really doesn't look like a function to me.

Now of course I expect my dear reader to be screaming at their screen right now. "Kevin, you idiot baudrate is an integer not a function!" I imagine they are saying. Some may even continue with "just look at the memory locations, clearly readline also has a `baudrate` symbol which has not been [Dynamically Linked](https://en.wikipedia.org/wiki/Dynamic_linker) on account of the original global variable!". Touché my dear reader, you are correct.

Renaming that one global variable solves all our problems. At least for today.
