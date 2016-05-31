---
layout: post
title:  Forced Perforce Saving with Vim
date: 2015-07-17 20:08:31
categories: 
---
One of the awkward mechanics of Perforce is that all files in the local repository are marked read-only unless explicitly opened through `p4 open`. This can be especially irritating when Vim is already open and half a dozen changes have been made to the file. Lets create a Vim command to deal with this elegantly.

The following vimscript can go straight into your `.vimrc` file.
{% highlight vim %}
command! Fu call FU()

function! FU()
    silent !p4 open %
    execute "w!"
    execute "redraw!"
endfunction

{% endhighlight %}

This is more or less what one might type into Vim in order to remedy the situation manually with the exception of `redraw!` which was required for me to avoid graphical glitches.

Now our burden has been reduced to typing `:Fu`† in Vim and the file will be legitimately opened and saved.

----
† `Fu` may be an abbreviation of something. Use your imagination.
