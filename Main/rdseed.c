/*===========================================================================*/
/* SEED reader     |                rdseed                 |    main program */
/*===========================================================================*/
/*
Name:		rdseed
Purpose:	read Standard Exchange of Earthquake Data (SEED) format volume
Usage:		Current usage is:
			rdseed inputfile -{a | c | d [list] | l | s | t}
			where
				inputfile = name of input file or device (required);
			a = retrieve abbreviation dictionaries;
			c = retrieve table of contents records from volume.
			d = retrieve all [selected] data, selection by list of
				record numbers;
			l = list contents of each block in volume;
			s = retrieve all station header information;
			t = show start/end times of all events on volume
			S = extract station records
			e = retrieve event information

		One of the function switch set {a c d l s t} is required.

		Eventually (subject to revision), usage will be:
		rdseed inputfile [precl] -{a | c | d [list] | l | s [list] | t}
			where
			inputfile = name of input file or device (required);
				precl = length of physical records on inputfile,
					default = 32768 (32 Kbytes) (optional);
				a = retrieve abbreviation dictionaries;
				c = retrieve volume table of contents.
				d = retrieve all [selected] data, selection by list of
				record numbers, station names, or times;
		    	l = list contents of each block in volume;
			s = retrieve all [selected] station header information,
				selection via list of station names;
		    	t = show start/end times of all events on volume
		One of the function switch set {a c d l s t} is required.

Input:		an input file or device on which the SEED data reside
		a function switch, from the list given above
		for function 'd', an optional space-separated list of desired 
		seismograms, selected by beginning record number, station 
		name, or time.  If no list is given, all seismograms present
		are extracted.  If record numbers are specified, only those 
		seismograms which begin at the cited record numbers are 
		extracted.  

		The following may be added at in a later release:
		the physical record length of the data present on the input file
		for function 'd', If station names are given, all seismograms
		from the selected stations are written.  If times are given,
		all seismograms starting near (plus or minus some time
	    	interval) the specified times are output.
		for function 's', an optional space-separated list of desired 
		station headers.

Output:		for function 'a':  the abbreviation dictionary is written to
			the standard output.
		for function 'c':  the set of time headers for the volume, 
			indicating station, component, start time, and starting
			record number, are written to the standard output.
		for function 'd':  seismograms are written to files named by
		beginning time, station, and component; for example,
		1988.023.15.34.08.2800.ANMO.SPZ is the filename for a
		seismogram from year 1988, Julian day 23, 15:34:08.2800 UT,
		recorded at station ANMO from component SPZ.
		for function 's':  station header information is written to the
		standard output.
		for function 't':  start/stop times for all events on the volume
Warnings	several warnings are given by subprocedures.  Typical response
		to a warning is that a message is sent to the standard error
		device and execution continues.
Errors:		various errors are trapped by subprocedures.  Typical response
		to an error is that a message is sent to the standard error
		device and execution is terminated.
Called by:	user
Calls to:	Each logical record type has its own processor, as listed below.
		Each processor includes several parsing routines which parse
		the various blockette types; the parsing routines are described
		more fully in the leading comments of the process_xxxx source
		files.  The processing routines are:
		process_data - for data records
		process_timeh - for time index header records
		process_stationh - for station header records
		process_abbrevdic - for abbreviation dictionary records
		process_volh - for volume header records
		process_blank - for blank records
		(default) - for unknown record types
Algorithm:	Open the input file for reading; if the input file cannot be 
		opened, issue an error message and quit.  Process the input
		file one logical record at a time; each record type has an
		initial type flag which is used by this procedure to determine
		which type of processing to apply.  The information contained
		in each header type (except volume) is concatenated into a 
		volume-inclusive table for each different type:  six
		abbreviation dictionaries containing data format, comment
		description, cited source, generic abbreviation, units
		abbreviation, and beam configuration dictionaries (see Halbert
		et al.); a station header table containing complete information
		for each channel of each station; and a volume time span index.

Tables:		The structure of tables created by this process are described
		here.  In all cases, -> and | represent links in a linked list
		and NULL indicates end-of-table.

		1.	Structure of the volume header list.
			volume id
			volume station header index
			volume time span index
			
		2.	Abbreviation dictionary tables.
	data format 1 -> ... -> last data format -> NULL
	comment description 1 -> ... -> last comment descrip -> NULL
	cited source 1 -> ... -> last cited source -> NULL
	generic abbreviation 1 -> ... -> last generic abbrev -> NULL
	units abbreviation 1 -> ... -> last units abbrev -> NULL
	beam configuration 1 -> ... -> last beam config -> NULL

		3.	Structure of the station table.
  	station1   ->   station2   -> ... -> last station -> NULL
	     |               |                       |
	information     information             information
	  specific        specific                specific
	     to              to                      to
	  station1        station2              last station

	An entry for an individual station is also a linked list;
	entries specific to a particular station are represented
	above by a phrase and are shown in detail below.

	             generic station entry structure
       	+++++++++++++++++++++++++++++++++++++++++++
       |                  |                      |
station comment 1      channels         station update link
       |                 and                  or NULL
station comment 2      responses            (See Note 1)
       |                [to A]
      ...
       |                  .
station comment N
       |                  .
      NULL
                          .

	      	.  .  .  .  .  .  .  .

		     	  .
		      [from A]
		         |
		channel1   ->   channel2   -> ... -> last channel -> NULL
		     |               |                       |
		information     information             information
		  specific        specific                specific
		     to              to                      to
		  channel1        channel2              last channel

		     generic channel entry structure
		+++++++++++++++++++++++++++++++++++++++++++
		|                 |                       |
	    response 1    channel comment 1      channel update link
	       |                 |                    or NULL
	   response 2    channel comment 2          (See Note 2)
	       |                 |
	      ...               ...
	       |                 |
	 last response  last channel comment
	       |                 |
	      NULL              NULL

	An alternative picture of the link structure described
	above is:

	station 1 --> station 2 --> ... --> last station --> NULL
	 |  |  |
	 |  |  +--> station update link --> ... --> last update --> NULL
	 |  |
	 |  +--> station comment 1 --> ... --> last comment --> NULL
	 |
	 +--> channel 1 --> channel 2 --> ... --> last channel --> NULL
	       |  |  |
	       |  |  +--> channel update link --> ... --> last update --> NULL
	       |  |
	       |  +--> channel comment 1 --> ... --> last comment --> NULL
	       |
	       +--> response 1 --> ... --> last response --> NULL

	where the link structure contained in all station entries
	is shown only for station 1, and the link structure
	contained in all channel entries is shown only for
	channel 1.

	4.	Structure of the time span index.
	time span 1 -> ... -> last time span -> NULL

	Notes:	1.  Not implemented as of initial release date.
		2.  Not implemented as of initial release date.
		3.  This implementation has not been optimized for speed.
Problems:	see various subprocedures.
		"read", used in "rdseed.c" and "read_blockette.c", is not
		defined in the ANSI standard for C.
References:	Halbert, S. E., R. Buland, and C. R. Hutt (1988).  Standard for
		the Exchange of Earthquake Data (SEED), Version V2.0,
		February 25, 1988.  United States Geological Survey,
		Albuquerque Seismological Laboratory, Building 10002,
		Kirtland Air Force Base East, Albuquerque, New Mexico
		87115.  82 pp.
Language:	C, hopefully to ANSI standard for portability.
Author:		Dennis O'Neill
Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
		11/21/88  Dennis O'Neill  changed option names; added -t option
		11/21/88  Dennis O'Neill  Production release 1.0
		7/1/90	Allen Nance release 2.1 changes
		6/11/93	Chris Laughbon 	added stderr logging

Changes needed in main and/or subprocs:
	get logical record length from volume header
	determine physical record length from volume or user input
	eliminate use of "memncpy" with direct references using pointers 
	blockette length (see structures.h, rdseed.h, globals.h, read_blockette)
	should be dynamically allocated
*/

#define MAIN
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/param.h>
#if 0
#include <sys/ioctl.h>
#include <sys/mtio.h>
#endif

#include <signal.h>
#include "rdseed.h"		/* rdseed headers */
#include "version.h"
#include "patchlevel.h"

extern int errno;

extern char *getenv();		/* define getenv system call */
extern void init_error_file();

extern int rdseed_alert;

FILE *get_alt_file();		/* returns next alt response file */

void init_alt_files(char *);    /* Initialize alt response file usage */
int scan_for_lrecl(char *b);
int skip_to_DRQ(FILE *inputfile);
void position_to_record_boundary (int *num_bytes_read, char *precord, char *lrecord, FILE *inputfile);

#define NUMARGS 3		/* min # of command line args */

#define TAPE_DEVICE 	0	/* used by check_block_mode */
#define DISK_DEVICE 	-1	/* for readability */

#define NO_NL(s) 	s[strlen(s) - 1] = 0

/* function control flags, defined here for local subprocedure access */
static int abbrev_flag = FALSE;		/* abbreviation dict. flag */
static int data_flag = FALSE;		/* data recovery flag */
static int stations_flag = FALSE;	/* station info flag */
static int contents_flag = FALSE;	/* volume tbl of cnts flag */
static int snoop_flag = FALSE;		/* contents lister flag */
static int times_flag = FALSE;		/* start/stop times flag */
static int event_flag = FALSE;		/* output event info */
static int ex_stn_flag = FALSE;		/* extract station stuff */
static char inputfilename[100];		/* name of input file */
static int outputformat_flag = FALSE;	/* output format defined */
static int output_pipe = FALSE;		/* output pipe defined */
static int Volume_start;
static int station_comments_flag;
static int channel_comments_flag;
static int handle_var_rec = FALSE;

