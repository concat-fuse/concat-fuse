concat-fuse
===========

`concat-fuse` is a fuse based virtual filesystem that allows handling
a collection of files as if they were a single large file. It is
similar to doing:

    cat *.txt > large_file

But instead of creating `large_file`, it creates a virtual file and
accesses the original `*.txt` files.


Compilation
-----------

`concat-fuse` requires `libfuse-dev`. Compiling is done with `waf`:

    ./waf configure
    ./waf


Usage
-----

First lets create some test files:

    echo "File Number One" > /tmp/1.txt
    echo "File Number Two" > /tmp/2.txt

Then we create a directory for `concat-fuse` and mount it:

    mkdir /tmp/concat
    build/concat-fuse /tmp/concat

Finally we list the virtual file with `ls` and print it with `cat`:

    ls -l $(build/concat-pattern /tmp/concat "/tmp/*.txt")
    cat $(build/concat-pattern /tmp/concat "/tmp/*.txt")

This will produce the output:

    -r--r--r-- 2 root root 32 Jan  1  1970 /tmp/concat/by-glob/%2ftmp%2f%2a%2etxt
    File Number One
    File Number Two

The job of `concat-pattern` in this example is to translate the
pattern `/tmp/*.txt` into the name of the virtual file
`/tmp/concat/by-glob/%2ftmp%2f%2a%2etxt`.
