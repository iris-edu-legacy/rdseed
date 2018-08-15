
/*===========================================================================*/
/* SEED reader     |              output_segy               |    subprocedure */
/*===========================================================================*/
/*
	Name:	output_segy
	Purpose:build and write a seismogram file from the SEED header tables
		and data files; serve as an example for users who wish to write
		their own "output_data" procedure.  This procedure writes segy 
		files.
	Usage:	void output_segy();

	Input:	none (gets its data from globally-available tables and files)
	Output:	none (writes a seismogram file; seismogram files are named by
		beginning time, station, and component; for example,
		1988.023.15.34.08.2800.ANMO.SPZ is the filename for a
		seismogram from year 1988, Julian day 23, 15:34:08.2800 UT,
		recorded at station ANMO from component SPZ.  Writes message
		to stderr describing the seismogram file being written.
	Externals:data_hdr - a table containing information about this seismogram
		current_station - a pointer to the SEED header tables for the
		station at which this seismogram was recorded
		current_channel - a pointer to the SEED header tables for the
		channel of the station at which this seismogram was recorded
	Warnings:unable to open seismogram file for writing
		failure to properly write the seismic data
	Errors:	none
	Called by:time_span_out
	Calls to:none
	Algorithm:build the seismogram file header by copying values from the
			structure "data_hdr" and the "current_station" and
			"current_channel" tables.  Write out the header and the data.
			This procedure may write either ASCII or binary data, depending
			upon the values of the variables "ascii" and "binary"; as
			implemented here, the programmer sets "ascii" to TRUE or FALSE
			and "binary" is automatically set to the negation of "ascii".
	Notes:	"output_data" is the name of the process called by
		"process_data" to write a seismogram file.  In the case shown
		here, the data are written in SAC format; the SAC format was
		chosen because many users have SAC available and will be able to
		use the output files directly.  If the user desires a different
		output format, this code and these notes should contain enough
		information for that user to construct his or her own 
		"output_data" procedure.

		The structure "data_hdr" contains the following information from
		the SEED fixed-length data header:

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
*/

#include <stdio.h>
#include <sys/param.h>

#include "rdseed.h"	/* SEED tables and structures */
#include "segy.h"	/* SEGY format structures */


extern int EventDirFlag;

char *summary_file_get_event();
char *get_event_dir_name();


struct type53 *find_type_53();
struct type43 *find_type_43();
struct type34 *find_type_34();
extern struct type48 *find_type_48_stage(int, struct response *);

float get_overall_gain();
 
char strlst();

