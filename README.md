concat-fuse
===========

`concat-fuse` is a [FUSE](http://fuse.sourceforge.net/) based virtual
filesystem for Linux that allows handling a collection of files as if
they were a single one. It is essentially doing:

    cat *.txt > large_file

But instead of creating `large_file`, it creates a virtual file and
accesses the original `*.txt` files instead. Alternatives such as
`mkfifo` or regular piping can be used similarly, but fail at
providing random access to the data.

`concat-fuse` is especially useful when it comes to do gapless
playback of video files (when the video format allows it) or to
accessing archive files that have been `split` into multiple parts and
where a temporary file would be to slow and cumbersome to deal with.


Requirements
------------

* [FUSE](http://fuse.sourceforge.net/)
* [Mhash](http://mhash.sourceforge.net/)
* [Minizip](http://www.winimage.com/zLibDll/minizip.html)

On Ubuntu 15.04 they can be installed with:

    apt-get install libfuse-dev libmhash-dev libminizip-dev


Compilation
-----------

Compiling is done with `cmake`:

    mkdir build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make

To switch on test case building and extra warning flags use:

    cmake .. -DBUILD_TESTS=1 -DDEVELOPER=1


Installation
------------

    make install

The install location can be configured with `PREFIX` and `DESTDIR`:

    cmake .. -DCMAKE_INSTALL_PREFIX:PATH=/your/prefix/here
    make install DESTDIR="/some/absolute/path"


Usage
-----

`concat-fuse` comes with a little script `cfconcat` that does the mounting
magic behind the scene, usage is thus very simple:

    $ echo "File Number One" > /tmp/1.txt
    $ echo "File Number Two" > /tmp/2.txt
    $ cat $(cfconcat /tmp/*.txt)
    File Number One
    File Number Two

The man pages for `concat-fuse` and `cfconcat` provide further information
and examples.