int strip_flag;				/* strip out records with
					 * zero for a sample count
					 */

int swapped = FALSE;			/* has fsdh been reversed*/
int mini_flag = FALSE;			/* make data only seed   */
int Seed_flag = FALSE;			/* make new seed volume  */
int Output_PnZs = FALSE;		/* write poles and Zeros */
static int got_station = FALSE;
static int from_weed = FALSE;
int read_summary_flag = FALSE;
int ignore_net_codes = FALSE;		/* don't scan for network codes */
int ignore_loc_codes = FALSE;           /* ditto location codes */


float WeedVersion;
int EventDirFlag;			/* whether to group output traces 
					 * in directories per event, as
					 * found in the summary file
					 */ 

char Rdseed_Cwd[MAXPATHLEN];

char output_dir[MAXPATHLEN];

void usage ();			/* fcn to show usage */

/* --------------------------------   Main   ------------------------------ */

main (argc, argv, evp)
int argc;
char *argv[];
char *evp[];

{

	int i;				/* counter */
	int j;				/* counter */
	int eof_count;			/* count of EOF marks found */
	int opt_count;			/* count of options found */
	struct input_data_hdr *input_data_hdr;	/* fixed data header */
	char *input_data_ptr;

	struct optstruct *options;
	FILE *save_file;		/* save place for input file */

	char wrkstr[512];

	WeedVersion = 2.0;	/* default */
	EventDirFlag = FALSE;

	getcwd(Rdseed_Cwd, MAXPATHLEN);

	strcpy(output_dir, "./");

	if (argc > 1)
	if (strcmp(argv[1], "FROM_WEED") == 0)
	{
		extern char *optarg;
		extern int optint;
		extern int opterr;

		int c;

		from_weed = TRUE;

		/* increment past the FROM_WEED arg */
		argc--; argv++;

		while ((c = getopt(argc, argv, "v:")) != -1)
                	switch(c)
                	{
				case 'v' : WeedVersion = atof(optarg);
					   break;

				default  :
			   fprintf(stderr, "Bad option scanned: %c\n", c);
					   break;

			}

		/* return the arg now - getopt doesn't like it */
		argc++; argv--;


	}

	/* set up the error file for stderr messages */
	init_error_file();

	/* no patchlevel  */

	fprintf(stdout, "<< IRIS SEED Reader, Release %s >>\n", VERSION);

/*                 +=======================================+                 */
/*=================|    Process command line, set flags    |=================*/
/*                 +=======================================+                 */

/* - development - echo command line arguments */
/*
		fprintf (stderr,"Command line was:  \n");
		fprintf (stderr,"argc = %d\n", argc);
		for (i = 0; i < argc; i++) fprintf (stderr,"item %d: %s\n", i, argv[i]);
*/

	/* options:
		a	read abbreviations dictionaries
		c	read volume table of contents
		l	list contents (deterministic)
		s	read station headers
		t	list time segments (deterministic)
		d:	recover data
		f:	input filename
		o:  	output data format
	*/


begin:

	LRECL = 4096;		/* start at 4096 until volume header has been read */

	input_data_hdr = (struct input_data_hdr *) input.data;

	input_data_ptr = input.data;

	outputfile = stdout;

	output_pipe = FALSE;
	volume_number = 1;
	station_count = 0;
	channel_count = 0;
	network_count = 0;
	location_count = 0;
	start_time_count = 0;
	end_time_count = 0;
	seis_buffer_length = MAX_DATA_LENGTH;

	strcpy(inputfilename,"/dev/nrst0");

	outresp_old_flag = FALSE;
	outresp_flag = FALSE;
	output_flag = FALSE;
	output_format = 0;
	output_endtime = FALSE;
	check_reverse = 0;
	q_flag = 'E';
	opt_count = 0;
	abbrev_flag = FALSE;
	contents_flag = FALSE;
	snoop_flag = FALSE;
	stations_flag = FALSE;
	times_flag = FALSE;
	strip_flag = FALSE;

	station_comments_flag = channel_comments_flag = FALSE;

	/* set flags to false in process_stnh() */
	reset_stn_chn_flags();

	Volume_start = 0;

	strcpy(channel_list, "");
	strcpy(station_list, "");

	mini_flag = data_flag = Seed_flag = read_summary_flag = FALSE;

	outputformat_flag = FALSE;

	event_flag = FALSE;
	ex_stn_flag = FALSE;

	Output_PnZs = FALSE;
	EventDirFlag = FALSE;
	got_station = FALSE;
	ignore_net_codes = FALSE;
	ignore_loc_codes = FALSE;

	/* read in alias strings */
	get_environment();

	if (!from_weed)
	if ((options = cmdlineproc (argc, argv, "iacEklprRsStdeuhg:z:o:f:v:x:C:q:Q:b:")) != NULL)
	{

		for (i = 0; i < options->number_options; i++)
		{
			if (!options->option_present[i])
				continue;

			opt_count++;

			parse_option(options->option[i], 
					options->argument[i]);

		}


		if (output_format == 4) 	/* Seed output */
		{
			/* removes files perhaps left behind */
			clean_up_output_seed(); 
			Seed_flag = TRUE;   /* set for new seed */

		}

	}

	if (opt_count == 0) 
		option_prompt();

	if (data_flag||mini_flag) 
		if ((seismic_data = (double *)malloc(seis_buffer_length*sizeof(double))) == NULL)
		{
			fprintf (stderr, "\tERROR - Data Buffer too large\n");
			exit(1);
		}

		
	if ((inputfile = fopen(inputfilename, "r")) == NULL)
	{
		fprintf (stderr, "\tERROR (rdseed):  ");
		fprintf (stderr,
			"Input file %s unavailable for reading.\n",
			inputfilename);
		perror(argv[0]);
		fprintf (stderr, "\tExecution terminated.\n");
		exit (-1);
	}

	/* determine type of file character or block */
	input_file_type = check_block_mode(inputfilename);

	if (data_flag||mini_flag)
	{
		if (!input_file_type)
			printf("WARNING - Input Device is Character Type - Processing will be SEQUENTIAL\n");

		if (station_volume&&input_file_type)
			printf("WARNING - Volume Type is Station - Processing will be SEQUENTIAL\n");

		else 
		if (alt_headerfile != NULL) 
			printf("WARNING -  Alternate Header File Present - Processing will be SEQUENTIAL\n");

	}
		
	switch (output_format)
	{
		case 0:  sprintf (outputformat, "sac.binary"); break;
		case 1:  sprintf (outputformat, "ah.binary"); break;
		case 2:  sprintf (outputformat, "css.binary"); break;
		case 3 : sprintf (outputformat, "mini.seed"); break;
		case 4 : sprintf(outputformat, "seed"); break;
		case 5 : sprintf(outputformat, "sac.ascii"); break;
		case 6 : sprintf(outputformat, "SEGY"); break;
		case 7 : 
		case 8 : sprintf(outputformat, "Simple Ascii"); break;

		default: sprintf (outputformat, "unknown"); break; 
	}

	if (outputformat_flag)
	{	
		fprintf (stdout,"Output data format will be %s.\n", outputformat);		

		if (output_format == 0 || output_format == 5)		// sac output
		{
			printf("INFO: sac variable EVDP will be in KILOMETERS\n");
		}
	}


/*                 +=======================================+                 */
/*=================|            Open input file            |=================*/
/*                 +=======================================+                 */

/*                 +=======================================+                 */
/*=================|             Initialization            |=================*/
/*                 +=======================================+                 */

	init_data_hdr ();
	start_record = 0;
	offset = 0;
	more_data = TRUE; eof_count = 0;
	found_lrecl_flag = at_volume = FALSE; volume_count = 0;
	sample_rate_accum = 0.0;
	sample_rate_count = 0;

	if (alt_headerfile != NULL)
	{
		save_file = inputfile;
		inputfile = alt_headerfile;
		reading_alt_file = TRUE;
	}
	else reading_alt_file = FALSE;

	/* print nice header for times data...if times data was specified.
	 * option "t"
	 */
	if (times_flag)
	{
		fprintf(outputfile, "Time series: \n");
		fprintf(outputfile, "  Rec# Sta   Cha Net Loc      Start Time              End Time         Sample Rate Tot Samples\n");
	}

/*                 +=======================================+                 */
/*=================| Read and process file until EOF found |=================*/
/*                 +=======================================+                 */

/*	if (snoop_flag) snoop (inputfile); */
/*	if (times_flag) times (inputfile); */

MORE_ALT_FILES:
	
	while (more_data == TRUE)
	{
		// swapped = FALSE;

		if (offset == 0) /* read a physical record */
		{

		/*****************************************************/
		/* read a physical record and point to its beginning */
		/*****************************************************/
			num_bytes_read = fread(precord, 1, PRECL, inputfile);

			if (num_bytes_read != 0 && (alt_headerfile!=NULL) && !reading_alt_file) {
				if (precord[6] == 'M' | precord[6] == 'D' || precord[6] == 'Q' || precord[6] == 'R') {
					/* in data, may be variable record length and need repositioning */

					position_to_record_boundary(&num_bytes_read, precord, precord, inputfile);
				}
			}
			if (num_bytes_read == 0)
			{
				if (reading_alt_file)
				{
					/* No more alt files */
					reading_alt_file = FALSE;

					inputfile = save_file;

					/* force a read (in skip_to_(d)) */
					offset = PRECL;
					/* skip to 'D' */
					if ((num_bytes_read = skip_to_DRQ(inputfile)) == 0)
						break;
				}
				else 
				if ((eof_count > 0) || input_file_type)
				{
					if (at_volume)
						break;

					fprintf(stderr,"!!! ERROR - Volume Not Found !!!\n");
					fprintf(stderr, "try setting ALT_RESPONSE_FILE to a dataless seed volume\n");

					fclose(inputfile);
					exit(1);
				}
				else
				{
					eof_count++;
					fclose(inputfile);
					inputfile = fopen(inputfilename,"r");
					if (at_volume) more_data = FALSE; 
					continue;
				}
			}
			else if (num_bytes_read < 0)
			{

				fprintf(stderr,"!!! ERROR - Fatal I/O Read Error, Exiting Program !!!\n");
				perror(argv[0]);

				fclose(inputfile);
				exit(1);
			}		
			else eof_count = 0;

			precord_ptr = (char *)precord;
			lrecord_ptr = precord_ptr;

		}

skip_flag:

		/* point to beginning of this logical record */
		input_data_hdr = (struct input_data_hdr *) (lrecord_ptr + 8);

		input_data_ptr = (lrecord_ptr + 8);

		/* extract a logical record from the physical record */
		read_logical_record (lrecord_ptr);

		if (snoop_flag) fprintf(stdout,"%06ld ", input.recordnumber);

		/* process logical record according to its type */
		/* "read_blockette" handles spans across logical or physical records */
		if (found_lrecl_flag || input.type=='V')
		switch (input.type)
		{
			case 'D':
			case 'R':
			case 'Q':
			case 'M':
				/* hopefully, rdseed will not be around by 3010 */
				if (input_data_hdr->time.year < 1950 || 
						input_data_hdr->time.year > 3010)
				{
					swap_fsdh(&input_data_ptr); 
					swapped = 1;
				}

				/* check for sanity */ 
				if (input_data_hdr->time.year < 1950 || 
						input_data_hdr->time.year > 3010)
				{ 
					fprintf(stderr, "ERROR - rdseed(): Unknown word order for station %s, channel %s, network %s\n", input_data_hdr->station, input_data_hdr->channel, input_data_hdr->network);
					fprintf(stderr, "Skipping data record.\n");
 
					continue;
				}

				/* bale here if only interested in event info */
				if (event_flag  || ex_stn_flag || stations_flag 
					|| station_comments_flag || channel_comments_flag)
				{
					more_data = FALSE;
					break;
				}

				if (reading_alt_file)
				{
					reading_alt_file = FALSE;

					inputfile = save_file;

					at_volume = FALSE;
					break;
				}
				else 
					if ((alt_headerfile!=NULL) && (volume_count==0))
						at_volume = TRUE; 

				if (abbrev_flag || stations_flag) 
				{
					more_data = FALSE;
					break;
				}

				if (times_flag)
				{
                                        if (chk_station(input_data_hdr->station)&&
                                                        chk_channel(input_data_hdr->channel) &&                                         
                                                        chk_network(input_data_hdr->network))
                                        {
                                                /* check if byteswapping will be needed                                          
                                                 * to recover data */
 
                                                LRECL = get_LRECL(input_data_hdr, lrecord_ptr);

						times_data(0);

					}

					break;
				}
				if (contents_flag)
				{
					if (type74_head != NULL) 
					{
						more_data = FALSE;
						break;
					}
				}

				if (input_file_type == DISK_DEVICE)
				{
					/* scan in all atc data from block 74s */
					atc_load(inputfile);

					atc_dump();

				}

				if ((data_flag||mini_flag)  && 
					   input_file_type      &&
					  (volume_number==1)    &&
					  (alt_headerfile==NULL)&& (!station_volume)) /* whew!! */
				{

					if (i = process_time_span_index()) /* 0 = type74; 1 = type73; -1 = none */
					{
						if  (i < 0) 
						{
							input_file_type = 0;
					 		if (chk_station(input_data_hdr->station) &&
							    chk_channel(input_data_hdr->channel) && chk_network(input_data_hdr->network))
							{
								process_data (swapped);
							}
						}
						else offset = 0;
					}
					else more_data = FALSE;
				}
				else  
				if (data_flag)
				{ 
printf("#2\n");


					if (chk_station(input_data_hdr->station)&& 
							chk_channel(input_data_hdr->channel) &&
							chk_network(input_data_hdr->network))
					{

						LRECL = get_LRECL(input_data_hdr, lrecord_ptr);

						process_data (swapped);

						output_flag = TRUE;
					}
				}

				break;

			case 'T':
				/* bale here if only interested in station*/
				if (ex_stn_flag || stations_flag)
                                {
                                        more_data = FALSE;
                                        break;
                                }


				if (reading_alt_file)
				{
					reading_alt_file = FALSE;
					close_alt_files();

					inputfile = save_file;

					at_volume = FALSE;	

					/* reset offset so we load
					 * in new stuff */
					offset = 0;

					break;
				}

				if (!at_volume) break;
				process_timeh ();

				break;
			case 'S':

				/* if we are not reading the alt_response file &&
				 * we aren't at the right volume number, skip it
				 */
				if ((!reading_alt_file) && (!at_volume))
					break;

				/* bale if they want only abbrev */
				if (abbrev_flag)
				{
					more_data = FALSE;
					continue;
				}
					
                                /* if they want contents, 
                                 * skip to the block 70s
                                 */
                                if (contents_flag || event_flag)
				{
	
					if ((input_file_type != TAPE_DEVICE) &&
						 (!reading_alt_file))
                                	{
                                		process_B70s(inputfile);
						more_data = FALSE;	
						continue;
                                	}

					/* else alt response file ?*/
					if (reading_alt_file)
					{
						/* drop out fini*/
						more_data = FALSE;
                                                continue;
					}
					/* else - continue to loop */
					
					break;

				}

				 
				if (!at_volume || 
				   ((alt_headerfile!=NULL) && !reading_alt_file)) 
					break;

 				if (input_file_type != TAPE_DEVICE) 
				{
					if (type11_head != NULL)
					{
						process_B11(inputfile);

						/* finished with the stations, 
					 	 * go on to the timepsans
					 	 */

						if (stations_flag || 
							(Output_PnZs && (!data_flag)) ||
							ex_stn_flag ||
							station_comments_flag ||
							channel_comments_flag)
						{
							more_data = FALSE;
							break;
						}
		
                                                if (reading_alt_file)
                                       		{

                                                        if (!(inputfile = get_alt_file()))
                                       			{
	
                                                                reading_alt_file = FALSE;

                                                                inputfile = save_file;

                                                                /* force a read */
                                                                offset = PRECL;

                                                                /* skip to 'D' */
                                                                if ((num_bytes_read = skip_to_DRQ(inputfile)) == 0)
                                                                {
                                                                        break;
                                                                }

                                                                goto skip_flag;

                                                        }

							at_volume = 0;
					
							volume_count--;
	
                                                        goto MORE_ALT_FILES;

                                                }
	
						if (data_flag || Output_PnZs ||
							mini_flag || 
								Seed_flag)
						{

							if (type12.number_spans == 0)   // dataless
                                                        { 
                                                                more_data = FALSE;
                                                                continue;
                                                        }
                             
							process_B70s(inputfile);
							continue;	
						}
				
						if (outresp_flag)
						{
							/* bale */
							more_data = FALSE;
							continue;
						}

						break; 
					}
	
				}

				if (input.continuation && !got_station)
					break;
						
				/* look to the blockette type for B050 */
				sprintf(wrkstr, "%3.3s", input.data);

				if (atoi(wrkstr) == 50)
				{
					if ((got_station = scan_stn_net()) == 0)
						break;

				}

				if (got_station)
					process_stnh ();

				break;

			case 'A':

				if (!at_volume || 
				   ((alt_headerfile!=NULL) && !reading_alt_file)) 
					break;
				
				process_abrvd ();	/* Ver 3.1 */

				break;
			case 'V':
				/* check to see if already reading seed volume,
				 * if multi volume seed file
				 */
				if (at_volume && input.recordnumber == 1)
				{

					if (reading_alt_file)
					{
						/* No more alt files */
                                        	reading_alt_file = FALSE;

                                        	inputfile = save_file;

                                        	at_volume = FALSE;
					
						continue;
					}

				}

				if (input.recordnumber == 1)
					at_volume = ++volume_count == volume_number;

				if (!at_volume)
				{
					if (volume_count > volume_number)
					{
						more_data = FALSE;
					}

                                        break;

				}

				if (snoop_flag)
				{
					fprintf(stdout, "Volume header record\n");
				}

				/* minus the 32k read in above */
				Volume_start = ftell(inputfile) - num_bytes_read;

				process_volh ();

				/* make check for version >= 2.3 if they
				 * selected output to sub seed volume
				 */

				if (at_volume)
				if ((type10.version < 2.3) && Seed_flag)
				{
					fprintf(stderr, "Unable to make a sub-seed volume from this seed volume as the version needs to be greater than 2.3\nFound version %1.1f\n", type10.version);

					fclose(inputfile);

					goto begin;
					
				}

				break;
			case ' ':
				if (!at_volume) break;
				if (snoop_flag)
				{
					fprintf(stderr, "Blank record\n");
					break;
				}
				process_blank ();
				break;
			default:
				fprintf (stderr, "WARNING (rdseed):  ");
				fprintf (stderr, "Record number %ld was of unknown type %c.\n", 
					input.recordnumber, input.type);
				fprintf (stderr, "\tExecution continuing.\n");
		}

		lrecord_ptr = lrecord_ptr + LRECL;

		/* set up offset for next time around */
		offset += LRECL;
		if (offset >= num_bytes_read) 
			offset = 0;

	}

	if (reading_alt_file)
		inputfile = save_file;

	if (station_comments_flag && channel_comments_flag)
		output_stn_chn_comments();
	else
	if (station_comments_flag)
		output_stn_comments();
	else
	if (channel_comments_flag)
		output_chn_comments();

	if (event_flag)
		output_event_info();

	if (ex_stn_flag)
		output_station_info();

	if (data_hdr->nsamples) {
		dump_seismic_buffer();
	}

	close_b2k_file();	/* if open */

	/* create seed volume - data and block 74 are extracted by now */

	if (Seed_flag)
	{
		output_seed_volume();

		/* removes files perhaps left behind */

		clean_up_output_seed();

	}


	if (outresp_flag)
	    output_resp();

	if (times_flag)
		times_data (1);

	/* report volume header and time index of volume */
	if (contents_flag)
	{
		print_volh ();
		print_timeh ();
	}

	/* list all information for selected stations */
	if (stations_flag)
	{
		print_volh ();

		for (current_station = type50_head; 
			 	current_station != NULL;
				current_station = current_station->next)
		{
					print_stnh ();
		}

	}

	/* write out the abbreviation dictionaries */
	if (abbrev_flag)
	{
		if (snoop_flag)
			fprintf(stdout, "Print Abbreviations\n");
		print_volh ();
		print_abrvd ();
	}

	if (Output_PnZs)
		output_polesNzeros();


/*===========|                clean up               |=================*/
/*           +=======================================+                 */

	if (output_pipe) pclose(outputfile);
	else fclose (outputfile);

	outputfile = stdout;

	fclose (inputfile);

	if (inputfile = get_alt_file())
	{

		/* more alt files */
		reading_alt_file = TRUE;

		if (data_flag || mini_flag)
			free((char *)seismic_data);

		free_all(); 

		rewind(save_file); /* start over with the original seed volume */

		at_volume = FALSE;

		goto MORE_ALT_FILES;
	}
	else
		close_alt_files();

	if (!read_summary_flag && !contents_flag)
	{
		if (start_time_point != NULL)
			free(start_time_point);

		if (end_time_point != NULL)
			free(end_time_point);

		start_time_point = end_time_point = NULL;
	}

	if (read_summary_flag)
	{
		free_stn_nodes();

		read_summary_flag = FALSE;

	}

	if (opt_count == 0)
	{
		if (data_flag || mini_flag)
			free((char *)seismic_data);

		free_all();	/* free all memory */

		goto begin;
	}

	if (rdseed_alert)
		fprintf(stdout, "Attention, please note there are alert messages in the rdseed alert file: rdseed.alert_log.todays_date\n");

	fprintf (stdout, "rdseed completed.\n");
}