void output_segy(data_hdr, offset)
struct data_hdr *data_hdr;
int offset;
{
	FILE *outfile;				/* output file pointer */
	char outfile_name[100], channel[10];	/* output file name */
	struct segy segy_header;		/* SEGY header structure */
	int i, j,k;				/* counter */
	char character[8+1];		/* for character transfer */
	int ascii;			/* switch for ASCII output */
	int binary;			/* switch for binary output */
	int reverse_flag;		/* Data reversal flag */
	double dip, azimuth;		/* */
	int this_sample;
	struct type48 *type_48;
	double duration;
	struct time data_hdr_endtime;


	struct response *response; int flag;

	char orig_dir[MAXPATHLEN];

	getcwd(orig_dir, MAXPATHLEN);

	chdir(output_dir);

	if (data_hdr->nsamples == 0)
	{
		fprintf(stderr,"\tWARNING (output_segy):  Output Data Contains No Samples\n");
		chdir(orig_dir);
		return;
	}


	/* select ASCII or binary output (binary chosen here) */
	ascii = FALSE;		

	binary = !ascii;

	/* initialize SEGY Header to all nulls */
	memset(&segy_header, 0, sizeof(segy_header));

	segy_header.tr_seg_num = 1;
	segy_header.reel_tr_seg_num = 1;
	segy_header.orig_fld_num = 1;
	segy_header.tr_num_orig_rec = 1;
	segy_header.tr_id = 1;

	if (type71_head != NULL)
	{
		int error;

		float dist, az, baz;
		
		struct time t;

		segy_header.src_depth = type71_head->depth * 100; 


		segy_header.src_coordinate_x = type71_head->longitude * 10000;
		segy_header.src_coordinate_y = type71_head->latitude * 10000;

		error = delaz(type71_head->latitude,
				type71_head->longitude,
				current_channel->latitude,
				current_channel->longitude,
				&dist, &az, &baz);

		if (error)
                {
                        fprintf(stderr, "Error: output_segy() unable to compute event distance/azimuth and back azimuth for station %s; channel %s!\n",
                                        current_station->station,
                                        current_channel->channel);
 
                        dist = 0;
                }

		segy_header.dist_from_src = dist; 

		timecvt(&t, type71_head->origin_time);

		segy_header.comment.p_stuff.event_time_yr = t.year;
		segy_header.comment.p_stuff.event_time_day = t.day;
		segy_header.comment.p_stuff.event_time_hour = t.hour;
		segy_header.comment.p_stuff.event_time_min = t.minute;
		segy_header.comment.p_stuff.event_time_sec = t.second;
		segy_header.comment.p_stuff.event_time_ms = t.fracsec/10;

	}
	else
	{
		segy_header.src_surf_elev = 0; 
                segy_header.src_depth = 0;
		segy_header.dist_from_src = 0;
	}

	segy_header.rec_grp_elev = current_channel->elevation * 100;

        segy_header.elev_depth_scaler = -100;  /* divisor if neg */
	segy_header.coordinate_scaler = -10000; 
	
        segy_header.grp_coordinate_x = current_channel->longitude * 10000;
	segy_header.grp_coordinate_y = current_channel->latitude * 10000;
 
        segy_header.coordinate_units = 2;

	if (data_hdr->nsamples >= 32767)
	{
		/* flag for reading program to use field in passcal extension */
		segy_header.number_of_samples = 32767;
		segy_header.comment.p_stuff.num_samples = data_hdr->nsamples;
	}
	else
		segy_header.number_of_samples = data_hdr->nsamples;

	/* sample rate in micro secs */
	if ((1000000/data_hdr->sample_rate) > 32767)
	{
		/* flag for reading programs to use passcal extension */
        	segy_header.sample_interval = 1;
        	segy_header.comment.p_stuff.sample_rate = (1000000/data_hdr->sample_rate); 

	}
	else
                segy_header.sample_interval = 1000000/data_hdr->sample_rate; 

	segy_header.gain_type = 1;	/* floating point data */ 
        segy_header.inst_gain_const = 1;

	segy_header.year_data_recorded = data_hdr->time.year;
	segy_header.day_of_year = data_hdr->time.day;
	segy_header.hour = data_hdr->time.hour;
	segy_header.minute = data_hdr->time.minute;
	segy_header.second = data_hdr->time.second;
	segy_header.tmz = 2; /* set GMT */

	strcpy(segy_header.comment.p_stuff.stn, data_hdr->station);

	strcpy(segy_header.comment.p_stuff.chn, data_hdr->channel); 

	/* convert from .1 millsecs (seed time) to millsecs (passcal ) */
	segy_header.comment.p_stuff.start_tr_ms = data_hdr->time.fracsec/10;

	segy_header.comment.p_stuff.int_size_flag = 1;  /* 32 bit data */

	segy_header.comment.p_stuff.scale_factor = get_overall_gain();

	if (segy_header.comment.p_stuff.scale_factor == -1)
	{
		fprintf(stderr, "Warning: output_segy() - unable to compute the sample factor.\n");
		fprintf(stderr, "Sample factor is set to -1\n");
        
		fprintf(stderr, "For station: %s; channel: %s\n",
                                		current_station->station,
                                		current_channel->channel);

	}


	for (k=0;k<data_hdr->num_mux_chan;k++)
	{
		if (current_channel == NULL)
		{
			fprintf(stderr, "\tERROR - Unexpected End of Multiplexed Channel List\n");
			chdir(orig_dir);

			return;
		}

		/* find min and max values in trace */	
		segy_header.comment.p_stuff.max_samp = 0xF0000000;
		segy_header.comment.p_stuff.min_samp = 0x7FFFFFFF;

                j = k * (seis_buffer_length/data_hdr->num_mux_chan);

		for (i = 0; i < data_hdr->nsamples; i++)
		{
			if (seismic_data[offset+i+j] > segy_header.comment.p_stuff.max_samp)
				
                	segy_header.comment.p_stuff.max_samp = seismic_data[offset+i+j];

			if (seismic_data[offset+i+j] < segy_header.comment.p_stuff.min_samp) 
                                 
			segy_header.comment.p_stuff.min_samp = seismic_data[offset+i+j];
	
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


		reverse_flag = check_for_reversal(current_channel->dip,
						  current_channel->azimuth,
						  channel[2]);


		flag = FALSE;
        	for (response = current_channel->response_head;
                            response != NULL; response = response->next)
        	{
                	reverse_flag &= 1;
                
			if ((response->type != 'S') && response-> type != 'R')
				continue;

			if (response->type == 'S') {
               			if ((response->ptr.type58)->stage != 0)
					continue;

                        	if (((response->ptr.type58)->sensitivity < 0.0) &&
                                	(check_reverse & 2))
                                	reverse_flag |= 2;
 
                        	flag = TRUE;
			} else if (response->type == 'R') {
				type_48 = find_type_48_stage(0, response);
				if (type_48 == NULL) continue;

				if ((type_48->sensitivity < 0.0) && 
					    (check_reverse & 2))
						reverse_flag |= 2;

				flag = TRUE;
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
                	reverse_flag = 0; 	/* Double inversion */
                	fprintf (stderr, "Warning... Gain and Dip/Azimuth Reversal found %s.%s, Data was not inverted\n",
                        	current_station->station, channel);
        	}
 
        if (reverse_flag & 2) 
		segy_header.comment.p_stuff.scale_factor = -segy_header.comment.p_stuff.scale_factor;
		
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
                         "%04d.%03d.%02d.%02d.%02d.%04d_%04d.%03d.%02d.%02d.%02d.%04d.%s.%s.%s.%s.%c.SEGY",
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
                         input.type);
		} else {
			sprintf (outfile_name, 
					"%04d.%03d.%02d.%02d.%02d.%03d.%s.%s.%s.%s.%c.SEGY",
			data_hdr->time.year,
			data_hdr->time.day,
			data_hdr->time.hour,
			data_hdr->time.minute,
			data_hdr->time.second,
			data_hdr->time.fracsec/10,
			type10.version >= 2.3 ? data_hdr->network : "",
			data_hdr->station,
			data_hdr->location,
			channel,
			input.type);
		}

		if ((outfile = fopen (outfile_name, "w+")) == NULL)
		{
			fprintf (stderr, "\tWARNING (output_segy):  ");
			fprintf (stderr, "Output file %s is not available for writing.\n", outfile_name);

			perror("output_segy()");

			fprintf (stderr, "\tExecution continuing.\n");

			chdir(orig_dir);
		
			return;
		}

/*                 +=======================================+                 */
/*=================|       write a SEGY binary file        |=================*/
/*                 +=======================================+                 */

		/* describe the file being written */
		printf("Writing %s: %s: %s: %s, %5d samples (binary),",
			data_hdr->network,
			data_hdr->station,
			data_hdr->location,
			channel,
			data_hdr->nsamples);

		printf(" starting %04d,%03d %02d:%02d:%02d.%03d UT\n",
			data_hdr->time.year,
			data_hdr->time.day,
			data_hdr->time.hour,
			data_hdr->time.minute,
			data_hdr->time.second,
			data_hdr->time.fracsec/10);

		if (reverse_flag)
		{
			int tmpx;

			tmpx = segy_header.comment.p_stuff.max_samp;
			segy_header.comment.p_stuff.max_samp =
				segy_header.comment.p_stuff.min_samp;

        		segy_header.comment.p_stuff.min_samp = tmpx;
	
		}

		/* write the SAC header */
		if (fwrite (&segy_header, sizeof(struct segy), 1, outfile) != 1)
		{
			fprintf (stderr, "\tWARNING (output_segy_header):  ");
			fprintf (stderr, 
				"failed to properly write SEGY header to %s.\n",
					outfile_name);

			perror("output_segy()");

			fprintf(stderr, "\tExecution continuing.\n");

			fclose(outfile);

			chdir(orig_dir);

			return;

		}

		/* write the SEGY data */

		/* find index into start of data in buffer, if multiplexed
		 * it starts at locations other than zero
		 */
		j = k*(seis_buffer_length/data_hdr->num_mux_chan);

                if (reverse_flag)
                {
                        char wrkstr[200];

                        for (i=0;i<data_hdr->nsamples;i++)
                                seismic_data[offset+i+j] = -seismic_data[offset+i+j];
                        sprintf(wrkstr, "Data reversal initiated for SEGY file %s", outfile_name);
 
                        rdseed_alert_msg_out(wrkstr);
                        
                }
 

		for (i = 0; i < data_hdr->nsamples; i++)
		{
			this_sample = (int) seismic_data[offset+j+i];

			if (fwrite (&this_sample, sizeof(int), 1, outfile) != 1)
			{
				fprintf(stderr, "\tERROR - output_segy():failed to properly write SEGY data to %s.\n", outfile_name);

				fprintf(stderr, "Giving up on this file\n");

				perror("output_segy()");
			
				i = data_hdr->nsamples; /* drop out of loop */
			}

		}		/* for i < num_samples */

		fclose(outfile);

	} 		/* the big for loop - if multiplexed */


	chdir(orig_dir);
}

/* -------------------------------------------------------------------- */
struct type58 *find_type_58_stage_0();

struct type48 *find_type_48();


float get_overall_gain()

{
	struct type58 *t_58;
	struct type48 *t_48;

	t_58 = find_type_58_stage_0(current_channel->response_head);

	if (t_58 != NULL)
		return t_58->sensitivity;

	t_48 = find_type_48(current_channel->response_head);

	if (t_48 != NULL)
		return t_48->sensitivity;

	/*else , an error */	

	fprintf(stderr, "WARNING - couldn't find blockette (58/48)!!\n");
 
	fprintf(stderr, "For station: %s; channel: %s\n",
				current_station->station, 
				current_channel->channel);
 
	return -1;      /* error condition */

}

/* ------------------------------------------------------------------------- */

