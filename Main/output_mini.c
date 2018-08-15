/*===========================================================================*/
/* SEED reader     |            output_mini                |    subprocedure */
/*===========================================================================*/
/*
				
	Problems:	none known
	Language:	C, hopefully ANSI standard
	Author:		Chris Laughbon	
	Revisions:	01/26/96 added D Newhouser's corrections for geoscope
*/

#include "rdseed.h"			/* SEED tables and structures */
#include <stdio.h>
#include <math.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/param.h>


#define TIME_PRECISION 10000		/* number of divisions in a */

#define BOOL int

static int seed_data_record[5000];

static int c = 0;

/* Globally defines as process_data() fills it up. */
struct mini_data_hdr mini_data_hdr;

char strlst();
char *scan_for_eob();
char *scan_for_blk_1000();
char *get_event_dir_name();

void blockette_swap(struct data_blk_hdr *b_ptr);

extern char prev_quality;

extern int EventDirFlag;

struct type50 *get_station_rec();

struct Fsdh
{
	char   SequenceNumber[6];
	char   Data_header_indicator;
	char   Reserved_bytes_A;
	struct input_data_hdr hdr;	/* fixed section of header */
};

/* ---------------------------------------------------------------------- */
void output_mini(data_hdr, offset)
struct data_hdr *data_hdr;
int offset;
{
	FILE *outfile;				/* output file pointer */
	char outfile_name[100], channel[10];	/* output file name */
	int i, j,k;				/* counter */

	struct Fsdh fsdh;

	int lrecl;

	int num_records;			/* number of Steims records */
	int output_as_doubles;

	struct data_blk_1000 d_blk_1000;

	double dip, azimuth;		/* Temp place for these values */
	char *p;
	double duration;
	struct time newtime;
	struct type52 *old_channel;
	char orig_dir[MAXPATHLEN];

	old_channel = current_channel;

	getcwd(orig_dir, MAXPATHLEN);

	chdir(output_dir);

	sprintf(outfile_name, "mini.seed");

	if ((outfile = fopen(outfile_name, "a")) == NULL)
	{
		fprintf (stderr, "\tWARNING (output_mini):  ");
		fprintf (stderr, 
			"Output file: %s is not available for writing.\n",
			outfile_name);

		perror("output_mini");

		fprintf (stderr, "\tExecution continuing.\n");

		chdir(orig_dir);

		return;
	}


	for (k=0;k<data_hdr->num_mux_chan;k++)
	{

		j = k*(seis_buffer_length/data_hdr->num_mux_chan);

		output_as_doubles = samples_decimal_non_zero(&seismic_data[offset+j], data_hdr->nsamples);

		if (current_channel == NULL)
		{
			fprintf(stderr, "\tERROR - Unexpected End of Multiplexed Channel List\n");
		
			chdir(orig_dir);

			return;
		}


		for (i=0;i<10;i++) 
			channel[i] = 0;

		strcpy(channel,current_channel->channel);

		if (data_hdr->num_mux_chan > 1)
		{
                	int i;
                	for (i=0; i<3; i++) 
			{
                        	if (channel[i] == ' ' || 
				    channel[i] == '\0') 
					break;
                	}
                	if (i > 2) 
				i = 2;

                	channel[i] = data_hdr->mux_chan_name[k];
        	}


		/* make sure the mini_data_hdr has the right 
		 * channel orientation code. If geoscope, it
		 * will be missing 
		 */
		memcpy(mini_data_hdr.hdr.channel, channel, 3);
 
/*                 +=======================================+                 */
/*=================|  build name for and open output file  |=================*/
/*                 +=======================================+                 */

		/* give the user some indication as to what we are doing. */ 

		printf("Appending Net:Stn:Loc:Chn %.2s:%.5s:%.2s:%.3s at %04d.%03d.%02d.%02d.%02d.%04d to mini.seed\n",
				type10.version >= 2.3 ? 	
					data_hdr->network : "N/A", 
				data_hdr->station, 
				data_hdr->location,
				channel,
            			data_hdr->time.year,
            			data_hdr->time.day,
            			data_hdr->time.hour,
            			data_hdr->time.minute,
            			data_hdr->time.second,
            			data_hdr->time.fracsec);
	
/*                 +=======================================+                 */
/*=================|        write header section           |=================*/
/*                 +=======================================+                 */

		memset((char *)&fsdh, 0, sizeof(fsdh));

		memcpy((char *)&fsdh.hdr, 
			(char *)&mini_data_hdr.hdr, sizeof(fsdh.hdr)); 

		/* update the timing info - may have changed */
		fsdh.hdr.time.year = data_hdr->time.year;
		fsdh.hdr.time.day  = data_hdr->time.day; 
		fsdh.hdr.time.hour = data_hdr->time.hour; 
		fsdh.hdr.time.minute = data_hdr->time.minute; 
		fsdh.hdr.time.second = data_hdr->time.second; 
		fsdh.hdr.time.fracsec= data_hdr->time.fracsec; 
        
		// saved from process_data

		fsdh.Data_header_indicator = prev_quality;

		/* data_hdr's nsamples are accumulated over the entire time span */
		fsdh.hdr.nsamples = data_hdr->nsamples;

/*:: DSN Change */
/* Since a blockette 1000 may be added to the mini_data_hdr blockette   */
/* list, we need to update the info in the mini_data_hdr to reflect the */
/* changes.  Otherwise, if we reuse this info for multiplexed data,     */
/* the mini_data_hdr and the blockette info will not correspond.        */

		/* mini seed record will always be in big endian format.
		 * fsdh, blockettes and the compressed records
		 */

		/* check for presence of the blockette 1000 */
		if (mini_data_hdr.hdr.bofb) 	
			p = scan_for_blk_1000(mini_data_hdr.blockettes, 
						((char *)&mini_data_hdr.hdr) - 8);
		else
			p = NULL;

		if (p)
		{

			if (output_as_doubles)
				// see blk 1000 in manual
				((struct data_blk_1000 *)p)->encoding_fmt = 5;
			else
				/* see SEED manual for steim code */	
				((struct data_blk_1000 *)p)->encoding_fmt = 10;  

			// always big endian 

			((struct data_blk_1000 *)p)->word_order = 1;

			((struct data_blk_1000 *)p)->rec_length = 12;

			/* lets make sure the number_of_blockettes >= 1 */
			if (mini_data_hdr.hdr.number_blockettes < 1)
			{
				mini_data_hdr.hdr.number_blockettes = 1;
			}

		}
		else
		{
			/* now scan for end of blocks */
			if (mini_data_hdr.hdr.bofb)
				p = scan_for_eob(mini_data_hdr.blockettes, 
						 mini_data_hdr.hdr.bofb);
			else 
				p = mini_data_hdr.blockettes;

			if (p == (char *)NULL)
			{
				fprintf(stderr, "output_mini: unable to add blockette 1000\n");
				fprintf(stderr, "output_mini: unable to continue\n");
			
				chdir(orig_dir);

				return;
			}

			/* add one for blk 1000 */
			mini_data_hdr.hdr.number_blockettes++;  

			/* create block 1000, and tag onto end of blockettes */
			memset((char *)&d_blk_1000, 0, sizeof(d_blk_1000));

			d_blk_1000.hdr.type = 1000;
			
			if (output_as_doubles)
				// see blk 1000 in manual
				d_blk_1000.encoding_fmt = 5;
			else
				/* see SEED manual for steim code */
    				d_blk_1000.encoding_fmt = 10;  

			/* the next blockette field stays at zero */

			d_blk_1000.word_order = 1;  // always little endian

			d_blk_1000.rec_length = 12;

			/* copy to blockettes holding buffer */
			memcpy((char *)p, (char *)&d_blk_1000, sizeof(d_blk_1000));

			mini_data_hdr.hdr.bofb = 48;

			/* calculate "new" end of blocks */	
			mini_data_hdr.hdr.bod = mini_data_hdr.hdr.bofb + 
					((p + sizeof(d_blk_1000)) - mini_data_hdr.blockettes);

			/* if it overflowed the 64 byte boundary, bump it up */	
			mini_data_hdr.hdr.bod = 
			  ((int)((mini_data_hdr.hdr.bod / 65)) + 1) * 64;


		}		/* else block 1000 not there */

		 
                /* update the start of data - first 64 byte boundary
                 * after any and all data blockettes
                 */
                fsdh.hdr.bofb = mini_data_hdr.hdr.bofb;
                fsdh.hdr.bod = mini_data_hdr.hdr.bod;

                fsdh.hdr.number_blockettes =
                        mini_data_hdr.hdr.number_blockettes;

		lrecl = (2 << (((struct data_blk_1000 *)p)->rec_length - 1));

		/* write the Steim Compressed data */

		if (data_hdr->nsamples == 0)
		{
			int num_bytes;
			short int save_bofb;
			short int save_bod;

			/* maybe blk 201 or other */

			memset(seed_data_record, ' ', sizeof(seed_data_record));

			save_bofb = fsdh.hdr.bofb;
			save_bod = fsdh.hdr.bod;

			if (get_word_order() == 0)      // intel
		 	{

				char *ptr;

				
				// set to zero so that swap_fsdh_back does not try to swap non-existent blks
				fsdh.hdr.bofb = 0;

				ptr = (char *)&fsdh.hdr;
 
				swap_fsdh_back(&ptr);

                 		// blockette_swap_back(mini_data_hdr.blockettes, ((char *)&mini_data_hdr.hdr) - 8);

                 		blockette_swap_back(&mini_data_hdr.blockettes);

				// add in the real bofb, see above note.
			 	fsdh.hdr.bofb = swap_2byte(save_bofb);

		 	}

			num_bytes = fwrite(&fsdh, 1, sizeof(fsdh), outfile);

			num_bytes += fwrite(&mini_data_hdr.blockettes, 1, save_bod - save_bofb, outfile);

			/* fill up the logical record */
		
			num_bytes += fwrite(seed_data_record, 1, lrecl - num_bytes, outfile);

			if (get_word_order() == 0)	// intel
			{
				blockette_swap((struct data_blk_hdr *)&mini_data_hdr.blockettes);
		 	}
			
		}
		else
		{

			j = k*(seis_buffer_length/data_hdr->num_mux_chan);

			if (output_as_doubles)
			{

				to_double_mseed(outfile, 
						&seismic_data[offset+j], 
						&fsdh,
						data_hdr->nsamples,
						lrecl,
       		 				fsdh.hdr.number_blockettes,
						mini_data_hdr.blockettes);
			}
			else
			{
				num_records= 
					Steim_comp(outfile, &seismic_data[offset+j],
						&fsdh, 
						data_hdr->nsamples, 	
						lrecl, 
						seed_data_record,
						fsdh.hdr.number_blockettes, 
						fsdh.hdr.bod - fsdh.hdr.bofb, 
						mini_data_hdr.blockettes, 
						0, 0, NULL);

				if (num_records == -1)
				{
					fprintf (stderr, "\tWARNING (output_steim):  ");
					fprintf (stderr, "failed to properly write Steim data to %s.\n",
								outfile_name);

					perror("output_mini()");

					fprintf (stderr, "\tExecution continuing.\n");
				}

			}

			current_channel = current_channel->next;

		}

	}		/* end of big for loop (for k) */

	fclose(outfile);

	chdir(orig_dir);

	return;

}				/* output_mini */

