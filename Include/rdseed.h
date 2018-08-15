/*===========================================================================*/
/* SEED reader     |              rdseed.h                 |     header file */
/*===========================================================================*/
/*
	Name:		rdseed.h
	Purpose:	contains variable and function definitions for rdseed programs
	Usage:		#include "rdseed.h"
	Input:		not applicable
	Output:		not applicable
	Warnings:	not applicable
	Errors:		not applicable
	Called by:	#included by many or all "rdseed" procedures
	Calls to:	stdio.h - standard C include file for standard io functions
			string.h - standard C include file for string functions
			math.h - standard C include file for math functions
			macros.h - local include file with various C macros
			structures.h - local include file defining all structures
	Algorithm:	not applicable
	Notes:		"globals.h" defines and allocates space for the several
			globally-available variables and arrays.  "rdseed.h" defines
			these elements as "EXTERN" for use by subprocedures and
			also defines all subprocedures.  The construct 
			"#ifndef . . .  #endif" prevents #includes and variable 
			definitions from being performed more than once if both
			"globals.h" and "rdseed.h" are #included.

			Both "globals.h" and "rdseed.h" are #included by the main
			program, while only "rdseed.h" is #included by any
			subprocedures.

			"stdio.h", "strings.h", and "math.h" are system-supplied
			headers, and contain no code to prevent multiple inclusion;
			therefore such code is contained here.  "structures.h" and
			"macros.h" are part of the "rdseed" source code and contain
			codes to prevent multiple inclusion.
	Problems:	in main routine, "globals.h" must be #included *before*
			"rdseed.h".
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
			11/09/88  Dennis O'Neill  included byteswap flag
			11/21/88  Dennis O'Neill  Production release 1.0
			02/06/89  Dennis O'Neill  added decode_32bit subroutine
*/

/* If this header is already included, don't include it again */
#ifndef RDSEED_H
#define RDSEED_H

/*                 +=======================================+                 */
/*=================|  Global and local-to-main constants   |=================*/
/*                 +=======================================+                 */
#define LRECL_DEF	32768		/* default logical record length */
#define PRECL		32768		/* default phys rec length */
#define MAX_BLKT_LENGTH	16384		/* initial space allocation */
#define MAX_DATA_LENGTH	20000000		/* initial space allocation */
#define TEMP_CHAR_LENGTH 16384		/* space allocation */
#define DECODEKEY       50+1		/* lngth of decoder key */
#define MAXSGMS         1000		/* max # seismograms wanted*/
#define LRECL_FMT	"%4096c"	/* input format for lgcl rec */
#define STRINGTERM	"~"		/* vbl lntgh string term */
#define PI		3.14159		/* The pi constant */

#define isaleap(year) ((((year)%100 != 0) && ((year)%4 == 0)) || ((year)%400 == 0))

/* if "MAIN" is #defined, give value to constants (do this only in main proc) */
#ifdef MAIN
#define EXTERN
#define EQNULL              =NULL		/* init for linked lists */
#define _PRECL              PRECL + 1
#define _LRECL              LRECL_DEF               
#define _MAX_BLKT_LENGTH    MAX_BLKT_LENGTH
#define _MAX_DATA_LENGTH    MAX_DATA_LENGTH
#define _TEMP_CHAR_LENGTH   TEMP_CHAR_LENGTH + 1
#define _DECODEKEY          DECODEKEY
#define _MAXSGMS            MAXSGMS
#define _STRLEN				80
#else
/* otherwise all of these should be blank */
#define EXTERN              extern
#define EQNULL
#define _PRECL
#define _LRECL
#define _MAX_BLKT_LENGTH
#define _MAX_DATA_LENGTH
#define _TEMP_CHAR_LENGTH
#define _DECODEKEY
#define _MAXSGMS
#define _STRLEN
#endif

#define fprintf		save_myfprintf

#define TAPE_DEVICE 0
#define DISK_DEVICE -1


/*                 +=======================================+                 */
/*=================|   Inclusion of other include files    |=================*/
/*                 +=======================================+                 */

#ifndef STDIO_H
#include <stdio.h>
#define STDIO_H
#endif

#ifndef STDLIB_H
#include <stdlib.h>
#define STDLIB_H
#endif

#ifndef STRING_H
#include <string.h>
#define STRING_H
#endif

#ifndef MATH_H
#include <math.h>
#define MATH_H
#endif

#include "macros.h"
#include "structures.h"

/*                 +=======================================+                 */
/*=================|      Globally-available variables     |=================*/
/*                 +=======================================+                 */

