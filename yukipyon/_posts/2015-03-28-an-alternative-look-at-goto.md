---
layout: post
title:  "An Alternative Look at Go To Considered Harmful"
date:   2015-03-28 13:16:12
categories: 
---

In this post I'd like to present my own objective argument for why the goto keyword may be considered harmful with an example casestudy based on real experiance.

Introduction
===========

Initial disclaimer, I promised objectivity. This is not a post about "goto is hard" or "makes code unreadable" which is essentially what Djikstra's original paper boiled down to. In this post I want to dig a little deeper than that.

I orginiannly ran into this problem in university. I do not remember the exact context so I'll create a contrived example.

In the world of exercise there is this concept of ladders.
Lets say the problem is finding the sum when counting "ladders". If the target is 4 we will count (1) + (1 + 2) + (1 + 2 + 3) + (1 + 2 + 3 + 4) (1 + 2 + 3 + 4 + 5).
