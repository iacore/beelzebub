---
layout: post
title: Chibi Scheme Part 3 -- Returning Values from Scheme to C
date: 2016-03-04 19:36:20
categories: 
---
Getting Values Out of Scheme
===
This is perhaps one of the most useful things to do. Once we can lookup values defined in a Scheme file and pull them into C, we can use Chibi Scheme for configuration files instead of using XML, JSON or writing custom file parsing code.

luckynum.c

    #include <stdio.h>
    #include <chibi/eval.h>

    int main()
    {
      sexp ctx;
      ctx = sexp_make_eval_context(NULL, NULL, NULL, 0, 0);
      sexp_load_standard_env(ctx, NULL, SEXP_SEVEN);
      //sexp_load_standard_ports(ctx, NULL, stdin, stdout, stderr, 0);

      sexp_gc_var2(file_path, ret);
      sexp_gc_preserve2(ctx, file_path, ret);

      file_path = sexp_c_string(ctx, "luckynum.ss", -1);
      sexp_load(ctx, file_path, NULL);

      ret = sexp_eval_string(ctx, "lucky-number", -1, NULL);

      int lucky_number = -1;
      if (sexp_integerp(ret)) {
        lucky_number = sexp_unbox_fixnum(ret);
      }

      printf("It looks like the lucky number is (%d)\n", lucky_number);

      sexp_gc_release2(ctx);
      sexp_destroy_context(ctx);
    }

luckynum.ss

    (import (chibi))
    (define lucky-number 666)

A few things to note here:

* those familiar with Lisp may already understand this; in `sexp_eval_string(..., "lucky-number", ...)` we are asking the Scheme interpreter to evaluate the variable name, which will return us its value. If we change `lucky-number` to be a function within `luckynum.ss` then we would need to wrap it in parenthesis in order to execute it (and get back the return value of the function).

* we do not need `sexp_load_standard_ports` for this one because Scheme is not doing any IO.

* you may need to link against the standard maths library (`-lm`)

With the call to `sexp_integerp(ret)` we check that `ret` is an integer. We then use `sexp_unbox_fixnum(ret)` to get the value as a C `int`. The "p" at the end stands for "predicate", possibly because it is convention in Scheme to name functions that return boolean values with a "?" at the end. Since C does not support this syntax I guess the Chibi Scheme developers decided to use a "p" instead. In the documentation you will find several other predicate and conversion functions for different types. In addition to `int` I will cover strings later on.

