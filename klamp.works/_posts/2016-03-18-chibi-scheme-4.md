---
layout: post
title: Chibi Scheme Part 4 -- Passing Values from C to Scheme
date: 2016-03-18 19:36:20
categories: 
---
Calling a Scheme Function with Arguments
===
This is perhaps one of the last great pieces of the puzzle: passing a value from C into Scheme.

The way I do this is a little round about, and I am not sure if there is a better way so you have been warned.

To pass a value from C to Scheme we first use `sexp_intern` to create a new Scheme variable. We then assign it a value with `sexp_env_define`. And from there we can then use our new Scheme variable as a function argument (even though it is more like a global variable really).

square.c

    #include <stdio.h>
    #include <chibi/eval.h>

    int main()
    {
      const int num = 4;
      sexp ctx;
      ctx = sexp_make_eval_context(NULL, NULL, NULL, 0, 0);
      sexp_load_standard_env(ctx, NULL, SEXP_SEVEN);

      sexp_gc_var4(file_path, arg_sym, arg_val, ret);
      sexp_gc_preserve4(ctx, file_path, arg_sym, arg_val, ret);

      arg_val = sexp_make_fixnum(num);
      arg_sym = sexp_intern(ctx, "arg", -1);
      sexp_env_define(ctx, sexp_context_env(ctx), arg_sym, arg_val);

      file_path = sexp_c_string(ctx, "square.ss", -1);
      sexp_load(ctx, file_path, NULL);

      printf("Asking Scheme for the square of (%d)...\n", num);
      ret = sexp_eval_string(ctx, "(square arg)", -1, NULL);
      int result = -1;
      if (sexp_integerp(ret)) {
        result = sexp_unbox_fixnum(ret);
      }

      printf("...Scheme told us (%d)\n", result);

      sexp_gc_release4(ctx);
      sexp_destroy_context(ctx);
    }

square.ss

    (import (chibi))
    (define (square a)
      (* a a))

Here we use `sexp_make_fixnum` to assign the `int` value to an sexpression (`arg_val`). Then we use `sexp_intern` to create a new symbol within the Scheme context called `arg`. Lastly we use `sexp_env_define` to bind the `int` value `arg_val` to the `arg` symbol (referred to as `arg_sym` on the C side).

We can now do `sexp_eval_string(ctx, "(square arg)"` to call the Scheme `square` function with our `int` argument.
