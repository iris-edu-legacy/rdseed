# rdseed
rdseed – Read an FDSN SEED format volume

### NAME

rdseed - Read an FDSN SEED format volume

### SYNOPSIS
```
rdseed [options] [-f filename]
```
### OPTIONS
```
The following options may be selected:
    -a retrieve the abbreviation dictionaries
    -b n set the seismogram input buffer size (bytes)
    -c retrieve volume table of contents
    -C arg retrieve the comments where 'arg' is either STN or CHN
    -d output data records (follow with -o to select a format or it defaults to SAC)
    -e extract event/station data
    -E output filename will include endtime
    -f file input file name
    -g file specify alternate SEED volume for metadata (see ALT_RESPONSE_FILE)
    -h help (this list)
    -i ignore location codes
    -k strip records from output which have a zero sample count (miniSEED and SEED output only)
    -l list contents of each record in the volume
    -o n specify output format (numeric value, see examples below). Use -d with this option.
    -p output poles and zeros
    -q dir specify the output directory
    -Q qual specify data quality [E,D,M,Q,R] (see Input Options below for details)
    -R print response data
    -s retrieve all station header information
    -S retrieve station summary information
    -t show start/stop times, start blocks of events
    -u show usage (this list)
    -v n select volume number
    -x use summary file
    -z n  check for reversals and apply sign change correction based on the number n=[0-3] 
          (see Check Reversal under Options section below)
```

### DESCRIPTION
`rdseed` reads and interprets Standard for Exchange of Earthquake Data [(SEED)](https://ds.iris.edu/dms/nodes/dmc/data/formats/#seed) files. This is the format defined by the Federation of Digital Seismographic Networks [(FDSN)](http://www.fdsn.org) to represent seismic data.  According to the command line function option specified by the user, `rdseed` will read the volume and recover the volume table of contents (`-c` option), the set of abbreviation dictionaries (`-a` option), or station and channel information and instrument response tables (`-s` option).

The desired sequential volume in a file or on tape can be selected with the (`-v` option). The first volume is "1", the second "2" and so forth.  **Note:** This option only applies to physical volumes that contain multiple logical volumes.

Additional options allow access to detailed information concerning the actual contents of the volume.  The first of these options (`-t`) writes out a list of data start and stop times along with the starting record sequence numbers at which those data may be found.  The other option (`-l`) is primarily a diagnostic tool; it writes a description of every record in the volume.

While there are a large number of command line options for rdseed, the user can also run it in [User Prompt Mode](../rdseed/master/README.md/#user-prompt), discussed below.

As data is extracted from the SEED volume, @rdseed@ looks at the orientation and sensitivity of each channel to determine if the channel polarity is reversed. Refer to the description of blockettes *52* and *58* in the SEED manual for a description of reversed polarity. A negative sensitivity in *blockette 58* is indicative of a reversed polarity. The user can request that reversed channels be corrected (@-z@ option). This correction is a simple multiplication by -1.0 to the data samples for that specific channel.  The output response file information is _NOT_ adjusted for channels where @rdseed@ inverts the data.
