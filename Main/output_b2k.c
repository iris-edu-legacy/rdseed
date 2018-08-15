/*===========================================================================*/
/* SEED reader     |            output_b2k                |    subprocedure */
/*===========================================================================*/
/*
				
	Problems:	none known
	Language:	C, hopefully ANSI standard
	Author:		Chris Laughbon	
	Revisions:
*/

#include "rdseed.h"		/* SEED tables and structures */
#include <stdio.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <strings.h>


#define TRIM(s) {char *p; if ((p = strchr(s, ' ')) != NULL) *p = 0;}
	
	 
static int file_open = FALSE;

static FILE *outfile;

static char prev_station[5];
static char prev_channel[4];
static char prev_network[3];
static char prev_location[3];

/* ---------------------------------------------------------------------- */
int output_b2k(struct input_data_hdr *hdr, struct data_blk_2000 *b2k, int rec_length)


{
	char *ptr;

	if (time_for_new_file(hdr))
	{
		if (file_open)
			close_b2k_file();

		open_file(hdr);

	}		/* if file is not open */

	strncpy(prev_station, hdr->station, sizeof(hdr->station));
	strncpy(prev_channel, hdr->channel, sizeof(hdr->channel));
	strncpy(prev_location, hdr->location, sizeof(hdr->location));
	strncpy(prev_network, hdr->network, sizeof(hdr->network));

	/* struct blk 2000 has a data hdr which we don't write.
 	 *
	 */

	ptr = (char *)(((char *)b2k) + sizeof(struct data_blk_hdr));

	if (fwrite(ptr, rec_length, 1, outfile) == -1)
	{

		fprintf(stderr, "Error, output_b2k(): Unable to write to file!\n");
 
		perror("output_b2k");
 
		return 0;

	}

	return 1;

}				/* output_b2k */

/* ------------------------------------------------------------------------ */
int close_b2k_file()

{

	if (file_open)
		fclose(outfile);

	file_open = FALSE;

}

/* ------------------------------------------------------------------------ */

int open_file(struct input_data_hdr *data_hdr)

{

	char outfname[MAXPATHLEN];

	char s[32];
	char n[32];
	char l[32];
	char c[32];

	strncpy(s, data_hdr->station, sizeof(data_hdr->station));
	s[sizeof(data_hdr->station)] = 0;
	TRIM(s);

	strncpy(c, data_hdr->channel, sizeof(data_hdr->channel)); 
	c[sizeof(data_hdr->channel)] = 0;
	TRIM(c);

	strncpy(n, data_hdr->network, sizeof(data_hdr->network)); 
	n[2] = 0;
        TRIM(n); 
 
        strncpy(l, data_hdr->location, sizeof(data_hdr->location)); 
	l[2] = 0;
        TRIM(l); 


	/* give the user some indication as to what we are doing. */ 

	printf("Writing:  Net/Stn/Loc/Chn %.2s:%.5s:%.2s:%.3s at %04d.%03d.%02d.%02d.%02d.%04d to disk\n",
			n,
			s, 
			l,
			c,
       			data_hdr->time.year,
       			data_hdr->time.day,
       			data_hdr->time.hour,
       			data_hdr->time.minute,
       			data_hdr->time.second,
       			data_hdr->time.fracsec);

		sprintf (outfname,
                        "%04d.%03d.%02d.%02d.%02d.%04d.%s.%s.%s.%s.OPAQUE",
                		data_hdr->time.year,
                		data_hdr->time.day,
                		data_hdr->time.hour,
                		data_hdr->time.minute,
                		data_hdr->time.second,
                		data_hdr->time.fracsec,
                		n,
                		s,
                		l,
                		c);

	if ((outfile = fopen(outfname, "a")) == NULL)
	{
		fprintf(stderr, "Error, output_b2k(): Unable to open file!\n");

		perror("output_b2k");

		file_open = FALSE;

		return 0;

	}

	file_open = TRUE;

}

/* ----------------------------------------------------------------------- */
int time_for_new_file(struct input_data_hdr *h)

{

	if (strncmp(h->station, prev_station, sizeof(h->station)) != 0)
		return 1;

	if (strncmp(h->channel, prev_channel, sizeof(h->channel)) != 0)
		return 1;

	if (strncmp(h->network, prev_network, sizeof(h->network)) != 0) 
                return 1; 
 
        if (strncmp(h->location, prev_location, sizeof(h->location)) != 0) 
                return 1;

	return 0;

}

/* --------------------------------------------------------------------- */
int scan_for_blk_2000(b_ptr, base)
struct data_blk_hdr *b_ptr;
char *base;		/* start of the logical rec */

{

	while (1)
    	{

		if (b_ptr->type == 2000)
			/* eureka, we've found it */
			return 1;
 
        	if (b_ptr->next_blk_byte == 0)
			return 0;

		/* garbage check */
		switch (b_ptr->type)
		{
                	case 100 :
                	case 201:
                	case 300:
                	case 310:
                	case 200:
                	case 320:
                	case 390:
                	case 395:
                	case 400:
                	case 405:
			case 1000:
                	case 1001:
				break;
                	default : /* oh, oh */
         
                    		fprintf(stderr, 
"scan_for_blk_2000(): Bad blockette scanned\n Blockette = %d\n", b_ptr->type);
                    	
				return 0;

		}		/* switch */
	
		b_ptr = (struct data_blk_hdr *)(base + b_ptr->next_blk_byte);
 
    	}   /* while */

	/* Should never get here */ 
	return 0;
}

/*----------------------------------------------------------------- */
