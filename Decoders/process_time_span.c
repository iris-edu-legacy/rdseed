/*===========================================================================*/
/* SEED reader     |       process_time_span_index         |                 */
/*                 |              Release 3.1              |                 */
/*===========================================================================*/
/*
	Name:	process_time_span_index
	Purpose:Parse blockette 72 and 74 information for selected data blocks
	Usage:	

	Input:	an input file or device on which the SEED data reside and processed 73,74 blocks

	Output:		
	Warnings:	None
	Errors:		None

	Called by:rdseed.c
	Calls to:process_data (), chk_station (), chk_channel (), chk_network

	Algorithm:Look through either type73 or 74 data for selected stations/channel/time sections
	and move to that part of file for data processing.

	Notes:		

	Problems:Time span on blockette 73 not checked, but station.channel is checked.


	Language:C, hopefully to ANSI standard for portability.
	Author:		Allen Nance
	Revisions:	07/12/91  Allen Nance  Initial preliminary release 3.1

*/

#include <stdio.h>
#include <stdlib.h>

#include <ctype.h>
#include "rdseed.h"			/* rdseed headers */
#include "version.h"

#define SUBSTR(str,xch,ch)      \
        { char *c; \
          while (c = strchr(str, xch)) \
                *c = ch; \
        }

extern int output;
struct data_blk_1000 *scan_for_blk_1000();

process_time_span_index()
{

	if (type73_head != NULL)
	{
		process_type73 ();
		flush_type73 ();
		return (1);
	}
	else if (type74_head != NULL)
	{
		process_type74 (); 
		return(0);
	}
	else return (-1);

}


/*===========================================================================*/
/* SEED reader     |            process_type73             |                 */
/*                 |              Release 3.1              |                 */
/*===========================================================================*/
process_type73()
{

	struct type73 *type73;
	int i, j;

	type73 = type73_head;
	while (type73 != NULL)
	{
		for (i = 0; i < type73->number_datapieces-1; i++)
		{
			if (chk_station(type73->datapiece[i].station) && 
			    chk_channel(type73->datapiece[i].channel))
			{
				fseek(inputfile, (type73->datapiece[i].sequence_number-1)*LRECL, 0);
				for (j = 0;
				     j < (type73->datapiece[i+1].sequence_number-type73->datapiece[i].sequence_number);
				     j++)
				{ 
					/*num_bytes_read = read (fileno (inputfile), precord, LRECL);*/
					num_bytes_read = fread(precord, LRECL, 1, inputfile);
					lrecord_ptr = (char *)precord;
					read_logical_record (lrecord_ptr);
					process_data (0);
					output_flag = TRUE;
				}
			}
		}
		fseek(inputfile, (type73->datapiece[type73->number_datapieces-1].sequence_number-1)*LRECL, 0);
		type73 = type73->next;
	}
}

/*===========================================================================*/
/* SEED reader     |            process_type74             |                 */
/*                 |              Release 3.1              |                 */
/*===========================================================================*/
process_type74()
{
	
	struct type74 *type74;

	type74 = type74_head;
	while (type74 != NULL)
	{

		if (this_timespan_needed(type74))
			extract_this_timespan(type74);

		type74 = type74->next;
	}
}

/* --------------------------------------------------------------------- */
extern int tspan_flag;		// declared in process_data.c. Used so process_data will
				// spit out warns and errors for one timespan, not every record

int extract_this_timespan(t74)
struct type74 *t74;

{

	int i, j, num_items, default_Lrecl;

	long int now_where;

	default_Lrecl = LRECL;

	tspan_flag = TRUE;

	now_where = ftello(inputfile);

	if (fseeko(inputfile, ((t74->start_index-1)*LRECL) - now_where, 1) == -1)
		perror("time_span_out");

	LRECL = get_stn_chn_Lrecl(t74->station,
				  t74->channel,
				  type10.version >= 2.3 ? 
						t74->network_code :
						"",
				  t74->location,
				  t74->starttime);

	if (LRECL == 0 || (LRECL % 256 != 0))
	{
		fprintf(stderr, "Bad logical record length scanned for station %s; channel %s:location:%s. Assuming logical record length of %d\n", 
			t74->station,
                        t74->channel,
			t74->location,
			default_Lrecl);

		LRECL = default_Lrecl;

	}
	
	/* we are assuming that the data record size will not
	 * change for a timespan 
	 */
 	LRECL = get_blk_1000_Lrecl(inputfile);

	for (j = t74->start_index; j <= t74->end_index; j++)
	{ 
	
		for (i = 1; i <= default_Lrecl/LRECL; i++)
		{ 

			/* only on last record of the timespan */
			if ((j == t74->end_index) && (LRECL < default_Lrecl))
				if (i > t74->end_subindex)
				{
					break;
				}

			num_items = fread(precord, LRECL, 1, inputfile);

			if (num_items != 1)
			{
				fprintf(stderr, "extract_this_timespan: Unable to read the data\n");

				if (num_items < 0)
					perror("rdseed");

				LRECL = default_Lrecl;

				return 0;

			}

			lrecord_ptr = (char *)precord;

			read_logical_record (lrecord_ptr);

			process_data (0);
 
		}

	}


	/* Always reset LRECL back to blk10 , default */
	LRECL = default_Lrecl;

}

