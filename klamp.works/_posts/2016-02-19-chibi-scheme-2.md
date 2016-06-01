---
layout: post
title: Chibi Scheme Part 2 -- Calling a Scheme Function from C
date: 2016-02-19 19:36:20
categories: 
---
Calling a Scheme Function
===
The Chibi C API has a function called `sexp_eval_string`. This can be used to run arbitrary Scheme code.

Here is an example of adding two numbers:

    #include <stdio.h>
    #include <chibi/eval.h>

    int main()
    {
      sexp ctx;
      ctx = sexp_make_eval_context(NULL, NULL, NULL, 0, 0);
      sexp_load_standard_env(ctx, NULL, SEXP_SEVEN);
      sexp_load_standard_ports(ctx, NULL, stdin, stdout, stderr, 0);

      sexp_eval_string(ctx, "(display (+ 1 1))", -1, NULL);

      sexp_destroy_context(ctx);
    }

We can use this same strategy to call functions defined inside a Scheme file:

callme.c

    #include <stdio.h>
    #include <chibi/eval.h>

    int main()
    {
      sexp ctx;
      ctx = sexp_make_eval_context(NULL, NULL, NULL, 0, 0);
      sexp_load_standard_env(ctx, NULL, SEXP_SEVEN);
      sexp_load_standard_ports(ctx, NULL, stdin, stdout, stderr, 0);

      sexp_gc_var1(file_path);
      sexp_gc_preserve1(ctx, file_path);

      file_path = sexp_c_string(ctx, "callme.ss", -1);
      sexp_load(ctx, file_path, NULL);

      sexp_eval_string(ctx, "(say-hello)", -1, NULL);

      sexp_gc_release1(ctx);
      sexp_destroy_context(ctx);
    }

callme.ss

    (import (chibi))
    (define (say-hello)
      (display "The say hello function was called!")
      (newline))

