---
layout: post
title: Chibi Scheme Part 5 -- Putting it All Together
date: 2016-04-01 19:36:20
categories: 
---
Putting it All Together
===
For the final example I will exhibit all the techniques shown previously in a single application. We will write a Scheme function which will calculate and return the length of a string.

For the impatient, the quickest way to get started may be to look at this example with the official [API Reference](http://synthcode.com/scheme/chibi/) to hand.

The only thing explicitly not covered in the previous posts is the use of `sexp_procedurep` to check that a Scheme function has been defined before trying to call it; just for the sake of printing an error message and letting the user know what is wrong (nothing bad would happen otherwise).

strlen.c

    #include <stdio.h>
    #include <chibi/eval.h>
  
    int my_str_len(sexp ctx, const char *message)
    {
        int len = -1;
        sexp_gc_var3(arg_sym, arg_val, ret);
        sexp_gc_preserve3(ctx, arg_sym, arg_val, ret);

        arg_val = sexp_c_string(ctx, message, -1);
        arg_sym = sexp_intern(ctx, "arg", -1);
        sexp_env_define(ctx, sexp_context_env(ctx), arg_sym, arg_val);
        ret = sexp_eval_string(ctx, "(my-str-len arg)", -1, NULL);

        if (sexp_integerp(ret)) {
            len = sexp_unbox_fixnum(ret);
        }

        sexp_gc_release3(ctx);
        return len;
    }

    int is_defined(sexp ctx, const char *sym)
    {
        sexp_gc_var1(ret);
        sexp_gc_preserve1(ctx, ret);

        ret = sexp_eval_string(ctx, sym, -1, NULL);

        int defined = sexp_procedurep(ret);

        sexp_gc_release1(ctx);

        return defined;
    }

    int main(int argc, char **argv)
    {
        if (argc < 2) {
            printf("Usage: %s [MESSAGE]...\n", argv[0]);
            return 1;
        }

        int i;
        sexp ctx;
        ctx = sexp_make_eval_context(NULL, NULL, NULL, 0, 0);
        sexp_load_standard_env(ctx, NULL, SEXP_SEVEN);

        sexp_gc_var1(file_path);
        sexp_gc_preserve1(ctx, file_path);
        file_path = sexp_c_string(ctx, "strlen.ss", -1);
        sexp_load(ctx, file_path, NULL);

        const char *sym = "my-str-len";
        if (is_defined(ctx, sym))
            printf("%s is defined.\n", sym);
        else {
            printf("%s is not defined!\n", sym);
            return 1;
        }

        for (i = 1; i < argc; ++i) {
            const char *msg = argv[i];
            int len = my_str_len(ctx, msg);
            printf("Chibi told me that the length of <%s> is <%d>\n", msg, len);
            printf("C told me that the length of <%s> is <%lu>\n", msg, strlen(msg));
        }

        sexp_gc_release1(ctx);
        sexp_destroy_context(ctx);
    }

strlen.ss

    (define (my-str-len s)
      (letrec ((my-str-len-h (lambda (s acc)
        (if (null? s)
          acc
          (my-str-len-h (cdr s) (+ 1 acc))))))
      (my-str-len-h (string->list s) 0)))

To build:

    $ gcc -Iinclude/ -L. -lchibi-scheme -lm strlen.c -o strlen

Example usage:

    $ LD_LIBRARY_PATH=. ./strlen hello something unicode†string
    my-str-len is defined.
    Chibi told me that the length of <hello> is <5>
    C told me that the length of <hello> is <5>
    Chibi told me that the length of <something> is <9>
    C told me that the length of <something> is <9>
    Chibi told me that the length of <unicode†string> is <14>
    C told me that the length of <unicode†string> is <16>

Note how in the case of the last string which contains unicode, C gets the wrong answer. ;)