struct data_blk_1000 *scan_for_blk_1000();
/* -------------------------------------------------------------------- */
int scan_for_lrecl(char *b)

{
	int where;

	struct input_data_hdr *hdr_1 = (struct input_data_hdr *)b;
	struct input_data_hdr *hdr_2;



	/* check 512 */
	where = 512;		

	while (where <= 8092)
	{

		hdr_2 = (struct input_data_hdr *)&b[where];

		if ((strncmp(hdr_1->station, hdr_2->station, 5) == 0) &&
	    	    (strncmp(hdr_1->channel, hdr_2->channel, 3) == 0) &&
	    	    (strncmp(hdr_1->location, hdr_2->location, 2) == 0) &&
	    	    (strncmp(hdr_1->network, hdr_2->network, 2) == 0))
			return where;	

		where += 2;
	}

	return where;
	
}

/* -------------------------------------------------------------------- */

int process_blk_2k(struct input_data_hdr *hdr, struct data_blk_2000 *blk, int rec_length)

{
	int err;

	err =  output_b2k(hdr, blk, rec_length);

	return err;

}

/* -------------------------------------------------------------------- */


int process_B70s(inputfile)
FILE *inputfile;

{
	int i;
	long int seek_pos;

	char *p;

	for (i = 0; i < type12.number_spans; i++)
	{

		/* relative to where we are */
		seek_pos = ((type12.timespan[i].sequence_number - 1) * LRECL) - ftello(inputfile) + Volume_start;
		
		fseeko(inputfile, seek_pos, 1);

		/* sequentially read thru the file until 0000D are found */

		/*** (...but stop reading at the end of time span)  ****/

		num_bytes_read = fread(precord, 1, PRECL, inputfile);

		if (num_bytes_read == -1)
		{
			fprintf(stderr, "Unable to scan blockette 70s!\n");
			perror("process_B70s");

			return;

		}

		lrecord_ptr = precord;
		offset = 0;


		/* check for 'T' indicating timespan section.
	  	 * We sometimes get garbage in the blockette 12s,
		 * necessitating a check here 
		 */

		if (precord[6] != 'T')
		{
                        fseek(inputfile, 0, 0);

			num_bytes_read = fread(precord, 1, PRECL, inputfile);
 
                        if (num_bytes_read < 0)
                        {
				fprintf(stderr, "Unable to read volume for time series information\n");
 
				perror("process_B70s");

                                return;
 
                        }

			offset = 0;
			lrecord_ptr = precord;

			if (skip_to_T(inputfile) == 0)
                        	return;

		}

		do 
		{
			if (lrecord_ptr[6] != 'T')
			{
				/* ### Kludge alert: if this is the first "chunk"
				 * of precord, reset file pointer, as with the 
				 * offset being zero will cause the "main" loop
				 * to do an unnecessary read, missing this one
				 */
				if (offset == 0) 
					fseek(inputfile, -num_bytes_read, 1);

				break;
			}

			read_logical_record(lrecord_ptr);
			process_timeh ();

			if (offset + LRECL >= PRECL)
			{
				num_bytes_read = fread(precord, 
							1, PRECL, 
							inputfile);
                		if (num_bytes_read < 0)
                        		return;

				offset = 0;
				lrecord_ptr = precord;
			}
			else
			{
				lrecord_ptr += LRECL;
				offset += LRECL;

			}

			blockette_length = 0;
			blockette_type = -1;

			p = lrecord_ptr;

			read_blockette (&p, blockette, &blockette_type, &blockette_length);

			if (blockette_type < 71 || blockette_type > 74) /* next time span */
				break;

		} while (1);

	}

}


