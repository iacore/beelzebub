---
layout: post
title: Editing the Vim Splash Screen
date: 2016-01-01 19:59:55
categories: 
---
Vim is great but do you ever get tired of having "Help poor children in Uganda!" thrown in your face everytime it opens? I don't particularly appreciate the author using Vim as a platform for his personal views. Although perhaps naming my own [projects](https://github.com/yukipyon?tab=repositories) after biblical demons is not so different. In any event, you may have other compelling reasons to customise your Vim splash page into something more useful or motivating.

Below is some Vimscript which can be added to a `.vimrc` to produce a custom splash page. This one simply prints the word "hello". I have added lots of comments so hopfully every part is clear even if the reader does not know much Vimscript.

{% highlight vim %}
fun! Start()

  "Create a new unnamed buffer to display our splash screen inside of.
  enew

  " Set some options for this buffer to make sure that does not act like a
  " normal winodw.
  setlocal
    \ bufhidden=wipe
    \ buftype=nofile
    \ nobuflisted
    \ nocursorcolumn
    \ nocursorline
    \ nolist
    \ nonumber
    \ noswapfile
    \ norelativenumber

  " Our message goes here. Mine is simple.
  call append('$', "hello")

  " When we are done writing out message set the buffer to readonly.
  setlocal
    \ nomodifiable
    \ nomodified

  " Just like with the default start page, when we switch to insert mode
  " a new buffer should be opened which we can then later save.
  nnoremap <buffer><silent> e :enew<CR>
  nnoremap <buffer><silent> i :enew <bar> startinsert<CR>
  nnoremap <buffer><silent> o :enew <bar> startinsert<CR>

endfun

" http://learnvimscriptthehardway.stevelosh.com/chapters/12.html
" Autocommands are a way of setting handlers for certain events.
" `VimEnter` is the event we want to handle. http://vimdoc.sourceforge.net/htmldoc/autocmd.html#VimEnter
" The cleene star (`*`) is a pattern to indicate which filenames this Autocommand will apply too. In this case, star means all files.
" We will call the `Start` function to handle this event.

" http://vimdoc.sourceforge.net/htmldoc/eval.html#argc%28%29
" The number of files in the argument list of the current window.
" If there are 0 then that means this is a new session and we want to display
" our custom splash screen.
if argc() == 0
  autocmd VimEnter * call Start()
endif
{% endhighlight %}

If you'd rather separate the message from the code, it is possible to read it in from an external file. I think adding a random [BSD Fortune](https://en.wikipedia.org/wiki/Fortune_%28Unix%29) quote to the top is a neat idea. Or perhaps a little bit of ascii art with the help of [jp2a](http://sourceforge.net/projects/jp2a/).

{% highlight bash %}
$ jp2a god.jpg --width=80 > .god.txt
{% endhighlight %}

{% highlight vim %}
fun! Start()
    ...
    exec ":r !fortune"
    exec ":r ~/.god.txt"
{% endhighlight %}

![aa]({{ site.baseurl }}/images/haruhi_ascii.jpg)