EXTERN FILE 	*inputfile;		/* input data file or device */
EXTERN FILE	*alt_headerfile;	/* alternate header file */
EXTERN int	output_flag;		/* Flag that data has been output */
EXTERN int	output_endtime;		/* Flag that output files should have endtime */
EXTERN int	reading_alt_file;	/* Flag read of alt header file */

EXTERN int	LRECL;			/* default logical rec length */
EXTERN int	byteswap;		/* flag for byteswapping */
EXTERN int	check_reverse;		/* flag for reversal checking */
EXTERN int 	q_flag;
EXTERN int	more_data;		/* more data tp process flag */
EXTERN int	outresp_old_flag;	/* output response data flag (in old format) */
EXTERN int	outresp_flag;		/* output respo0nse data flag */
EXTERN int	at_volume;		/* At correct volume flag */
EXTERN int	found_lrecl_flag;	/* founf logical record flag */
EXTERN int	input_file_type;	/* flag type of input file 0 = char; -1 = block*/

EXTERN char 	output_dir[];

EXTERN int 	Seed_flag;		/* whether to output presumably smaller seed volume */

EXTERN int	station_volume;		/* flag type of volume 0 = seq; -1 = block*/
EXTERN int	volume_count;		/* Count of passed volumes */
EXTERN int	volume_number;		/* Volume number to use */
EXTERN char	precord[_PRECL];	/* physical input record */
EXTERN char	*precord_ptr;		/* ptr to above */
EXTERN int	offset;			/* offset into above */
EXTERN char	*lrecord_ptr;		/* ptr to logical record */
EXTERN int	num_bytes_read;		/* # bytes read from p recd */
EXTERN int	station_count;		/* count of station match strings */
EXTERN char	*station_point[1500];	/* list of pointers to station match strings */
EXTERN char	station_list[3500];	/* string of station match strings */
EXTERN int	channel_count;		/* count of channel match strings */
EXTERN char	*channel_point[250];	/* list pf pointers to channel match strings */

EXTERN int 	network_count;
EXTERN char	*network_point[20];     /* list of pointers to station match strings */
EXTERN char	network_list[80];       /* string of station match strings */

EXTERN int      location_count;
EXTERN char     *location_point[20];     /* pointers to location strings */
EXTERN char 	location_list[200];	 /* ditto */


EXTERN char	channel_list[200];	/* string of channel match strings */ 
EXTERN int	alias_count;		/* count of alias file lines */
EXTERN char	*alias_point[50];	/* list of alias file line pointers */
EXTERN int	start_time_count;		/* count of start time fields */
EXTERN struct time	*start_time_point;	/* list of start time pointers */
EXTERN int	end_time_count;			/* count of end times */
EXTERN struct time	*end_time_point;	/* list of end time pointers */

EXTERN int	event_start_time_count;	/* count of event start time fields */
EXTERN struct time	*event_start_time_point;/* list of event start time pointers */
EXTERN int	event_end_time_count;	/* count of event end times */
EXTERN struct time	*event_end_time_point;	/* list of event end time pointers */

EXTERN int	start_phase_count;	/* count of start phase fields */
EXTERN struct phase	*start_phase_point;	/* list of start phase pointers */
EXTERN int	end_phase_count;		/* count of end phases */
EXTERN struct phase	*end_phase_point;	/* list of end phase pointers */

EXTERN char	temp_char[_TEMP_CHAR_LENGTH];	/* temp storage */
EXTERN char	blockette[_MAX_BLKT_LENGTH];	/* blockette storage */
EXTERN int	blockette_type;			/* blockette type number */
EXTERN int	blockette_length;		/* blockette length (bytes) */

EXTERN struct	logical_record input;	/* data from input file */

EXTERN char	outputformat[_STRLEN];
EXTERN int	output_format;		/* Data Output Format Selection */
EXTERN FILE 	*outputfile;		/* Global output file */
EXTERN char 	css_filename[100];	/* CSS file name from volume times */

EXTERN int	Output_PnZs;	/* flag to write Poles & Zeros */

/*                 +=======================================+                 */
/*=================|   Volume and station header tables    |=================*/
/*                 +=======================================+                 */

/* volume header blockettes */
EXTERN struct type10	type10;			/* type 010 blockette */
EXTERN struct type11	*type11_head EQNULL;	/* type 011 blockette */
EXTERN struct type12	type12;			/* type 012 blockette */

