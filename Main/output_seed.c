
/*===========================================================================*/
/* SEED reader     |            output_seed                |    subprocedure */
/*===========================================================================*/
/*
				
	Problems:	none known
	Language:	C, hopefully ANSI standard
	Author:		Chris Laughbon	
	Revisions:	August 11, 95 - changed path_74 to output the record
			properly
*/

#include "rdseed.h"					/* SEED tables and structures */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>


#define SEED_VOL_FNAME		"seed.rdseed"

#define DATA_FILE 		"seed.data"
#define FILE_70_NAME 		"seed.070"
#define STATION_FRONT_NAME 	"seed.station_headers"
#define ABBREV_FNAME 		"seed.abbrev_headers"
#define VOL_FNAME		"seed.vol_headers"
#define BOOL int

static int End_recnum = 0;
static int DataRecNum = 0;
static int Start_recnum = 0;
static int Num_time_spans = 0;

static int this_lrecl = 0;

/* used in type 10 generation */
static struct time beg_time = {9999,0,0,0,0, 0};
static struct time end_time = {0,0,0,0,0, 0};

struct station_list
{
	char station[5];
	struct station_list *next;
};

extern struct data_blk_1000 *scan_for_blk_1000();
static struct station_list *s_listhead = NULL;

static struct station_list *s_listtail = NULL; 

extern struct stn_tspan *stn_listhead;

struct tspan_list {
	char beg_time[23];
	char end_time[23];
	struct tspan_list *next;
};

static struct tspan_list *t_span_head = NULL;
static struct tspan_list *t_span_tail = NULL;

static BOOL got_a_time_series = FALSE;


/* ---------------------------------------------------------------------- */
/* absconded from structures.h, network code moved to end of record,
 * and the *next field was eliminated, as was the accelerator struct - 
 * not used. All *char were made into arrays of required size
 */
struct          local_type74            /* time series index */
{
	char 	type[3];		/* block 74 */
	char 	blk_length[4];		/* length */
    	char    station[5];             /* station name */
    	char    location[2];            /* station location */
    	char    channel[3];             /* channel name */
    	char    starttime[23];          /* start time of time series */
    	char 	start_index[6];         /* index to start of data */
    	char    start_subindex[2];      /* start subindex number */
    	char    endtime[23];            /* end time of time series */
    	char	end_index[6];           /* index to end of data */
    	char	end_subindex[2];        /* end subindex number */
    	char 	number_accelerators[3]; /* number of data pieces */
    	char    network_code[2];        /* v2.3 network code */
};

/* --------- Prototypes ------------- */
struct type50 *get_station_rec();
void fix_rec_length();
void position_to_single_record_read(int *, char *, FILE *);

void chdir();

off_t get_file_size();
char *get_date();

/* ---------------------------------------------------------------------- */

void output_seed_data_file(input_data_ptr)
char *input_data_ptr;

{

	char orig_dir[MAXPATHLEN];

	/* fixed data header part */
    	struct input_data_hdr *data_hdr = 
				(struct input_data_hdr *)input_data_ptr; 

	FILE *outfile;		/* output file pointer */

        getcwd(orig_dir, MAXPATHLEN);

  	chdir(output_dir);

	if ((outfile = fopen(DATA_FILE, "a")) == NULL)
	{
		fprintf (stderr, "\tWARNING (output_seed):  ");
		fprintf (stderr, "Output file %s is not available for writing.\n",
			DATA_FILE);
		fprintf (stderr, "\tExecution continuing.\n");
		
		chdir(orig_dir);

		return;
	}

	// save the current log rec size for update_type74, may change btw calls
	this_lrecl = LRECL;

	if (fwrite("000000", 1, 6, outfile) != 6)
	{
		fprintf (stderr, "\tWARNING (output_seed):  ");
        	fprintf (stderr, "Output file %s is not available for writing.\n", DATA_FILE);
        	fprintf (stderr, "\tExecution continuing.\n");

		chdir(orig_dir);

        	return;
	}

	if (fwrite(&input.type, 1, 1, outfile) != 1)
	{
		fprintf (stderr, "\tWARNING (output_seed):  ");
        	fprintf (stderr, "Output file %s is not available for writing.\n", DATA_FILE);
        	fprintf (stderr, "\tExecution continuing.\n");

		chdir(orig_dir);

        	return;
	}

	if (fwrite(" ", 1, 1, outfile) != 1)
	{
		fprintf (stderr, "\tWARNING (output_seed):  ");
        	fprintf (stderr, "Output file %s is not available for writing.\n", DATA_FILE);
        	fprintf (stderr, "\tExecution continuing.\n");

		chdir(orig_dir);

        	return;

	}

	if (fwrite(input_data_ptr, 1, LRECL - 8, outfile) != LRECL - 8) 
	{ 
        	fprintf (stderr, "\tWARNING (output_seed):  "); 
        	fprintf (stderr, "Output file %s is not available for writing.\n", DATA_FILE); 
        	fprintf (stderr, "\tExecution continuing.\n"); 

		chdir(orig_dir);

		return; 
	}

	fclose(outfile);

	/* set the record count variable to reflect new logical record */

	DataRecNum += LRECL;

	// if (DataRecNum % (2 << (type10.log2lrecl - 1)) == 0)
	if (DataRecNum % (2 << (15 - 1)) == 0)
	{
		End_recnum++;
	 	DataRecNum = 0;
	}

	chdir(orig_dir);

	return;

}				/* output_seed */

/* -------------------------------------------------------------------------- */
/* this routine is called br parse_type71() */

void update_type71(b_71)
char *b_71;