int to_double_mseed(FILE *fptr, double *seismic_data, struct Fsdh *fsdh, int nsamples, 
				int lrecl, int number_blockettes, char *blockettes)

{
	char *ptr;

	double duration;

	int now_sample;

	struct time new_time;
	struct time now_time;

	int i;
	int num_bytes;

	int finished = 0;
	
	int seqnum = 1;

	int sample_rate_multiplier;
	double sample_rate;

	char wrkstr[200];

	fsdh->hdr.nsamples = (lrecl - fsdh->hdr.bod) / sizeof(double);

	num_bytes = 0;

	now_sample = 0;

	char   Data_header_indicator;
	fsdh->Data_header_indicator = 'D';

	// since the num samples is unvarying per lrec (except for last) we
	// can calculate these values once here.

	// default number of samples for full record
	fsdh->hdr.nsamples = (lrecl - fsdh->hdr.bod) / sizeof(double);

	sample_rate = fsdh->hdr.sample_rate;

	sample_rate_multiplier = fsdh->hdr.sample_rate_multiplier;

	if (sample_rate_multiplier > 0)
		sample_rate = sample_rate * sample_rate_multiplier;
	else
	if (sample_rate_multiplier < 0)
		sample_rate = sample_rate / (-sample_rate_multiplier);

	parse_type100alt(blockettes, &sample_rate);

	duration = (fsdh->hdr.nsamples*10000.0)/sample_rate;

	while (now_sample < nsamples)
	{
		sprintf(fsdh->SequenceNumber, "%06d", seqnum++);

		// sprintf will put a \0 in the next char, so redo this
		fsdh->Data_header_indicator = 'D';

		// but if at end of the line, use only whats left
		if (now_sample + fsdh->hdr.nsamples > nsamples)
			fsdh->hdr.nsamples = nsamples - now_sample;

		num_bytes = fwrite(fsdh, 1, sizeof(struct Fsdh), fptr);

		num_bytes = fwrite(blockettes, 1, (fsdh->hdr.bod - fsdh->hdr.bofb), fptr);

		/* fill up the logical record */
		
		num_bytes += fwrite(&seismic_data[now_sample], 1, fsdh->hdr.nsamples * sizeof(double), fptr);

		// nsamples -= fsdh->hdr.nsamples;

		now_sample += fsdh->hdr.nsamples;

		now_time.year = fsdh->hdr.time.year;
		now_time.day  = fsdh->hdr.time.day;
		now_time.hour = fsdh->hdr.time.hour;
		now_time.minute = fsdh->hdr.time.minute;
		now_time.second = fsdh->hdr.time.second;
		now_time.fracsec = fsdh->hdr.time.fracsec;

		new_time = timeadd_double(now_time, duration);


		fsdh->hdr.time.year	= new_time.year;
		fsdh->hdr.time.day	= new_time.day;
		fsdh->hdr.time.hour	= new_time.hour;
		fsdh->hdr.time.minute  	= new_time.minute;
		fsdh->hdr.time.second  	= new_time.second;
		fsdh->hdr.time.fracsec 	= new_time.fracsec;

	}

	return 0;
}