int process_B11(inputfile)
FILE *inputfile;

{
        int i;
        long int seek_pos;

	struct type11 *type11;

	if (type11_head != NULL)
        {

		for (type11 = type11_head; type11 != NULL; type11 = type11->next)
                {

			for (i = 0; i < type11->number_stations; i++)
			{

	        		if (!chk_station(type11->station[i].station_id))
                			continue;

				/* relative to where we are */
				seek_pos = ((type11->station[i].sequence_number - 1) * LRECL) - 
								ftello(inputfile) + Volume_start;

				fseeko(inputfile, seek_pos, 1);

				process_station_lrecs(inputfile, i, type11);

			}

		}
	}
}

/* The function position_to_record_boundary is called after a physical record read, 
if alt_response_file has been read.  It's purpose is to make sure the read 
didn't end up in the middle of a logical record, which is possible since 
miniseed may have variable record sizes.  Not used For full SEED, assuming that
full SEED was written by pod and is padded out to logical record boundary. 
SRS 2006/04
*/

void where_am_i(char *lrecord, FILE *file) {

	if (lrecord != NULL) 
		printf("lrec = %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c",
			lrecord[0],
			lrecord[1],
			lrecord[2],
			lrecord[3],
			lrecord[4],
			lrecord[5],
			lrecord[6],
			lrecord[7],
			lrecord[8],
			lrecord[9],
			lrecord[10],
			lrecord[11],
			lrecord[12],
			lrecord[13],
			lrecord[14],
			lrecord[15],
			lrecord[16],
			lrecord[17],
			lrecord[18],
			lrecord[19],
			lrecord[20]
);
	printf ("\n");
	return;
}

void blockette_swap(struct input_data_hdr *);

void position_to_record_boundary(int *num_bytes_read, char *precord, char *lrecord, FILE *inputfile) {

	char *lptr = lrecord;
	struct input_data_hdr *hdr = (struct input_data_hdr *)(lptr + 8);
	struct data_blk_1000 *p;
	/* i = how far we are into precord */
	int i = lrecord - precord;
	int rec_len = 0;

	while (i < *num_bytes_read) {
		/* can run into this if logical record padding in file, assume don't need to repos */
		if (lptr[6] != 'M' && lptr[6] != 'D' && lptr[6] != 'Q' && lptr[6] != 'R')  {
			return;
		}

		hdr = (struct input_data_hdr *)(lptr+8);

		if (hdr->time.year < 1950 || hdr->time.year > 3010)
		{
                	swap_fsdh(&hdr);
			swapped = 1;
		}
		else
			swapped = FALSE;

        	if (hdr->bofb != 0) {
		{
// blockette_swap((struct input_data_hdr *)(lptr + hdr->bofb), (char *)(lptr - 8));

                	p = scan_for_blk_1000(lptr + hdr->bofb, lptr);
		}
	        } else {
			p = NULL;
		}

        	if (p) {
			rec_len = (2 << (p->rec_length - 1));
			if (i + rec_len > *num_bytes_read) break; 
        	} else {
			/* hdr = (struct input_data_hdr *)(lptr+8); */
			
			rec_len = get_LRECL(hdr, lptr);

			if (i + rec_len > *num_bytes_read) break;

		}
		i += rec_len;
		lptr += rec_len;
	}
	
	/* precord */
	if (*num_bytes_read != i && !feof(inputfile)) {
		fprintf(stdout,  "Repositioning back %d bytes from %d to be on an even record boundary.\n", 
				(*num_bytes_read - i), ftell(inputfile));
		fseek(inputfile, (long) -(*num_bytes_read - i), SEEK_CUR);
		*num_bytes_read = i;
	}

	return;
}


