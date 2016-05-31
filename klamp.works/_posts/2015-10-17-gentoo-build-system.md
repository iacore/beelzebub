---
layout: post
title: Creating a Gentoo Build System
date: 2015-10-16 20:40:18
categories: 
---
One of the cumbersome things about maintaining multiple Gentoo Linux installations is the amount of duplicate compilation required when using similar configurations across machines. Fortunately Potage offers support for handling binary packages and while the Gentoo project does not maintain any infrastructure for building and distributing binary packages, there is no reason why individuals and organisations cannot run their own Gentoo build system.

Server
---
On the build machine, adding the following line to `make.conf` will ensure that all software build by the machine will also be converted into a binary package which can then be transferred and installed on another machine.

    FEATURES="${FEATURES} buildpkg"

It may also be necessary to add the following line to indicate where on the filesystem binary packages should be kept.

    PKGDIR="${PORTDIR}/packages"

Client
---
On the client machine, add the following line to `make.conf` to make sure that all software will be installed from binary packages and it will never go off and try to compile things itself.

    EMERGE_DEFAULT_OPTS="${EMERGE_DEFAULT_OPTS} --getbinpkgonly"

If you prefer a hybrid approach where a regular source based install will occur if a binary package is not available then use the `--getbinpkg` option instead.

There are a number of options in terms of how to transfer packages between machines: HTTP, SSH and NFS. For a personal network I feel that SSH is the best choice so I will only cover that.

To tell the client where to find binary packages add the following line to `make.conf`.

    PORTAGE_BINHOST="ssh://user@hostname/usr/portage/packages"

Note that there is no colon between the hostname and the file path as there is in SCP.

Typing in an SSH password before each emerge is not particularly cool. If you setup [passwordless SSH](https://wiki.gentoo.org/wiki/SSH#Passwordless_Authentication) access with public key cryptography then an additional option is required to tell Portage where to find key material.

    PORTAGE_SSH_OPTS="-i /home/user/.ssh/id_rsa"