/* pointers to starts of abbreviation dictionary tables (linked lists) */
EXTERN struct type30	*type30_head EQNULL;	/* type 030 blockette */
EXTERN struct type31	*type31_head EQNULL;	/* type 031 blockette */
EXTERN struct type32	*type32_head EQNULL;	/* type 032 blockette */
EXTERN struct type33	*type33_head EQNULL;	/* type 033 blockette */
EXTERN struct type34	*type34_head EQNULL;	/* type 034 blockette */
EXTERN struct type35	*type35_head EQNULL;	/* type 035 blockette */

/* pointers to starts of response dictionaries */
EXTERN struct type41	*type41_head EQNULL;	/* type 041 blockette */
EXTERN struct type42	*type42_head EQNULL;	/* type 042 blockette */
EXTERN struct type43	*type43_head EQNULL;	/* type 043 blockette */
EXTERN struct type44	*type44_head EQNULL;	/* type 044 blockette */
EXTERN struct type45	*type45_head EQNULL;	/* type 045 blockette */
EXTERN struct type46	*type46_head EQNULL;	/* type 046 blockette */
EXTERN struct type47	*type47_head EQNULL;	/* type 047 blockette */
EXTERN struct type48	*type48_head EQNULL;	/* type 048 blockette */

/* pointers to start of station header tables (linked list) */
EXTERN struct type50	*type50_head EQNULL;	/* type 050 blockette */
						/* other type 05x blockettes */
						/* are subsumed under this */

/* pointers to starts of time span control header structures (linked lists) */
EXTERN struct type70	*type70_head EQNULL;	/* type 070 blockette */
EXTERN struct type71	*type71_head EQNULL;	/* type 071 blockette */
EXTERN struct type72	*type72_head EQNULL;	/* type 072 blockette */
EXTERN struct type73	*type73_head EQNULL;	/* type 073 blockette */
EXTERN struct type74	*type74_head EQNULL;	/* type 074 blockette */

/*                 +=======================================+                 */
/*=================|  Seismic data and related variables   |=================*/
/*                 +=======================================+                 */

EXTERN struct type50	*current_station EQNULL;/* ptr to current station */
EXTERN struct type52	*current_channel EQNULL;/* ptr to current channel */
EXTERN struct type71	*current_origin  EQNULL;/* ptr to current origin record */

EXTERN struct data_hdr	*data_hdr;			/* data header */
EXTERN struct time	last_time;			/* time of previous record */
EXTERN double		last_sample_rate;		/* sample rate, previous rcd */
EXTERN int		last_nsamples;			/* # samples, previous rcd */
EXTERN int		seis_buffer_length;

EXTERN double 		*seismic_data;			/* seismic data */
EXTERN double 		*seismic_data_ptr;		/* ptr to above */
EXTERN char		decode[_DECODEKEY];		/* data decoder key */

EXTERN int		numitems;			/* number of sgms wanted */
EXTERN char		*item[_MAXSGMS];		/* list of sgms wanted */
EXTERN long int		start_record;			/* starting rec # of sgm */
EXTERN double sample_rate_accum;			/* average sample rate accum */
EXTERN int sample_rate_count;				/* count for average sample rate calc */
EXTERN double adj_tolerance;				/* tolerance adjust, read from enviornment */

/*                 +=======================================+                 */
/*=================|          process definitions          |=================*/
/*                 +=======================================+                 */

/* Utilities */
char *alloc_linklist_element();

void read_logical_record ();			/* process a logical rec */
int memncpy ();					/* cp, null-term. n chars */
void read_blockette ();				/* read a blockette */
void pagetop ();				/* top-of-page printer */
void allocation_error ();			/* memory allocation error */
int parse_int ();				/* parse an integer field */
long int parse_long ();				/* parse a long integer field */
double parse_double ();				/* parse a double field */
char *parse_nchar ();				/* parse n-char wide ASCII */
char *parse_varlstr ();				/* parse vbl-length ASCII */
struct time timeadd ();				/* add seconds to a time */
struct time timesub();

struct time timeadd_double ();			/* add double seconds to a time */
struct time timeaddphase ();			/* add seconds to a time */
int timecmp ();					/* compare two times */
double timedif ();				/* difference of two times */
void snoop ();					/* list block contents */
void snoop_data ();				/* sub for snoop for data */
unsigned long int swap_4byte ();		/* swap bytes in 4-byte words */
unsigned short int swap_2byte ();		/* swap bytes in 2-byte words */
int find_wordorder ();				/* determine word order */
struct optstruct *cmdlineproc ();		/* command line processor */

