---
layout: post
title:  "Setting up a Secure Shell Server on Windows"
date:   2015-03-28 13:16:12
categories: 
---
Install Cygwin and be sure to tick the box for openssh.

Run a Cygwin terminal as Administrator.
Run ssh-host-config This is important and will guide you through some Windows specific configuration items.

Then you can edit /etd/sshd_config

Run ssh-agent.exe. You can check that it is running with Tast Manager under the Porcesses tab.


Running ssh localhost can help determin if sshd is running.

Must allow c:\cygwin\bin\cygrunsrv.exe and inbound connections to port 22 (two separate rules). Or just disable Windows firewall.

debug1: Offering RSA public key: /home/kokoro/.ssh/id_rsa
Connection closed by 192.168.100.173
-o PreferredAuthentications="password" 
