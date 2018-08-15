/*===========================================================================*/
/* SEED reader     |              output_simple            |    subprocedure */
/*===========================================================================*/
/*
	Name:	output_simple
	Purpose:build and write a seismogram file from the SEED header tables
		and data files; serve as an example for users who wish to write
		their own "output_data" procedure.  This procedure writes SAC
		files.
	Usage:	void output_simple();

	structure element                     description
	====================================  ==========================
	struct input_data_hdr                 fixed data header
	{
		char station[5];                   station name
		char location[2];                  station location
		char channel[3];                   channel name
		char unused[2];                    not used
		struct input_time time;            time (see below)
		unsigned short int nsamples;       number samples
		short int sample_rate;             sample rate factor
		short int sample_rate_multiplier;  sample rate multiplier
		char activity_flags;               activity flags
		char io_flags;                     i/o flags
		char data_quality_flags;           data quality flags
		char number_blockettes;            # blockettes which follow
		long int number_time_corrections;  # .0001s time corrections
		unsigned short int bod;            beginning of data
		unsigned short int bofb;           beginning 1st blkt
	};
	struct input_time                     time from input stream
	{
		unsigned short int year;           year
		unsigned short int day;            Julian day
		char hour;                         hour
		char minute;                       minute
		char second;                       second
		char unused;
		unsigned short int fracsec;        10000ths of second
	};

	These header data are available to the programmer in the usual
	ways.  In the case shown here, a SAC header structure was
	defined in "sac.h" and various elements of the SAC header were
	filled in with data available from the SEED fixed-length data
	header.  See the section of code entitled "Build the SAC header"
	for examples.

	Other data are available from the tables pointed to by
	"current_station" and "current_channel".  These data are 
	described in Halbert et al. under the heading "Station Control
	Header Format".  The table structure is described in the leading
	comments to the main program ("rdseed.c") and in the 
	programmer's technical reference manual; the pointers
	"current_station" and "current_channel" are set by the procedure
	"process_data".  See the section of code within "Build the SAC 
	header" commented with "give values available from SEED tables 
	to SAC optional variables" for examples of the use of these
	station- and channel-specific items.

	Numeric data formats included here are correct for SAC.  The
	user may have to change them for output of data to be used by
	other seismic processing programs.
			
	Problems:	none known
	References:	Halbert, S. E., R. Buland, and C. R. Hutt (1988).  Standard for
			the Exchange of Earthquake Data (SEED), Version V2.0,
			February 25, 1988.  United States Geological Survey,
			Albuquerque Seismological Laboratory, Building 10002,
			Kirtland Air Force Base East, Albuquerque, New Mexico
			87115.  82 pp.
		O'Neill, D. (1987).  IRIS Interim Data Distribution Format
		(SAC ASCII), Version 1.0 (12 November 1987).  Incorporated
		Research Institutions for Seismology, 1616 North Fort Myer
		Drive, Suite 1440, Arlington, Virginia 22209.  11 pp.
		Tull, J. (1987).  SAC User's Manual, Version 10.2, October 7,
		1987.  Lawrence Livermore National Laboratory, L-205,
		Livermore, California 94550.  ??? pp.
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
		11/21/88  Dennis O'Neill  Production release 1.0
		09/19/89  Dennis O'Neill  corrected output string length
		05/01/92  Allen Nance  corrected CMPINC Header variable for SEED Dip definition
		sach.cmpinc = current_channel->dip + 90.0;
		01/19/01  Doug Neuhauser - reintegrated SAC ascii and binary output into
		this single routine.  Made ascii output more efficient.  Reindented code.
*/

#include <stdio.h>
#include <time.h>
#include <sys/param.h>

#include "rdseed.h"	/* SEED tables and structures */
#include "sac.h"	/* SAC format structures */


extern int EventDirFlag;
extern int output_endtime;
extern int read_summary_flag;
extern char micro_sec;

extern struct type48 *find_type_48_stage(int, struct response *);

char *summary_file_get_event();
char *get_event_dir_name();
char *YMDtoYDDD();


struct type53 *find_type_53();
struct type43 *find_type_43();
struct type34 *find_type_34();


