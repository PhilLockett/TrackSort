# TrackSort
'TrackSort' is a command-line utility for splitting tracks across multiple
'sides'.

**USE AT OWN RISK.**

## Overview
This utility reads a text file contating a list of track lengths and titles
and splits them across multiple 'sides' based on the options provided.

To use 'TrackSort' you will need a C++ compiler and 'make' utility installed. 

## Usage
With 'TrackSort' compiled the following command will display the help page:

    $ ./TrackSort -h
    Usage: ./TrackSort [Options]

    Splits a list of tracks across multiple discs of a given length.

    Options:
            -h --help               This help page and nothing else.
            -i --input <file>       Input file name containing the track listing.
            -d --duration <seconds> Maximum length of each disc.
            -t --timeout <seconds>  The maximum time to spend looking.
            -e --even               Require an even number of discs (sides).
            -s --shuffle            Reorder tracks for optimal fit.

### Track list file (mandatory)
The text file containing the track list is specified using '-i' or '--input'.
Each line of the text file has the track length followed by the track title.
The track length can be specified using the hh:mm:ss format, but can also use
mm:ss format or simply in seconds.

### Side length (mandatory)

### Maximum processing timeout

### Even number of sides

### Re-ordering tracks

## Cloning and compiling
To clone and compile this code, execute the following unix/linux commands:

    git clone https://github.com/PhilLockett/TrackSort.git
    cd TrackSort/
    make

## Points of interest
This code has the following points of interest:

  * Uses getopt_long() to help handle command line parameters.
  * The command line parameters are stored in the Config class.
  * The Config class is implemented as a singleton.
  * The Config setters are private so only methods and friends can use them.