/* ------------------------------------------------------------------------ */
int printSamps(double *samps, int num)
{
	int i;

	for (i = 0; i < num; i++, samps++)
		printf("%d: %6.4f\n", i, *samps);


}

/* ------------------------------------------------------------------------ */



#define FSDH_SIZE 48

void blockette_swap(struct data_blk_hdr *b_ptr)


{
	short type, next_blk_byte;

	char *base;

	base = (char *)b_ptr - FSDH_SIZE;

	while (1 == 1)
    	{
		b_ptr->type = swap_2byte(b_ptr->type);

                b_ptr->next_blk_byte = swap_2byte(b_ptr->next_blk_byte);

		switch (b_ptr->type)
		{
                	case 100 :
				{
				char *p;
				float s_rate;
				
				/* must switch the sample rate - float */
                                p = (char *)&((struct data_blk_100 *)b_ptr)->sample_rate;
 
                                *((int *)&s_rate)=swap_4byte(*((int *)p));

				((struct data_blk_100 *)b_ptr)->sample_rate = s_rate;

				}

				break;

				
                	case 201:
				{
				char *p;
				float floater;
				int inter;

				p = (char *)&((struct data_blk_201 *)b_ptr)->signal_amplitude;

                                *((int *)&floater)=swap_4byte(*((int *)p));
 
				((struct data_blk_201 *)b_ptr)->signal_amplitude = floater;
				
				p = (char *)&((struct data_blk_201 *)b_ptr)->signal_period;

                                *((int *)&floater)=swap_4byte(*((int *)p));
 
				((struct data_blk_201 *)b_ptr)->signal_period = floater;


				p = (char *)&((struct data_blk_201 *)b_ptr)->backgr_est;

                                *((int *)&floater)=swap_4byte(*((int *)p));
 
				((struct data_blk_201 *)b_ptr)->backgr_est = floater;

				/* BTIME structure */

				// year
				p = (char *)&(((struct data_blk_201 *)b_ptr)->sig_onset.year);

				 *((int *)&inter) = swap_2byte(*((int *)p));

				((struct data_blk_201 *)b_ptr)->sig_onset.year = inter;

				// day

				p = (char *)&(((struct data_blk_201 *)b_ptr)->sig_onset.day);
				
                                *((int *)&inter) = swap_2byte(*((int *)p));                                  

                                ((struct data_blk_201 *)b_ptr)->sig_onset.day = inter;

				// frac seconds

				p = (char *)&(((struct data_blk_201 *)b_ptr)->sig_onset.fracsec);
				
                                 *((int *)&inter) = swap_2byte(*((int *)p));                                  

                                ((struct data_blk_201 *)b_ptr)->sig_onset.fracsec = inter;

				}

				break;
				
                	case 300:
                	case 310:
                	case 200:
                	case 320:
                	case 390:
                	case 395:
                	case 400:
                	case 405:
			case 500:
			case 1000:
			case 1001:
				break;
			case 2000:
				{
				struct data_blk_2000 *ptr;

				ptr = (struct data_blk_2000 *)b_ptr;

				ptr->blk_length = swap_2byte(ptr->blk_length);
				ptr->opaque_offset = swap_2byte(ptr->opaque_offset);
				ptr->rec_num= swap_4byte(ptr->rec_num);

				}

				break;
                	default : /* oh, oh */
         
                    		fprintf(stderr, 
					"blockette swapper: Bad blockette scanned\n Blockette = %d\n", b_ptr->type);
                    	
				return;

		}		/* switch */
	
        	if (b_ptr->next_blk_byte == 0)
			return;

		b_ptr = (struct data_blk_hdr *)(base + b_ptr->next_blk_byte);
 
    	}   /* while */

}