float summary_file_get_version();
 
char strlst();

time_t compute_time(int yy, int ddd, int h, int m, int s);

int sac_reverse_seismic_data();
extern char prev_quality;

void output_simple(data_hdr, offset, simple_flag)
struct data_hdr *data_hdr;
int offset;
int simple_flag;

{

	FILE *outfile;				/* output file pointer */
	char outfile_name[100], channel[10];	/* output file name */
	int i, ix, j, k;			/* counter */
	char character[8+1];	/* for character transfer */

	int reverse_flag;	/* Data reversal flag */
	double dip, azimuth;	/* temp holding for these variables */
	struct type48 *type_48;

	char wrkstr[1024], event_time[64];

	struct time ev_time;

	struct time data_hdr_endtime;
// 	struct time trace_time;

	time_t t;

	struct tm *tm_ptr;
	char date_time[100];

	struct time newtime;

	struct type34 *type_34;
	double duration;

	float *seismic_dataFP;

	float weed_version = 3.0;

	struct response *response; int flag;

	char orig_dir[MAXPATHLEN];
	char type_label[30];

	getcwd(orig_dir, MAXPATHLEN);

	chdir(output_dir);

	if (simple_flag == 0)
		strcpy(type_label, "SLIST");
	else
		strcpy(type_label, "TSPAIR");

	for (k=0;k<data_hdr->num_mux_chan;k++)
	{
		if (current_channel == NULL)
		{
			fprintf(stderr, "\tERROR - Unexpected End of Multiplexed Channel List\n");
			chdir(orig_dir);
			return;
		}

		reverse_flag = 0;
		for (i=0;i<10;i++) 
			channel[i] = 0;

		strcpy(channel,current_channel->channel);

        	if (data_hdr->num_mux_chan > 1) 
		{
                	int i;

                	for (i=0; i<3; i++) {
                        	if (channel[i] == ' ' || 
				    channel[i] == '\0') 
					break;
			}

                	if (i > 2) 
				i = 2;

                	channel[i] = data_hdr->mux_chan_name[k];
        	}

		dip = current_channel->dip;
		azimuth = current_channel->azimuth;

		reverse_flag = check_for_reversal(dip, azimuth, channel[2]);


		flag = FALSE;
		for (response = current_channel->response_head; 
		     response != NULL; response = response->next)
		{
			reverse_flag &= 1;

			if (response->type == 'S') { 
				if ((response->ptr.type58)->stage == 0)
				{
					if (((response->ptr.type58)->sensitivity < 0.0) && 
					    (check_reverse & 2))
						reverse_flag |= 2;

					flag = TRUE;
				}
			} else if (response->type == 'R') {
				type_48 = find_type_48_stage(0, response);
				if (type_48 != NULL) {
					if ((type_48->sensitivity < 0.0) && 
					    (check_reverse & 2))
						reverse_flag |= 2;

					flag = TRUE;
				}
			}
		}
		if (!flag && (check_reverse & 2))
			fprintf(stderr, "Warning - Stage Zero Not Found on Gain Reversal check\n");
 
		if (check_reverse & 1)
		{
			if (reverse_flag & 1)
				fprintf (stderr, "Warning... Azimuth/Dip Reversal found %s.%s, Data will be automatically inverted\n           Response Information will not be modified\n",
					 current_station->station, channel);
		}
		else
		{
			if (reverse_flag & 1)
				fprintf (stderr, "Warning... Azimuth/Dip Reversal found %s.%s, Data inversion was not selected\n",
					 current_station->station, channel);
			reverse_flag &= 2;
		}

		if (check_reverse & 2)
		{
			if (reverse_flag & 2)
				fprintf (stderr, "Warning... Gain Reversal found %s.%s, Data will be automatically inverted\n           Response Information will not be modified\n",
					 current_station->station, channel);
		}
		else
		{
			if (reverse_flag & 2)
				fprintf (stderr, "Warning... Gain Reversal found %s.%s, Data inversion was not selected\n",
					 current_station->station, channel);
			reverse_flag &= 1;
		}

		if (reverse_flag == 3)
		{
			reverse_flag = 0;		/* Double inversion */
			fprintf (stderr, "Warning... Gain and Dip/Azimuth Reversal found %s.%s, Data was not inverted\n",
				 current_station->station, channel);
		}

		if (reverse_flag & 1)
			switch (channel[2])
			{
			case 'Z':
				dip = -90.0;
				break;
			case 'N':
				azimuth = 0.0;
				break;
			case 'E':
				azimuth = 90.0;
				break;
			case 'A':
				dip = -60.0;
				azimuth = 0.0;
				break;
			case 'B':
				dip = 60.0;
				azimuth = 120.0;
				break;
			case 'C':
				dip = -60.0;
				azimuth = 240.0;
				break;
			default:
				break; 
			}

/*                 +=======================================+                 */
/*=================|          Build the SAC header         |=================*/
/*                 +=======================================+                 */


		if (data_hdr->nsamples == 0)
		{
			fprintf(stderr,"\tWARNING (output_simple):  Output Data Contains No Samples\n");
			chdir(orig_dir);
			return;
		}

        	type_34 = find_type_34(type34_head,
				current_channel->signal_units_code);

		if (read_summary_flag)
		{
			char **parts;
			char **pieces;
			int n, nn;

			char wrkstr[255];

			float weed_version;

			weed_version = summary_file_get_version();

			n = split(summary_file_get_event(), &parts, ',');

			strcpy(event_time, "");

			// old weed (version < 3.0 has seismic code and region added

			/* save ev time for later computation */
			strcpy(event_time, parts[1]);
	
			/* convert from YY/MM/DD... to time structure */
			timecvt(&ev_time, event_time);
	
			fuse(&parts, n);
					
		}
		else
			/* event lat/lon from hypo B071 */
			if (type71_head != NULL)
			{
				timecvt(&ev_time, type71_head->origin_time);
			}
		
/*                 +=======================================+                 */
/*=================|  build name for and open output file  |=================*/
/*                 +=======================================+                 */

		/* 1988.023.15.34.08.2800.ANMO.SPZ is the filename for a seismogram 
		 *	from year 1988, Julian day 23, 15:34:08.2800 UT, recorded 
		 *	at station ANMO from component SPZ. 
		 */

		/* if requested from WEED, place into subdirectory */
		if (EventDirFlag)
		{
			char dirname[MAXPATHLEN];

			strcpy(dirname, get_event_dir_name());

			if (!dir_exists(dirname))
			{
				if (mkdir(dirname, 0777) == -1)
				{
					fprintf(stderr, "Unable to create event directory. Using current directory!\n");
					perror("output_sac");
					strcpy(dirname, "./");
				}
 
			}
		
			chdir(dirname);
 
		}

		if (output_endtime == TRUE) {
			duration  =
			((double)(data_hdr->nsamples)*10000)/data_hdr->sample_rate;
			data_hdr_endtime = timeadd_double(data_hdr->time, duration);
			sprintf (outfile_name,
			 "%04d.%03d.%02d.%02d.%02d.%04d_%04d.%03d.%02d.%02d.%02d.%04d.%s.%s.%s.%s.%c.ascii",
			 data_hdr->time.year,
			 data_hdr->time.day,
			 data_hdr->time.hour,
			 data_hdr->time.minute,
			 data_hdr->time.second,
			 data_hdr->time.fracsec,
			 data_hdr_endtime.year,
			 data_hdr_endtime.day,
			 data_hdr_endtime.hour,
			 data_hdr_endtime.minute,
			 data_hdr_endtime.second,
			 data_hdr_endtime.fracsec,
			 type10.version >= 2.3 ? data_hdr->network : "",
			 data_hdr->station,
			 data_hdr->location,
			 channel,
			 // input.type,
			 prev_quality);
		} else {

			sprintf (outfile_name, 
			 	"%04d.%03d.%02d.%02d.%02d.%04d_%s.%s.%s.%s.%c.ascii",
			 	data_hdr->time.year,
				data_hdr->time.day,
				data_hdr->time.hour,
				data_hdr->time.minute,
			 	data_hdr->time.second,
			 	data_hdr->time.fracsec,
			 	type10.version >= 2.3 ? data_hdr->network : "",
			 	data_hdr->station,
				data_hdr->location,
			 	channel,
			 	prev_quality);
		}

		if ((outfile = fopen (outfile_name, "w+")) == NULL)
		{
			fprintf (stderr, "\tWARNING (output_simple):  ");

			fprintf (stderr, "Output file %s is not available for writing.\n",
				 outfile_name);
			fprintf (stderr, "\tExecution continuing.\n");

			chdir(orig_dir);
		
			return;
		}

/*                 +=======================================+                 */
/*=================|        write a simple ASCII file         |=================*/
/*                 +=======================================+                 */

		/* describe the file being written */
		printf ("Writing %s.%s.%s.%s, %5d samples (ASCII),",
			type10.version >= 2.3 ? data_hdr->network : "",
			data_hdr->station,
			data_hdr->location,
			channel,
			data_hdr->nsamples);

		printf (" starting %04d,%03d %02d:%02d:%02d.%04d UT\n",
				data_hdr->time.year,
				data_hdr->time.day,
				data_hdr->time.hour,
				data_hdr->time.minute,
				data_hdr->time.second,
				data_hdr->time.fracsec);

		// use gmtime to cnvt DDD to MM/dd

		t = compute_time(data_hdr->time.year, data_hdr->time.day, data_hdr->time.hour,
				data_hdr->time.minute, data_hdr->time.second);

		tm_ptr= gmtime(&t);

		sprintf(date_time, "%d-%02d-%02dT%02d:%02d:%02d.%04d00", 
				1900 + tm_ptr->tm_year, tm_ptr->tm_mon + 1, tm_ptr->tm_mday, 
				tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec,
				 data_hdr->time.fracsec);

		fprintf(outfile, "TIMESERIES %s_%s_%s_%3s_%c, %d samples, %4.1f sps, %s, %s, FLOAT, Counts\n",
					data_hdr->network,
					data_hdr->station,
                               		data_hdr->location,
                               		channel,
					prev_quality,
					data_hdr->nsamples,
					data_hdr->sample_rate,
					date_time,
					type_label);

		j = k*(seis_buffer_length/data_hdr->num_mux_chan);

		/* reverse the polarity of the data if necessary */
		if (reverse_flag) {
			sac_reverse_seismic_data ( offset, j, data_hdr->nsamples, outfile_name);
		}


		if (simple_flag == 0)		// Timeseries SLIST type
		{
			/* write the data */
			for (i = 0; i < data_hdr->nsamples; i++)
			{
				fprintf (outfile, "%15.7g\n", 
							 seismic_data[offset+i+j]);
			}
		}
		else
		{
			duration = 10000.0/data_hdr->sample_rate;

			newtime = data_hdr->time;
	
			for (i = 0; i < data_hdr->nsamples; i++)
			{

				/* we do this do use the month/day format instead of the
				 * day of year which is what newtime is
				 */

				t = compute_time(newtime.year, newtime.day, newtime.hour,
					newtime.minute, newtime.second);

				tm_ptr= gmtime(&t);

				// sprintf(date_time, "%d-%02d-%02dT%02d:%02d:%02d.%s00", 
				//		1900 + tm_ptr->tm_year, tm_ptr->tm_mon, tm_ptr->tm_mday, 
				//		tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec,
				//		fracs);

				fprintf(outfile, "%d-%02d-%02dT%02d:%02d:%02d.%04d00  %15.7g\n", 
						newtime.year, tm_ptr->tm_mon + 1, tm_ptr->tm_mday, 
						newtime.hour, newtime.minute, newtime.second,
						newtime.fracsec, seismic_data[offset+i+j]);

				// fprintf(outfile, "%s %15.7g\n", date_time, seismic_data[offset+i+j]);
		
				newtime = timeadd_double(newtime, duration);

			}

		}

		fclose(outfile);

		current_channel = current_channel->next;

	}		/* the BIG k loop If multiplexed */

	chdir(orig_dir);

	return;

}

/* ------------------------------------------------------------------------- */