/* --------------------------------------------------------------------- */
extern int read_summary_flag;	/* defined and set in main() */
int this_timespan_needed(t_74)
struct type74 *t_74;

{
        struct time s_time, e_time;

	timecvt(&s_time, t_74->starttime);
	timecvt(&e_time, t_74->endtime);

	if (!chk_station(t_74->station))
		return 0;

	if (!chk_channel(t_74->channel))
		return 0;

	if (!chk_network(type10.version >= 2.3 ? t_74->network_code : ""))
		return 0;
 

	if (!chk_location(t_74->location))
		return 0;
 
	if (!chk_time(s_time, e_time))
		return 0;

	return 1;

}

/* ----------------------------------------------------------------- */
int get_blk_1000_Lrecl(fptr)
FILE *fptr;

{
	int num_bytes;

	char buff[32768];

	struct input_data_hdr *input_data_hdr;	
	char *input_data_ptr;

  	struct data_blk_1000 *p;
                       
	if ((num_bytes = fread(buff, 1, sizeof(buff), fptr)) == 0)
	{
		fprintf(stderr, "get_blk_1000_Lrecl(). Unable to read data block. Using default data record length\n");
		return LRECL;
	}

	input_data_hdr = (struct input_data_hdr *) &buff[8];
	input_data_ptr = &buff[8];

	/* check if byteswapping will be needed to recover data */
	/* hopefully, rdseed will not be around by 3010 */

	if (input_data_hdr->time.year < 1950 ||
			input_data_hdr->time.year > 3010)
		swap_fsdh(&input_data_ptr);

	/* check for sanity */
	if (input_data_hdr->time.year < 1950 ||
			input_data_hdr->time.year > 3010)
	{
		fprintf(stderr, "ERROR - process_data(): Unknown word order for station %s, channel %s, network %s\n", input_data_hdr->station, 
				input_data_hdr->channel, input_data_hdr->network);

		fprintf(stderr, "Skipping data record.\n");
		return;

	}

	/* sanity check */
	if (input_data_hdr->bofb != 0)
	if ((input_data_hdr->bofb < 48) || (input_data_hdr->bofb > 96))
	{
		fprintf(stderr, "get_blk_1000_Lrecl(). Unable to scan for blockette 1000. Bad bofb entry!\n");
		fprintf(stderr, "Defaulting to %d\n", LRECL);

                return LRECL;

	}

	if (input_data_hdr->bofb != 0)
		p = scan_for_blk_1000(&buff[input_data_hdr->bofb],
                               			buff);
	else
		p = 0;

	/* be sure to back up to previous fseek position */
	fseek(fptr, -num_bytes, 1);

	if (p == NULL)
		return LRECL;

	return 2 << (p->rec_length - 1);
	
}

/* ----------------------------------------------------------------- */
int get_LRECL(input_data_hdr, input_data_ptr)
struct input_data_hdr *input_data_hdr;  /* fixed data header */
char *input_data_ptr;



{
	int lrecl, default_lrecl;

	char stn[10];
	char chn[10];
	char net[10];
	char loc[10];

	char starttime[30];
	char endtime[30];

	struct data_blk_1000 *p;

	default_lrecl = LRECL;

	sprintf(starttime, "%d,%d,%d:%d:%d.%d", 
				input_data_hdr->time.year,
				input_data_hdr->time.day,
				input_data_hdr->time.hour,
				input_data_hdr->time.minute,
				input_data_hdr->time.second,
				input_data_hdr->time.fracsec);

	/* we dont compute endtime here, fake anything */

	sprintf(endtime, "%d,%d,%d:%d:%d.%d",  
                  	input_data_hdr->time.year, 
                       	input_data_hdr->time.day + 1, 
                       	input_data_hdr->time.hour, 
                       	input_data_hdr->time.minute, 
                       	input_data_hdr->time.second, 
                       	input_data_hdr->time.fracsec); 

	sprintf(stn, "%-5.5s", input_data_hdr->station);
	stn[5] = 0;
	SUBSTR(stn, ' ', 0);

	sprintf(chn, "%-3.3s", input_data_hdr->channel);
	chn[3] = 0;
	SUBSTR(chn, ' ', 0);

	sprintf(net, "%-2.2s", input_data_hdr->network); 
	net[2] = 0;
	SUBSTR(net, ' ', 0);

	sprintf(loc, "%-2.2s", input_data_hdr->location);
	loc[2] = 0;
	SUBSTR(loc, ' ', 0);

	/* Need to check for blk1000 */
	if (input_data_hdr->bofb != 0)
                p = scan_for_blk_1000(input_data_ptr + input_data_hdr->bofb,
                                                input_data_ptr);
        else
                p = 0;

	if (p)
        	lrecl = (2 << (p->rec_length - 1));
	else
	{
		lrecl = get_stn_chn_Lrecl(stn,
					chn,
					net,
					loc,
					starttime);

        	if (lrecl == 0 || (lrecl % 256 != 0))
        	{
                	fprintf(stderr, "Bad logical record length scanned for station %s; channel %s. Assuming logical record length of %d\n",
                        	input_data_hdr->station,
                        	input_data_hdr->channel,
                        	default_lrecl);

                	return(default_lrecl);
        	}


	}

	return lrecl;

}

/* ----------------------------------------------------------------- */