/* ------------------------------------------------------------------------ */
int print_blks(b_ptr)
struct data_blk_hdr *b_ptr;
                        
{                       
                                
	char *base;             /* start of the logical rec */

	base = (char *)b_ptr - FSDH_SIZE;

        while (1)       
        {               
                /* garbage check */
                switch (b_ptr->type)
                {               
                        case 100 : printf("found blk 100's\n");
				printf("sample rate=%6.4f\n", ((struct data_blk_100 *)b_ptr)->sample_rate);

					break;

                        case 201 : printf("found blk 201's\n");
				printf("signal amplitude=%6.4f\n", ((struct data_blk_201 *)b_ptr)->signal_amplitude);
				printf("signal period=%6.4f\n", ((struct data_blk_201 *)b_ptr)->signal_period);
				printf("background est=%6.4f\n", ((struct data_blk_201 *)b_ptr)->backgr_est);

				printf("sig onset time: %d,%d %d:%d:%d.%d\n",
						((struct data_blk_201 *)b_ptr)->sig_onset.year,
						((struct data_blk_201 *)b_ptr)->sig_onset.day,
						((struct data_blk_201 *)b_ptr)->sig_onset.hour,
						((struct data_blk_201 *)b_ptr)->sig_onset.minute,
						((struct data_blk_201 *)b_ptr)->sig_onset.second,
						((struct data_blk_201 *)b_ptr)->sig_onset.fracsec);
				break;

			case 1000: printf("found blk 1000's\n");
				   break;

			case 1001: printf("found blk 1001's\n");
				   break;

                        default : /* oh, oh */

                                fprintf(stderr,
"print_blks(): Bad blockette scanned\n Blockette = %d\n", b_ptr->type);

                                return 0;

                }               /* switch */

                if (b_ptr->next_blk_byte == 0)
                        return 0;

                b_ptr = (struct data_blk_hdr *)(base + b_ptr->next_blk_byte);

        }   /* while */