{
	FILE *fptr;
        char wrkstr[20];
	int length;

	char orig_dir[MAXPATHLEN];

        getcwd(orig_dir, MAXPATHLEN);

	chdir(output_dir);

	/* get block size - Always byte 3 four four bytes */
	sprintf(wrkstr, "%4.4s", b_71 + 3);

	length = atoi(wrkstr);

        /* out to file */
        fptr = fopen(FILE_70_NAME, "a");

        if (fptr == NULL)
        {
                fprintf(stderr, "Warning!  update_type71: unable to open the file: %s.\n", FILE_70_NAME);

                perror("update_type71");

		chdir(orig_dir);

                return;
        }

        if (fwrite(b_71, length, 1, fptr) != 1)
        {
		fprintf(stderr, "output_block_71: Unable write block 71s!\n");
            	perror("update_blk_71");
	
		chdir(orig_dir);
	
		return;
        }

	chdir(orig_dir);

        fclose(fptr);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* this routine is called by time_span_out every time a time span is 
 * realized (process_data). 
 */

void update_type74(hdr)
struct data_hdr *hdr;

{
	FILE *fptr;
	char wrkstr[200];

	struct station_list *station_node;
	struct tspan_list *tspan_node;

	/* create a blockette 74 with record numbers as computed above */
	int end_sub_seq_num = 4;	// default for 4096 logical rec size

	struct local_type74 t_74;

	int num_seconds;
	struct time t;

	char orig_dir[MAXPATHLEN];

        getcwd(orig_dir, MAXPATHLEN);

	/* if no station/channel information found simply exit */
	if ((current_station == NULL) || (current_channel == NULL))
		return;

	chdir(output_dir);

	FILE *outfile;		/* output file pointer */

	off_t where;
	int vol_lrecl;
	char pad[512];

	memset(pad, ' ', sizeof(pad));

	if ((outfile = fopen(DATA_FILE, "a")) == NULL)
	{
		fprintf (stderr, "\tWARNING (output_seed):  ");
		fprintf (stderr, "Output file %s is not available for writing.\n",
			DATA_FILE);
		fprintf (stderr, "\tExecution continuing.\n");
		
		chdir(orig_dir);

		return;
	}

	where = ftello(outfile);

// 	vol_lrecl = 2 << (type10.log2lrecl - 1);

	vol_lrecl = 32768;

	if ((where % vol_lrecl) != 0)
	{
		end_sub_seq_num = (where % vol_lrecl)/this_lrecl;

		// pad to logical record boundary
		while((ftello(outfile) % vol_lrecl) != 0)
		{
			if (fwrite(pad, 1, sizeof(pad), outfile) != sizeof(pad))
			{

				fprintf(stderr, "update_type74: Unable pad output file to logical record boundary!\n");


				perror("update_type74");
	
				chdir(orig_dir);
			}
		}
	}
	else 	// landed on a logrec boundary, logical record all filled. Ending sub sequence is LRECL/data_rec length
		end_sub_seq_num = vol_lrecl/this_lrecl;


	fclose(outfile);
	
	if (!already_saved(hdr->station))
	{
		/* save station information for later use */
		if ((station_node = (struct station_list *)malloc(sizeof(struct station_list))) == NULL)
		{
			fprintf(stderr, "update_type74: Out of Memory!! Can't output type 74s\n");
			chdir(orig_dir);
			return;
		}

		strcpy(station_node->station, hdr->station);
		station_node->next = NULL;

		append_linklist_element(station_node, s_listhead, s_listtail);

	}

	Num_time_spans++;

	memset((char *)&t_74, ' ', sizeof(t_74));
	sprintf(wrkstr, "074%04d", (int)sizeof(t_74));
	memcpy(t_74.type, wrkstr, 7);

	memcpy(t_74.station, hdr->station, strlen(hdr->station));

	memcpy(t_74.channel, hdr->channel, strlen(hdr->channel));

	memcpy(t_74.location, hdr->location, strlen(hdr->location));

	sprintf(wrkstr, "%d,%03d,%02d:%02d:%02d.%04d~",
				hdr->time.year, 
				hdr->time.day, 
				hdr->time.hour, 
				hdr->time.minute, 
				hdr->time.second, 
				hdr->time.fracsec);

	memcpy(t_74.starttime, wrkstr, strlen(wrkstr));

	sprintf(wrkstr, "%06d", Start_recnum);

	memcpy(t_74.start_index, wrkstr, 6);

	memcpy(t_74.start_subindex, "01", 2);
	
	/* need to compute ending time based on start time and number of samples
	 * and the sample rate.
	 */
	num_seconds = hdr->nsamples / hdr->sample_rate;

	/* multiply by 10000 moves past the fractional part...see timeadd() */
	t = timeadd_double(hdr->time, ((double)num_seconds) * 10000);  

	sprintf(wrkstr, "%d,%03d,%02d:%02d:%02d.%04d~", 
			t.year, 
			t.day, 
			t.hour, 
			t.minute, 
			t.second, 
			t.fracsec);	

	memcpy(t_74.endtime, wrkstr, strlen(wrkstr));

	/* save the starting time and ending time IF smaller or larger for
	 * type10 volume info later */
	if (timecmp(hdr->time, beg_time) < 0)
		memcpy((char *)&beg_time, (char *)&hdr->time, sizeof(struct time));

	if (timecmp(t, end_time) > 0)    
        memcpy((char *)&end_time, (char *)&t, sizeof(struct time));

	/* save tspan for later update of type12 */

	if ((tspan_node = (struct tspan_list *)malloc(sizeof(struct tspan_list))) == NULL)
	{
		fprintf(stderr, "update_type74: Out of Memory!! Can't output type 74s\n");

		chdir(orig_dir);

		return;
	}

	sprintf(tspan_node->beg_time, "%d,%03d,%d:%d:%d.%4d", 
				beg_time.year, beg_time.day,
			  	beg_time.hour, beg_time.minute,
				beg_time.second, beg_time.fracsec);

	sprintf(tspan_node->end_time, "%d,%03d,%d:%d:%d.%4d", 
               	end_time.year, end_time.day,
                end_time.hour, end_time.minute,
                end_time.second, end_time.fracsec);

	tspan_node->next = NULL;

   	append_linklist_element(tspan_node, t_span_head, t_span_tail);

	// if (Start_recnum < End_recnum)
	//	End_recnum--;

	if (DataRecNum == 0)
		End_recnum--;

    	// sprintf(wrkstr, "%06d", End_recnum - 1); 
    	sprintf(wrkstr, "%06d", End_recnum); 

    	memcpy(t_74.end_index, wrkstr, 6);

	sprintf(wrkstr, "%02d", end_sub_seq_num);

	memcpy(t_74.end_subindex, wrkstr, 2);

	memcpy(t_74.number_accelerators, "000", 3);

	if (type10.version >= 2.3)
		memcpy(t_74.network_code, 
					hdr->network,
					strlen(hdr->network));

	/* if this is changed, make sure to update waffle, which reads
	 * these fields, stn, chn, net 
	 */
	printf("Writing %s,%2.2s,%2.2s,%s from %d,%03d,%02d:%02d:%02d,%04d to %d,%03d,%02d:%02d:%02d,%04d\n", 
			hdr->station, 
			type10.version >= 2.3 ?
				hdr->network:
				"NA",
			hdr->location,
		 	hdr->channel, 
			hdr->time.year,
			hdr->time.day,
			hdr->time.hour,
			hdr->time.minute,
			hdr->time.second,
			hdr->time.fracsec,
                	t.year,
                	t.day, 
                	t.hour, 
                	t.minute, 
                	t.second, 
                	t.fracsec); 
                 

				

	/* out to file */
	fptr = fopen(FILE_70_NAME, "a");
	
	if (fptr == NULL)
	{
		fprintf(stderr, "Warning!  update_type70: unable to open the file: %s.\n", FILE_70_NAME);	

		perror("update_type74");
		chdir(orig_dir);
		return;
	}

	if (fwrite(&t_74, 
			type10.version < 2.3 ? sizeof(t_74) - 2 :sizeof(t_74), 
			1, fptr) != 1)
    	{
            	fprintf(stderr, "output_block_74: Unable write block 74s!\n");
            	perror("update_blk_74");

		chdir(orig_dir);

		return;

	}

//	if (DataRecNum % (2 << (type10.log2lrecl - 1)) != 0)
	if (DataRecNum % (2 << (15 - 1)) != 0)

	{

			DataRecNum = 0;
	}

	/* reset the starting recnum to be the ending */
	End_recnum = Start_recnum = End_recnum + 1;

	fclose(fptr);

	got_a_time_series = TRUE;	/* flag output_seed_volume so it knows 
					 * that we got at least 1 time series 
					 */

	chdir(orig_dir);

	return;
	
}
/* ------------------------------------------------------------------------ */
/* - output blockettes 	volume
 * -                  	abbreviations
 * -    	   	station/channel/responses
 * -                   	blockette 74s
 * -   		   	data file
 *
 */
void output_seed_volume()

{
	char orig_dir[MAXPATHLEN];

        getcwd(orig_dir, MAXPATHLEN);

	chdir(output_dir);

	/* do headers at 4096 */

	// LRECL = 2 << (type10.log2lrecl - 1);

	LRECL = 32768;

	if (!got_a_time_series)
	{
		chdir(orig_dir);

		return;
	}
	
	/* open up the output volume */
	if (!output_volume_headers())
	{
		chdir(orig_dir);
		return;
	}

	if (!output_abbrev_headers())
	{
		chdir(orig_dir);
		return;

	}

	if (!output_station_headers())
	{
		chdir(orig_dir);

		return;
	}

	/* fix up the various record indexes */
	if (!patch_indexes())
	{
		chdir(orig_dir);

		return;
	}

	/* wow - ready to pack em together into a volume */

	if (!pack_em())
	{
		fprintf(stderr, "Error! output_seed_volume: unable to pack output seed file!\n");

		perror("output_seed_volume");

	}

	free_station_list(&s_listhead);

 	clean_up_output_seed();  

	Start_recnum = 0;
	End_recnum = 0;
	Num_time_spans = 0;

	chdir(orig_dir);

	return;

}

/* ------------------------------------------------------------------------- */

int pack_em()

{
	FILE *fptr;
	FILE *out_fptr;

	char *buff;

 	char fname[200];
	char wrkstr[200];

    	struct station_list *s_list_ptr = s_listhead;
    	int num_bytes = 0;
    	off_t tot_bytes = 0;

	char pad[256];

	memset(pad, ' ', sizeof(pad));

	if ((buff = malloc(LRECL)) == 0)
	{
		fprintf(stderr, "Error! pack_em: no memory for buffer\n");
		perror("pack_em");
		return 0;
	}

	out_fptr = fopen(SEED_VOL_FNAME, "w");

	fptr = fopen(VOL_FNAME, "r");

	while ((num_bytes = fread(buff, 1, LRECL, fptr)) > 0)
	{
		/* one last thing we need to do is to update the control
	     	 * header with the current record count
		 */
		sprintf(wrkstr, "%06d", (int)((ftello(out_fptr) / LRECL) + 1));

		memcpy(buff, wrkstr, strlen(wrkstr));

		if (fwrite(buff, 1, num_bytes, out_fptr) != num_bytes)
		{
			free(buff);

            		fclose(fptr); 
            		fclose(out_fptr); 

			return 0;
		}

		tot_bytes += num_bytes;


	}

	fclose(fptr);

	fptr = fopen(ABBREV_FNAME, "r");

    	while ((num_bytes = fread(buff, 1, LRECL, fptr)) > 0)
    	{

		/* update control header record count */
        	sprintf(wrkstr, "%06d", (int)((ftello(out_fptr) / LRECL) + 1)); 
 
        	memcpy(buff, wrkstr, strlen(wrkstr)); 

        	if (fwrite(buff, 1, num_bytes, out_fptr) != num_bytes)
		{
			free(buff);

            		fclose(fptr); 
            		fclose(out_fptr); 

            		return 0;
		}

		tot_bytes += num_bytes;

    	}

	fclose(fptr);

    	while (s_list_ptr)
    	{
        	sprintf(fname, "%s.%s", 
			STATION_FRONT_NAME, 
			s_list_ptr->station);

		
    		fptr = fopen(fname, "r");

		while ((num_bytes = fread(buff, 1, LRECL, fptr)) > 0)
    		{

			/* update control header record count */ 
        		sprintf(wrkstr, "%06d", 
				(int)((ftello(out_fptr) / LRECL) + 1));  
  
        		memcpy(buff, wrkstr, strlen(wrkstr));  
 
        		if (fwrite(buff, 1, num_bytes, out_fptr) != num_bytes)
			{
				free(buff);

            			fclose(fptr); 
            			fclose(out_fptr); 

            			return 0;
			}
		
			tot_bytes += num_bytes;

    		}
 
    		fclose(fptr);

        	s_list_ptr = s_list_ptr->next;

    	}		/* while stations */

	fptr = fopen(FILE_70_NAME, "r");
 
    	while ((num_bytes = fread(buff, 1, LRECL, fptr)) > 0)
    	{
		/* update control header record count */ 
        	sprintf(wrkstr, "%06d", (int)((ftello(out_fptr) / LRECL) + 1));  
  
        	memcpy(buff, wrkstr, strlen(wrkstr));  
 
        	if (fwrite(buff, 1, num_bytes, out_fptr) != num_bytes)
		{
			free(buff);

            		fclose(fptr); 
            		fclose(out_fptr); 

            		return 0;
		}

		tot_bytes += num_bytes;

    	}
 
    	fclose(fptr);

	/* now the data file */

	fptr = fopen(DATA_FILE, "r");

    	while ((num_bytes = fread(buff, 1, LRECL, fptr)) > 0)
    	{

		/* this repositions and sets LRECL if needed */
		// position_to_single_record_read(&num_bytes, buff, fptr);

        	/* update control header record count */ 
        	sprintf(wrkstr, "%06d", (int)((ftello(out_fptr) / LRECL) + 1));  
  
        	memcpy(buff, wrkstr, strlen(wrkstr));  
 
        	if (fwrite(buff, 1, num_bytes, out_fptr) != num_bytes)
        	{
            		free(buff);
			fclose(fptr);
			fclose(out_fptr);

            		return 0;
        	}

		tot_bytes += num_bytes;

		fflush(out_fptr);

    	}    

	// make sure the seed volume ends on an lrecl boundary, which may not happen if the last record 
	// written is < lrecl

	if ((tot_bytes % LRECL) != 0)
	{
		while ((tot_bytes % LRECL) != 0)
		{
			if (fwrite(pad, 1, sizeof(pad), out_fptr) != sizeof(pad))
			{
				
				fprintf(stderr, "Unable to pack output seed file to LECL length!\n");
	
				fclose(fptr);
				fclose(out_fptr);

				free(buff);
				return 0;

			}

			tot_bytes += sizeof(pad);

		}

	}

    	fclose(fptr);
	fclose(out_fptr);

	free(buff);
	return 1;
	
}

/* ------------------------------------------------------------------------ */

int patch_indexes()

{

	if (!patch_70())
		return 0;

	/* now patch the volume header indexes for station recs */
	if (!patch_vol_header())
		return 0;

	return 1;
			
}

/* ------------------------------------------------------------------------- */
int patch_vol_header()

{
	char buff[800]; /* must be large enough for max size blk 10 */
	char tmp_fname[200];
	char wrkstr[100];
	int i, nbytes;

	off_t ix;
 
	FILE *fptr, *fptr_tmp;

	fptr = fopen(VOL_FNAME, "r");

	if (fptr == NULL)
	{
		fprintf(stderr, "Error! patch_vol_header: unable to open volume header output file: %s\n", FILE_70_NAME);

		perror("patch_vol_header");

		return 0;

	}

	sprintf(tmp_fname, "%s.tmp", VOL_FNAME);

	fptr_tmp = fopen(tmp_fname, "w");

	if (fptr_tmp == NULL)
	{
		fprintf(stderr, "Error! patch_vol_header: unable to open temp volume header file\n");

		perror("patch_vol_header");

		return 0;
	}

	/* read in and transfer the data record part */
	if (fread(buff, 1, 8, fptr) != 8)       /* refer to SEED manual */
	{
		fprintf(stderr, "Error! patch_vol_header: bad read\n");
		perror("patch_vol_header");
		return 0;
	}

	/* out to temp file */
	if (fwrite(buff, 1, 8, fptr_tmp) != 8)
	{
		perror("patch_vol_header");
		return 0;
	}
 
	/* read the block 10 - output to temp file */
	if (fread(buff, 1, 7, fptr) != 7) 		/* refer to SEED manual */
	{
		fprintf(stderr, "Error! patch_vol_header: bad read\n");
		perror("patch_vol_header");
		return 0;
	}

	buff[7] = 0;

	ix = atoi(&buff[3]) - 7;		/* record size - refer to manual */

	/* out to temp file */
	if (fwrite(buff, 1, 7, fptr_tmp) != 7)
	{
		perror("patch_vol_header");
		return 0;
	}

	/* now the rest */

	if (fread(buff, 1, ix, fptr) != ix)       /* */  
	{
		fprintf(stderr, "Error! patch_vol_header: bad read\n"); 
		perror("patch_vol_header"); 
		return 0; 
	}
    
	/* out to temp file */ 
	if (fwrite(buff, 1, ix, fptr_tmp) != ix)
	{
		perror("patch_vol_header"); 
		return 0;
	}
	
	/* now the block 11 */
	if (fread(buff, 1, 10, fptr) != 10)       /* refer to manual */  
	{  
		fprintf(stderr, "Error! patch_vol_header: bad read\n");    
		perror("patch_vol_header");    
		return 0;  
	} 	

	/* extract and use the number of stations */

	buff[10] = 0;


	ix = atoi(&buff[7]);		/* refer to manual */

	/* don't forget to write to temp */
	if (fwrite(buff, 1, 10, fptr_tmp) != 10)   
	{  
		perror("patch_vol_header");    
		return 0;  
	}

	for (i = 0; i < ix; i++)
	{
		int station_loc;
		if (fread(buff, 1, 11, fptr) != 11)       /* sta ID and seq #*/
  	  	{  
       		 	fprintf(stderr, "Error! patch_vol_header: bad read\n");    
        		perror("patch_vol_header");    
   		     	return 0;  
    		} 

		sprintf(wrkstr, "%5.5s", buff);

		station_loc = scan_stations(wrkstr);

		station_loc /= LRECL;

		/* since rdseed starts at rec # 1 - add one to total */
		station_loc += 1;

		sprintf(&buff[5], "%06d", station_loc);

		if (fwrite(buff, 1, strlen(buff), fptr_tmp) != strlen(buff))    
	    	{   
       		 	perror("patch_vol_header");     
        		return 0;   
    		} 

	}		/* for i to num_stations */

	/* continue with blockette 12 */
	/* we always write 63 byte block 12s */
	if (fread(buff, 1, 63, fptr) != 63)       
	{
       		fprintf(stderr, "Error! patch_vol_header: bad read\n");
		perror("patch_vol_header");
		return 0;
	}  

	ix = get_file_size(VOL_FNAME) + get_file_size(ABBREV_FNAME);

	ix += get_all_stations_fsize();


	ix /= LRECL;

	/* since rdseed starts at rec # 1 - add one to total */ 
	ix += 1;

	sprintf(&buff[57], "%06d", (int)ix);	/* seq # starts at byte 57 */
	
	/* out to temp file */
	if (fwrite(buff, 1, 63, fptr_tmp) != 63)
	{
		perror("patch_vol_header");
		return 0;
	}  
 
	/* now the rest - blank fill until LRECL boundary */
	while ((nbytes = fread(buff, 1, sizeof(buff), fptr)) != 0)
		if (fwrite(buff, 1, nbytes, fptr_tmp) != nbytes)
		{
			perror("patch_vol_header");
        		return 0;
		}

	fclose(fptr);
	fclose(fptr_tmp);


	rename(tmp_fname, VOL_FNAME);

	return 1;

 
}

/* ------------------------------------------------------------------------- */

int patch_70()

{

int num_bytes;
	
	off_t where, xx;

	struct local_type74 *t_74;

	struct station_list *s_list_ptr = s_listhead;

	char buff[32768];

	char wrkstr[200], tmp_fname[200];

	FILE *fptr, *fptr_tmp;

	/* figure out the logical record offsets of the data block , so we can 
	 * update the timespans later - mearly means adding up the file sizes
	 * of the header files.
	 * Note: file 70 lacks the record header (8 bytes per LRECL) so
	 *       add it into the soup. Also hasn't been buffed up to the 
	 * 	     LRECL boundary....
	 */
	where = get_file_size(VOL_FNAME) + 
				get_file_size(ABBREV_FNAME) + get_file_size(FILE_70_NAME);

	/* add in the stations, could be many */
	where += get_all_stations_fsize();

	/* Notice, we need to add in the block 70s size to the data record
     	 * offset added later
     	 */
    	where += 54;

	/* Notice, add in the 8 bytes per LRECLs too */
	where += (((get_file_size(FILE_70_NAME) / LRECL) + 1) * 8);

	/* since FILE_70_NAME is the only file which hasn't been buffed up to
	 * the LRECL boundary, compute it here 
	 */

	/* added in the 54 bytes to be added later */
	xx = get_file_size(FILE_70_NAME) + 54;

	if (xx < LRECL)
		where += LRECL - xx;
	else
		where += (LRECL - (xx % LRECL));

	/* where += (where % LRECL); */

	/* "where" is the # bytes, make into LRECLs */
	where /= LRECL; 

	/* add one because rdseed subtracts one - don't ask why... */
	where += 1;

    	fptr = fopen(FILE_70_NAME, "r");

    	if (fptr == NULL)
	{

		fprintf(stderr, "Error! patch_70: unable to open timespan output file: %s\n", FILE_70_NAME);

		perror("patch_70");
 
		return 0;
 
	}

	sprintf(tmp_fname, "%s.tmp", FILE_70_NAME);

	fptr_tmp = fopen(tmp_fname, "w");
 
	if (fptr_tmp == NULL)
	{
		fprintf(stderr, "Error! patch_70: unable to open temp timespan output file\n");
 
		perror("patch_70");
 
		return 0;
 
	}

	/* writeout the block 70 - always 54 bytes in length */
	sprintf(wrkstr, "0700054%1s%4d,%03d,%02d:%02d:%02d.%04d~%4d,%03d,%02d:%02d:%02d.%04d~",
                type70_head ? type70_head->flag ? type70_head->flag :"P" :"P",
                beg_time.year, beg_time.day, beg_time.hour,
                beg_time.minute, beg_time.second, beg_time.fracsec,
                end_time.year, end_time.day, end_time.hour,
                end_time.minute, end_time.second, end_time.fracsec);
 
	if (!out_to_disk(fptr_tmp, wrkstr, strlen(wrkstr), 'T'))
    	{
		fprintf(stderr, "Error! output_seed_volume: unable to write block 70 output file\n");
 
        	perror("patch_70");
 
        	return 0;
    	}

	/* read in blockette type and length */
	while (fread(buff, 7, 1, fptr) == 1)
    	{
        	int ix, type, length;
		
		sprintf(wrkstr, "%3.3s", buff);

		type = atoi(wrkstr);

		sprintf(wrkstr, "%4.4s", &buff[3]);

                length = atoi(wrkstr);

		if (type < 70 || type > 80)
		{
			fprintf(stderr, "patch_70s(), Bad blockette scanned, blockette :%d\nUnable to continue\n", type);

			fclose(fptr);

			return 0;

		}

		/* 10000 is arbitrary */
                if (length <= 0 || length > 10000)
                { 
                        fprintf(stderr, "patch_70s(), Bad blockette length scanned, blockette :%d, length: %d\nUnable to continue\n", type, length);

			fclose(fptr); 

                        return 0;

                }

		/* read in the rest of the story */ 
		if (fread(&buff[7],length - 7, 1, fptr) != 1)
		{
			fprintf(stderr, "patch_70s(), Unable to continue, bad read (I/O)\n");
			perror("patch_70()");

			fclose(fptr);
 
                        return 0;
		}


		switch (type)
		{
			case 71:
				if (!out_to_disk(fptr_tmp, buff,
							length, 'T'))
                                {

       fprintf(stderr, "Error! output_seed_volume: unable to write block 70 output file\n");
 
                                        perror("patch_70");
 
                                        return 0;

				}

				break;
	
			case 74:

				t_74 = (struct local_type74 *)buff;
		 
        			memcpy(wrkstr, 
					t_74->start_index, 
					sizeof(t_74->start_index));

        			wrkstr[sizeof(t_74->start_index)] = 0;
    
        			ix = atoi(wrkstr);
 
        			sprintf(wrkstr, "%06d", (int)(ix + where));
        			memcpy(t_74->start_index, 
					wrkstr, 	
					strlen(wrkstr));

        			/* ------------- */

        			memcpy(wrkstr, 
					t_74->end_index, 
					sizeof(t_74->end_index));

        			wrkstr[sizeof(t_74->end_index)] = 0;

				/* ### took out the ++1 add one so 
				 *      it points past the T rec 
				 */ 
        			ix = atoi(wrkstr);  

				/* ix++; */
 
        			sprintf(wrkstr, "%06d", (int)(ix + where));
        			memcpy(t_74->end_index, 
					wrkstr, 
					strlen(wrkstr));
 
        			/* out to temp file */
 
        			if (!out_to_disk(fptr_tmp, t_74, 
						  type10.version < 2.3 ? 
						sizeof(*t_74) - 2:
						     sizeof(*t_74), 'T'))
        			{
       fprintf(stderr, "Error! output_seed_volume: unable to write block 70 output file\n");
 
            				perror("patch_70");
 
            				return 0;

        			}

				break;

		}	/* switch blockette type */

 	}		/* while fread block 70 file */
 
	
	/* ----------- */

 
    	if (!fill_to_LRECL(fptr_tmp))
    	{
        	fprintf(stderr, "Error! output_seed_volume: unable to process timespan output file: %s\n", FILE_70_NAME);

        	perror("output_seed_volume"); 

		fclose(fptr);
		fclose(fptr_tmp);
 
        	return 0;    

	}

	fclose(fptr);
	fclose(fptr_tmp);

	rename(tmp_fname, FILE_70_NAME);

	return 1;

}
/* ------------------------------------------------------------------------- */

int output_station_headers()

{
	FILE *fptr;
	char fname[200];
 
	char buff[500];
 
	struct type50 *type50_ptr;
	

	type50_ptr = type50_head;

	/* find the last updated station record  - later !!!!#### */

	while (type50_ptr)
    	{

		/* this checks for user defined stations from cmd line */
		if (!chk_station(type50_ptr->station))
		{
			type50_ptr = type50_ptr->next;
			continue;
		}	

		if ((type10.version >= 2.3) && 
			!chk_network(type50_ptr->network_code))        
		{
			type50_ptr = type50_ptr->next;
			continue;
        	}

		/* this scan list as gotten from the data records parsing */
		if (!already_saved(type50_ptr->station))
		{
			type50_ptr = type50_ptr->next;
			continue;
		}
	
		/* construct header file for each station */
		sprintf(fname, "%s.%s", STATION_FRONT_NAME, type50_ptr->station);
		if ((fptr = fopen(fname, "a")) == NULL)
		{
			fprintf(stderr, "Error! output_station_headers: unable to open output file: %s!\n", fname);

			perror("output_station_headers");

			return 0;
		}

		sprintf(buff, 
			"0500000%-5s%10.6f%11.6f%7.1f%04d%03d%s~%03d%04d%02d%s~%s~%1s%-2s",
				type50_ptr->station, 
				type50_ptr->latitude,
				type50_ptr->longitude,
				type50_ptr->elevation,
				type50_ptr->reserved1,
				type50_ptr->reserved2,    
                		type50_ptr->name, 
                		type50_ptr->owner_code,
				type50_ptr->longword_order,    
                		type50_ptr->word_order, 
                		type50_ptr->start== NULL ? "" : type50_ptr->start,
				type50_ptr->end == NULL ? "" : type50_ptr->end,    
                		type50_ptr->update, 
                		type10.version >= 2.3 ? type50_ptr->network_code : "");

		fix_rec_length(buff);	
	
		if (!out_to_disk(fptr, buff, strlen(buff), 'S'))	
		{
			fprintf(stderr, "Error! output_seed_headers: unable to write station header to output file!\n");

			perror("ouput_seed_headers");

			fclose(fptr);

			return 0;

		}

		/* type 51 - station comment */
		if (!output_station_comment(type50_ptr->type51_head, fptr))
		{
			fclose(fptr);
	
			return 0;

		}

		/* channel stuff */

		if (!output_channel(type50_ptr->type52_head, fptr))
		{

			fclose(fptr);

			return 0;

		}

		/* done with this station */

		/* bump up the file to a LRECL boundary */
		if (!fill_to_LRECL(fptr))
		{
			fprintf(stderr, "ouput_station_headers: unable to write station headers, file: %s\n", fname);
			perror("ouput_station_headers");

			fclose(fptr);
		
			return 0;

		}

		fclose(fptr);

		type50_ptr = type50_ptr->next;

	}		/* while more stations */

	return 1;

}


/* ------------------------------------------------------------------------- */
int output_station_comment(type51_ptr, fptr)
struct	type51 *type51_ptr;
FILE *fptr;

{
	char buff[500];	

	while (type51_ptr)
	{
		sprintf(buff, "0510000%s~%s~%04d%06d",
					type51_ptr->start == NULL ? "" : type51_ptr->start,
					type51_ptr->end == NULL ? "" : type51_ptr->end,
					type51_ptr->comment_code,
					(int)type51_ptr->level_code);

		fix_rec_length(buff);

		if (!out_to_disk(fptr, buff, strlen(buff), 'S'))
		{
			fprintf(stderr, "Error! output_type51: unable to write station comments to file!\n");

			perror("output_type51");

			return 0;

		}

		type51_ptr = type51_ptr->next;
	}
	
	return 1;
	
}

/* ------------------------------------------------------------------------- */
int output_channel(type52_ptr, fptr) 
struct  type52 *type52_ptr;  
FILE *fptr;
 
{

	char buff[500]; 

	while (type52_ptr) 
	{ 
		/* check to make sure it's a wanted channel */
		if (!chk_channel(type52_ptr->channel))
		{
			type52_ptr = type52_ptr->next;
			continue;
		}

		sprintf(buff, "0520000%2s%-3s%04d%03d%s~%03d%03d%10.6f%11.6f%7.1f%5.1f%5.1f%5.1f%04d%02d%10.4e%10.4e%04d%s~%s~%s~%1s",
					type52_ptr->location == NULL ?
						"" : type52_ptr->location,
					type52_ptr->channel,
					type52_ptr->subchannel,
       			                type52_ptr->instrument_code,
					type52_ptr->inst_comment == NULL ?
			                        "" : type52_ptr->inst_comment,
					type52_ptr->signal_units_code,
					type52_ptr->calib_units_code,
					type52_ptr->latitude,
					type52_ptr->longitude,
					type52_ptr->elevation,
					type52_ptr->local_depth,
					type52_ptr->azimuth,
					type52_ptr->dip,
					type52_ptr->format_code,
					type52_ptr->log2drecl,
					type52_ptr->samplerate,
					type52_ptr->clock_tolerance,
					type52_ptr->reserved1,
					type52_ptr->channel_flag == NULL ?
							"" : type52_ptr->channel_flag, 
					type52_ptr->start == NULL ? "" : type52_ptr->start,
					type52_ptr->end == NULL ? "" : type52_ptr->end, 
					type52_ptr->update == NULL ?
					"" : type52_ptr->update);

		fix_rec_length(buff);

		if (!out_to_disk(fptr, buff, strlen(buff), 'S')) 
		{ 

			fprintf(stderr, "Error! output_type51: unable to write station comments to file!\n");   
 
			perror("output_type51"); 
 
			return 0; 
 
		}

		if (!output_response(type52_ptr->response_head, fptr))
			return 0;

		if (!output_channel_comment(type52_ptr->type59_head, fptr))
			return 0;

		if (!output_channel(type52_ptr->channel_update, fptr))
			return 0;
	
		type52_ptr = type52_ptr->next;
    	}

	return 1;

} 

/* ------------------------------------------------------------------------- */ 
int output_response(response, fptr)
struct response *response;
FILE * fptr;

{
	int error = 0;

	while (response)
	{
		switch (response->type)
		{
			case 'P' :
				error = output_poles_zeros(response->ptr.type53, fptr);
				break;
			case 'C' :
				error = output_coefficients(response->ptr.type54, fptr);
				break;
			case 'L' :
				error = output_list(response->ptr.type55, fptr);
				break;
			case 'G' :
				error = output_generic(response->ptr.type56, fptr);
				break;
			case 'D' :
				error = output_decimations(response->ptr.type57, fptr);
				break;
			case 'S' :
				error = output_sensitivity(response->ptr.type58, fptr);
				break;
			case 'R' :
				/* error = output_reference(response->ptr.type60, fptr); */
				break;
			case 'F' :
				error = output_FIR(response->ptr.type61, fptr);
				break;
			case 'O' :
				error = output_poly(response->ptr.type62, fptr);
				break;
		}

		if (error)
		{
			fprintf(stderr, "Error! output_response: unable to output responses to disk!\n");

			perror("output_response");

			return 0;

		}

		response = response->next;
	}

	return 1;

}

/* ------------------------------------------------------------------------- */
int output_poles_zeros(type53_ptr, fptr)
struct type53 *type53_ptr;
FILE *fptr;

{
	char *ch_ptr;
	char wrkstr[10];

	char *buff;
	
	int i, num_bytes;

	while (type53_ptr)
	{
		/* allocate room for type 53 + zeros and poles - 
		 * 40 + 48 bytes; refer to seed docs 
		 * plus one for null byte and fudge if we miss counted
		 */
		num_bytes = 	3 + 		/* type */
				4 +		/* size */
			  	1 +		/* transfer     */
				2 +		/* seq number   */
				3 +		/* input units  */
				3 +		/* output units */
				12 +		/* A0 */
				12 +		/* Normalization   */
				3  +		/* number of zeros */
				3  + 		/* number of poles */
					        /* 46 total */
			  (48 * (type53_ptr->number_zeroes + 
					type53_ptr->number_poles)) + 10;
 
		buff = malloc(num_bytes);
 
       		if (buff == NULL)
        	{
            		return 1;       /* error condition */
        	}
 
	        memset(buff, 0, num_bytes);
 

		/* --------------------------------------- */

		sprintf(buff, "0530000%1s%2d%03d%03d%12.5e%12.5e%03d",
				type53_ptr->transfer,
				type53_ptr->stage,
				type53_ptr->input_units_code,
				type53_ptr->output_units_code, 
				type53_ptr->ao_norm, 
                		type53_ptr->norm_freq,
				type53_ptr->number_zeroes);

		ch_ptr = buff + strlen(buff);
		for (i = 0; i < type53_ptr->number_zeroes; i++)
		{

			sprintf(ch_ptr, "%12.5e%12.5e%12.5e%12.5e",
				type53_ptr->zero[i].real,
				type53_ptr->zero[i].imag,
 				type53_ptr->zero[i].real_error,
 				type53_ptr->zero[i].real_error);
	
			/* moves to the end */	
			ch_ptr += 48;	

		}

		sprintf(ch_ptr, "%03d", type53_ptr->number_poles);
		ch_ptr += 3;
	
		for (i = 0; i < type53_ptr->number_poles; i++) 
		{

			sprintf(ch_ptr, "%12.5e%12.5e%12.5e%12.5e", 
					type53_ptr->pole[i].real,
					type53_ptr->pole[i].imag,
					type53_ptr->pole[i].real_error, 
					type53_ptr->pole[i].real_error);
            
			/* moves to the end */ 
			ch_ptr += 48;   

		}

		/* fix up the blockette length field */
		sprintf(wrkstr, "%04d", (int)strlen(buff));

		memcpy(&buff[3], wrkstr, strlen(wrkstr));

		if (!out_to_disk(fptr, buff, strlen(buff), 'S'))
		{
			free(buff);

			return 1;   /* error */
        	}

		/* --------------------------------------- */
	
		free(buff);

		type53_ptr = type53_ptr->next;
	
	}		/* while type 53 */

	return 0;		/* no error condition */
}

/* ------------------------------------------------------------------------- */
int output_coefficients(type54_ptr, fptr)
struct type54 *type54_ptr;
FILE *fptr;

{
	char *ch_ptr;
	char wrkstr[10];
	
	int num_bytes;

	char *buff;

	int i;

	while (type54_ptr)
	{
		/* allocate room for blockette */

		num_bytes = 	3 +		/* blockette type */
				4   	+	/* size */
				1   	+	/* response type */	
				2	+	/* stage seq #   */
				3	+	/* input units	 */
				3 	+	/* output units  */
				4	+	/* # numerators  */
				4	+	/* # denominators*/	
				(24 * (type54_ptr->number_numerators+
                                	type54_ptr->number_denominators)) + 10; 

					/* plus 1 for null byte and fudge */

		buff = malloc(num_bytes);
 
		if (buff == NULL)
		{
			return 1;       /* error condition */
		}
 
		memset(buff, 0, num_bytes);

		/* --------------------------------------- */
 
		sprintf(buff, "0540000%1s%2d%03d%03d%04d",
					type54_ptr->transfer,
					type54_ptr->stage,
					type54_ptr->input_units_code,
					type54_ptr->output_units_code,
					type54_ptr->number_numerators);
	
		/* move to end so far */ 
		ch_ptr = buff + strlen(buff);

		for (i = 0; i < type54_ptr->number_numerators; i++)
		{
 
			sprintf(ch_ptr, "%12.5e%12.5e",
					type54_ptr->numerator[i].coefficient,
					type54_ptr->numerator[i].error);
    
			/* moves to the end */
			ch_ptr += 24;
 
		}
   
		sprintf(ch_ptr, "%04d", type54_ptr->number_denominators);
		ch_ptr += 4;
 
		for (i = 0; i < type54_ptr->number_denominators; i++)
		{
			sprintf(ch_ptr, "%12.5e%12.5e",
					type54_ptr->denominator[i].coefficient,
					type54_ptr->denominator[i].error);
            
			/* moves to the end */
			ch_ptr += 24;

		}

		/* fix up the blockette length field */
		sprintf(wrkstr, "%04d", (int)strlen(buff));
		
		memcpy(&buff[3], wrkstr, strlen(wrkstr));

		/* out to file */
		if (!out_to_disk(fptr, buff, strlen(buff), 'S'))
		{
			free(buff);
 
			return 1;   /* error */
		}

		/* --------------------------------------- */
           
		free(buff);
 
		type54_ptr = type54_ptr->next;
    

	}       /* while type 54 */

	return 0;       /* no error condition */

}

/* ------------------------------------------------------------------------- */
int output_list(type55_ptr, fptr)
struct type55 *type55_ptr;
FILE *fptr;

{
	char *ch_ptr;
	char wrkstr[10];

	char *buff;

	int i, num_bytes;

	while (type55_ptr)
	{

		/* allocate room for response */
		num_bytes = 19 + /* type + size + seq # + in + out units + # resps */
				(60 * type55_ptr->number_responses) + 10;

		buff = malloc(num_bytes);

		if (buff == NULL)
			return 1;       /* error condition */

		memset(buff, 0, num_bytes);    

		/* --------------------------------------- */

		sprintf(buff, "0550000%2d%03d%03d%04d",
                	type55_ptr->stage,
                	type55_ptr->input_units_code,
                	type55_ptr->output_units_code,
                	type55_ptr->number_responses);

		ch_ptr = buff + strlen(buff);

		for (i = 0; i < type55_ptr->number_responses; i++)
		{

			sprintf(ch_ptr, "%12.5e%12.5e%12.5e%12.5e%12.5e",
                		type55_ptr->response[i].frequency,
                		type55_ptr->response[i].amplitude,
				type55_ptr->response[i].amplitude_error,
				type55_ptr->response[i].phase,
				type55_ptr->response[i].phase_error);

			/* moves to the end */
			ch_ptr += 60;

		}

		/* fix up the blockette length field */
		sprintf(wrkstr, "%04d", (int) strlen(buff));

		memcpy(&buff[3], wrkstr, strlen(wrkstr));

		/* out to file */
		if (!out_to_disk(fptr, buff, strlen(buff), 'S'))
		{
			free(buff);
			return 1;   /* error */
		}

        	/* --------------------------------------- */
 
        	free(buff);
 
        	type55_ptr = type55_ptr->next;
 
    	}       /* while type 55 */

	return 0;	/* no error */

}			/* output_responses */

/* ------------------------------------------------------------------------- */
int output_generic(type56_ptr, fptr)
struct type56 *type56_ptr;
FILE *fptr;
 
{
	char *ch_ptr;
	char wrkstr[10];
 
	char *buff;
 
	int i;
 
	while (type56_ptr)
	{
		/* allocate room */
		buff = malloc(19 + (24 * type56_ptr->number_corners) + 10);
 
		if (buff == NULL)
			return 1;       /* error condition */

		memset(buff, 0, 19 + (24 * type56_ptr->number_corners) + 10);
 
		/* --------------------------------------- */
 
		sprintf(buff, "0560000%2d%03d%03d%04d",
					type56_ptr->stage,
					type56_ptr->input_units_code,
					type56_ptr->output_units_code,
					type56_ptr->number_corners);

		ch_ptr = buff + strlen(buff);

		for (i = 0; i < type56_ptr->number_corners; i++)
		{

			sprintf(ch_ptr, "%12.5e%12.5e",
					type56_ptr->corner[i].frequency,
					type56_ptr->corner[i].slope);

			/* moves to the end */
			ch_ptr += 24;
 
		}
    
		/* fix up the blockette length field */
		sprintf(wrkstr, "%04d", (int)strlen(buff));

		memcpy(&buff[3], wrkstr, strlen(wrkstr));

		/* out to file */
		if (!out_to_disk(fptr, buff, strlen(buff), 'S'))
		{
			free(buff);
			return 1;   /* error */
		}

		/* --------------------------------------- */
         
		free(buff);
 
		type56_ptr = type56_ptr->next;
 
	}       /* while type 56 */

	return 0;       /* no error condition */
 
}

/* ------------------------------------------------------------------------- */
int output_decimations(type57_ptr, fptr)
struct type57 *type57_ptr;
FILE *fptr;
 
{
	char buff[80];

	while (type57_ptr)
	{
		/* as of dec 9th, 94, this record is always 51 bytes long */
		sprintf(buff, "0570051%02d%10.4e%05d%05d%11.4e%11.4e",
						type57_ptr->stage,
						type57_ptr->input_sample_rate,
						type57_ptr->decimation_factor,
						type57_ptr->decimation_offset,
						type57_ptr->delay,
						type57_ptr->correction);

		if (!out_to_disk(fptr, buff, strlen(buff), 'S'))
			return 1; /* error condition */

		type57_ptr = type57_ptr->next;
	
	}
	 
	return 0;       /* no error condition */ 
}

/* ------------------------------------------------------------------------- */
int output_sensitivity(type58_ptr, fptr)
struct type58 *type58_ptr;
FILE *fptr;
 
{
	char *ch_ptr;
	char wrkstr[10];

	char *buff;

	int i;

	while (type58_ptr)
	{
		/* allocate room for history values -  plus two for the tilde and the null byte plus fudge */
		buff = malloc(35 + (47 * type58_ptr->number_calibrations) + 10);

		if (buff == NULL)
			return 1;       /* error condition */

		memset(buff, 0, 35 + (47 * type58_ptr->number_calibrations) + 10);

		/* --------------------------------------- */
 
		sprintf(buff, "0580000%02d%12.5e%12.5e%02d",
				type58_ptr->stage,
				type58_ptr->sensitivity,
				type58_ptr->frequency,
				type58_ptr->number_calibrations);
 
		ch_ptr = buff + strlen(buff);

		for (i = 0; i < type58_ptr->number_calibrations; i++)
		{
 
			sprintf(ch_ptr, "%12.5e%12.5e%-22s~",
					type58_ptr->calibration[i].sensitivity,
					type58_ptr->calibration[i].frequency,
					type58_ptr->calibration[i].time);

			/* moves to the end */
			ch_ptr = buff + strlen(buff);

		}

		/* fix up the blockette length field */
		sprintf(wrkstr, "%04d", (int)strlen(buff));

		memcpy(&buff[3], wrkstr, strlen(wrkstr));

        	/* out to file */
		if (!out_to_disk(fptr, buff, strlen(buff), 'S'))
        	{
            		free(buff);
            		return 1;   /* error */
        	}
 
        	/* --------------------------------------- */
 
		free(buff);
 
      		type58_ptr = type58_ptr->next;

    	}       /* while type 58 */
 
	return 0;       /* no error condition */ 
}

/* ------------------------------------------------------------------------- */

int output_channel_comment(type59_ptr, fptr)
struct type59 *type59_ptr;
FILE *fptr;

{
	char buff[200];

	while (type59_ptr)
	{
		sprintf(buff, "0590000%s~%s~%04d%06d",
			type59_ptr->start == NULL ? "" : type59_ptr->start,
			type59_ptr->end == NULL ? "" : type59_ptr->end,
			type59_ptr->comment_code,
			(int)type59_ptr->level_code);

		fix_rec_length(buff);

		if (!out_to_disk(fptr, buff, strlen(buff), 'S'))
		{
			fprintf(stderr, "Error! output_channel_comment: unable to write channel comment to output file!\n");

			perror("output_channel_comment");

			return 0;		/* error condition */

		}

		type59_ptr = type59_ptr->next;
	
	}
	
	return 1;  /* no error */

}

/* ------------------------------------------------------------------------- */
int output_FIR(type61_ptr, fptr)
struct type61 *type61_ptr;
FILE *fptr;

{
	char *ch_ptr;
	char wrkstr[20];
	char *buff;

	int buff_size;

	int i;

	while (type61_ptr)
	{

		// plus 10 for the tilde and the null at end of string

		buff_size = 9 + strlen(type61_ptr->name ? type61_ptr->name : "") + 
					11 + (14 * type61_ptr->number_numerators) + 10; 


        	/* allocate room for FIR values */
        	buff = malloc(buff_size);

	        if (buff == NULL)
        		return 1;       /* error condition */

		memset(buff, 0, buff_size);

	        /* --------------------------------------- */

		sprintf(buff, "0610000%02d%s~%c%03d%03d%04d",
                	type61_ptr->stage,
			type61_ptr->name == NULL ? "" : type61_ptr->name,
                	type61_ptr->symmetry_code,
			type61_ptr->input_units_code,
			type61_ptr->output_units_code,
                	type61_ptr->number_numerators);

		ch_ptr = buff + strlen(buff);

		for (i = 0; i < type61_ptr->number_numerators; i++)
		{

	  		sprintf(ch_ptr, "%14.7e", type61_ptr->numerator[i]);

			/* moves to the end */
			ch_ptr += 14;

		}

		/* fix up the blockette length field */
		sprintf(wrkstr, "%04d", (int)strlen(buff));
		memcpy(&buff[3], wrkstr, strlen(wrkstr));

        	/* out to file */
        	if (!out_to_disk(fptr, buff, strlen(buff), 'S'))
        	{
            		free(buff);
            		return 1;   /* error */
        	}
        	/* --------------------------------------- */
 
        	free(buff);
      		type61_ptr = type61_ptr->next;
 
    	}       /* while type 61 */
 
	return 0;       /* no error condition */
}

/* ------------------------------------------------------------------------- */
int output_poly(type62_ptr, fptr)
struct type62 *type62_ptr;
FILE *fptr;

{
	char *ch_ptr;
	char wrkstr[20];
	char *buff;

	int i, num_bytes;

	while (type62_ptr)
	{
		/* allocate room for coefficient values */
		num_bytes = 81 + (24 * type62_ptr->number_coefficients) + 10;

		buff = malloc(num_bytes);

		if (buff == NULL)
			return 1;       /* error condition */

		memset(buff, 0, num_bytes);

		/* --------------------------------------- */

		sprintf(buff, "0620000%c%02d%03d%03d%c%c%12.5e%12.5e%12.5e%12.5e%12.5e%03d",
			type62_ptr->transfer_fct_type,
	                type62_ptr->stage,
			type62_ptr->input_units_code,
			type62_ptr->output_units_code,
			type62_ptr->poly_approx_type,
			type62_ptr->valid_freq_units,
			type62_ptr->lower_valid_freq,
			type62_ptr->upper_valid_freq,
			type62_ptr->lower_bound_approx,
			type62_ptr->upper_bound_approx,
			type62_ptr->max_abs_error,
	                type62_ptr->number_coefficients);

        	ch_ptr = buff + strlen(buff);
        	for (i = 0; i < type62_ptr->number_coefficients; i++)
        	{

            		sprintf(ch_ptr, "%12.5e%12.5e",
                		type62_ptr->coefficient[i].coefficient, type62_ptr->coefficient[i].error);

            		/* moves to the end */
			ch_ptr += 24;

        	}

	        /* fix up the blockette length field */
        	sprintf(wrkstr, "%04d", (int)strlen(buff));
        	memcpy(&buff[3], wrkstr, strlen(wrkstr));

        	/* out to file */
        	if (!out_to_disk(fptr, buff, strlen(buff), 'S'))
        	{
        		free(buff);
            		return 1;   /* error */
        	}
 
        	/* --------------------------------------- */
 
        	free(buff);
 
      		type62_ptr = type62_ptr->next;
 
    	}       /* while type 62 */
 
    	return 0;       /* no error condition */
}

/* ------------------------------------------------------------------------- */




int output_abbrev_headers()

{
	FILE *fptr;
	char wrkstr[200];

	char buff[5000];

	char *ch_ptr = buff;

	int i;


	struct type30    *type30_ptr = type30_head;    /* type 030 blockette */
	struct type31    *type31_ptr = type31_head;    /* type 031 blockette */
	struct type32    *type32_ptr = type32_head;    /* type 032 blockette */
	struct type33    *type33_ptr = type33_head;    /* type 033 blockette */
	struct type34    *type34_ptr = type34_head;    /* type 034 blockette */
	struct type35    *type35_ptr = type35_head;    /* type 035 blockette */

	if ((fptr = fopen(ABBREV_FNAME, "w")) == NULL)
	{
		fprintf(stderr, "output_abbrev_header: Unable to open output file!\n");
		perror("output_abbrev_header");
		return 0;
	}

	while (type30_ptr)
	{
		sprintf(ch_ptr, "0300000%s~%04d%03d%02d",
					type30_ptr->name == NULL ? "" : type30_ptr->name, 
					type30_ptr->code,
					type30_ptr->family, type30_ptr->number_keys);

		ch_ptr = buff + strlen(buff);

		for (i = 0; i < type30_ptr->number_keys; i++)
		{
			sprintf(ch_ptr, "%s~",
					 type30_ptr->decoder_key[i] == NULL ? 
							"" : type30_ptr->decoder_key[i]);
			ch_ptr = buff + strlen(buff);
		}

		fix_rec_length(buff);

		/* out to disk */
		if (!out_to_disk(fptr, buff, strlen(buff), 'A'))
		{
			fprintf(stderr, "output_abbrev_header: Unable to open output file!\n");
			perror("output_abbrev_header");
			fclose(fptr); 
        		return 0;
		}

		type30_ptr = type30_ptr->next;

		ch_ptr = buff;
		memset(buff, 0, sizeof(buff));

	}		/* while type 30 */

	ch_ptr = buff;
	memset(buff, 0, sizeof(buff));

	while (type31_ptr)
	{  
		sprintf(buff, "0310000%04d%s%s~%03d",
                    type31_ptr->code, type31_ptr->class,
                    type31_ptr->comment == NULL ? "" : type31_ptr->comment, 
					type31_ptr->units);

		fix_rec_length(buff);

		/* out to disk */
		if (!out_to_disk(fptr, buff, strlen(buff), 'A'))
		{
			fprintf(stderr, "output_abbrev_header: Unable to open output file!\n");
			perror("output_abbrev_header");
			fclose(fptr);
			return 0;
		}
	
		type31_ptr = type31_ptr->next;
		memset(buff, 0, sizeof(buff));
	
	} 		/* while type31_ptr */

	while (type32_ptr)
	{ 
		sprintf(buff, "0320000%02d%s~%s~%s~",
                    	type32_ptr->code, 
			type32_ptr->author == NULL ? "" : type32_ptr->author,
                    	type32_ptr->catalog == NULL ? "" : type32_ptr->catalog, 
			type32_ptr->publisher == NULL ? "" : type32_ptr->publisher);

		fix_rec_length(buff);
 
        	/* out to disk */
		if (!out_to_disk(fptr, buff, strlen(buff), 'A'))
		{
			fprintf(stderr, "output_abbrev_header: Unable to open output file!\n");
			perror("output_abbrev_header");
			fclose(fptr);
			return 0;
		}
        
		type32_ptr = type32_ptr->next; 
		memset(buff, 0, sizeof(buff)); 
   
	}       /* while type32_ptr */

	while (type33_ptr) 
	{  
        	sprintf(buff, "0330000%03d%s~",  
                    		type33_ptr->code, 
				type33_ptr->abbreviation  == NULL ? 
					"" : type33_ptr->abbreviation);
 
		fix_rec_length(buff);
 
		/* out to disk */  
		if (!out_to_disk(fptr, buff, strlen(buff), 'A'))    
		{  
			fprintf(stderr, "output_abbrev_header: Unable to write output file!\n");                                                                           
			perror("output_abbrev_header");
			fclose(fptr);  
			return 0;   
		}  
           
		type33_ptr = type33_ptr->next; 
		memset(buff, 0, sizeof(buff)); 
   
	}       /* while type33_ptr */ 

	while (type34_ptr)
	{   
        	sprintf(buff, "0340000%03d%s~%s~",
                    	type34_ptr->code, 
			type34_ptr->name == NULL ? "" : type34_ptr->name, 
			type34_ptr->description == NULL ? 
					"" : type34_ptr->description);
  
 		fix_rec_length(buff);
 
	        /* out to disk */   
		if (!out_to_disk(fptr, buff, strlen(buff), 'A'))
       		{   
			fprintf(stderr, "output_abbrev_header: Unable to write output file!\n");                                                                            
			perror("output_abbrev_header"); 
			fclose(fptr);   
			return 0;
		}
 
		type34_ptr = type34_ptr->next;
		memset(buff, 0, sizeof(buff));
 
	}       /* while type34_ptr */
 
	while (type35_ptr)
	{   
		sprintf(buff, "0350000%03d%04d",
				type35_ptr->code, type35_ptr->number_beams);

		ch_ptr = buff + strlen(buff);

		for (i = 0; i < type35_ptr->number_beams; i++)
		{
			sprintf(ch_ptr, "%-5s%2s%3s%04d%1.3f",
				type35_ptr->beam[i].station,
				type35_ptr->beam[i].location,
				type35_ptr->beam[i].channel,
				type35_ptr->beam[i].subchannel,
				type35_ptr->beam[i].weight);

			ch_ptr = buff + strlen(buff);

		}

 		fix_rec_length(buff);
 
	        /* out to disk */   
		if (!out_to_disk(fptr, buff, strlen(buff), 'A'))     
		{   
			fprintf(stderr, "output_abbrev_header: Unable to open output file!\n");                                                                            
			perror("output_abbrev_header"); 
			fclose(fptr);   
			return 0;
		}
 
		type35_ptr = type35_ptr->next;
		memset(buff, 0, sizeof(buff));
 
	}       /* while type35_ptr */

	if (!fill_to_LRECL(fptr))
	{
		fprintf(stderr, "output_abbrev_header: Unable to write header!\n");

		perror("output_abbrev_header");
	}

	fclose(fptr);

	return 1;
 
}

/* ------------------------------------------------------------------------- */

int output_volume_headers()

{

	FILE *fptr;

	char buff[15000];		/* holds each blockette as it is built 
					 * needs to de big enough to hold 
					 * blks 10, 11, 12 */
	char *c_ptr = buff;
	char *xc_ptr;

	char wrkstr[250];


	struct station_list *s_list_ptr;
	struct tspan_list *t_list_ptr;

	if ((fptr = fopen(VOL_FNAME, "w")) == NULL)
	{
		fprintf(stderr, "output_volume_header: Unable to open output file!\n");
		perror("output_volume_header");

		return 0;

	}
	/* build the Volume header blockettes, 10, 11, 12 */

	/* type 10 */
	sprintf(c_ptr, "0100000");
	c_ptr = buff + strlen(buff);  			/* type plus size */

//	sprintf(c_ptr, "%04.1f%02d", type10.version, type10.log2lrecl);
	sprintf(c_ptr, "%04.1f%02d", type10.version, 15);

	c_ptr = buff + strlen(buff);

	sprintf(c_ptr, "%4d,%03d,%02d:%02d:%02d.%04d~", 
				beg_time.year, beg_time.day, beg_time.hour,
				beg_time.minute, beg_time.second, beg_time.fracsec);

	c_ptr = buff + strlen(buff);

	sprintf(c_ptr, "%4d,%03d,%02d:%02d:%02d.%04d~",    
                end_time.year, end_time.day, end_time.hour,
                end_time.minute, end_time.second, end_time.fracsec);    
 
	c_ptr = buff + strlen(buff);        /* sizeof time field */	

	if (type10.version > 2.2)
	{


		sprintf(c_ptr, "%s~", get_date(wrkstr, sizeof(wrkstr)));
		c_ptr = buff + strlen(buff);
	
		sprintf(c_ptr, "%s~", type10.organization);
		c_ptr = buff + strlen(buff);

		sprintf(c_ptr, "%s~", type10.volume_label);
		c_ptr = buff + strlen(buff);

	}
	else		/* create some fields */
	{
		sprintf(c_ptr, "%s~", get_date(wrkstr, sizeof(wrkstr)));

		c_ptr = buff + strlen(buff);
 
		sprintf(c_ptr, "%s~", "IRIS DMC"); 

		c_ptr = buff + strlen(buff); 

		sprintf(c_ptr, "%s~", "generated by rdseed"); 

		c_ptr = buff + strlen(buff);
	}

	fix_rec_length(buff);

	/* blockette 11s */
	sprintf(c_ptr, "0110000%03d", get_num_stations());
	/* remember the record length spot */
	xc_ptr = c_ptr;

	c_ptr = buff + strlen(buff);

	s_list_ptr = s_listhead;

	while (s_list_ptr)
    	{
		/* leave the sequence number at zero for now, fix up later */

		sprintf(c_ptr, "%-5s000000", s_list_ptr->station);

		c_ptr = buff + strlen(buff);

		s_list_ptr = s_list_ptr->next;
    	}

	fix_rec_length(xc_ptr);

	/* now do type 12s */
	sprintf(c_ptr, "01200630001%4d,%03d,%02d:%02d:%02d.%04d~%4d,%03d,%02d:%02d:%02d.%04d~000000",
                beg_time.year, beg_time.day, beg_time.hour,
                beg_time.minute, beg_time.second, beg_time.fracsec,
                end_time.year, end_time.day, end_time.hour,
                end_time.minute, end_time.second, end_time.fracsec);

	
	if (!out_to_disk(fptr, buff, strlen(buff), 'V'))
	{
		fprintf(stderr, "output_volume_header : Unable to write volume header!\n");

		perror("output_volume_headers");

		return 0;
	}

	if (!fill_to_LRECL(fptr))
	{
		fprintf(stderr, "output_volume_header : Unable to write volume header!\n");
 
        perror("output_volume_header");
	}

	fclose(fptr);

	return 1;
 
}

/* ------------------------------------------------------------------------ */
int out_to_disk(fptr, buffer, n, type)
FILE *fptr;
char *buffer;
int n;			/* number of chars to write 		*/
char type;		/* holds 'V' for volume, 'S' etc... */

{
	int i, ix;
	char wrkstr[200];

	/* if we are in range of 7 chars of a logical record break,
	 * stop and fill with blanks to logical record, then
	 * proceed. 7 = sizeof blockette type + blockette size
	 */

	if (spans_LRECL(fptr, 7))
	{
		fill_to_LRECL(fptr);

		/* make new control header - continuous by definition */
		sprintf(wrkstr, "%06d%c*", (int)((ftello(fptr) / LRECL) + 1), type);
 
        	if (fwrite(wrkstr, 1, strlen(wrkstr), fptr) != strlen(wrkstr))
		{
			fprintf(stderr, "output_seed : Unable to write seed volume!\n");
			perror("out_to_disk");
 
			return 0;
		}

	}

	/* is it time for a control header? */
	if (((ftello(fptr) % LRECL) == 0) || (ftello(fptr) == 0))
	{
		
		sprintf(wrkstr, "%06d%c ", (int)((ftello(fptr) / LRECL) + 1), type);

		if (fwrite(wrkstr, 1, strlen(wrkstr), fptr) != strlen(wrkstr))
		{
			fprintf(stderr, "output_seed : Unable to write seed volume!\n");
			perror("out_to_disk"); 

			return 0;
		}

	}

	i = 0;

	while (i < n)
	{
		/* is it time for a control header? */
		if ((ftello(fptr) % LRECL) == 0)
		{
			sprintf(wrkstr, "%06d%c*", (int)((ftello(fptr) / LRECL) + 1), type); 
 
        		if (fwrite(wrkstr, 
				   1, 
				   strlen(wrkstr), fptr) != strlen(wrkstr)) 
        		{

            			fprintf(stderr, "output_seed : Unable to write seed volume!\n"); 
            			perror("out_to_disk"); 
 
            			return 0; 
        		}
		}		/* if time for control header */

		if ((ix = fputc(buffer[i], fptr)) != (int)((unsigned char)buffer[i]))
		{
			fprintf(stderr, "output_seed : Unable to write seed volume!\n");                
			perror("out_to_disk");  
  
			return 0;
		}

		i++;

	}			/* while i < n */

	return 1;

}

/* ----------------------------------------------------------------------- */
int already_saved(station)
char *station;

{
	struct station_list *s_list_ptr = s_listhead;

	while (s_list_ptr)
	{
		if (strcmp(s_list_ptr->station, station) == 0)
			return 1;	/* found it */

		s_list_ptr = s_list_ptr->next;

	}
	
	return 0;

}

/* ----------------------------------------------------------------------- */
int scan_stations(s)
char *s;

{
	char fname[200];
	
	struct station_list *s_list_ptr = s_listhead;
	off_t i;
	
	/* start here */
	i = get_file_size(VOL_FNAME) + get_file_size(ABBREV_FNAME);
 
	while (s_list_ptr)
	{   
		if (strncmp(s_list_ptr->station, s, strlen(s_list_ptr->station)) == 0)
			return i;

		sprintf(fname, "%s.%s", STATION_FRONT_NAME, s_list_ptr->station);

		i += get_file_size(fname);

		s_list_ptr = s_list_ptr->next;
	}
   
	/* got here, than error, not in list */
	fprintf(stderr, "Error! scan_stations: unable to patch station index in block 11 for station %s\n", s_list_ptr->station);
 
    return 0;

}

/* ----------------------------------------------------------------------- */
#include <dirent.h>

/* erase the working files - reset some variables for next time around */

int clean_up_output_seed()

{
	DIR *dirp;
	struct dirent *dp;
	
	char orig_dir[MAXPATHLEN];

        getcwd(orig_dir, MAXPATHLEN);

	chdir(output_dir);

	unlink(DATA_FILE);
 	unlink(FILE_70_NAME);
	unlink(ABBREV_FNAME);
	unlink(VOL_FNAME);

	dirp = opendir(".");
   
	for (dp = readdir(dirp); dp != NULL; dp = readdir(dirp))
   		if (strstr(dp->d_name, STATION_FRONT_NAME)) 
			unlink(dp->d_name);


	closedir(dirp);

	memset((char *)&beg_time, 0, sizeof(struct time));
	beg_time.year = 9999;

	memset((char *)&end_time, 0, sizeof(struct time)); 

	got_a_time_series = FALSE;

	chdir(orig_dir);

	return 1;

}

/* ----------------------------------------------------------------------- */
int get_all_stations_fsize()

{
	char fname[200];
    
	struct station_list *s_list_ptr = s_listhead;
	off_t num_bytes = 0;
    
	while (s_list_ptr)
	{  

		sprintf(fname, "%s.%s", STATION_FRONT_NAME, s_list_ptr->station);
 
		num_bytes += get_file_size(fname);
 
		s_list_ptr = s_list_ptr->next;
	}
   
    return num_bytes;

}

/* ------------------------------------------------------------------------- */

int get_num_stations()

{

	struct station_list *s_list_ptr = s_listhead;
	int i = 0;

	while (s_list_ptr)
	{   
		i++;
		s_list_ptr = s_list_ptr->next;
	}
    
	return i;
 
}

/* ------------------------------------------------------------------------- */

int get_num_tspans()

{
	struct tspan_list *t_list_ptr = t_span_head;
	int i = 0;

	while (t_list_ptr)
	{
		i++;
		t_list_ptr = t_list_ptr->next;
	}

	return i;

}
 
/* ----------------------------------------------------------------------- */
void fix_rec_length(b)
char *b;

{
	char wrkstr[10];

	sprintf(wrkstr,"%04d", (int)strlen(b));

	memcpy(b + 3, wrkstr, strlen(wrkstr));

	return;
}

int fill_to_LRECL(fptr)
FILE *fptr;

{
	if ((ftello(fptr) % LRECL) != 0)
	{
		while((ftello(fptr) % LRECL))
		{
			if (fputc(' ', fptr) != ' ')
				return 0;
			
		}
	}

	return 1;

}

/* --------------------------------------------------------------------- */
int spans_LRECL(fptr, span)
FILE *fptr;
int span;

{
        off_t where;

        where = ftello(fptr);

        if (((where + span) / LRECL) > (where / LRECL))
                if ((where + span) % LRECL) /* land right on boundary */
                        return TRUE;

        return FALSE;

}


/* --------------------------------------------------------------------- */

off_t get_file_size(f)
char *f;

{
	struct stat s;

	stat(f, &s);

	return s.st_size;

}

/* ---------------------------------------------------------------------- */
int free_station_list(s_node)
struct station_list **s_node;

{

	if (*s_node)
	{
		free_station_list(&((*s_node)->next));
		free(*s_node);
		*s_node = NULL;
	}

    return 1;

}

/* ---------------------------------------------------------------------- */

void position_to_single_record_read(int *num_bytes_read, char *lrecord, FILE *fptr) {

	struct input_data_hdr *hdr = (struct input_data_hdr *)(lrecord + 8);
	struct data_blk_1000 *p;
	int rec_len = 0;

	if (lrecord[6] != 'M' && lrecord[6] != 'D' && lrecord[6] != 'Q' && lrecord[6] != 'R')  {
		return;
	}
        if (hdr->bofb != 0) {
               	p = scan_for_blk_1000(lrecord + hdr->bofb, lrecord);
	} else {
		p = NULL;
	}
        if (p) {
		rec_len = (2 << (p->rec_length - 1));
        } else {
		hdr = (struct input_data_hdr *)(lrecord+8);
		rec_len = get_LRECL(hdr, lrecord);
	}
	
	if (*num_bytes_read != rec_len) {
		/* undo the read, and reread, hopefully this doesn't happen too ofetn */
		fseek(fptr, (long) -(*num_bytes_read), SEEK_CUR);
		LRECL = rec_len;
		if(*num_bytes_read = fread(lrecord, 1, LRECL, fptr) != LRECL) {
			fprintf(stderr, "position_to_single_record_read(), Unable to read complete record\n");
			perror("position_to_single_record_read()");
                        return;
		}
		fprintf(stderr,  "Repositioned for %d logical record reads.\n", LRECL);
	}

	return;
}

