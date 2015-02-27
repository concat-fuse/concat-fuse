#!/usr/bin/env python

# concat-fuse
# Copyright (C) 2015 Ingo Ruhnke <grumbel@gmx.de>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


from __future__ import print_function

import argparse
import glob
import hashlib
import os
import subprocess
import sys


class ConcatFuse:

    @staticmethod
    def unmount():
        basedir = ConcatFuse.find_basedir()
        if os.path.ismount(basedir):
            subprocess.check_call(["fusermount", "-u", basedir])

    @staticmethod
    def find_basedir():
        runtime_base_dir = os.environ.get("XDG_RUNTIME_DIR")
        if runtime_base_dir:
            basedir = os.path.join(runtime_base_dir, "concat-fuse")
        else:
            basedir = os.path.join(os.environ["HOME"], ".concat-fuse")
        return basedir

    def __init__(self, basedir=None):
        """Mounts concat-fuse to basedir when needed"""

        if basedir is None:
            self.basedir = ConcatFuse.find_basedir()
        else:
            self.basedir = basedir

        if not os.path.exists(self.basedir):
            os.mkdir(self.basedir)

        if not os.path.ismount(self.basedir):
            subprocess.check_call(["concat-fuse", self.basedir])

    def concat(self, files):
        """Send the file list to concat-fuse and return the filename of the virtual file"""

        # write the file list to concat-fuse
        files_serialized = "\0".join(files)
        with open(os.path.join(self.basedir, "from-file0/control"), "wb") as fout:
            fout.write(files_serialized)

        # return the virtual file name
        multifile_id = hashlib.sha1(files_serialized).hexdigest()
        virtual_file = os.path.join(self.basedir, "from-file0", multifile_id)
        return virtual_file

    def glob(self, globs):
        """Send the glob list to concat-fuse and return the filename of the virtual file"""

        # write the file list to concat-fuse
        globs_serialized = "\0".join(globs)
        with open(os.path.join(self.basedir, "from-glob0/control"), "wb") as fout:
            fout.write(globs_serialized)

        # return the virtual file name
        multifile_id = hashlib.sha1(globs_serialized).hexdigest()
        virtual_file = os.path.join(self.basedir, "from-glob0", multifile_id)
        return virtual_file


def splitlines0(data):
    lst = data.split("\0")
    if lst and lst[-1] == "":
        del lst[-1]
    return lst


def main():
    parser = argparse.ArgumentParser(description="Create virtual concatenated files.",
                                     epilog="The name of the virtual file is returned on stdout.")

    # argparser does not allow freely mixing optional and positional
    # arguments, this limits the functionality of `vcat` somewhat, but
    # shouldn't be to critical: http://bugs.python.org/issue14191

    concat_fuse_group = parser.add_argument_group("concat-fuse options")
    concat_fuse_group.add_argument('-u', '--unmount', action='store_true', help="Unmount concat-fuse")

    files_group = parser.add_argument_group("file options", "Options for static file lists.")
    files_group.add_argument('-n', '--dry-run', action='store_true', help="Don't send file list to concat-fuse, print to stdout")
    files_group.add_argument('-k', '--keep', action='store_true', help="Continue despite errors")
    files_group.add_argument('FILES', nargs='*', help="Files to virtually concat")
    files_group.add_argument('--from-file', metavar='FILE', action='append', default=[], help="Read files from FILE, newline separated")
    files_group.add_argument('--from-file0', metavar='FILE', action='append', default=[], help="Read files from FILE, '\\0' separated")

    files_group = parser.add_argument_group("glob options",
                                            ("Glob pattern act as dynamic file lists and the underlying virtual "
                                            "file can be dynamically updated when new files arrive."))

    files_group.add_argument('-g', '--glob', metavar='GLOB', action='append', default=[], help="Use glob")

    stdin_group = files_group.add_mutually_exclusive_group()
    stdin_group.add_argument('-', dest="stdin", action='store_true', help="Read files from stdin")
    stdin_group.add_argument('-0', dest="stdin0", action='store_true', help="Read files from stdin, '\\0' separated")

    args = parser.parse_args()

    if args.unmount:
        ConcatFuse.unmount()
    else:
        # generate the file list
        files = []

        if args.stdin:
            files.extend(sys.stdin.read().splitlines())
        elif args.stdin0:
            files.extend(splitlines0(sys.stdin.read()))

        for f in args.from_file:
            with open(f, "r") as fin:
                files.extend(fin.read().splitlines())

        for f in args.from_file0:
            with open(f, "rb") as fin:
                files.extend(splitlines0(fin.read()))

        files.extend(args.FILES)

        files = [os.path.abspath(f) for f in files]

        # generate glob list
        globs = []
        globs.extend(args.glob)
        globs = [os.path.abspath(g) for g in globs]

        if globs and files:
            print("Can't mix globs and regular files", file=sys.stderr)
            sys.exit(1)

        errors = False
        for f in files:
            if not os.path.exists(f):
                print("%s: No such file or directory" % f, file=sys.stderr)
                errors = True

        if errors and not args.keep:
            sys.exit(1)

        if files:
            if args.dry_run:
                for f in files:
                    print(f)
            else:
                concat_fuse = ConcatFuse()
                virtual_filename = concat_fuse.concat(files)
                print(virtual_filename)
        elif globs:
            if args.dry_run:
                for g in globs:
                    print(g)
            else:
                concat_fuse = ConcatFuse()
                virtual_filename = concat_fuse.glob(globs)
                print(virtual_filename)
        else:
            print("%s: fatal error: no input files provided" % sys.argv[0], file=sys.stderr)
            sys.exit(1)


if __name__ == "__main__":
    main()


# EOF #