        /* Should never get here */
        return 0;

}


/* ------------------------------------------------------------------------ */


char *scan_for_blk_1000(b_ptr, base)
struct data_blk_hdr *b_ptr;
char *base;		/* start of the logical rec */

{

	while (1)
    	{

		if (b_ptr->type == 1000)
			/* eureka, we've found it */
			return (char *)b_ptr;
 
        	if (b_ptr->next_blk_byte == 0)
			return (char *) NULL;

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
                	case 1001:
				break;
                	default : /* oh, oh */
         
                    		fprintf(stderr, 
"scan_for_blk_1000(): Bad blockette scanned\n Blockette = %d\n", b_ptr->type);
                    	
				return (char *) 0;

		}		/* switch */
	
		b_ptr = (struct data_blk_hdr *)(base + b_ptr->next_blk_byte);
 
    	}   /* while */

	/* Should never get here */ 
	return (char *) 0;
 
}

/* ----------------------------------------------------------------------- */
/* ------------------------------------------------------------------------ */
char *scan_for_eob(b_ptr, position)
struct data_blk_hdr *b_ptr;
int position;

{
	int count = 0;

	BOOL finished = FALSE;

	while (!finished)
	{

		count++;

        	if (b_ptr->next_blk_byte == 0)
            		finished = TRUE;
 
            	switch (b_ptr->type)
            	{
 
                /* if finished...then update the next block field so it
                 * points to where the new block 1000 will go.
		 * position is current position from start of data record,
		 * plus the sizeof the blockette just scanned
                 */
                	case 100 :
                    		if (finished)
					/* update next block field */
					b_ptr->next_blk_byte = position + 12;
 
				b_ptr = (struct data_blk_hdr *)((char *)b_ptr + 12);
				position += 12;

                    		break;
                	case 201:
                    		if (finished)
					/* update next block field */
                        		b_ptr->next_blk_byte = position + 36;
 
                    		b_ptr = (struct data_blk_hdr *)((char *)b_ptr + 36);
				position += 36;

                    		break;
                	case 300:
                	case 310:
                    		if (finished)
					/* update next block field */
                        		b_ptr->next_blk_byte = position + 32;

                    		b_ptr = (struct data_blk_hdr *)((char *)b_ptr + 32);
				position += 32;

                    		break;

                	case 200:
                	case 320:
                	case 390:
                    		if (finished)
					/* update next block field */
                        		b_ptr->next_blk_byte = position + 28;

                    		b_ptr = (struct data_blk_hdr *)((char *)b_ptr + 28);
				position += 28;

                    		break;
                	case 395:
                	case 400:
                    		if (finished)
					/* update next block field */
                        		b_ptr->next_blk_byte = position + 16;

                    		b_ptr = (struct data_blk_hdr *)((char *)b_ptr + 16);
				position += 16;

				break;

                	case 405:
                    		if (finished)
					/* update next block field */
                        		b_ptr->next_blk_byte = position + 6;

                    		b_ptr = (struct data_blk_hdr *)((char *)b_ptr + 6);
				position += 6;

                    		break;

			case 1000:
				fprintf(stderr, "output mini seed: Found unexpected blockette 1000! Output maybe incorrect.\n"); 

                    		return (char *) 0;
	
			case 1001:
				break;

                	default : /* oh, oh */
         
fprintf(stderr, "scan_for_eob(): Bad blockette scanned\n Blockette = %d\n", b_ptr->type);
                    		return (char *) 0;
 

            	}       /* switch */
 
	}   /* while */

	/* Kludge alert: sometime the data header has the wrong count for
	 * number of blockettes that follow. So make consistent here 
	 */

	mini_data_hdr.hdr.number_blockettes = count;
 
	return (char *)b_ptr;
 
}

