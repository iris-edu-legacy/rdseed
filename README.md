# rdseed
rdseed – Read an FDSN SEED format volume

### Install the following libraries

```bash
apt install make g++ libnsl-dev libntirpc-dev
```

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

While there are a large number of command line options for rdseed, the user can also run it in [User Prompt Mode](./README.md/#user-prompt-mode), discussed below.

As data is extracted from the SEED volume, `rdseed` looks at the orientation and sensitivity of each channel to determine if the channel polarity is reversed. Refer to the description of blockettes **52** and **58** in the SEED manual for a description of reversed polarity. A negative sensitivity in **blockette 58** is indicative of a reversed polarity. The user can request that reversed channels be corrected (`-z` option). This correction is a simple multiplication by -1.0 to the data samples for that specific channel.  The output response file information is _NOT_ adjusted for channels where `rdseed` inverts the data.


#### Output Header Corrections

`rdseed` corrects the header information in the SAC, AH and CSS files as follows:

* **SAC** headers contain dip and azimuth information.  If a dip/azimuth reversal checking is active (`-z` option 1) and detected, `rdseed` will reverse the sign of the data samples and the dip and azimuth values.  If gain reversal checking is active (`-z` option 2) and detected, then `rdseed` will reverse the sign of the data samples (no header information is altered).  If both dip/azimuth and gain reversals are being checked for (`-z` option 3), correction is only performed if one or the other is found to be reversed.  Otherwise nothing is altered.

* **CSS** headers are handled in the same way as SAC headers, except that a scale factor is inserted into the CALIB variable.  If gain reversal checking is active and a reversal is found, the data are inverted and the header CALIB value is also inverted.

* **AH** headers have no dedicated place to store dip/azimuth information, but do have a comment field where this information is added.  The same rules as above are followed for reversal checking options 1, 2, and 3, otherwise.

#### User Prompt Mode

When `rdseed` is run without any options, the user is prompted for each option, along with additional options described below.

#### Input Options

`[command line equivalent in brackets]`

##### Input Device (Default: `/dev/nrst0`):

`[-f]` the input device can be changed from the default of `/dev/nrst0`, which refers to a magnetic tape device.  Note that when a tape is being accessed, it is best to use the no-rewind device. This allows `rdseed` to search for multiple volumes on tape across tape file marks.

##### Output Device (Default: `stdout`):

Non-error text displayed by `rdseed` goes to the terminal by default, but can be redirected to a file that you indicate here.  Some of the options send results to the Output Device (such as the station summary [`-S`]). Use the Unix `'>'` redirect command when calling `rdseed` from the command line.

##### Volume #[(1)-N]:

`[-v]` the volume number can be changed from the default value of 1.

##### Options [_various_]:

One mode option must be selected, the complete list of which is displayed by the program.  All options except the following have the same meaning as the corresponding command line options listed above, with the following exceptions:

* the `e` option will generate a _weed_ compatible event file.  **NOTE:**  _'weed' has evolved to JWEED and now PyWEED._
* the `d` option means the user wants to output a data file, the type of which is designated by the Output Format option shown below.

One or more of the following options will be presented to the user if the `d`, `s`, or `S` option was selected:

##### Summary File (Default: None):

`[-x]` a summary file can be selected as input for `rdseed` to filter stations, channels, and time windows.  Created by "JWEED", a summary file lists events, and phase oriented time windows for station data.  **NOTE:**  _JWEED has been replaced by PyWEED._

##### Station List (ALL) :

A list of selected stations separated by spaces or commas. Wildcard substitution using characters `*` , `?` and `.` is allowed.  A station name can be an _alias_ whose name is defined in a file whose filename is specified by the environment variable `SEEDALIAS`.  [_See details below_](./README.md/#the-alias-file). Hitting `RETURN` accepts all stations.

##### Channel List (ALL) :

A list of selected channels separated by spaces or commas. Wildcard substitution using characters `*`, `?` and `.` is allowed. Hitting `RETURN` accepts all channels.

##### Network List (ALL) :

A list of selected networks separated by spaces or commas. Hitting RETURN accepts all networks.

##### Loc Ids (ALL [`--` for spaces]):

A list of location identifiers (two alpha-numeric characters) that encapsulate a set of related channels.  Originally, SEED left the IDs undefined with space characters, so some data may be identified with spaces only.  Hitting `RETURN` accepts all location IDs.  Option `-i` from the command line tells `rdseed` to ignore location codes.

##### Output Format [(1)..9]:

```
[-d -o] select output format as one of the following:
1=SAC -- (default) SAC binary format
2=AH - AH (Ad Hoc) format
3=CSS - CSS Schema format
4=miniSEED - data-only SEED records
5=SEED - full SEED with metadata
6=SAC ALPHA - SAC alphanumeric (ASCII) format
7=SEGY - SEG-Y format
8=Simple ASCII (SLIST) - Single column sample values in ASCII
9=Simple ASCII (TSPAIR) - Time and sample column pairs in ASCII
```

##### Output file names include endtime? [Y/(n)]

`[-E]` select if the user wants each output file to be tagged with the waveform end time in the file name.

##### Output poles and zeroes?[Y/(n)]:

`[-p]` select if the user wants a SAC PZ (poles and zeros) file to be created. SAC PZ files contain header annotation that identifies key metadata features about the station being described. (See ["example of SAC PZ format below"](./README.md#sac-poles-and-zeroes-sac-pz-file)).

##### Check Reversal [0..3]

```
0=No (default)
1=Dip.Azimuth
2=Gain
3=Both:
  [-z] select signal reversal check and data change on dip/azimuth, gain, or both.
```

(See ["Output Header Corrections"](./README.md/#output-header-corrections) above for details)

##### Start Time(s) (FIRST) :

A list of seismogram start times of the form `YYYY.DDD.HH:MM:SS.FFFF` or `YYYY/MM/DD.HH:MM:SS.FFFF` separated by spaces. `YYYY` may be `YY` i.e. "90" for "1990".  Least significant parts may be omitted, in which case they become zero i.e. `90.270` is time 00:00:00.0000 of the 270th day of 1990.

##### End Time(s) (LAST) :

A list of seismogram end times of the same form as start times. Each start time (except the last one) must have a corresponding end time. If the last start time does not have a corresponding end time, the end time is assumed to be the last time in the volume.

##### Sample Buffer Length [2000000]:

`[-b]` each seismogram is assembled in a sample buffer prior to output. The size of the buffer can be changed. This number is the number of samples (not bytes). If the length is too small for the any of the requested seismograms, an error message will inform the user that the waveform is being broken into pieces.  The user may increase the size of the buffer to avoid this problem.

##### Extract Responses [Y/(N)] :

`[-R]` get channel response information in RESP format.  (See ["example of RESP format below"](./README.md/#seed-resp-file))

##### Select Data Type [(E=Everything), D=Data of Undetermined state, M=Merged Data, R=Raw waveform Data, Q=QC'd data]

`[-Q]` filter the data based on the quality code.  The default behavior is to accept all quality codes.

#### Alternate Response Files and the Use of miniSEED Volumes

The user can specify that station configuration and responses be taken from another SEED file, identified using either the `-g` command line option or setting the `ALT_RESPONSE_FILE` environment variable to the name of the SEED file. This can be a SEED file complete with data or one with only station metadata, called a ["dataless SEED"](https://ds.iris.edu/dms/nodes/dmc/data/formats/dataless-seed) file.  You can then run `rdseed` with a SEED or miniSEED file as input.  The station metadata used will be pulled from the specified alternate SEED file.

You can specify multiple file names by separating each name by a colon `':'` character.

See the ["EXAMPLES"](./README.md/#8-using-another-seed-volume-for-metadata-a-combination-of-miniseed-and-dataless) section below for example usage.

#### The Alias File

An alias file can be created which contains a list of station alias names. The first word in each line of the file is the alias. The words that follow are station names which will match the corresponding alias. The alias file name must be defined in the `SEEDALIAS` environment variable. For example, the file `rdseed.alias` contains the following:

`CHINA BJI XIAN SHNG`

All references to the term 'CHINA' will match station BJI, XIAN or SHNG.

`MY_IU FURI MAJO KIEV ANMO`

Would refer to the stations FURI, MAJO, KIEV, and ANMO when the term 'MY_IU' was listed as a station name.

#### Time Tear Tolerance

Normally, the tolerance for determining time tears is found in the station header information (max clock drift in Blockette 52).  Some stations may have clocks that wander excessively, which may cause time tears in the data.  The drift tolerance can be adjusted by defining an environment variable called `SEEDTOLERANCE`.  Its value is multiplied by the Blockette 52 max clock drift to get the tolerance in seconds.  Thus a value of 3.0 will increase the drift tolerance by a factor of three.   Clock Drift is defined in units of "seconds per sample" and is typically around .00005.

#### _rdseed_ Alert message file

When `rdseed` determines that data reversal is necessary and the user specifies that `rdseed` should reverse the data, `rdseed` creates a file with the data reversal information inside. This information includes the file name where the reversal was applied. When the user exits the program, a message is displayed reminding the user to look at this file.  This file is called `rdseed.alert.log` and is located in the startup directory.

#### _rdseed_ error logging

All rdseed error messages are logged to a file, called `rdseed.error.log` with the date.   This file is only created if an error is encountered.

#### Data Output from _rdseed_

There are two necessary steps to recovering seismograms from a SEED file.

The first step consists of finding out what is in the file.  The user can do this by using the command line options `-c` or `-t`, to list the station and channel names, starting times, and record numbers of the seismograms contained in the volume.

Seismic data are recovered from SEED files in the second step. Using the station, channel and time information, use **User Prompt Mode** to select start and stop times for individual seismograms.

Seismogram files are written to the current directory with names of the form

```
seed.rdseed for full SEED
mini.seed for mini seed
yyyy.ddd.hh.mm.ss.ffff.NN.SSSSS.LL.CCC.Q.SAC for SAC Files
yyyy.ddd.hh.mm.ss.ffff.NN.SSSSS.LL.CCC.Q.AH for AH Files
rdseed00000nnn.Q.w for CSS Files
yyyy.ddd.hh.mm.ss.ffff.NN.SSSSS.LL.CCC.Q.SAC_ASC for sac ASCII
yyyy.ddd.hh.mm.ss.ffff.NN.SSSSS.LL.CCC.Q.SEGY for SEGY
yyyy.ddd.hh.mm.ss.ffff_NN.SSSS.LL.CCC.Q.ascii for columnar ASCII
```

where

```
yyyy is the year,
ddd is the Julian day,
hh.mm.ss.ffff is the time of day of the start of the first record,
NN is the network identifier
SSSSS is the station name,
LL is the location ID
CCC is the component name for the particular seismogram being recovered, and
Q is the quality control marker (M, Q, D, R).
00000nnn is a sequence number
```

This seismogram file naming convention was chosen to provide unique names to output files without user intervention; however, the large number of files which can be generated to a single directory might cause problems for some operating systems. Notice that CSS uses a slightly different format that puts channel data in subdirectories. This is due to a limitation in the filename field in the CSS database. For CSS there are additional files created:

```
rdseed.affiliation
rdseed.network
rdseed.site
rdseed.sitechan
rdseed.wfdisc
```

Be aware that `rdseed` always appends onto the `mini.seed` file.  The user needs to manually remove this file in order to start over.

#### Metadata Output from _rdseed_

The `rdseed` user can write out supporting files to data that contain information about the instrumentation.  This information can support the user's interpretation of the digital waveforms, which are subject to scaling and frequency response changes from the original ground motion readings at the point it is recorded at the digitizer.

##### SAC Poles and Zeroes (SAC PZ) file

If the user indicated that they wanted to get the poles and zeroes with their SAC output (see ["Input Options"](./README.md/#input-options) above) or selected the `-p` option on the command line, a separate text file with an annotated header is provided.  An important fact to note about the SAC PZ output is that acceleration and velocity responses are converted to displacement to conform to the SAC convention.  The example below is created by `rdseed 5.1` and later and is compatible with `SAC v101.4` and later.

```
**********************************
* NETWORK   (KNETWK):   II
* STATION    (KSTNM)      :   PFO
* LOCATION   (KHOLE)    :   00
* CHANNEL   (KCMPNM) :  BHZ
* CREATED                :   2011-08-12T21:51:26
* START                      :   2010-07-30T18:50:00
* END                         :   2599-12-31T23:59:59
* DESCRIPTION        :  Pinon Flat, California, USA
* LATITUDE               :   33.610700
* LONGITUDE           : -116.455500
* ELEVATION            : 1280.0
* DEPTH                    : 5.3
* DIP                          : 0.0
* AZIMUTH               : 0.0
* SAMPLE RATE       : 20.0
* INPUT UNIT           : M
* OUTPUT UNIT       : COUNTS
* INSTTYPE              : Streckeisen STS-1 Seismometer with Metrozet E300
* INSTGAIN              : 3.314400e+03 (M/S)
* COMMENT            : S/N #119005
* SENSITIVITY         : 5.247780e+09 (M/S)
* A0                           : 7.273290e+01
* **********************************
ZEROS    6
    +0.000000e+00    +0.000000e+00
    +0.000000e+00    +0.000000e+00
    +0.000000e+00    +0.000000e+00   
    -7.853982e+01     +0.000000e+00   
    -1.525042e-01      +0.000000e+00   
    -1.525042e-01      +0.000000e+00   
POLES    6
    -1.207063e-02     +1.224561e-02   
    -1.207063e-02     -1.224561e-02   
    -1.522510e-01    +9.643684e-03   
    -1.522510e-01     -9.643684e-03   
    -4.832398e+01    +5.817080e+01   
    -4.832398e+01    -5.817080e+01   
CONSTANT    3.816863e+11
```

##### SEED RESP file

The RESP file has been present for `rdseed` users for a long time, and very little has changed with the format over the years.  It is a fairly complete instrument response representation, complete with blockette annotations, presented in an easy to read ASCII format.  The example shown represents just a portion of what is otherwise a lengthy representation. Users get the RESP file when they respond 'Y' to Extract Responses (see ["Input Options"](./README.md/#input-options) above) or the command line option `-R`.

```
B050F03     Station:     PFO
B050F16     Network:     II
B052F03     Location:    00
B052F04     Channel:     BHZ
B052F22     Start date:  2010,211,18:50:00
B052F23     End date:    2599,365,23:59:59
#
#                  +-------------------------------------------+
#                  |    Response (Poles and Zeros)     
#                  |        II  PFO    00  BHZ                 
#                  |     07/30/2010 to 12/31/2599        
#                  +-------------------------------------------+
#
B053F03     Transfer function type:                B
B053F04     Stage sequence number:                 1
B053F05     Response in units lookup:              M/S - Velocity in Meters Per Second
B053F06     Response out units lookup:             V - Volts
B053F07     A0 normalization factor:               +1.15758E+01
B053F08     Normalization frequency:               +5.00000E-02
B053F09     Number of zeroes:                      5
B053F14     Number of poles:                       6
#              Complex zeroes:
#              i  real          imag          real_error    imag_error
B053F10-13     0  +0.00000E+00  +0.00000E+00  +0.00000E+00  +0.00000E+00
B053F10-13     1  +0.00000E+00  +0.00000E+00  +0.00000E+00  +0.00000E+00
B053F10-13     2  -1.25000E+01  +0.00000E+00  +0.00000E+00  +0.00000E+00
B053F10-13     3  -2.42718E-02  +0.00000E+00  +0.00000E+00  +0.00000E+00
B053F10-13     4  -2.42718E-02  +0.00000E+00  +0.00000E+00  +0.00000E+00
#              Complex poles:
#              i  real          imag          real_error    imag_error
B053F15-18     0  -1.92110E-03  +1.94895E-03  +0.00000E+00  +0.00000E+00
B053F15-18     1  -1.92110E-03  -1.94895E-03  +0.00000E+00  +0.00000E+00
B053F15-18     2  -2.42315E-02  +1.53484E-03  +0.00000E+00  +0.00000E+00
B053F15-18     3  -2.42315E-02  -1.53484E-03  +0.00000E+00  +0.00000E+00
B053F15-18     4  -7.69100E+00  +9.25817E+00  +0.00000E+00  +0.00000E+00
B053F15-18     5  -7.69100E+00  -9.25817E+00  +0.00000E+00  +0.00000E+00
#
#                  +-------------------------------------------+
#                  |      Channel Sensitivity/Gain    
#                  |        II  PFO    00  BHZ        
#                  |     07/30/2010 to 12/31/2599     
#                  +-------------------------------------------+
#
B058F03     Stage sequence number:                 1
B058F04     Sensitivity:                           +3.31440E+03
B058F05     Frequency of sensitivity:              +5.00000E-02
B058F06     Number of calibrations:                0
```

##### Recovering auxiliary data from a SEED Volume

One may also retrieve the set of abbreviation dictionaries or the set of station information tables from an FDSN SEED volume. Abbreviation dictionaries are retrieved with the command:

`rdseed -af inputfile`

Station information tables are accessed with:

`rdseed -sf inputfile`

### DIAGNOSTICS

Various warnings and error messages are issued to the standard error device `stderr` by the procedure. Typical response of the procedure to a warning condition is to write a message to the standard error device and then to continue execution. An error condition, on the other hand, will cause a message to be generated to the standard error device followed by immediate termination of the procedure.

### EXAMPLES

#### 1. Reading the table of contents from a volume on tape.

`% rdseed -cf /dev/rmt8 > tape.contents`

or

```
% rdseed
Input Device (/dev/rst0) : /dev/rmt8
Output Device (stdout) : tape.contents
Volume # [(1)-N] :
Options [acsSrRtde] : c
```

reads the table of contents from the tape on device `/dev/rmt8` into a file called `tape.contents`.  The result is formatted ASCII that lists volume information, the time spans for data, and any hypocenter information that may be present.

The user can do the same thing reading from a disk file.  Just replace `/dev/rmt8` with the file name.

#### 2. Determining event start/stop times on a SEED file.

`% rdseed -tvf 2 myFile.seed > myFile.times`

or

```
% rdseed
Input Device (/dev/rst0) : myFile.seed
Output Device (stdout) : myFile.times
Volume # [(1)-N] : 2
Options [acsSrRtde] : t
```

reads a disk file called `myFile.seed` and creates a table containing starting record numbers, station and channel names, start and stop times of events, nominal sample rate, calculated sample rate and numbers of samples for that file. Output is written to the file `myFile.times`.

#### 3. Creating a detailed list of the contents of a SEED file:

`% rdseed -lf myFile.seed > tape.list &`

reads the file `myFile.seed` and writes a list of the contents of each record to a file called `tape.list`.  This job is run in the background by using an ampersand on the end.

#### 4. Reading all data from a tape.

```
% rdseed
Input Device (/dev/rst0): /dev/rmt8
Output Device (stdout): tape.extraction.list
Volume # [(1)-N]:
Options [acsSrRtde]: d
Station List (ALL):
Channel List (ALL):
Loc Ids (ALL ["--" for spaces]):
Output format [(1-SAC),2-AH...]:
Start Time(s) (FIRST) :
End Time(s) (LAST):
Sample Buffer Length [2000000]:
Extract Responses [Y/(N)]:
```

reads all seismograms from the tape on device `/dev/rmt8` into the current directory (defaults to SAC format) and writes informational output to a file called `tape.extraction.list`.

#### 5. Reading the abbreviation dictionaries.

`% rdseed -af myFile.seed > abbreviations.txt`

or
```
% rdseed
Input Device (/dev/rst0): myFile.seed
Output Device (stdout): abbreviations.txt
Volume # [(1)-N]: 1
Options [acsSrRtde]: a
```

extracts the abbreviation dictionaries from a SEED file and sends the result to a text file.

#### 6. Reading station information.

`% rdseed -sf /dev/rmt8 > tape.station.information`

or

```
% rdseed
Input Device (/dev/rst0): /dev/rmt8
Output Device (stdout): tape.station.information
Volume # [(1)-N]: 1
Options [acsSrRtde]: s
```

recovers station and channel location and response information from the tape on device `/dev/rmt8` and writes the information to a file.

#### 7. Reading specific station/channel/time information.

```
% rdseed
Input Device (/dev/rst0) : /export/home/myFile.seed
Output Device (stdout) :
Volume # [(1)-N] : 2
Options [acsSrRtde] : d
Summary File (None) :
Station List (ALL) : BJI YKW1
Channel List (ALL) : *Z
Network List (ALL) :
Loc Ids (ALL ["--" for spaces]):
Output format [(1-SAC),2-AH..]:
Start Time(s) (FIRST) : 1990,270,20:30
End Time(s) (LAST) : 1991/2/1
Sample Buffer Length [2000000]: 3000000
Extract Responses [Y/(N)] : Y
```

reads all seismograms from a SEED file for stations BJI and YKW1, all Z channels, from year 1900, Julian day 270, hour 20, minute 30 to February 1, 1991. The buffer size was increased to 3 million samples and the channel response information will be output.

#### 8. Using another SEED volume for metadata, a combination of miniSEED and dataless.

`% rdseed -d -o 1 -f mydata.miniseed -g mymetadata.dataless`

writes binary SAC files for the provided miniSEED data using the provided dataless SEED volume as metadata.  Note that the -d and -o options are used together to indicate the data output format.  The equivalent action through the interactive mode is shown here:

```
% setenv ALT_RESPONSE_FILE mymetadata.dataless
% rdseed
Input Device (/dev/rst0) : mydata.miniseed
Output Device (stdout) :
Volume # [(1)-N] :
Options [acsSrRtde] : d
Summary File (None) :
Station List (ALL) :
Channel List (ALL) :
Network List (ALL) :
Loc Ids (ALL ["--" for spaces]):
Output format [(1-SAC),2-AH..]: 1
Start Time(s) (FIRST) :
End Time(s) (LAST) :
Sample Buffer Length [2000000]:
Extract Responses [Y/(N)] :
```

### ENVIRONMENT VARIABLES

| Variable      | Description   |
| -------------:|:--------------|
| SEEDALIAS      | Indicates a file name that lists station group aliases (see above). |
| ALT_RESPONSE_FILE      | Indicates a SEED dataless file that contains station metadata that corresponds to the data Input File read by `rdseed`, this is an alternative to using the `-g` command line option.      |
| SEEDTOLERANCE | The multiplier for the clock drift to determine the maximum acceptable time gap in the data to still be treated as a continuous data stream.      |

To set environment variable values in csh:

`% setenv ALT_RESPONSE_FILE myDataless.seed`

in sh or bash:

```
$ ALT_RESPONSE_FILE=myDataless.seed
$ export ALT_RESPONSE_FILE
```

### 32-BIT SUPPORT

Since version 5.0 of `rdseed`, all of the released executables have been 64-bit binaries. This will cause problems on older 32-bit systems, which will report a 'bad executable' error or report a 'wrong CPU type'. This is necessary to support some of `rdseed's` newer features and `rdseed` running as a 32-bit application may experience some limitations when processing large datasets.

Nonetheless, you can recompile `rdseed` as a 32-bit application and get reliable functionality equivalent to versions 4.8 and earlier. There is a `makefile` in the distribution where you uncomment the `CFLAGS` line to force 32-bit compilation:

`#CFLAGS = -O -m32 -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE`

Uncomment this by removing the hash mark, then save the makefile and type:
```
$ make clean
$ make
```
The executable is named `rdseed` by default and should now be executable on 32-bit systems.
