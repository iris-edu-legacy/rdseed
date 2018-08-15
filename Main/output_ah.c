/*===========================================================================*/
/* SEED reader     |              output_ah                |    subprocedure */
/*===========================================================================*/
/*
	Name:		output_ah
	Purpose:	build a seismogram file from the SEED header tables
			and data files and writes output seismogram files.
			This procedure writes output in L-DGO AH-files format
			in binary, which
			include response table (A0, DS, Poles & Zeroes of 
			analog response and coefficients of numerator and 
			denominator of digital FIR filter used.
	Usage:		void output_ah ();
			output_ah (data_hdr, offset);
	Input:		pointer to current data and offset in data buffer and 
			gets its data from globally-available tables and files
	Output:		none (writes a seismogram file; seismogram files are named by
			station, component, beginning time, and a tag; for example,
			ANMO.SPZ.1988.01.23.15.34.08.AH is the filename for a
			seismogram from year 1988, January 23, 15:34:08 UT,
			recorded at station ANMO from component SPZ.  Writes message
			to stderr describing the seismogram file being written.
	Called by:	time_span_out();
	Calls to:	fill_ah_resp(); ahio_routine();
	Author:		Won-Young Kim, Lamont-Doherty Geol. Obs
	Revisions:	03/31/91  Base on Dennis O'Neill's (09/19/89) output_data.
			05/06/92  Allen Nance - added "stype" variable in header
*/

#include "rdseed.h"		/* SEED tables and structures */
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#ifndef _CYGWIN
#include <rpc/rpc.h>
#else
#include <rpc/xdr.h>
#endif

#include <math.h>
#include "ahhead.h"		/* LDGO AH format structures */

char *ddd2mmdd();

static int days_in_month[]= {31,28,31,30,31,30,31,31,30,31,30,31,31};

extern int read_summary_flag;
extern int EventDirFlag;
extern struct type48 *find_type_48_stage(int, struct response *);

char *summary_file_get_event();
char *get_event_dir_name();