/* ------------------------------------------------------------------ */
int skip_to_DRQ(inputfile)
FILE *inputfile;

{
	int num_bytes_read;

	do {

		if (offset + LRECL > PRECL)
		{
			/* should always come here first */
                        num_bytes_read = fread(precord, 1, PRECL, inputfile);
                        if (num_bytes_read < 0)
			{
				fprintf(stderr, "ERROR: skip_to_(): unable to read the inputfile while scanning for records\n");
				perror("rdseed");
				return 0;

			}
			/* didn't find any */	
			if (num_bytes_read == 0)
				return 0;
			offset = 0;
			lrecord_ptr = precord;
		}

		if (lrecord_ptr[6] == 'M' || lrecord_ptr[6] == 'D' || lrecord_ptr[6] == 'Q' || lrecord_ptr[6] == 'R') {
			/* if found data, adjust to data boundary */
			position_to_record_boundary(&num_bytes_read, precord, lrecord_ptr, inputfile);
			break;
		}
		lrecord_ptr += LRECL;
		offset += LRECL;
	
	} while (1);


	return num_bytes_read;

}

/* ------------------------------------------------------------------ */

int skip_to_T(inputfile)
FILE *inputfile;

{
	int num_bytes_read;

	do 
	{
		if (offset + LRECL > PRECL)
		{
                        /*num_bytes_read = read(fileno(inputfile), precord, PRECL);*/
                        num_bytes_read = fread(precord, 1, PRECL, inputfile);
                        if (num_bytes_read < 0)
			{
				fprintf(stderr, "ERROR: skip_to_(): unable to read the inputfile while scanning for records\n");

				perror("rdseed");

				return 0;

			}
	
			/* didn't find any */	
			if (num_bytes_read == 0)
				return 0;

			offset = 0;
			lrecord_ptr = precord;
		}

		if (lrecord_ptr[6] == 'T')
			break;
	
		lrecord_ptr += LRECL;
		offset += LRECL;
	
	} while (1);

	return 1;

}

/* ------------------------------------------------------------------ */
int process_station_lrecs(inputfile, which_station, t11)
FILE *inputfile;
int which_station;
struct type11 *t11;

{
	offset = 0;

	/* read in a 32768 buffer */
	memset(precord, ' ', PRECL);

	num_bytes_read = fread(precord, 1, PRECL, inputfile); 
  
	lrecord_ptr = precord_ptr = precord; 

	offset = 0;

	while (1)
	{
		read_logical_record(lrecord_ptr);
 
		if (input.type != 'S')
		{
			if (offset == 0)
			{
				/* so subsequent read() in "main" will be right */

				fseek(inputfile, -num_bytes_read, 1);

			}
	
			lrecord_ptr -= LRECL;
			offset -= LRECL;

			return;	/* bale */
		}
		else
		{
			if (process_stnh() == 0)
				/* took a look and the station isn't wanted */
				return;

		}


		/* don't look if at end of station list, just bale, see above */

		if (which_station < t11->number_stations - 1)	
			/* if current logical record is <= the next stations, we are done */
			if (input.recordnumber >= t11->station[which_station+1].sequence_number - 1)
			{
				return;	/* finished with this station */
			}

		/* if a blockette happens to exactly match the end of LRECL 
		 * buffer, procees_stnh() returns, but there is really more
		 */
		if (offset + LRECL >= PRECL)
		{
			/* we've exhausted this buffer, grab some more */
			/* but clear precord first in case we are already 
			 * at EOF. 
			 */
			memset(precord, ' ', PRECL);
 
			num_bytes_read = fread(precord, 1, PRECL, inputfile);
			lrecord_ptr = precord_ptr = precord;
 
			offset = 0;

		}
		else
		{
			/* not quite done with the big buffer */

			lrecord_ptr = precord + offset + LRECL;
			offset += LRECL;
		}

	}		/* while not finished */

	return;

}

/*=====================================================================*/
/* SEED reader    |         parse_option            |    subprocedure */
/*=====================================================================*/

int parse_option(option, arg)
int option;
char *arg;

{

	struct stat output_stat; 		/* stat output_dir */ 
	switch (option)
	{
	case 'a':
		abbrev_flag = TRUE;
		fprintf (stdout,"\ta = read abbreviation dictionaries\n");
		break;
	case 'b':
		seis_buffer_length = atoi(arg);

		fprintf(stdout, "\tb = set sample buffer length : %d\n", seis_buffer_length);
		break;

	case 'C':
		if (strcasecmp(arg, "STN") == 0)
		{
			station_comments_flag = TRUE;
			fprintf(stdout, "Output station comments selected\n");
		}
		else
		if (strcasecmp(arg, "CHN") == 0)
		{
			channel_comments_flag = TRUE; 
			fprintf(stdout, "Output channel comments selected\n");
		} 
               	else 
		{
			fprintf(stdout, "Output station and channel comments selected (default)\n");
			station_comments_flag = channel_comments_flag = 1;

		}
		break;
	case 'c':
		contents_flag = TRUE;
		fprintf (stdout,"\tc = read volume table of contents\n");
		break;

	case 'd':
		data_flag = TRUE;
		fprintf (stdout,"\td = read data from tape\n");
		break;
	case 'e':
		fprintf(stdout,"\te = extract event/station data\n");
               	event_flag = TRUE;
               	break;
	case 'E':
		fprintf(stdout,"\tE = output filename includes endtime\n");
                output_endtime = TRUE;
                break;

	case 'f':
		strcpy (inputfilename, arg);
		/*fprintf (stdout,"\tTaking input from %s\n", inputfilename);*/
		fprintf (stdout,"\tTaking input from %s\n", arg);
		break;
	case 'g':
		fprintf (stdout,"\tUsing alt response file(s): %s\n", arg);
		close_alt_files();
		init_alt_files(arg);
		break;
	case 'h':
		if (inputfile != NULL) fclose (inputfile);
		usage ();
		exit (1);
	case 'i':
		fprintf(stdout, "\ti = ignore network codes\n");
		ignore_net_codes = TRUE;
		break;	

	case 'k':
                fprintf(stdout, "\tk = strip records w zero samples\n");
                strip_flag = TRUE;
                break;  

	case 'l':
		snoop_flag = TRUE;
		if (abbrev_flag || contents_flag || 
			stations_flag || times_flag || data_flag)
		{
			fprintf (stdout, "l is an exclusive option.\n");
			usage ();
		}
		fprintf (stdout,"\tl = list contents of each logical record\n");
		break;
	case 'o':
		outputformat_flag = TRUE;

		output_format = (*arg) - 49;		// output_format starts at zero but the arg starts at 1

		if (output_format < 0 || output_format > 8)
			output_format = 0;		// default to sac

		break;

	case 'p':
		fprintf(stdout,"\tp = output poles and zeros\n");
                Output_PnZs = TRUE;
                break;

	case 'q':
		strcpy(output_dir, arg);

		break;


	case 'Q':
		switch (*arg) 
		{
			case('D') : 
				q_flag = 'D';
				fprintf(stdout,"\tQ = scanning for Undetermined Quality\n");

				break;
			case('R') :
				q_flag = 'R';
				fprintf(stdout,"\tQ = scanning for Raw Quality\n");

				break;
			case('Q') :
				q_flag = 'Q';
				fprintf(stdout,"\tQ = scanning for Quality controlled data \n");

				break;
			case('M') :
				q_flag = 'M';
				fprintf(stdout,"\tM = scanning for Merged data \n");

				break;


			default :
	                       	q_flag = 'E';
				fprintf(stdout,"\tQ = scanning for all data \n");
				break;

		}
		break;
 					
	case 'R':
		fprintf(stdout,"\tR = print response data (with addressing for evresp)\n");
		outresp_flag = TRUE;
		break;
	case 'S':
	    	fprintf(stdout,"\tS = extract station data\n");
		ex_stn_flag = TRUE;
		break;
	case 's':
		stations_flag = TRUE;
		fprintf (stdout,"\ts = read station headers\n");
		break;
	case 't':
		times_flag = TRUE;
		if (abbrev_flag || contents_flag || snoop_flag ||
				stations_flag || data_flag)
		{
			fprintf (stdout, "t is an exclusive option.\n");
			usage ();
		}

		fprintf (stdout,"\tt = list start/stop times\n");
		break;
	case 'u':
		if (inputfile != NULL) fclose (inputfile);
		usage ();
		exit (1);
	case 'v':
		sscanf(arg,"%d",&volume_number);
		if (volume_number < 1 || volume_number >100) 
			volume_number = 1;
		fprintf(stdout,"\tv = volume number %d selected\n",volume_number);
		break;
	case 'x': 
        	read_summary_flag = TRUE;

               	/* if not io_error */
               	read_summary_flag = 
		     read_summary_file(arg);

		data_flag = TRUE;

		break;

	case 'z':
		sscanf(arg,"%d",&check_reverse);
		if (check_reverse < 0 || check_reverse > 3) 
			check_reverse = 0;
		fprintf(stderr,"\tz = check reversal %d selected\n",check_reverse);
		break;




	default:
		break;
	}
	if (stat(output_dir, &output_stat)) {
		fprintf(stdout,"Output dir %s not found.\n", output_dir); 
		exit (1);
	} 
	/* verify output_dir is dir */
	if (!S_ISDIR(output_stat.st_mode)) {
		fprintf(stdout,"Output dir %s not a directory.\n", output_dir);
		exit (1);
	}
	
}