/* Functions to read and process the volume identifier block */
void process_volh ();				/* process volume header */
void parse_type10 ();				/* parse volume id blkette */
void parse_type11 ();				/* parse vol stn hdr index */
void parse_type12 ();				/* parse vol time span index */
void print_volh ();				/* print volume header */
void print_type10 ();				/* print volume id blkette */
void print_type11 ();				/* print vol stn hdr index */
void print_type12 ();				/* print vol time span index */

/* Function to process a blank or unknown type block */
void process_blank ();

/* Functions to read and process the abbreviation dictionary block */
void process_abrvd ();				/* process abbreviation hdr */
void parse_type30 ();				/* parse type 30 blockette */
void parse_type31 ();				/* parse type 31 blockette */
void parse_type32 ();				/* parse type 32 blockette */
void parse_type33 ();				/* parse type 33 blockette */
void parse_type34 ();				/* parse type 34 blockette */
void parse_type35 ();				/* parse type 35 blockette */
void print_abrvd ();				/* print abbrev dictionaries */
void print_type30 ();				/* print type30 table */
void print_type31 ();				/* print type31 table */
void print_type32 ();				/* print type32 table */
void print_type33 ();				/* print type33 table */
void print_type34 ();				/* print type34 table */
void print_type35 ();				/* print type35 table */

/* Functions to read and process the response dictionary blocks */
void parse_type41 ();				/* parse sym coefficients blkt */
void parse_type42 ();				/* parse polynomial blkt */
void parse_type43 ();				/* parse poles & zeroes blkt */
void parse_type44 ();				/* parse coefficients blkt */
void parse_type45 ();				/* parse rspns list blkt */
void parse_type46 ();				/* parse generic rspns blkt */
void parse_type47 ();				/* parse generic rspns blkt */
void parse_type48 ();				/* parse ch sensitivity */

/* Functions to read and process the station header blocks */
int process_stnh ();		/* process station headers */
int parse_type50 ();		/* parse station id blkt */
void parse_type51 ();		/* parse station comment */
int parse_type52 ();		/* parse channel id */
struct response *get_response ();	/* get a response for a chl */
void parse_type53 ();			/* parse poles & zeroes blkt */
void parse_type54 ();			/* parse coefficients blkt */
void parse_type55 ();			/* parse rspns list blkt */
void parse_type56 ();			/* parse generic rspns blkt */
void parse_type57 ();			/* parse decimation blkt */
void parse_type58 ();			/* parse ch sensitivity */
void parse_type59 ();			/* parse ch comment */
void parse_type60 ();			/* parse ch comment */
void parse_type61 ();			/* parse sym coefficients blkt */
void parse_type62 ();			/* parse polynomial blkt */

void print_stnh ();			/* print station headers */
void print_type50 ();			/* print station id blkt */
void print_type51 ();			/* print station comment */
void print_channel ();			/* print all channel info */
void print_type52 ();			/* print channel id */
void print_response ();			/* print all response info */
void print_type53 ();			/* print poles & zeroes blkt */
void print_type54 ();			/* print coefficients blkt */
void print_type55 ();			/* print rspns list blkt */
void print_type56 ();			/* print generic rspns blkt */
void print_type57 ();			/* print decimation blkt */
void print_type58 ();			/* print ch sensitivity */
void print_type59 ();			/* print ch comment */
void print_type60 ();			/* print ch comment */
void print_type61 ();			/* print sym coefficients blkt */
void print_type62 ();			/* print polynomial blkt */

/* Functions to read and process the time span header blocks */
void process_timeh ();			/* process time span headers */
void parse_type70 ();			/* parse time span id blkt */
void parse_type71 ();			/* parse hypocenter id blkt */
void parse_type72 ();			/* parse event phases blkt */
void parse_type73 ();			/* parse t.s. data st index */
void print_timeh ();			/* print time span headers */
void print_type70 ();			/* print time span id blkt */
void print_type71 ();			/* print hypocenter id blkt */
void print_type72 ();			/* print event phases blkt */
void print_type73 ();			/* print t.s. data st index */

/* Functions to read and process the data */
void process_data ();			/* process data blocks */
void init_data_hdr ();			/* initialize a data header */
void convert_seedhdr ();		/* parse fixed size data hdr */
void decode_asro ();			/* decode asro data */
void decode_cdsn ();			/* decode cdsn data */
void decode_dwwssn ();			/* decode dwwssn data */
void decode_steim ();			/* decode steim data */
void decode_sro ();			/* decode sro data */
void decode_32bit ();			/* decode 32-bit int data */
void output_data (); 			/* write output data */

/*                 +=======================================+                 */
/*=================|          end of header file           |=================*/
/*                 +=======================================+                 */

/* the next line should be the end of the header file */
#endif
