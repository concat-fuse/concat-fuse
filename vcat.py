#!/usr/bin/env python2

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


import argparse
import os
import urllib
import subprocess


def init_concat_fuse():
    """Returns the name of the concat-fuse directory and mount it when needed"""
    
    runtime_base_dir = os.environ.get("XDG_RUNTIME_DIR")
    if runtime_base_dir:
        concat_fuse_dir = os.path.join(runtime_base_dir, "concat-fuse")
    else:
        concat_fuse_dir = os.path.join(os.environ["HOME"], ".concat-fuse")

    if not os.path.exists(concat_fuse_dir):
        os.mkdir(concat_fuse_dir)

    if not os.path.ismount(concat_fuse_dir):
        subprocess.check_call(["concat-fuse", concat_fuse_dir])
        
    return concat_fuse_dir


def main():
    parser = argparse.ArgumentParser(description="Create virtual concatenated files from a glob pattern",
                                     epilog="The name of the virtual file is returned on stdout.")
    parser.add_argument('GLOB', action='store', nargs=1, help="A glob pattern")
    args = parser.parse_args()

    concat_fuse_dir = init_concat_fuse()

    quoted_glob = urllib.quote(args.GLOB[0], safe="")
    virtual_file = os.path.join(concat_fuse_dir, "by-glob", quoted_glob)
    print virtual_file


if __name__ == "__main__":
    main()

    
# EOF #
