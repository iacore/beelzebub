---
layout: post
title: Chibi Scheme Part 1 -- Hello World
date: 2016-02-05 19:36:20
categories: 
---
Chibi Scheme is a Scheme implementation which can be easily embedded inside C or C++ applications.

For the impatient, skip to part 5 for a full example of most things you probably want to do with Chibi.

This can be immensely useful for allowing uses to customise an application without diving into the source code and also for developers to rapidly prototype ideas with a shorter feedback loop.

I have had a number of projects where I wanted to quickly tweak an algorithm on a "production" machine which didn't have a compiler and other build tools and Chibi has been immensely useful.

There are other languages that are also specifically designed for this same thing such as Lua and TCL.

It is also possible to embed Python, Ruby and Perl, which can be more awkward but opens the possibility of using their libraries from an application written in another language.

This post series is about Chibi Scheme because you like Lisp right?

setting up
===
Not all package managers ship Chibi Scheme so to keep things universally simple for this discussion we will clone the official repository and do all our examples inside it.

    $ git clone https://github.com/ashinn/chibi-scheme.git
    $ cd chibi-scheme
    $ make
    $ ls libchibi-scheme.so

If compilation went well then there should be a `libchibi-scheme.so` shared library now available.

Note: you can now do `sudo make install` to install globally but we won't do this for this post. When we are done we can just delete the `chibi-scheme` directory created by `git clone` and everything will be back to normal.

hello world
===
Our first application will be a C program which will read and execute a Scheme file at runtime.

hello.c

      #include <stdio.h>
      #include <chibi/eval.h>
  
      int main()
      {
          puts("Hello world from C.");
    
          sexp ctx;
          ctx = sexp_make_eval_context(NULL, NULL, NULL, 0, 0);
          sexp_load_standard_env(ctx, NULL, SEXP_SEVEN);
          sexp_load_standard_ports(ctx, NULL, stdin, stdout, stderr, 0);
    
          sexp_gc_var1(file_path);
          sexp_gc_preserve1(ctx, file_path);
    
          file_path = sexp_c_string(ctx, "hello.ss", -1);
          sexp_load(ctx, file_path, NULL);
    
          sexp_gc_release1(ctx);
          sexp_destroy_context(ctx);
    }

As you can see there are a few lines of setup boilerplate, creating "context" with a standard "environment". I have not needed to tweak these lines but very good documentation is available [online](http://synthcode.com/scheme/chibi/) if you'd like to dig deeper.

The `sexp_load_standard_ports` line is important if you'd like Scheme code to have access to `stdin`, `stdout`, etc.

Next we create a Scheme variable with `sexp_gc_var1`. We can create two variables with `sexp_gc_var2(a, b)`, three with `sexp_gc_var3(a, b, c)` and do on. Once we have a variable we need to tell the Scheme Garbage Collector to not delete it by indicated that it is in use by a context (our context here is called `ctx`). We do this with `sexp_gc_preserve1`.

Finally, almost, we execute the Scheme file with `sexp_load`. But first we need to assign the filename to the Scheme variable we just created using `sexp_c_string`.

Before exiting we use `sexp_gc_release1` to tell the garbage collector that we are finished with the variable and it is safe to delete. Note that it only takes the context as an argument, not the variable(s).

And `sexp_destroy_context` is self explanatory.

We can compile this code as follows:

   $ gcc hello.c -I include/ -L . -l chibi-scheme

Unless `make install` has been run, we need to use the [LD_LIBRARY_PATH](http://linux.die.net/man/8/ld-linux) environmental variable to point to where the Chibi shared library is.

    $ LD_LIBRARY_PATH=. ./a.out
    Hello world from C.
    ERROR: couldn't open input file: "hello.ss"


Excellent. Let's create a Scheme file and watch is execute.

hello.ss
    (import (chibi))
    (display "Hello world from Scheme.")

And run with:

    $ LD_LIBRARY_PATH=. ./a.out
    Hello world from C.
    Hello world from Scheme.$

Oops we forgot the newline at the end of the Scheme message.

hello.ss
    (import (chibi))
    (display "Hello world from Scheme.")
    (newline)

Note that we obviously do not need to recompile any C code while fiddling around with Scheme files.

    $ LD_LIBRARY_PATH=. ./a.out
    Hello world from C.
    Hello world from Scheme.