/*                 +=======================================+                 */
/*======================================================================*/
/* SEED reader    |         option_prompt             |    subprocedure */
/*======================================================================*/
/* read interactive user input */
option_prompt()
{
	char buffer[120], *p, *p2;
	int i, comments_flag;

	comments_flag = FALSE;

	/* signal "weed" that the request has cycled */
	if (from_weed)
	{
		kill(getppid(), SIGUSR1); 

		sleep(3);
	}

	i = 1;
	while (i)
	{
		printf("Input  File (/dev/nrst0) or 'Quit' to Exit: ");
		fflush(stdout);

		if (fgets(buffer, sizeof(buffer), stdin) == NULL) exit (0);
		NO_NL(buffer);

		strip_blanks(buffer, buffer);
		if (strcasecmp(buffer,"QUIT") == 0) 
		{
			exit(0);
			if (rdseed_alert)
                		fprintf(stdout, "Attention, please note there are alert messages in the rdseed alert file: rdseed.alert_log.todays_date\n");

		}

		if (strlen(buffer) != 0) strcpy(inputfilename,buffer);

		if ((inputfile = fopen(inputfilename, "r")) == NULL)
		{

			fprintf (stderr, "ERROR... Input file %s unavailable for reading.\n", inputfilename);

			perror("rdseed");
		}
		else i = 0;
	}

	fclose(inputfile);

	printf("Output File (stdout)\t: ");
	fflush(stdout);
	if (fgets(buffer, sizeof(buffer), stdin) == NULL) exit (0); 
	NO_NL(buffer);

	strip_blanks(buffer, buffer);
	if (strlen(buffer) != 0)
	{
		if ((outputfile = fopen(buffer, "w")) == NULL)
		{
			fprintf(stderr,"!!! Error - Invalid Output File Name... Device will be STDOUT\n");
			outputfile = stdout;
			output_pipe = TRUE;
		}
	}
	else
	{
		outputfile = stdout;
		output_pipe = TRUE;
	}

	printf("Volume #  [(1)-N]\t: ");
        fflush(stdout); 

	if (fgets(buffer, sizeof(buffer), stdin) == NULL) exit(0);
	NO_NL(buffer);

	i = sscanf(buffer,"%d",&volume_number);
	if (i == 0) volume_number = 1;

get_options:
	printf("Options [acCsSpRtde]\t: ");
        fflush(stdout); 
	if (fgets(buffer,sizeof(buffer),stdin) == NULL) exit(0);
	NO_NL(buffer);
	if (strlen(buffer) == 0) goto get_options;

	i = 0;
	while(buffer[i])
	{
		switch (buffer[i++])
		{
			case 'a': abbrev_flag 	= TRUE;		break;
			case 'c': contents_flag = TRUE;		break;
			case 'C': comments_flag = TRUE;		break;
			case 's': stations_flag = TRUE;		break;
			case 'p': Output_PnZs   = TRUE;		break;
			case 't': times_flag 	= TRUE;		break;
			case 'd': data_flag 	= TRUE;	outputformat_flag = TRUE;	break; 
			case 'R': outresp_flag 	= TRUE;		break;
			case 'e': event_flag 	= TRUE;		break;
			case 'S': ex_stn_flag 	= TRUE;		break;
		}
	}

	if (comments_flag)
	{	
		/* ask whether station or channel or both */

		printf("Station/Channel comments or both (S/C/[B]oth)\t: ");
		fgets(buffer,sizeof(buffer),stdin);
		NO_NL(buffer);

		if (buffer[0] == 's' || buffer[0] == 'S')
			station_comments_flag = TRUE;
		else
		if (buffer[0] == 'c' || buffer[0] == 'C')
			channel_comments_flag = TRUE;
		else
			station_comments_flag=channel_comments_flag=TRUE;
	}

	if (data_flag||mini_flag)
	{
		char *summary_fname;

		/* ask for summary file */
		printf("Summary file (None)\t: ");

		fgets(buffer, sizeof(buffer), stdin);
		NO_NL(buffer);

		if (strcmp(buffer, "") != 0)
		{
			read_summary_flag = TRUE;			

			/* Format of input can be "sum_file_name event_flag" */
			summary_fname = strtok(buffer, " ");
			EventDirFlag = TRUE;

			/* if not io_error */
			read_summary_flag = read_summary_file(summary_fname);

		}

	}

	if (data_flag||mini_flag||stations_flag||ex_stn_flag||
		outresp_flag || outresp_old_flag || 
			comments_flag || Output_PnZs /* whew */) 
	{

		if (!read_summary_flag)
		{
		printf("Station List (ALL)\t: ");
	        fflush(stdout); 

		if (fgets(station_list, sizeof(station_list), stdin) == NULL) 
			exit(0);

		NO_NL(station_list);

		station_count = 0;
	
		p = strtok(station_list," ,");

		while(p != NULL)
		{
			station_point[station_count] = p;
			station_count++;
			p = strtok(NULL," ,");
		}

		printf("Channel List (ALL)\t: ");
	        fflush(stdout); 

		if (fgets(channel_list, sizeof(channel_list), stdin) == NULL) 	
			exit(0);

		NO_NL(channel_list);

		channel_count = 0;
	
		p = strtok(channel_list," ,");
	
		while(p != NULL)
		{
			channel_point[channel_count] = p;
			channel_count++;
			p = strtok(NULL," ,");
		}

		printf("Network List (ALL)\t: ");
	        fflush(stdout); 

        	if (fgets(network_list, sizeof(network_list), stdin) == NULL)
            		exit(0);
		NO_NL(network_list);
 
        	network_count = 0;
    
        	p = strtok(network_list," ,");
  
		if (p == NULL)
		{
			ignore_net_codes = TRUE;
			/* maybe not needed, but ... */
			network_point[network_count++] = "??";
		}
		else 
        	while(p != NULL)
        	{
            		network_point[network_count] = p;

			if (strcmp(network_point[network_count], "??") == 0)
			{	
				ignore_net_codes = TRUE;
			}

            		network_count++;
            		p = strtok(NULL," ,");

        	}

		printf("Loc Ids (ALL [\"--\" for spaces])\t: ");
                fflush(stdout);
 
                if (fgets(location_list, sizeof(location_list), stdin) == NULL)
                        exit(0);
 
		NO_NL(location_list);

                location_count = 0;
  
                p = strtok(location_list," ,");
 
                if (p == NULL)
                {
                        /* maybe not needed, but ... */
                        ignore_loc_codes = TRUE;
 
                        location_point[location_count++] = "??";
                }
                else
                while(p != NULL)
                {
			if (strcmp(p, "--") == 0)
				location_point[location_count] = "";
			else
                        	location_point[location_count] = p;
 
                        rdseed_strupr(location_point[location_count]);
 
                        if (strcmp(location_point[location_count], "??") == 0)
                        {
                                ignore_loc_codes = TRUE;
 
                        }
 
                        location_count++;
                        p = strtok(NULL," ,");
		}

		if (Output_PnZs)
		{
			if (output_pipe) 
				outputfile = popen("more", "w");

			return;
		}
		
		if (stations_flag||event_flag||
			ex_stn_flag||outresp_flag||outresp_old_flag) 
		/* bale here */
		{
			if (stations_flag)
			{
				char ch = 0;

                                printf("Output poles & zeroes ? [Y/(N)]");
                                fflush(stdout);
 
                                ch = getchar();
 
                                ch = toupper(ch);
                                
                                if (ch == 'Y')
                                        Output_PnZs = TRUE;
		
				printf("Extract Responses [Y/(N)]     : ");
		        	fflush(stdout); 

				if (fgets(buffer, sizeof(buffer), stdin) == NULL) exit(0);
				NO_NL(buffer);

				if (toupper(*buffer) == 'Y') 
					outresp_flag = TRUE;
				else outresp_flag = FALSE;
			}
			
			if (output_pipe) 
				outputfile = popen("more", "w");

			return;

		}

		}		/* if not read_summary_flag */

		if (!comments_flag)
		{
			printf("Output Format [(1=SAC), 2=AH, 3=CSS, 4=mini seed, 5=seed, 6=SAC ASCII, 7=SEGY, 8=Simple ASCII(SLIST), 9=Simple ASCII(TSPAIR)] : ");

	        	fflush(stdout); 

			if (fgets(buffer, sizeof(buffer), stdin) == NULL) exit(0);
			NO_NL(buffer);
	
			i = sscanf(buffer,"%d",&output_format);

			if (i == 0) 
				output_format = 1;	/* default to sac */

			/* decrement user input to start at zero */
			output_format--;

			if ((output_format < 0) || 
				(output_format > 8)) 
					output_format = 0;

			if (output_format < 2) {
				char ch = 0;

				printf("Output file names include endtime? [Y/(N)]");
	        		fflush(stdout); 

               			ch = getchar(); 

				ch = toupper(ch);
				
				if (ch == 'Y')
					output_endtime = TRUE;

				if (ch != '\n')
					while((ch = getchar() != '\n') && ch != EOF);
			}

			if (output_format == 3)
				mini_flag = TRUE;	/* set for mini seed */

			if (output_format == 4)
			{

				/* removes files perhaps left behind */
				clean_up_output_seed();	

            			Seed_flag = TRUE;   /* set for new seed */

			}

			if (from_weed && (WeedVersion < 2.0))
			{
				/* bale here for earlier versions of weed */
				if (output_pipe) 
					outputfile = popen("more", "w");
				return;
			}	

			if (output_format == 0)	
				/* SAC - ask for poles and zeroes output */
			{
				char ch = 0;

				printf("Output poles & zeroes ? [Y/(N)]");
	        		fflush(stdout); 

               			ch = getchar(); 

				ch = toupper(ch);
				
				if (ch == 'Y')
					Output_PnZs = TRUE;

				if (ch != '\n')
					while((ch = getchar() != '\n') && ch != EOF);

			}

			if (mini_flag || Seed_flag)
			{
				char ch = 0, xch;

				printf("Strip out records with zero sample count? [(Y)/N]");
                                fflush(stdout);

                                while (xch != '\n')
                                {
                                        xch = getchar();

                                        if (xch != '\n')
                                                ch = xch;
                                }

                                ch = toupper(ch);

                                if (ch == 'Y' || ch == 0)
                                        strip_flag = TRUE;

			}
	
			/* No reversal check for mini or seed output */
			if (output_format != 3 && output_format != 4)
			{
				printf("Check Reversal [(0=No), 1=Dip.Azimuth, 2=Gain, 3=Both]: ");
	        		fflush(stdout); 
	
				if (fgets(buffer, sizeof(buffer), stdin) == NULL) exit(0);
				NO_NL(buffer);

				i = sscanf(buffer,"%d",&check_reverse);
				if (i = 0) check_reverse = 0;
				if ((check_reverse < 0) || 
					(check_reverse > 3)) 
						check_reverse = 0;

			}

			/* ask for Quality option */
			printf("Select Data Type [(E=Everything), D=Data of Undetermined State, M=Merged data, R=Raw waveform Data, Q=QC'd data] :");

			if (fgets(buffer, sizeof(buffer), stdin) == NULL) exit(0);
			NO_NL(buffer);

			*buffer = toupper(*buffer);

			if (strlen(buffer) == 0)
			{
				q_flag = 'E';
			}
		

			switch (*buffer)	
			{
				case('D') : q_flag = 'D';
						break;
				case('R') :
					    q_flag = 'R';
						break;	
				case('Q') :
				  	    q_flag = 'Q';
						break;

				case('M') :
				  	    q_flag = 'M';
						break;

				default :
					    q_flag = 'E';
						break;
			}	/* switch */

		}		/* if not comments_flag */

		if (!read_summary_flag)
		{
			printf("Start Time(s) YYYY,DDD,HH:MM:SS.FFFF : ");
			fflush(stdout); 

			if (fgets(buffer, sizeof(buffer), stdin) == NULL) exit (0);

			NO_NL(buffer);

			buffer[strlen(buffer)+1] = '\0';

			p = malloc (strlen(buffer)+1);
			strcpy(p,buffer);
			i = 0;
			p2 = strtok(p," ");
			while(p2 != NULL)
			{
				i++;
				p2 = strtok(NULL," ");
			}

			start_time_count = 0;
			if (i == 0)
			{
				/* make a really small starting time */
				start_time_point = (struct time *) malloc(sizeof(struct time));

				start_time_point[0].year = 1900;

				start_time_count = 1;

			}
			else
			{
				start_time_point = (struct time *) malloc(sizeof(struct time)*i);
				p2 = strtok(buffer," ");
				while(p2 != NULL)
				{
					strcpy(p,p2);
					timecvt(&start_time_point[start_time_count], p);
					start_time_count++;
				
					/* this is needed because 
					 * timecvt calls strtok() */

					p2 += (strlen(p2)+1);
					p2 = strtok(p2," ");
				}
			}

			free (p);

			printf("End Time(s)   YYYY,DDD,HH:MM:SS.FFFF : ");
	        	fflush(stdout); 

			if (fgets(buffer, sizeof(buffer), stdin) == NULL) exit(0);

			NO_NL(buffer);

			buffer[strlen(buffer)+1] = '\0';
			p = malloc (strlen(buffer)+1);
			strcpy(p,buffer);
			i = 0;
			p2 = strtok(p," ");

			while(p2 != NULL)
			{
				i++;
				p2 = strtok(NULL," ");
			}

			end_time_count = 0;

			if (i == 0)
			{
				end_time_point = (struct time *)malloc(sizeof(struct time));


				/* I should be retired by then */
				end_time_point[0].year = 9999;
				end_time_count = 1;

			}
			else
			{
				end_time_point = (struct time *) malloc(sizeof(struct time)*i);
				p2 = strtok(buffer," ");
				while(p2 != NULL)
				{
					strcpy(p,p2);
					timecvt(&end_time_point[end_time_count], p);
					end_time_count++;

					/* this is needed because 
					 * timecvt calls strtok() */

					p2 += (strlen(p2)+1);
					p2 = strtok(p2," ");
				}
			}

			free (p);

			if (((start_time_point != NULL) || 
				(end_time_point != NULL))   &&
					(output_format == 4))
			{
				/* warn user that time series boundaries 
				 * are rounded up and * down to the 
				 * nearest LRECL
		 	 	 */
		
				printf("Warning - output time series' will be will be rounded up and down to the nearest logical record boundary in the output seed volume.\n");

			}

		}


	}

	if (data_flag)
	{

		printf("Sample Buffer Length [20000000]: ");
        	fflush(stdout); 

		if (fgets(buffer, sizeof(buffer), stdin) == NULL) exit(0);
		NO_NL(buffer);

		i = sscanf(buffer,"%d",&seis_buffer_length);
		if (i == 0) seis_buffer_length = 20000000;

		if (!outresp_flag)
		{
			printf("Extract Responses [Y/(N)]     : ");
		        fflush(stdout); 

			if (fgets(buffer, sizeof(buffer), stdin) == NULL) exit(0);
			NO_NL(buffer);

			if (toupper(*buffer) == 'Y') outresp_flag = TRUE;
			else outresp_flag = FALSE;
		}

	}

	if (output_pipe) 
		outputfile = popen("more", "w");

}

