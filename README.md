LD_PRELOAD concat hack
======================

Usage:

    CONCAT_PATTERN="/tmp/*.txt" LD_PRELOAD=concat.so cat CONCAT_MAGICFILE

"CONCAT_MAGICFILE" is a magic filename that will contain all the
content of the files matched by CONCAT_PATTERN.
