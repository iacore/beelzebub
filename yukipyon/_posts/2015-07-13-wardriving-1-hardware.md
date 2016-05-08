---
layout: post
title:  "Wardriving Part 1 - Hardware"
date:   2015-03-28 13:16:12
categories: 
---
Wardring is the pasttime of passivly gathering information on wireless networks, usually with the goal of visually plotting that information on a geographical map. In this series of posts I will cover a viable approach to wardriving.

Hardware
===
Wardriving inherently involves moving around, which restricts the availablity of power hence, a portable computer powered by battery is in order. 

Smart Phones sure are portable but their antennas have limited range. It is also worth noting that mobile OSes like Android and iOS are commercial products, while you may be able to "root" the device and get it to work with alternative hardware and software, I would rather not fight against what the systems were designed for.

Laptops are also portable, battery powered but at the same time very general purpose. One issue I have with laptops though is that they can get very hot if they are sealed up in a bag for many hours. Additionally, they are severely overspecced for what we want to do, it kind of feels like a waste.

Single board computers were all the rage a few years ago and really hit the meainstream with the arrivial of the Raspberry Pi. Since I was the kind of teenager who received Raspberry Pis as presenets, this seems like a good choice.

Battery
===
The Raspberry Pi needs power. One solution is to use a portable power bank. These are objects about the size of a smart phone which can provide power over USB. Perfect. Persoanally I use the [Anker Astro E3](http://www.amazon.co.uk/gp/product/B0063AAIRG).

Wireless Card 
===
Any wireless card that works with your operating system of choice would be a good start. If you're serious though, a card with a detachable antenna is essential to gain extra range. If you don't already have a suitable card, one I cannot recommend enough is the [TL-WN722N](http://www.amazon.co.uk/gp/product/B002SZEOLG) which uses the open source ath9k_htc driver on Linux.

USB Splitter Cable 
===
One disadvantage of the Raspberry Pi is that the power output for USB devices is severly limited. So much so that any decent USB wireless card will not work at all. One popular solution is to use a powered USB hub. That's okay, but can be a bit bulky to carry around, espeically since we only have 2 USB devices to plug in anyway, we won't even use the extra USB ports a hub would provide. Wastful.

One neat solution is to use a split USB cable, one with 2 male connectors and 1 female. Much like [this one I have](http://www.amazon.co.uk/gp/product/B003OC4KX4). One male goes in the battery, the other in the Pi. In the female end you put the high power USB device, in our case the wireless card. Bingo.

GPS 
===
There is little point capturing wireless data if you have no idea where you are, so lets add a USB GPS module to the mix. One that I have found works well is the [Bluenext BN903S](http://www.amazon.co.uk/gp/product/B003Q6KZOQ). This does not require much power and can be plugged straight into the Pi. However, I would reccomend using USB extension cable. 

While everything discussed so far will easily fit inside a small bag or laptop bag depending on the size of the antenna, keep in mind that the GPS module requires a unobstructed line of sight to the GPS satelites, therefor the best place to put it would be outside of said bag. If you are in a car then taping it to the dashboard or back of the rear view mirror works quite well in my experiance.

Putting it All Together
=== 
A final setup may look something like this.

![wardriving-hardware-setup]({{ site.baseurl }}/images/wardriving-hardware-setup.jpg)