/*===========================================================================*/
/* SEED reader     |                 usage                 |    subprocedure */
/*===========================================================================*/
/* show usage of program */

void usage ()
{
	printf("Usage:  rdseed -f inputfile -{a | d [list] | l | s | t}\n");
	printf("\twhere inputfile = name of input file or device;\n");
	printf("\t a = retrieve abbreviation dictionaries;\n");
	printf("\t b = set the seismogram input buffer size (bytes);\n");
	printf("\t c = retrieve volume table of contents;\n");
	printf("\t C = retrieve the comments where arg is either STN or CHN;\n");

	printf("\t d = read data from tape;\n");
	printf("\t e = extract event/station data;\n");
	printf("\t E = output filename includes endtime;\n");
	printf("\t f = input filename;\n");
	printf("\t g = alternate response file(s);\n");
	printf("\t h = help (this list);\n");
	printf("\t i = ignore location codes;\n");
	printf("\t k = strip records from output which have a zero sample count. (miniseed and seed only);\n");
	
	printf("\t l = list contents of each block in volume;\n");
	printf("\t o = specify output format (1=SAC, 2=AH, 3=CSS, 4=MINISEED, 5=SEED, 6=SAC ASCII, 7=SEGY, 8=Simple ASCII(SLIST), 9=Simple ASCII(TSPAIR) [default=1]);\n");
	printf("\t p = output poles and zeros;\n");

	printf("\t q = specify the output directory;\n");
	printf("\t Q = specify data type (E=Everything, D=Data of undetermined state, M=Merged Data, R=Raw waveform data, Q=QC'd data);\n");
	printf("\t R = print response data;\n");
	printf("\t s = retrieve all station header information;\n");
	printf("\t S = retrieve station summary information;\n");
	printf("\t t = show start/stop times, start blocks of events;\n");
	printf("\t u = usage (this list);\n");
	printf("\t v = select volume number;\n");
	printf("\t x = use summary file;\n");
	printf("\t z = check for reversals;\n");

}

/*=======================================================================*/
/* SEED reader     |            chk_station            |    main program */
/*=======================================================================*/
/* see if station is in select list */
int chk_station(station)
char *station;
{
	int i, j; char buffer[200], *p;

	if (station_count == 0) return(TRUE);

	for (i=0;i<station_count;i++)
	{
		rdseed_strupr(station_point[i]);
		if (wstrcmp(station,station_point[i],5) == 0) return(TRUE);

/* look through alias strings */
		for (j=0;j<alias_count;j++)
		{
			strcpy(buffer,alias_point[j]);

			p = strtok(buffer," ,");
			if (p != NULL)
			{

				if (wstrcmp(p,station_point[i],5) == 0)
				{
					p = strtok(NULL," ,");
					while (p != NULL)
					{
						if (wstrcmp(station,p,5) == 0) 
							return(TRUE);

						p = strtok(NULL," ,");

					}
				}
			}
		}
	}
	return(FALSE);
}

