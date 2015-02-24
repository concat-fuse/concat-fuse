concat-fuse
===========

`concat-fuse` is a [FUSE](http://fuse.sourceforge.net/) based virtual
filesystem that allows handling a collection of files as if they were
a single one. It is essentially doing:

    cat *.txt > large_file

But instead of creating `large_file`, it creates a virtual file and
accesses the original `*.txt` files instead. Alternatives such as
`mkfifo` or regular piping can be used similarly, but fail at
providing random access to the data.

`concat-fuse` is especially useful when it comes to do gapless
playback of video files (when the video format allows it) or to
accessing archive files that have been `split` into multiple parts and
where a temporary file would be to slow and cumbersome to deal with.


Compilation
-----------

`concat-fuse` requires `libfuse-dev`. Compiling is done with `waf`:

    ./waf configure
    ./waf

To switch on test case building and extra warning flags use:

    ./waf configure --developer --build-tests


Installation
------------

    ./waf install

The options `--prefix=PREFIX` and `--destdir=DESTDIR` are available to
change the installation directory.


Usage
-----

`concat-fuse` comes with a little script `vcat` that does the mounting
magic behind the scene, usage is thus very simple:

    $ echo "File Number One" > /tmp/1.txt
    $ echo "File Number Two" > /tmp/2.txt
    $ cat $(vcat "/tmp/*.txt")
    File Number One
    File Number Two

Don't forget to quote the glob pattern when passing to `vcat`.


Behind the scenes
-----------------

The following example should give an rough idea of how `concat-fuse`
works. First lets create some test files that we later want to join:

    $ echo "File Number One" > /tmp/1.txt
    $ echo "File Number Two" > /tmp/2.txt

Then we create a directory for `concat-fuse` and mount it:

    $ mkdir /tmp/concat
    $ build/concat-fuse /tmp/concat

Finally we list the virtual file with `ls` and print it with `cat`:

    $ ls -l $(build/concat-pattern /tmp/concat "/tmp/*.txt")
    -r--r--r-- 2 root root 32 Jan  1  1970 /tmp/concat/by-glob/%2ftmp%2f%2a%2etxt
    $ cat $(build/concat-pattern /tmp/concat "/tmp/*.txt")
    File Number One
    File Number Two

The job of `concat-pattern` in this example is to translate the
pattern `/tmp/*.txt` into the name of the virtual file
`/tmp/concat/by-glob/%2ftmp%2f%2a%2etxt`.

The pattern is simply
[URL encoded](http://en.wikipedia.org/wiki/Percent-encoding) and then
looked up in the `by-glob/` directory. Accessing files in the
`by-glob/` directory will trigger the creation of the virtual file and
collect the neccessary information.