/* ------------------------------------------------------------------------ */

char *code_to_english(fmt)
char fmt;

{
	switch (fmt)
	{
		case 0: return("");

		case 1 :
			return("16-bit");
		case 2 :
			return("");
		case 3:
			return("32-bit");
        	case 4 :
			return("SUN I");
        	case 5: 
			return ("SUN D");
        	case 10: 
			return("STEIM");
        	case 11:
			return("STEIM2");
		case 12:
        	case 13: 
        	case 14: 
		/* need to find a way to differentiate btw 24, 16, etc */
			return("GEOSCOPE");
            		break; 
        	case 15:
			fprintf(stderr, "Unable to support blockette 1000 data type! Found code = %d\n", fmt);

            		return "";
        	case 16: 
			return("CDSN");
        	case 17: 
			return("GRAEF");
        	case 18:
			fprintf(stderr, "Unable to support blockette 1000 data type! Found code = %d\n", fmt);
 
            		return "";
        	case 30:
			return("SRO G");
        	case 31: 
			fprintf(stderr, "Unable to support blockette 1000 data type! Found code = %d\n", fmt);  
 
            		return ""; 
  
        	case 32:
			return("DWWSS");
        	case 33: 
			return("RSTN");

		default:
			fprintf(stderr, "Unable to support blockette 1000 data type! Found code = %d\n", fmt);
 
                        return "";

	}

	return "";

}

/* ------------------------------------------------------------------------ */
int samples_decimal_non_zero(double * seismic_data, int nsamples)

{
	double flr;
	int i;

	for (i = 0; i < nsamples; i++)
	{
		flr = floor(seismic_data[i]);

		if ((seismic_data[i] - flr) > 0)
			return TRUE;

	}

	return FALSE;

}