/*=======================================================================*/
/* SEED reader     |            chk_channel            |    main program */
/*=======================================================================*/
/* see if channel is in select list */
int chk_channel(channel)
char *channel;
{
	int i;

	if (channel_count == 0) 
		return(TRUE);

	for (i=0;i<channel_count;i++)
	{
		rdseed_strupr(channel_point[i]);

		if (wstrcmp(channel, channel_point[i], 3) == 0) 
			return(TRUE);
 	}		

	return(FALSE);
}
/*=======================================================================*/
/* SEED reader     |            chk_location           |    main program */
/*=======================================================================*/
/* see if network is in select list */
int chk_location(location)
char *location;
{
        int i;

        char loc[3];

        if (location_count == 0)
                return TRUE;

        if (ignore_loc_codes)
                return TRUE;

        strncpy(loc, location, 2);      /* sizeof location code */
        loc[2] = 0;
 
        if (isalnum(loc[0]) && (loc[1] == ' '))
                loc[1] = 0;
 
        if ((loc[0] == ' ') && (isalnum(loc[1])))
        {
                /* i.e: make ' G' look like 'G', if there */
                loc[0] = loc[1];
                loc[1] = 0;
        }
	if (strlen(loc) == 0)
		strcpy(loc, "  ");
 
        for (i = 0; i < location_count; i++)
        {

                if (wstrcmp(loc, location_point[i], strlen(loc)) == 0)
                        return TRUE;

		/* if loc = "01" and they entered "1" - report ok */
		/* filter out if any are ull strings, as str_isnum
		 * apparently returns true if null string enteres
		 */
		if ((strlen(loc) != 0) && (strlen(location_point[i]) != 0))
		if (str_isnum(loc) && str_isnum(location_point[i]))
		if (strlen(loc) != strlen(location_point[i]))
		{
			if (atoi(loc) == atoi(location_point[i]))
				return TRUE;
		}	
        }
 
        return(FALSE);
 
}

/*=======================================================================*/
/* SEED reader     |            chk_network            |    main program */
/*=======================================================================*/
/* see if network is in select list */
int chk_network(network)
char *network;
{
    	int i;

	char netwrk[3];
	 
    	if (network_count == 0)
		return TRUE;

	if (ignore_net_codes)
		return TRUE;

	strncpy(netwrk, network, 2); 	/* sizeof network code */
	netwrk[2] = 0;

	if (netwrk[1] == ' ')
		netwrk[1] = 0;

	if (netwrk[0] == ' ')
	{
		/* make ' G' look like 'G', if there */
		netwrk[0] = netwrk[1];
		netwrk[1] = 0;
	}

    	for (i = 0; i < network_count; i++)
	{
		if (strncasecmp(network_point[i], 
					netwrk, strlen(network_point[i])) == 0) 
			return TRUE;

	}

    	return(FALSE);

}      
/*=======================================================================*/
/* SEED reader     |              wstrcmp              |    main program */
/*=======================================================================*/
/* wild card string compare */
int wstrcmp(string1, string2, length)
char *string1, *string2;
int length;
{
	int i, mode;
	char c1, c2;

	i = 0;
	while (TRUE)
	{
		mode = 0;
		if (*string2 == '.') mode = 1;
		if (*string2 == '?') mode = 1;
		if (*string2 == '*') mode = 2;
		switch (mode)
			{
			case 0:
				if (*string2 == '\0' && *string1 == '\0') return(0);
				if (*string1 == ' '  && *string2 == '\0') return(0);

				if (*string1 != *string2) return(1); 

				break;
			case 1:
				if (*string1 == '\0') return(1);
				if (*string1 == ' ')  return(1);
				break;
			case 2:
				string2++;
				if (*string1 == '\0') return(0);
				if (*string1 == ' ')  return(0);
				if (*string2 == '\0') return(0);
				string1++;

				while (*string1 != *string2) 
					{
					if (*string1 == '\0') return(1);
					if (*string1 == ' ')  return(1);
					string1++;
					i++; if (i >= length) return(1);
					}
				break;
			}

		/* strings are the same to this point */

		string1++; string2++;
		i++; if (i >= length) return(0);
	}
}

/*=======================================================================*/
/* SEED reader     |               rdseed_strupr              |    main program */
/*=======================================================================*/
/* convert string to upper case */
rdseed_strupr(string)
char *string;
{
	while (*string != '\0')
		{
		if (islower(*string)) *string = toupper(*string);
		string++;
		}
}

/*=======================================================================*/
/* SEED reader     |           get_environment         |    main program */
/*=======================================================================*/
/* get rdseed environment variables */
get_environment()
{
	FILE *fp;
	char buffer[200], *p, *p2;
	int i, j, ix;

	alias_count = 0;

	p = getenv("SEEDALIAS");
	if (p != NULL)
	{
		fp = fopen(p,"r");
		if (fp != NULL)
		{
			i = get_file_line(buffer, 199, fp);
			while (i)
			{
				p = malloc(i+1);

				ix = 0;

				while (buffer[ix]) 
				{
					p[ix++] = toupper(buffer[ix]);
				}

				p[ix] = '\0';

				alias_point[alias_count] = p;

				alias_count++;
				i = get_file_line(buffer, 199, fp);
			}
		}
	}

	adj_tolerance = 1.0;
	p = getenv("SEEDTOLERANCE");
	if (p != NULL)
	{
		int i;

		printf("\n**** Using SEED Tolerance: %s ****\n", p);

		if (0 == sscanf(p, "%lf", &adj_tolerance)) adj_tolerance = 1.0;
	}

	alt_headerfile = NULL;
	p = getenv("ALT_RESPONSE_FILE");
	if (p != NULL)
	{
		printf("\n**** Using alternate response file(s): %s ****\n", p);

		init_alt_files(p);

	}
	p = getenv("STATION_VOLUME");
	if (p != NULL)
		station_volume = TRUE;
	else 
		station_volume = FALSE;

}

/*=======================================================================*/
/* SEED reader     |           init_alt_files          |    main program */
/*=======================================================================*/
/* Initialize alternate response file usage */
void init_alt_files(filenames)
char *filenames;
{
	char *fns;

	fns = strdup(filenames);

	if (open_alt_files(fns) == -1)
		alt_headerfile = NULL;
	else
	{

		alt_headerfile = get_alt_file();

		if (alt_headerfile == NULL)
		{
			fprintf(stdout, "WARNING - unable to open alternate header file\n");
		}
	}

	free(fns);
}

/*=======================================================================*/
/* SEED reader     |           get_file_line           |    main program */
/*=======================================================================*/
/* read a \n terminated line from a file */
int get_file_line(buffer, length, fp)
char *buffer;
int length;
FILE *fp;
{
	char *p; int i;

	i = 0;
	p = buffer;
	while (fread(p,1,1,fp) != 0)
		{
		// if (*p == ' ') continue;
		if (*p == '\n') break;
		p++;
		i++; if (i == length) break;
		}
	*p = '\0';
	return(strlen(buffer));
}

/*=======================================================================*/
/* SEED reader     |         check_block_mode          |    main program */
/*=======================================================================*/
/* can file be read in block mode? */
check_block_mode(filename)
char *filename;
{

	struct stat buf;

	stat(filename, &buf);

	if (S_ISCHR(buf.st_mode)) 
		return(TAPE_DEVICE);
	else 
		return(DISK_DEVICE);
}

/*=======================================================================*/
/* SEED reader     |           strip_blanks            |    main program */
/*=======================================================================*/
/* strip blanks from string */
strip_blanks(input, output)
char *input, *output;
{
	int j; char *p, buf[100];

	p = input; j = 0;
	while (*p != '\0')
	{
		if (*p != ' ') {buf[j] = *p; j++; }
		p += 1;
	}
	buf[j] = '\0';
	strcpy(output,buf);
}

/* ----------------------------------------------------------------------- */
/* - used by main to determine what seed version we are dealing with.
 * - that way, rdseed will know weather or not to ask for network code.
 */

int get_type10(fptr)
FILE *fptr;

{
	int num_bytes;
	struct input_data_hdr *input_data_hdr;          /* fixed data header */
	char *ptr;

	/*num_bytes = read(fileno(fptr), precord, PRECL);*/
        num_bytes = fread(precord, 1, PRECL, fptr);

   	if (num_bytes == 0)
	{
		fprintf(stdout, "Unable to \"read\" the seed volume!\n");

		perror("rdseed");

		rewind(fptr);

		return 0;

	}

	 precord_ptr = (char *)precord;
 
	/* point to beginning of this logical record */
   	lrecord_ptr = precord_ptr;
   	input_data_hdr = (struct input_data_hdr *) (lrecord_ptr + 8);
 
	/* extract a logical record from the physical record */
	/* fills up input struct */

	read_logical_record (lrecord_ptr);

	if (input.type != 'V')
	{
		fprintf(stderr, "Error! get_type10: Unable to read volume control header label. No \"V\" found!\n");

		rewind(fptr);
 
        return 0;

	}

	ptr = &input.data[7];	
	type10.version = parse_double(&ptr, 4);

	rewind(fptr);

	return 1;
 
		
}

/* -------------------------------------------------------------------- */
int scan_stn_net()

{
	char wrkstr[10];
	char net[3];
	int blen;

	sprintf(wrkstr, "%5.5s", &input.data[7]);                                                          
	if (!chk_station(wrkstr))
		return FALSE;

	if (type10.version >= 2.3)
	{
		/* look at the network code, always the last two
		 * chars of the record.
		 */

		/* peal off the block length */
		sprintf(wrkstr, "%4.4s", &input.data[3]);
		blen = atoi(wrkstr);

		sprintf(net, "%2.2s", &input.data[blen - 2]);

		if (!chk_network(net))
			return FALSE;

		return TRUE;

	}
	else
		return TRUE;
 
}

/* -------------------------------------------------------------------- */
int dir_exists(dname)
char *dname;

{
	int status;

	struct stat s;

	status = stat(dname, &s);

	if (status == -1)
	{	
		if (errno == ENOENT)
			return 0;	
		
		perror("rdseed; dir_exists()");

	}


	return(S_ISDIR(s.st_mode));

}

/* ------------------------------------------------------------------ */
