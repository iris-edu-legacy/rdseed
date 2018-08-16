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
    -z n  check for reversals and apply sign change correction based on the number n=[0-3] (see Check Reversal under Options section below)
```

### DESCRIPTION
`rdseed` reads and interprets Standard for Exchange of Earthquake Data [(SEED)](https://ds.iris.edu/dms/nodes/dmc/data/formats/#seed) files. This is the format defined by the Federation of Digital Seismographic Networks (FDSN) to represent seismic data.