void output_ah (data_hdr, offset)
struct data_hdr *data_hdr;
int offset;
{
	register struct ah_time *t;		/* AH stuff begins */
	ahhed 	hed;
	XDR	xdr_out;
	void 	month_day();
	int 	ier;
	long 	jday;
	void	fill_ah_resp();			/* AH stuff ends */
	char	*get_type33 (), *p;
	FILE 	*outfile;			/* output file pointer */
	char 	outfile_name[100], channel[10];	/* output file name */
	int 	i, j,k;				/* counter */
	char 	character[8+1];			/* for character transfer */
	int 	ascii;				/* switch for ASCII output */
	int 	binary;				/* switch for binary output */
	int 	reverse_flag;			/* Data reversal flag */
	double	dip, azimuth;			/* Temp for dip and azimuth */
	double duration;
	struct time data_hdr_endtime;
	struct type48 *type_48;

	char wrkstr[512];
        int y, ddd;

	char dirname[MAXPATHLEN];

	char orig_dir[MAXPATHLEN];

	getcwd(orig_dir, MAXPATHLEN);

	chdir(output_dir);

	if (EventDirFlag)
	{
		strcpy(dirname, get_event_dir_name());

		if (!dir_exists(dirname))
		{
			if (mkdir(dirname, 0777) == -1)
			{
				fprintf(stderr, "Unable to create event directory. Using current directory!\n");
				perror("output_ah");
				strcpy(dirname, "./");
			}

		}

		chdir(dirname);

	}
		
	/* select ASCII or binary output (binary chosen here) */

	ascii = FALSE;
	binary = !ascii;

	for (k=0;k<data_hdr->num_mux_chan;k++)
	{
		if (data_hdr->nsamples == 0)
		{
			fprintf(stderr,"Output Data Contains No Samples\n");
			chdir(orig_dir);
			return;
		}		

		if (current_channel == NULL)
		{
			fprintf(stderr, "\tERROR - Unexpected End of Multiplexed Channel List\n");
			chdir(orig_dir); 

			return;
		}

		reverse_flag = 0;

		memset(channel,0,sizeof(channel));
                strcpy(channel,current_channel->channel);
                if (data_hdr->num_mux_chan > 1) {
                        int i;
                        for (i=0; i<3; i++) {
                                if (channel[i] == ' ' || channel[i] == '\0') break;
                        }
                        if (i > 2) 
				i = 2;

                        channel[i] = data_hdr->mux_chan_name[k];
                }


		dip = current_channel->dip;
		azimuth = current_channel->azimuth;

		reverse_flag = check_for_reversal(dip, azimuth, channel[2]);
	
		{
			struct response *response; int flag;

			flag = FALSE;
			for (response = current_channel->response_head; response != NULL; response = response->next)
			{
				if (response->type == 'S') {
					if ((response->ptr.type58)->stage == 0) {
						if ((response->ptr.type58)->sensitivity < 0.0)
							reverse_flag |= 2;
						flag = TRUE;
					}
				} else if (response->type == 'R') {
					type_48 = find_type_48_stage(0, response);
					if (type_48 != NULL) {
						if (type_48->sensitivity < 0.0) 
							reverse_flag |= 2;
	
						flag = TRUE;
					}
				}
			}
			if (!flag && (check_reverse & 2)) fprintf(stderr, "Warning - Stage Zero Not Found on Gain Reversal check\n");
		}

		if (reverse_flag == 3)
		{
			reverse_flag = 0;		/* Double inversion */
			fprintf (stderr, "Warning... Gain and Dip/Azimuth Reversal found %s.%s, Data was not inverted\n",
				current_station->station, channel);
		}

		/* get null ah header */

		get_null_head(&hed);

		/* Fill AH staion instrument response	*/
		/* if not channels == AT, SOH, LOG */

		if ((strcmp(current_channel->channel, "AT")  != 0) &&
		    (strcmp(current_channel->channel, "LOG") != 0) && 
            	    (strcmp(current_channel->channel, "SHO") != 0))
		{
			fill_ah_resp (&hed);	

		}
		if (check_reverse & 1)
		{
			if (reverse_flag & 1)
				fprintf (stderr, "Warning... Azimuth/Dip Reversal found %s.%s, Data will be automatically inverted\n",
					current_station->station, channel);
		}
		else
		{
			if (reverse_flag & 1)
			{
			fprintf (stderr, "Warning... Azimuth/Dip Reversal found %s.%s, Data inversion was not selected\n           Header Scale Negated, but Response File will not be modified\n\n", current_station->station, channel);

			

			hed.station.DS = -hed.station.DS;
			}

			reverse_flag &= 2;

		}

		if (check_reverse & 2)
		{
			if (reverse_flag & 2)
				fprintf (stderr, "Warning... Gain Reversal found %s.%s, Data will be automatically inverted\n           Response File will not be modified\n",
					current_station->station, channel);
		}
		else
		{
			if (reverse_flag & 2)
				fprintf (stderr, "Warning... Gain Reversal found %s.%s, Data inversion was not selected\n",
					current_station->station, channel);
			reverse_flag &= 1;
		}

		/* Change on both Dip/Azimuth and Sensitivity reversals */

		if (reverse_flag & 2) 
			hed.station.DS = -hed.station.DS;

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
 

		/* place the azimuth and the dip into record 
		 * comment. Units is always DISP (m)
		 */
			
		sprintf(hed.record.rcomment, 
				"Comp azm=%3.1f,inc=%3.1f; Disp (m);",
				azimuth, dip);

	/* AH record information */

		hed.record.delta=1/data_hdr->sample_rate;
		hed.record.ndata= data_hdr->nsamples;
		
		hed.record.abstime.yr=data_hdr->time.year;
		jday= data_hdr->time.day;/* given Julian day get Month and Day */
		month_day(&(hed.record.abstime), (int)jday);
		hed.record.abstime.hr= (short) data_hdr->time.hour;
		hed.record.abstime.mn= (short) data_hdr->time.minute;
		hed.record.abstime.sec= (float) data_hdr->time.second+(float)data_hdr->time.fracsec*0.0001;

		hed.record.type=DOUBLE;

		if(reverse_flag) 			
			strncat(hed.record.rcomment, 
				"Signal Reversal found & Data inverted;",
				 COMSIZE - strlen(hed.record.rcomment));

		/* AH station information	*/

		sprintf (character, "%-5.5s\0", data_hdr->station);
		strncpy (hed.station.code, character, 6);
		sprintf (character, "%-5.5s\0", channel);
		strncpy (hed.station.chan, character, 6);
		p = get_type33(current_station->owner_code);
		if (p != NULL) strncpy (hed.station.stype, p, 8); else hed.station.stype[0] = 0;
		hed.station.slat=current_channel->latitude;
		hed.station.slon=current_channel->longitude;
		hed.station.elev=current_channel->elevation;

		/* AH event information	*/
		if (read_summary_flag)
		{

			memset(wrkstr, 0, sizeof(wrkstr));

			/* recover current event from summary file */
			strncpy(wrkstr, 
				  summary_file_get_event(), 511);
	
			/* skip the source */
			p = strtok(wrkstr, ",");

			/* date and time */
			p = strtok(NULL, "/");

			if (p == NULL) 
                                p = "";
        
                        hed.event.ot.yr = atoi(p);

			p = strtok(NULL, "/");
 
                        if (p == NULL)  
                                p = "";
         
                        hed.event.ot.mo = atoi(p);

			p = strtok(NULL, " ");
 
                        if (p == NULL)  
                                p = "";
         
                        hed.event.ot.day = atoi(p);

			/* hour:minute:second.frac */
			p = strtok(NULL, ":");
 
                        if (p == NULL) 
                                p = "";
   			hed.event.ot.hr = atoi(p);

                        /* minute */
                        p = strtok(NULL, ":");

                        if (p == NULL)
                                p = "";
 
                        hed.event.ot.mn = atoi(p);
 
                        /* seconds */
                        p = strtok(NULL, ",");
 
                        if (p == NULL)
                                p = "";
 
                        hed.event.ot.sec = atof(p);
			
			/* lat and long */
			p = strtok(NULL, ",");	/* latitiude */
			if (p == NULL)
                                p = "";

			hed.event.lat = atof(p);

			p = strtok(NULL, ",");  /* longitude */
                        if (p == NULL)
                                p = ""; 
 
                        hed.event.lon = atof(p);

			p = strtok(NULL, ",");  /* depth */
                        if (p == NULL)
                                p = ""; 
 
                        hed.event.dep = atof(p);
	
		}
		else
		if (type71_head != NULL)
		{
			hed.event.lat= type71_head->latitude;
			hed.event.lon= type71_head->longitude;
			hed.event.dep= type71_head->depth;
			
			/* convert date/time from YYYYDDD... to MM/DD/YYYY */
			strcpy(wrkstr, type71_head->origin_time);

			/* peal off the year */
			p = strtok(wrkstr, ",");

			if (p == NULL)
				p = "";
			else
				y = atoi(p);

			/* peal off the day of year */
			p = strtok(NULL, ",");
 
                        if (p == NULL) 
                                p = ""; 
                        ddd = atoi(p); 
	
			/* go ahead and extract the time from wrkstr */

			/* hour */
			p = strtok(NULL, ":");       
 
                        if (p == NULL) 
                                p = "";        

                        hed.event.ot.hr = atoi(p);   	

			/* minute */
			p = strtok(NULL, ":");       
 
                        if (p == NULL) 
                                p = "";        

                        hed.event.ot.mn = atoi(p);        
 
			/* seconds */ 
                        p = strtok(NULL, ",");        
  
                        if (p == NULL)  
                                p = "";         
 
                        hed.event.ot.sec = atof(p); 
		
			/* start over with new string */	
			strcpy(wrkstr, ddd2mmdd(ddd, y));

			/* MM/DD/YYYY */
			p = strtok(wrkstr, "/");
		
			if (p == NULL)
				p = "";
		
			hed.event.ot.mo = atoi(p);

			/* day */

			p = strtok(NULL, "/"); 
                         
                        if (p == NULL) 
                                p = ""; 
                        
                        hed.event.ot.day = atoi(p);     
	
			/* year */
			p = strtok(NULL, "/");  
                          
                        if (p == NULL)  
                                p = "";  
	
                	hed.event.ot.yr = atoi(p);
 
		}
		else
		{
			hed.event.lat=hed.event.lon=hed.event.dep=0.0;
			hed.event.ot.yr=hed.event.ot.mo=hed.event.ot.day=0;
			hed.event.ot.hr=hed.event.ot.mn=0;
			hed.event.ot.sec=0.0;
		}	


		if (output_endtime) {
              		duration  =
                   ((double)(data_hdr->nsamples)*10000)/data_hdr->sample_rate;

                	data_hdr_endtime = timeadd_double(data_hdr->time, duration);

			sprintf (outfile_name, 
					"%04d.%03d.%02d.%02d.%02d.%04d_%04d.%03d.%02d.%02d.%02d.%04d.%s.%s.%s.%s.%c.AH",
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
			data_hdr->network,
			data_hdr->station,
			data_hdr->location,
			channel,
			input.type);
		} else {
			sprintf (outfile_name, 
					"%04d.%03d.%02d.%02d.%02d.%04d.%s.%s.%s.%s.%c.AH",
			data_hdr->time.year,
			data_hdr->time.day,
			data_hdr->time.hour,
			data_hdr->time.minute,
			data_hdr->time.second,
			data_hdr->time.fracsec,
			data_hdr->network,
			data_hdr->station,
			data_hdr->location,
			channel,
			input.type);
		}

		if ((outfile = fopen (outfile_name, "w")) == NULL)
		{
			fprintf (stderr,"\tWARNING (output_ah):  ");
			fprintf (stderr,"Output file %s is not available for writing.\n",outfile_name);
			fprintf (stderr, "\tExecution continuing.\n");

			perror("output_ah()");

			chdir(orig_dir); 

			return;	
		}

		xdrstdio_create(&xdr_out, outfile, XDR_ENCODE);
 
		if (binary)
		{

			/* describe the file being written */

			printf ("Writing %s: %s: %s: %s, %5d samples (binary),",
						data_hdr->network,
						data_hdr->station,
						data_hdr->location, 
						channel,
						data_hdr->nsamples);

			printf (" starting %04d,%03d %02d:%02d:%02d.%04d UT\n",
				data_hdr->time.year,data_hdr->time.day,
				data_hdr->time.hour,data_hdr->time.minute,
				data_hdr->time.second,data_hdr->time.fracsec);			
			
			j = k*(seis_buffer_length/data_hdr->num_mux_chan);
			if (reverse_flag)
			{
				char wrkstr[200];

				for (i=0; i<data_hdr->nsamples; i++)
					seismic_data[offset+i+j] = -seismic_data[offset+i+j];

				sprintf(wrkstr, "Data Reversal initiated for AH file %s", outfile_name);

				rdseed_alert_msg_out(wrkstr);
			}


			/*  find max. amplitude	*/

			ier = maxamp (&hed, &seismic_data[offset+j]);

			/* write ah header and data */

			if(xdr_puthead(&hed, &xdr_out) != 1) 
			{
				fprintf(stderr,"Error writing header in rdseed; output_ah\n");

				perror("xdr_puthead");

				exit(-3);
			}			
			if(xdr_putdata(&hed, (char *)&seismic_data[offset+j], &xdr_out) < 0)
			{
				fprintf(stderr,"Error writing data in rdseed; output_ah\n");
				perror("xdr_putdata");

				exit(-3);
			}
		}

		xdr_destroy(&xdr_out);

		fclose (outfile);

		fflush(stdout);

		current_channel = current_channel->next;

	}

	chdir(orig_dir);

#if 0
	strcpy(channel,current_channel->channel);
	for (current_channel=current_channel->next; current_channel != NULL; current_channel=current_channel->next)
		if (strcmp(channel, current_channel->channel) == 0) break;
#endif

	return;

}

void month_day(t, jul_day)
register struct ah_time *t;
int jul_day;
{
	int i, dim;

	t->day= jul_day;
	for (i= 0; i < 12; i ++) {
		dim= days_in_month[i];
		if (isaleap(t->yr) && (i == 1)) dim++;
		if (t->day <= dim ) break;
		t->day-= dim;
	}
	t->mo= i+1;
	return;
}

char *get_type33 (code)
int code;
{
	struct type33 *type33;

	for (type33 = type33_head; type33 != NULL; type33 = type33->next)
	{
		if (type33->code == code)
		{
			return(type33->abbreviation);
			break;
		}
	}
	if (type33 == NULL) return (NULL);
}
