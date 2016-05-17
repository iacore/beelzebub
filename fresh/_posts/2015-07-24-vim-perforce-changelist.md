---
layout: post
title:  "How to Make Vim recognise Perforce Changelist Submissions"
date: 2015-07-24 20:48:42
categories: 
---
If you're lucky enough to deal with Perforce on a regular basis you'll be familiar with the text based forms it uses to solicit user input. Since there are always fields that must be modified or added in the same way each time, it makes sense to automate some of the process. 

Perforce forms are basically temporary text files which are opened with the user's text editor of choice (indicated by the value of an environmental variable). We could write a script which performs our custom form modifications before passing it to the text editor or we could embed our automation in the text editor itself.

Lets try the latter option using Vim.

The following Vimscript can be embedded directly into `vimrc` or put in its own `.vim` file under the vim directory.

{% highlight vim %}
if did_filetype()
    finish
endif 

if getline(1) =~ '# A Perforce Change Specification.$'

    "setfiletype p4_submit
    "execute "normal gg23ggoJobs:\<esc>o\<esc>"
    "do something else
endif
{% endhighlight %}

The line `execute "normal gg23ggoJobs:\<esc>o\<esc>"` is an example of adding a `Jobs:` field to the form and then positioning the cursor on the line below.

There may be many other things specific to your workflow or environment that you may like to add. For example, while working at [CSR](https://en.wikipedia.org/wiki/CSR_%28company%29) I wrote a script which would query CSR's proprietary bug tracking database and insert a commented list of bug ids and descriptions I was currently assigned to under the `Jobs:` field. It then become a simple matter uncommenting the correct bug id instead of looking it up with a web browser and copy & pasting it over to the form manually.

I hope this post will give readers some ideas on how they can improve their productivity when interacting with Perforce.
