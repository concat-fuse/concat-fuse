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
    $ cat $(vcat /tmp/*.txt)
    File Number One
    File Number Two

The man pages for `concat-fuse` and `vcat` provide further information.
