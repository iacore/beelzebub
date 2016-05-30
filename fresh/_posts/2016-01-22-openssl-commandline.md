---
layout: post
title: OpenSSL Commandline Tools
date: 2016-01-22 19:33:23
categories: 
---
[Public Key Cryptography](http://blogs.msdn.com/b/plankytronixx/archive/2010/10/23/crypto-primer-understanding-encryption-public-private-key-signatures-and-certificates.aspx) is a useful technology which allows someone to encrypt data without being able to decrypt it again. This works by having a Public Key and Private Key. The Public Key can encrypt data but then only the corresponding Private Key can decrypt it again.

One thing I have been thinking about recently is the possibility of leaving a running system unattended to gather metrics or some other kind of data. If the system were compromised in some way, I would not want the attacker to have free reign over all the data collected so far but clearly I want the legitimate owner to be capable of reading the data back when needed.

This seems like the kind of problem which can be solved by Public Key Cryptography. By only making the Public Key available to the system, it could encrypt data in such a way that only someone in possession of the Private Key can decrypt it again. By processing data in memory and piping directly to the encryption process, we can avoid writing sensitive data in plaintext to permanent storage.

Full disk Encryption may seem like a more intuitive choice for this scenario however, if the system is to operate in a truly independent manner it must be able to boot and reboot without human interaction; if it can decrypt its own disk then so can anyone who gets their hands on it. Worse still, if an adversary gains access to the running system, extracting the symmetric encryption key from memory would be a trivial process and give the attacker full access to all the data collected. Additionally, there are various [legal](https://en.wikipedia.org/wiki/Key_disclosure_law) and [illegal](http://xkcd.com/538/) means by which someone can gain access to symmetrically encrypted data after the fact. I think it is much better (in some circumstances) to be in a position where it is physically impossible to decrypt the data without the Private Key; thus shifting the game away from I-obviously-know-the-password-but-won't-tell-you and towards one of you-can't-prove-I-that-I-did-not-destroy-the-private-key-or-was-ever-in-possession-of-it-to-begin-with.

As the title suggests I will use [OpenSSL](https://wiki.openssl.org/index.php/Command_Line_Utilities). Although [GPG](http://gnupg.org/) is a more established tool in this field I do not like the way it tries to manage 1000 different things at the same time and the rigid key management workflow it imposes on users. Next time the project starts begging for money, I have half a mind to donate to them a copy of [The Art of Unix Programming](https://en.wikipedia.org/wiki/The_Art_of_Unix_Programming) ☺.

First we need a Public/Private Key pair.
{% highlight bash %}
# Generate the Private Key
$ openssl genrsa -out key
# Derive the Public Key from the Private Key we just made.
$ openssl rsa -in key -pubout > key.pub
{% endhighlight %}

This is how we can encrypt a file.
{% highlight bash %}
$ echo Hello World > plain
$ openssl pkeyutl -in plain -out enc -encrypt -pubin -inkey key.pub
$ cat enc  | base64 
b9LzWMFvr3htwlx8dpP6rjD09D0QWZFJabG3+SKsCBBTnoRzH39xiFpt/OeUCijh/EYTbrt/T1i4
lZb+Phn/OlQiYyCAsQYJ4emxhv2L5qQO41tCAZfgXHVKs9ncBPxYNDOzTvSbcgssArMYJx09VWHd
kQWNqQG8EXOZpknlp3kSqfdPGNny6pIXMJ8yJjVvNambQw6OwrwAzLSRy+5HkOf99f+sW26iCDm7
Ax0XXuXtvBE6RfY4Ufh8Sj57FllZPUjO/fv8aW0SZ6i8Sjw9kiu/4p3L1maKih/6/87s9amei54Y
8iut7rfJlVLs5pIcD52nwyF1LkIVz55HJfZOfw==
{% endhighlight %}
Note that `-pubin` must come before `-inkey`, otherwise it will be ignored and
OpenSSL will be expecting a private key. The `enc` file should now be encrypted. If it is at all possible that excitement got the better of you or you otherwise accidentally did `cat` the encrypted file (without encoding the binary data into an ASCII compatible format like we did above with `base64`) and now there are weird symbols all over your terminal, type `reset` to fix that.

Decryption is much the same process except with the `-decrypt` flag instead of `-encrypt`, and passing in the Private Key instead of the Public one.
{% highlight bash %}
$ openssl pkeyutl -in enc -out decrypted -decrypt -inkey key
$ cat decrypted
Hello world
{% endhighlight %}

Excellent. Lets try something more exciting.

{% highlight bash %}
$ wget google.com -O - | openssl pkeyutl -out index.html.enc -encrypt -pubin -inkey key.pub
--2016-01-21 09:53:59--  http://google.com/
Resolving google.com (google.com)... 216.58.208.78, 2a00:1450:4009:801::200e
Connecting to google.com (google.com)|216.58.208.78|:80... connected.
HTTP request sent, awaiting response... 302 Found
Location: http://www.google.co.uk/?gfe_rd=cr&ei=pqqgVvbEO4fj8wfi8qroAw [following]
--2016-01-21 09:53:59--  http://www.google.co.uk/?gfe_rd=cr&ei=pqqgVvbEO4fj8wfi8qroAw
Resolving www.google.co.uk (www.google.co.uk)... 74.125.133.94, 2a00:1450:400c:c07::5e
Connecting to www.google.co.uk (www.google.co.uk)|74.125.133.94|:80... connected.
HTTP request sent, awaiting response... 200 OK
Length: unspecified [text/html]
Saving to: ‘STDOUT’

    [<=>                                    ] 0           --.-K/s              Public Key operation error
    140214184167072:error:0406D06E:rsa routines:RSA_padding_add_PKCS1_type_2:data too large for key size:rsa_pk1.c:151:
        [ <=>                                   ] 13,534      --.-K/s   in 0.02s


        Cannot write to ‘-’ (Broken pipe).
{% endhighlight %}
OpenSSL gets a lot of bad press these days but I think their error reporting looks pretty decent.

<pre>
Public Key operation error
140214184167072:error:0406D06E:rsa routines:RSA_padding_add_PKCS1_type_2:data too large for key size:rsa_pk1.c:151:
</pre>
As it turns out we cannot encrypt data longer than the length of our encryption keys, which are typically of the order of a few thousand bits. (Well okay maybe you *can* encrypt longer data but I'm not convinced it is a good idea).

The solution to this problem is to use symmetric encryption for the data and then Public Key encryption for the symmetric key. This was we get the best of both worlds.

{% highlight bash %}
symmetric_key=$(openssl rand 32 | base64)
in_file=plain
echo $symmetric_key | openssl enc -aes-256-cbc -in "$in_file" -out "$in_file".enc -pass stdin
echo $symmetric_key | openssl pkeyutl -out symmetric.key.enc -encrypt -pubin -inkey key.pub
symmetric_key=""
{% endhighlight %}

And to decrypt:
{% highlight bash %}
n_file="plain.enc"
openssl pkeyutl -in symmetric.key.enc -out symmetric.key -decrypt -inkey key
openssl enc -aes-256-cbc -d -in "$in_file" -out ${in_file%.enc} -pass file:symmetric.key
{% endhighlight %}

This immediately raises questions in my mind about how long environmental variables survive for and how easy it might be to extract key material from memory if an adversary were to compromise the system while it is running. Of course, if an adversary did have access to the memory of a running system then they could capture the data as it is being encrypted, there is not much we can do about that.

But at the very least, while the data is at rest it will be encrypted; and in such a way that the owner could plausibly argue that they have no way of decrypting it because someone else has the Private Key or the Private Key has been lost.
