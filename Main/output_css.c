/*===========================================================================*/
/* SEED reader     |              output_css               |    subprocedure */
/*===========================================================================*/
/*
	Name:	output_css
	Purpose:build a seismogram file from the SEED header tables
				and data files and writes output seismogram files.
		This procedure writes output in CSS format 
	Usage:	void output_css ();
			output_css (data_hdr, offset);
	Input:	pointer to current data and offset in data buffer and 
		gets its data from globally-available tables and files
	Output:	none (writes a seismogram file; seismogram files are named by
		station, component, beginning time, and a tag; for example,
		ANMO.SPZ.1988.01.23.15.34.08.CSS is the filename for a
		seismogram from year 1988, January 23, 15:34:08 UT,
		recorded at station ANMO from component SPZ.  Writes message
		to stderr describing the seismogram file being written.
	Called by:	time_span_out();
	Calls to:	
	Author:		Allen Nance
	Revisions:	03/31/91  Base on Dennis O'Neill's (09/19/89) output_data.
				01/19/95  CL - added disp_factor * frequency as per Pete Davis

*/

#include "rdseed.h"		/* SEED tables and structures */
#include <stdio.h>
#include <math.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <math.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <unistd.h>

#define STRUPPERCASE(s) { char *p = s; while(*p) *p++ = toupper(*p); }

#define ORID 1

/*#define isaleap(year) ((((year)%100 != 0) && ((year)%4 == 0)) || ((year)%400 == 0)) */

struct type58 *find_type_58_stage_0();

static int siteid = 0;
char *get_event_dir_name();
char *get_net();
struct type33 *find_type_33();
char *get_src_name();
int scan_affiliation(char *, char *, char *);

extern int EventDirFlag;
extern struct type48 *find_type_48_stage(int, struct response *);

