# 7date

This is a simple command-line utility to print out your system time as a
[7date](http://tylerneylon.com/a/7date_spec/).
It's meant as a simpler analog of the standard `date` command for the 7date
calendar.

## Building

I've only built this on mac os x. I suspect it would also work, possibly with
small tweaks, in linux.

    # Clone this repo, cd into it, and run:
    $ make 7date

If you're hardcore about the 7date, you may want it in your path:

    # Run this from the repo directory after building:
    sudo ln $(pwd)/7date /usr/local/bin/7date
    sudo mkdir -p /usr/local/bin/man/man1/
    sudo ln $(pwd)/man1/7date.1 /usr/local/bin/man/man1/

## Running

See today's 7date:

    $ 7date
    623.2031

See the 7date of a file called bob_the_file:

    $ 7date /path/to/bob_the_file
    123.2026

Use 7date's digital notation, which keeps the most-significant digit first and
is of fixed length:

    $ 7date -d
    2031-0623

Print a date at week or 7month resolution:

    $ 7date --week
    62-.2015

    $ 7date --7month
    6--.2015

