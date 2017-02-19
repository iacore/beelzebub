---
layout: post
title: Cross Compiling Android Native Binaries
date: 2016-05-06 19:36:20
categories: 
---
Most Android Native documentation focuses on cross compiling C extensions for Java based Android applications. However, if you look at Android as an embedded Linux box then there is really no reason why it is not possible to cross compile native binaries and run them outside of the Dalvik/Java ecosystem.

Depending on how Android specific the code is, simply using a generic ARM cross compiler may not work. However, Google does ship cross compilers, header files and everything one needs to compile native Android code in their [Android NDK](https://developer.android.com/ndk/index.html).

Setting up the cross compile environment can be tricky (which cross compiler? Where are the include files? `cannot find -lc`?). Therefor the `Android NDK` comes with it's own build system which can be configured with an `Android.mk` file.

Example source code:

    #include <stdio.h>

    int main()
    {
      printf("Hello, world!\n");
    }

Here is a minimal `Android.mk` file for building our `hello_world.c` example:

    LOCAL_PATH := $(call my-dir)
    include $(CLEAR_VARS)

    LOCAL_SRC_FILES := \
      hello_world.c

    LOCAL_MODULE := hello
    LOCAL_CFLAGS += -fPIE
    LOCAL_LDFLAGS += -fPIE -pie

    include $(BUILD_EXECUTABLE)

The [documentation](http://android.mk/) contains an explanation for the boiler plate. For our purposes the important lines are:

* `LOCAL_SRC_FILES`: This is a list of source files to compile, relative to the directory set in `LOCAL_PATH` (which is the same directory `Android.mk` is in our example). Like regular `Makefile`s, newlines must be escaped with a `\`.

* `LOCAL_MODULE`: This will be the name of the final executable we build.

* `LOCAL_CFLAGS`, `LOCAL_LDFLAGS`: Additional compiler and linker flags. In our example we enable the [Position Independent Executable](https://en.wikipedia.org/wiki/Position-independent_code#Position-independent_executables) flag because this is a [requirement for newer Android versions](https://source.android.com/security/enhancements/enhancements50.html).

To build, we run `ndk-build` (part of the Android NDK):

    $ ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=./Android.mk
    $ file obj/local/armeabi/hello
    obj/local/armeabi/hello: ELF 32-bit LSB shared object, ARM, EABI5 version 1 (SYSV), dynamically linked, interpreter /system/bin/linker, BuildID[sha1]=c192f0f232d53295290a371e824086f535e3e7dc, not stripped

A simple way to test the code without bothering with `ADB` and USB cables is to `scp` it to the phone and execute it though `ssh`.

    $ scp -P2222 hello 192.168.0.3:
    SSHelper Version 6.3 Copyright 2014, P. Lutus
    hello                             100%   40KB 351.1KB/s  39.8KB/s   00:00

    $ ssh -p2222 192.168.0.3
    SSHelper Version 6.3 Copyright 2014, P. Lutus
    Linux 3.0.72-gfb3c9ac armv7l
    u0_a68@tuna:home $ ./hello
    hello world
    u0_a68@tuna:home $