void output_css (data_hdr, offset)
struct data_hdr *data_hdr;
int offset;
{
	int	*i_seismic_data;	/* pointer for data storage */
	FILE 	*outfile;		/* output file pointer */
	char 	outfile_name[80], channel[12];	/* output file name */
	int 	i, j, k;	/* counters */
	char 	character[12];	/* for character transfer */
	int 	reverse_flag;	/* Data reversal flag */
	time_t	time_sec;
	double  dip;		/* temp holding for these variables */
	double  azimuth;	/* temp holding for these variables */
	double  calib, calper;	/* wfdisc calibration parms */
	struct	time	stn_stime;	/* start time of station info */
	struct	time	stn_etime;	/* end time of stninfo */
	char 	hold_time[50];	/*  */

	struct	time	chn_stime;	/* start time of channel info */
	struct	time	chn_etime;	/* end time of channel info */

	struct	tm	*ltime;
	char	lddate[20];	/* current date and time */

	int		wfid;	/* wfdisc id variable */
	int		cfid;	/* sitechan id variable */

	struct	stat statbuf;	/* file status buffer */
	char Wfdisc_buf[288];
	struct type48 *type_48;

	char station[20];
	char network[20];
	char channel_loc[20];

	char event_dir[MAXPATHLEN];
	char orig_dir[MAXPATHLEN];

	i_seismic_data = (int *) seismic_data;	/* Copy buffer pointer */

	struct response *response; int flag;
	struct type34 *type34;
	char temp[30];
	double disp_factor;
	struct type58 *t_58;

	int calib_zero_flag = 0;

	time(&time_sec);
	ltime = localtime(&time_sec);

	strftime(lddate, 18, "%D %T", ltime);

	getcwd(orig_dir, MAXPATHLEN);

	chdir(output_dir);

	if (EventDirFlag)
	{
		strcpy(event_dir, get_event_dir_name());

		if (!dir_exists(event_dir))
			if (mkdir(event_dir, 0777) == -1)
			{
				fprintf(stderr, "Unable to create event directory. Using current directory!\n");
				perror("output_css");
				strcpy(event_dir, ".");
			}
		
		chdir(event_dir);
	}
 
	for (k=0;k<data_hdr->num_mux_chan;k++)
	{
		if (current_channel == NULL)
		{
			fprintf(stderr, "\tERROR - Unexpected End of Multiplexed Channel List\n");

			chdir(orig_dir);

			return;
		}

		reverse_flag = 0;
		for (i=0;i<10;i++) channel[i] = 0;
			strcpy(channel,current_channel->channel);
		if (data_hdr->num_mux_chan > 1) channel[strlen(channel)] = data_hdr->mux_chan_name[k];
		if (check_reverse & 1)

		dip = 0.0;	/* for some strange reason these two variables */
		azimuth = 0.0;	/* had to be inited to operate properly */

		dip = current_channel->dip;
		azimuth = current_channel->azimuth;

		check_for_reversal(dip, azimuth, channel[2]);

		calib = 0.0;			/* default scaling info */
		calper = 0.0;

		t_58 = find_type_58_stage_0(current_channel->response_head);

		if (t_58 == NULL)
    		{
       			fprintf(stderr, 
				"Warning - couldn't find stage 0 - blockette (58)!!\n");

        		fprintf(stderr, "For station: %s; channel: %s\n",
                    		current_station->station, current_channel->channel);

			fprintf(stderr,"Calibration variable will be set to Zero\n");
			calib_zero_flag = 1;

    		}

/* look for velocity or acceleration channels, AH only likes displacement */
/* so integrate response by deleting zeroes */

		disp_factor = 1.0;
		if (t_58 != NULL)
		for (type34 = type34_head; type34 != NULL; type34 = type34->next)
		{
			if (type34->code == current_channel->signal_units_code)
			{
				if (type34->description != 0)
					strncpy(temp, 
						type34->description, 
						30);
				else
					strcpy(temp, "");

				rdseed_strupr(temp);

				if (strstr(temp,"VEL") != 0)
				{
					disp_factor = 2.0*M_PI*t_58->frequency;
				}
					
				if (strstr(temp,"ACCEL") != 0)
					disp_factor = 4.0*M_PI*M_PI*t_58->frequency;

				break;

			}
		}

		flag = FALSE;
		for (response = current_channel->response_head; response != NULL; response = response->next)
		{
			if (response->type == 'S') {
				if ((response->ptr.type58)->stage == 0) {
					if (((response->ptr.type58)->sensitivity < 0.0) && (check_reverse & 2))
					reverse_flag |= 2;
	
					if (calib_zero_flag)
						calib = 0;
					else
						calib = 1.0/((response->ptr.type58->sensitivity*disp_factor)/1000000000.0);
	
					calper = 1.0/(response->ptr.type58->frequency);

					flag = TRUE;
				}

			} else if (response->type == 'R') {
				type_48 = find_type_48_stage(0, response);
				if (type_48 != NULL) {
					if ((type_48->sensitivity < 0.0) && 
					    (check_reverse & 2))
						reverse_flag |= 2;

					if (calib_zero_flag)
						calib = 0;
					else
						calib = 1.0/(((float)type_48->sensitivity*disp_factor)/1000000000.0);
	
					calper = 1.0/((float)type_48->frequency);

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
		switch (strlst(channel))
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

		if (reverse_flag & 2) calib = -calib;

		if (data_hdr->nsamples == 0)
		{
			fprintf(stderr,"Output Data Contains No Samples\n");
			chdir(orig_dir);

			return;
		}
			
			/* WFDISC information */

		if (access("rdseed.wfdisc", R_OK)) wfid = 1;
		else
		{
			stat("rdseed.wfdisc", &statbuf);
			wfid = statbuf.st_size/284 + 1;
		}

		time_sec  = data_hdr->time.second;
		time_sec += data_hdr->time.minute  * 60;
		time_sec += data_hdr->time.hour    * 3600;
		time_sec += (data_hdr->time.day-1) * 3600 * 24;

		for (i=1971; i<= data_hdr->time.year; i++)
		{
			if (isaleap(i-1)) { time_sec += 366*24*3600; }

			else  { time_sec += 365*24*3600; }
		}

		strcpy(channel_loc, channel);
		strncat(channel_loc, data_hdr->location, 2);

		sprintf (outfile_name, "rdseed%08d.%c.w                    ",
			wfid, 
			input.type);

		*(strchr(outfile_name,' ')) = 0;

		if ((outfile = fopen (outfile_name, "w")) == NULL)
		{
			fprintf (stderr,"\tWARNING (output_css):  ");
			fprintf (stderr,"Unable to open output file %s \n",outfile_name);
			fprintf (stderr, "\tExecution aborted.\n");
	
			perror("output_css()");

			chdir(orig_dir);

			return;
		}

		/* describe the file being written */

		printf ("Writing %s, %s, %5d samples (binary),",
			data_hdr->station,channel,data_hdr->nsamples);

		printf (" starting %04d,%03d %02d:%02d:%02d.%04d UT\n",
			data_hdr->time.year,data_hdr->time.day,
			data_hdr->time.hour,data_hdr->time.minute,
			data_hdr->time.second,data_hdr->time.fracsec);			
			
		j = k*(seis_buffer_length/data_hdr->num_mux_chan);
		if (reverse_flag)
		{
			char wrkstr[200];

			for (i=0; i<data_hdr->nsamples; i++)
				i_seismic_data[offset+i+j] = (int) -seismic_data[offset+i+j];

			sprintf(wrkstr, "Data reversal initiated for CSS file %s", outfile_name);

			rdseed_alert_msg_out(wrkstr);
		}
		else
			for (i=0; i<data_hdr->nsamples; i++)
				i_seismic_data[offset+i+j] = (int) seismic_data[offset+i+j];



		/* write data */

			
		if (fwrite (&i_seismic_data[offset+j], data_hdr->nsamples * sizeof (int), 
		    1, outfile) != 1)
			{
				fprintf (stderr, "WARNING (output_css):  ");
				fprintf (stderr, "failed to properly write CSS data to %s.\n",
				outfile_name);
				perror("output_css()");

				fprintf (stderr, "\tExecution aborted.\n");
	
				chdir(orig_dir);

				return;
			}

		fclose (outfile);

		timecvt(&stn_stime, current_station->start);
		timecvt(&stn_etime, current_station->end);

		timecvt(&chn_stime, current_channel->start);
		timecvt(&chn_etime, current_channel->end);

		if ((outfile = fopen ("rdseed.sitechan", "r+")) == NULL)
		{
			if ((outfile = fopen ("rdseed.sitechan", "w+")) == NULL)
			{
				fprintf (stderr,"\tWARNING (output_css):  ");
				fprintf (stderr,"Output SITE file is not available for writing.\n");
				fprintf (stderr, "\tExecution continuing.\n");

				perror("output_css()");

				chdir(orig_dir);

				return;
			}
		}

		cfid = determineChanId(outfile, data_hdr->station);

		if (fprintf(outfile, "%-6s %-8s %04d%03d %8d %8d n %9.4f %6.1f %6.1f %-50s %17s\n",
					data_hdr->station,
					channel_loc,
					chn_stime.year, chn_stime.day,
					cfid,
					(chn_etime.year && chn_etime.day) == 0	? -1 : chn_etime.year * 1000 + chn_etime.day,
					current_channel->local_depth/1000,
					azimuth,
					dip+90,
					"-",
					lddate) == -1)

//			if (fwrite (Wfdisc_buf, strlen(Wfdisc_buf), 1, outfile) != 1)

		{
			fprintf (stderr, "WARNING (output_css):  ");
			fprintf (stderr, "failed to properly write CSS SITECHAN data to %s.\n", outfile_name);

			perror("output_css()"); 
 
			fprintf (stderr, "\tExecution continuing.\n");

		}

		fclose (outfile);

		/* ********** */

		if ((outfile = fopen ("rdseed.wfdisc", "a")) == NULL)
		{
			fprintf (stderr,"\tWARNING (output_css):  ");
			fprintf (stderr,"Output file %s is not available for writing.\n",outfile_name);
			
			perror("output_css()");

			fprintf (stderr, "\tExecution continuing.\n");
		}

		sprintf (outfile_name, "rdseed%08d.%c.w                    ",
			wfid, 
			input.type);

		if (fprintf(outfile, "%-6s %-8s %17.5f %8d %8d %8d %17.5f %8d %11.7f %16.6g %16.6f %-6s o  %-2s - %-64s, %-32s %10d %8d %17s\n",
  			data_hdr->station,
			channel_loc,
 			(double)time_sec + ((double)data_hdr->time.fracsec)/10000.0,
  			wfid, cfid,
			(data_hdr->time.year * 1000) + data_hdr->time.day,
			((double)time_sec + ((double)data_hdr->time.fracsec)/10000.0) + ((double)(data_hdr->nsamples-1)/data_hdr->sample_rate),
			data_hdr->nsamples,
 			data_hdr->sample_rate,
 			calib,
			calper,
  			"-",
			get_word_order() == 0 ?  // intel 
				"i4" :
				"s4",
			".",
			outfile_name,
  			0, -1, lddate) == -1)

// if (fwrite (Wfdisc_buf, strlen(Wfdisc_buf), 1, outfile) != 1)
		{
			fprintf (stderr, "WARNING (output_css):  ");
			fprintf (stderr, "failed to properly write CSS WFDISC data to %s.\n", outfile_name);

			perror("output_css()");

			fprintf (stderr, "\tExecution continuing.\n");
		}

		fclose (outfile);


		sprintf (outfile_name, "rdseed.site");

		if ((outfile = fopen ("rdseed.site", "r+")) == NULL)
		{
			if ((outfile = fopen ("rdseed.site", "w+")) == NULL)
			{
				fprintf (stderr,"\tWARNING (output_css):  ");
				fprintf (stderr,"Output SITE file is not available for writing.\n");
				fprintf (stderr, "\tExecution continuing.\n");

				perror("output_css()");

				chdir(orig_dir);

				return;
			}
		}


		// scan file for exiting station info
		if (!chkForStn(outfile, data_hdr->station))
		{

int n;
			// not there, put in
			if ((n = fprintf(outfile, "%-6s %04d%03d %8d %9.4f %9.4f %9.4f %-50.50s %-4s %-6s %9.4f %9.4f %17s\n",
				data_hdr->station,
				stn_stime.year, stn_stime.day,
				(stn_etime.year && stn_etime.day) == 0	? -1 : stn_etime.year * 1000 + stn_etime.day,
				current_channel->latitude,
				current_channel->longitude,
				current_channel->elevation/1000,
				current_station->name,
				"-",  "-",
				0.0, 0.0,
				lddate)) == -1)

// if (fwrite (Wfdisc_buf, strlen(Wfdisc_buf), 1, outfile) != 1)

			{
				fprintf (stderr, "WARNING (output_css):  ");
				fprintf (stderr, "failed to properly write CSS SITE data to %s.\n", outfile_name);

				perror("output_css()"); 
 
				fprintf (stderr, "\tExecution continuing.\n");
			
			}

		}

		fclose (outfile);

		current_channel = current_channel->next;


	}	/* for each muxed channel */


	/* scan for network/station already there */
	if (!scan_affiliation("rdseed.affiliation", 
				current_station->network_code,
				current_station->station))
	{

		if ((outfile = fopen("rdseed.affiliation", "a+")) == NULL)
               	{
                       	fprintf (stderr,"\tWARNING (output_css):  ");
                       	fprintf (stderr,"Output affliation file is not available for writing.\n");

                       	perror("output_css()");

                       	fprintf (stderr, "\tExecution continuing.\n");

               	}
             
		if (outfile != NULL) 
		{
			fprintf(outfile, "%-8.8s%-6.6s%17s\n", 
					current_station->network_code,
					current_station->station,
					lddate);

			fclose(outfile);

		}

	}		/* affiliation */


	if (!scan_network("rdseed.network", current_station->network_code)) 
	{
		if ((outfile = fopen ("rdseed.network", "a+")) == NULL)
		{
                       	fprintf (stderr,"\tWARNING (output_css):  ");
                       	fprintf (stderr,"CSS network file is not available for writing.\n");

                       	perror("output_css()");

                       	fprintf (stderr, "\tExecution continuing.\n");
                }
		else
		{
              
                	fprintf(outfile, 
				"%-8s%-80s%-4s%-15s %8d %17s\n",
                               	current_station->network_code,
				get_net(current_station->owner_code),
				"-",
				"-",
				-1,
                               	lddate);

			fclose(outfile);

		}

	}		/* network */


	/* no need to create file more than once */
	if ((access("rdseed.origin", F_OK) < 0) && (type71_head != NULL))
	{ 
		int etime;
		int netmag;
		char **jdate;
		char jul_date[10];
		struct type71sub *this_mag; 

		int n;

                if ((outfile = fopen ("rdseed.origin", "a+")) == NULL)
                {
                        fprintf (stderr,"\tWARNING (output_css):  ");
                        fprintf (stderr,"CSS origin file is not available for writing.\n");

                        perror("output_css()");

                        fprintf (stderr, "\tExecution continuing.\n");

                }
		else
		{
			struct type71 *t71_ptr = type71_head;

			while (t71_ptr)
			{
				struct time stime;

		
				n = split(t71_ptr->origin_time, &jdate, ',');

				if (n == 0)
				{
					fprintf(stderr, "Warning, output_css(): Bad hypo date encountered\n");
					strcpy(jul_date, "0001001");
				}	
				else
				{
					n == 1 ? 
					    /* no day number */
					    sprintf(jul_date, 
							"%s001", 
							jdate[0]): 
					    sprintf(jul_date, 
							"%s%03d", 
							jdate[0], 
							atoi(jdate[1]));
				}

				fuse(&jdate, n);

				timecvt(&stime, t71_ptr->origin_time);
		
				etime  = stime.second;
				etime += stime.minute  * 60;
				etime += stime.hour    * 3600;
				etime += (stime.day-1) * 3600 * 24;

				for (i = 1971; i <= stime.year; i++)
				{
					if (isaleap(i - 1)) 
						etime += 366*24*3600;
					else 
						etime += 365*24*3600;
				}

				fprintf(outfile, "%9.4f %9.4f %9.4f %17.5f %8d %8d %-8s %4d %4d %4d %8d %8d %-7s %9.4f %s",

					t71_ptr->latitude,
					t71_ptr->longitude,
					t71_ptr->depth,
					(float) etime,
					ORID,
					-1,
					jul_date,
					-1,		/* nass */
					-1,		/* ndef */
					-1,		/* ndp  */
					t71_ptr->seismic_region,
					t71_ptr->seismic_location,
					"-",		/* event type */
					-999.9, 	/* estimated phase depth */
					"-");		/* depth method used */



				for (i = 0; i < t71_ptr->number_magnitudes; i++) {
					STRUPPERCASE(t71_ptr->magnitude[i].type);
				}

				this_mag = NULL;
				for (i = 0; i < t71_ptr->number_magnitudes; i++) {
					if (strstr(t71_ptr->magnitude[i].type, "MB") != NULL)
						this_mag = &t71_ptr->magnitude[i];
				}

				if (this_mag == NULL) {
					fprintf(outfile, " %7.2f %8d ", -999.0, -1);
				} else {
					fprintf(outfile, " %7.2f %8d ", this_mag->magnitude, -1);
				}
			
				this_mag = NULL;
				for (i = 0; i < t71_ptr->number_magnitudes; i++) {
					if (strstr(t71_ptr->magnitude[i].type, "MS") != NULL)
						this_mag = &t71_ptr->magnitude[i];
				}
 
                        	if (this_mag == NULL) {
					fprintf(outfile, "%7.2f %8d ", -999.0, -1);
                        	} else {
                                	fprintf(outfile, "%7.2f %8d ", this_mag->magnitude, -1);
                        	}
       
				this_mag = NULL;
				for (i = 0; i < t71_ptr->number_magnitudes; i++) {
					if (strstr(t71_ptr->magnitude[i].type, "ML") != NULL)
						this_mag = &t71_ptr->magnitude[i];
				}

                        	if (this_mag == NULL) {
                                	fprintf(outfile, "%7.2f %8d ", -999.0, -1);
                        	} else {
                                	fprintf(outfile, "%7.2f %8d ", this_mag->magnitude, -1);
                        	}
        
				fprintf(outfile, "%-15s %-15s %8d %-17s\n", 
						"-",
						get_src_name(type71_head->source_code),
						-1,
						lddate);

				t71_ptr = t71_ptr->next;

			}	/* while more t_71s */

		} /* else */
	}

	chdir(orig_dir);

}
/**************************************************
 * int determineChanId()
 *	helper proc for output_css()
 *
 * 	scan file for matching station, increment counter.
 * 
 */

int determineChanId(FILE *fptr, char *s)

{

	char *stn; 
	int n;
	
	char buffer[256];	// CSS site file has each record max length=155

	n = 1;

	while (1)
	{
				/* note: load date is 2 fields */
		if (fgets(buffer, (int)sizeof(buffer), fptr) == NULL)
		{
			return n;	/* end of the line - bale */
		}

		stn = strtok(buffer, " ");

		if (strcasecmp(s, stn) == 0)
		{
			n++;
		}
	}

	/* should never get here */
	return n;
}

/*****************************************************

/**************************************************
 * int chkForStn()
 *	helper proc for output_css()
 *
 * 	scan file for matching station.
 * 
 */

int chkForStn(FILE *fptr, char *s)

{

	char *stn; 
	int n;
	
	char buffer[256];	// CSS site file has each record max length=155

	while (1)
	{

				/* note: load date is 2 fields */
		if (fgets(buffer, (int)sizeof(buffer), fptr) == NULL)
		{
			return 0;	/* end of the line - bale */
		}

		stn = strtok(buffer, " ");

		if (strcasecmp(s, stn) == 0)
		{
			return 1;	/* found it - bale */
		}
	}

	/* should never get here */
	return 1;
}

/*****************************************************


/**************************************************
 * int scan_affilition()
 *	helper proc for output_css()
 *
 * 	scan file for matching station/network combo.
 * 
 */

int scan_affiliation(char *fname, char *n, char *s)

{
	char stn[20], net[20], ld_date[20];
	FILE *f;

	if ((f = fopen (fname, "r")) == NULL)
		return 0;	/* not there, scan failed */

	while (1)
	{

				/* note: load date is 2 fields */
		if (fscanf(f, "%s %s %*s %*s\n", net, stn) != 2)
		{
			fclose(f);
			return 0;	/* end of the line - bale */
		}

		if ((strcasecmp(n, net) == 0) && (strcasecmp(s, stn) == 0))
		{
			fclose(f);
			return 1;	/* found it - bale */
		}
	}

	/* should never get here */
	return 0;
}

/*****************************************************
 * scan_network() helper proc for output_css()
 *	checks each line for existing network
 */

int scan_network(fname, n)
char *fname;
char *n;

{

	FILE *f;
        char net[20];

	char buffer[200];

	int num_flds;


        if ((f = fopen (fname, "r")) == NULL)
                return 0;       /* not there, scan failed */

        while (fgets(buffer, sizeof(buffer), f))
        {
		/* lop off at first space, only scan network code */
		sscanf(buffer, "%s", net);

                if ((strcasecmp(n, net) == 0))
                {
			fclose(f);
                        return 1;       /* found it - bale */
                }
        }

	fclose(f);
 
        return 0;

}

char *get_net(code)
int code;

{
	char *ch_ptr;

	struct type33 *t_33_ptr;

	t_33_ptr = find_type_33(type33_head, code);

	if (t_33_ptr == NULL)
	{
		return "";
	}

	return t_33_ptr->abbreviation;
	
}

