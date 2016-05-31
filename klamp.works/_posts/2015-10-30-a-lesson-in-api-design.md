---
layout: post
title: A Lesson In API Design
date: 2015-10-30 18:20:08
categories: 
---
This is an issue which hit me while reading [The Most Dangerous Code in the World](http://www.cs.utexas.edu/~shmat/shmat_ccs12.pdf). The popular cURL library contains an option called `CURLOPT_SSL_VERIFYHOST`. The intention of the option is to verify that a given SSL certificate is for the website actually being accessed (and not something a Man In The Middle has provided instead).

While a value of `0` will disable this option, as one might expect, a value of `1` will also disable it; `2` is the correct value. However, [many developers](https://github.com/search?q=CURLOPT_SSL_VERIFYHOST%2C+true&type=Code&ref=searchresults) intuitively try to set a value of `true` or `false`; with `true` actually represented as `1` behind the scenes of many programming languages.

Not surprisingly the cURL library will [no longer](http://curl.haxx.se/libcurl/c/CURLOPT_SSL_VERIFYHOST.html) support setting the `CURLOPT_SSL_VERIFYHOST` option to `1`.

It still remains as a gentle reminder to the importance of intuitive API design since even the most diligent developers do not always read the documentation.
