/*--------------------------------------------------------------------------*\
    Name:	encode_steim.c

    Purpose:	translate signed long integers to steim compressed format.

    Usage:
		long Steim_comp( float * p_dbuf;
				 DATA_HEADER * p_fsdh;
				 ULONG Number_of_samples ;
				 WORD data_rec_length;
				 LONG * p_seed_data_records;

		note: HUGE is used for 80x86 based machines to force
		      normalization of pointers. It is #defined null
		      for use on real hardware.

    Output:	An array of data records containing steim compressed
		data.

    Externals:	char verbose - enables display of processing messages.

    Called by:	main.c, or your favourite program.

    Calls:	memcpy, printf

    Algorithm:	The compressor is implemented as a Deterministic
		Finite Automaton.  A second DFA takes over when
		the input tape (the raw data) ends.  The transition
		table for the DFA is -

	   note: _f signifies a final state.
	   ----------------------------------------------------------
			  | # of    |		     | # of  |
			  | bytes   |		     | DIFS  |
			  | DIF     |		     | to    | DIF
	   Current state  | fits in | New State      | unget | index
	   ---------------+---------+----------------+-------+-------
	   _START_STATE   |  1	    | _D1	     | 0     | 0
	   _START_STATE   |  2	    | _D2	     | 0     | 0
	   _START_STATE   |  4	    | _D4_f	     | 0     | 0
		    _D1   |  1	    | _D1_D1	     | 0     | 1
		    _D1   |  2	    | _D2_D2_f	     | 0     | 1
		    _D1   |  4	    | _D4_f	     | 1     | -1
		    _D2   |  1	    | _D2_D2_f	     | 0     | 1
		    _D2   |  2	    | _D2_D2_f	     | 0     | 1
		    _D2   |  4	    | _D4_f	     | 1     | -1
		 _D1_D1   |  1	    | _D1_D1_D1      | 0     | 2
		 _D1_D1   |  2	    | _D2_D2_f	     | 1     | -1
		 _D1_D1   |  4	    | _D2_D2_f	     | 1     | -1
	      _D1_D1_D1   |  1	    | _D1_D1_D1_D1_f | 0     | 3
	      _D1_D1_D1   |  2	    | _D2_D2_f	     | 2     | -1
	      _D1_D1_D1   |  4	    | _D2_D2_f	     | 2     | -1
	   ----------------------------------------------------------

    Problems:	None known.

    Language:	C, ANSI standard.

    Notes:	This program was developed under OS/2 on a harris 80x86
		machine.  It can be run in DOS or OS/2 large model
		with HUGE (greater than 1 meg) data segments. To do
		this use -DHUGE in the cl command line.

    Author:	Guy Stewart, Round Rock TX  (512) 244-9081
		IRIS	     Austin TX	    (512) 471-0405

    Revision:	04/26/1991  G. Stewart	 Initial preliminary release 0.9
		05/01/1991  G. Stewart	 First release of version 1.0
		10/15/1992  A. Nance     Added parms for data record blockettes
                                 primarily blk 100
                                 Parms added:
				num_cont_blk, num_cont_blk_bytes, cont_blk,
				num_once_blk, num_once_blk_bytes, once_blk)

		04/12/95    CL - added some bug fixes from Neuberger
		03/27/97    CL - small bug fix in update_fsdh - fix for leap

\*--------------------------------------------------------------------------*/

#include <stdio.h>
#include <memory.h>    /* for memcpy */

#include <time.h>

#include "steim.h"

#define isaleap(year) (((year%100 != 0) && (year%4 == 0)) || (year%400 == 0))

time_t compute_time(int yy, int ddd, int h, int m, int s);

char verbose=0x00 ;

#define TRUE 1
#define FALSE 0

#define _START_STATE	0
#define _D1		1
#define _D2		2
#define _D1_D1		3
#define _D1_D1_D1	4
#define _D4_f		5
#define _D2_D2_f	6
#define _D1_D1_D1_D1_f	7


/*-------------------------------------------------------------------------*\
    _DATA_STATE - structure used to store the state of the current
		  fixed section data header.
\*-------------------------------------------------------------------------*/
typedef struct _DATA_STATE {
    int get_new_x0 ; /* set TRUE if new x0 is needed		*/
    LONG unused ;
    LONG  x0,	     /* forward integration constant (x sub 0)	*/
	  xN ;	     /* reverse integration constant (x sub n)	*/
    LONG w0 ;	     /* storage for all cks for a frame 	*/
    ULONG num_data_rec ;
    ULONG data_rec_length ;
    ULONG seed_frame ;
    ULONG seed_index ;
    ULONG record_offset ;
    int frames_per_record ;
    double sample_rate ;
    double left_over ;

    ULONG num_cont_blk;
    ULONG num_cont_blk_bytes;
    char *cont_blk;
    ULONG num_once_blk;
    ULONG num_once_blk_bytes;
    char *once_blk;
    } DATA_STATE ;

int final[] = {
    0,
    0,
    0,
    0,
    0,
    1,
    1,
    1
    } ;

typedef struct _TRANSITION {
    int new_state ;
    int unget ;
    int dif_index ;
    } TRANSITION ;


TRANSITION transition[] = {
     _D1	    , 0,  0,
     _D2	    , 0,  0,
     _D4_f	    , 0,  0,
     _D1_D1	    , 0,  1,
     _D2_D2_f	    , 0,  1,
     _D4_f	    , 1,  -1,
     _D2_D2_f	    , 0,  1,
     _D2_D2_f	    , 0,  1,
     _D4_f	    , 1,  -1,
     _D1_D1_D1	    , 0,  2,
     _D2_D2_f	    , 1,  -1,
     _D2_D2_f	    , 1,  -1,
     _D1_D1_D1_D1_f , 0,  3,
     _D2_D2_f	    , 2,  -1,
     _D2_D2_f	    , 2,  -1
    } ;


/*-------------------------------------------------------------------------*\
   DISPLAY HEADER for debugging and profiling
\*-------------------------------------------------------------------------*/
void display_header( p_fsdh )
DATA_HEADER * p_fsdh;
    {
    printf("-------------------------------------------\n");
    printf("- SequenceNumber[6]              %6.6s   \n", p_fsdh->SequenceNumber		   );
    printf("- Data_header_indicator          %c      \n", p_fsdh->Data_header_indicator 	   );
    printf("- Reserved_bytes_A               %c      \n", p_fsdh->Reserved_bytes_A		   );
    printf("- Station_identifier_code[5]     %5.5s   \n", p_fsdh->Station_identifier_code	   );
    printf("- Location_identifier[2]         %2.2s   \n", p_fsdh->Location_identifier		   );
    printf("- Channel_identifier[3]          %3.3s   \n", p_fsdh->Channel_identifier		   );
    printf("- Reserved_bytes_B[2]            %2.2s   \n", p_fsdh->Reserved_bytes_B		   );
    printf("- Record_start_time\n" ) ;
    printf("                     -     year: %hu\n", p_fsdh->Record_start_time.year );
    printf("  -                         day: %hu\n", p_fsdh->Record_start_time.day );
    printf("  -                       hours: %hu\n", (short) p_fsdh->Record_start_time.hours );
    printf("  -                     minutes: %hu\n", (short) p_fsdh->Record_start_time.minutes );
    printf("  -                     seconds: %hu\n", (short) p_fsdh->Record_start_time.seconds );
    printf("  -            1/10,000 seconds: %hu\n", p_fsdh->Record_start_time.frac_secs );
    printf("- Number_of_samples              %hu     \n", p_fsdh->Number_of_samples		   );
    printf("- Sample_rate_factor             %hd     \n", p_fsdh->Sample_rate_factor		   );
    printf("- Sample_rate_multiplier         %hd     \n", p_fsdh->Sample_rate_multiplier	   );
    printf("- Activity_flags                 %d      \n", p_fsdh->Activity_flags		   );
    printf("- IO_flags                       %d      \n", p_fsdh->IO_flags			   );
    printf("- Data_quality_flags             %d      \n", p_fsdh->Data_quality_flags		   );
    printf("- Number_of_blockettes_follow    %d      \n", p_fsdh->Number_of_blockettes_follow	   );
    printf("- Time_correction                %ld     \n", p_fsdh->Time_correction		   );
    printf("- Beginning_of_data              %hd     \n", p_fsdh->Beginning_of_data		   );
    printf("- First_blockette                %hd     \n", p_fsdh->First_blockette		   );
    }

/*-------------------------------------------------------------------------*\
   STATISTICS function for debugging profiling
\*-------------------------------------------------------------------------*/
statistics( stat, tossed, Number_of_samples )
ULONG stat[4] ;
ULONG tossed ;
ULONG Number_of_samples ;
    {
    ULONG total_bytes ;

    printf("-------------------------------------------\n");
    printf("# of 4:1 = %lu\n", stat[1]);
    printf("# of 2:1 = %lu\n", stat[2]);
    printf("# of 1:1 = %lu\n", stat[3]);

    total_bytes = (stat[1] + stat[2] + stat[3]) * 4 ;
    printf(" total bytes = %lu\n", total_bytes ) ;
    printf("total deltas = %lu\n", stat[1]*4 + stat[2]*2 + stat[3]) ;
    printf("* comp ratio = %f : 1.0\n", (double)((double)Number_of_samples)/((double)total_bytes/4) ) ;
    printf("Slots tossed = %lu\n", tossed ) ;
    printf(" Ideal ratio = %f : 1.0\n", (double)((double)Number_of_samples)/((double)(total_bytes-tossed/4)/4) ) ;
    printf("\n\n* ratio does not include space used by headers, codes or\n");
    printf(    "  integration constants\n");

    }

/*-------------------------------------------------------------------------*\
   EVAL_RATE Compute the Sample rate in SPS (Samples Per Seconds
\*-------------------------------------------------------------------------*/
double eval_rate(Sample_rate_factor, Sample_rate_multiplier )
WORD Sample_rate_factor ;
WORD Sample_rate_multiplier ;
    {
    if ((Sample_rate_factor > 0) && (Sample_rate_multiplier > 0)) {
	return (double)Sample_rate_factor * (double)Sample_rate_multiplier ;
	}
    else if ((Sample_rate_factor > 0) && (Sample_rate_multiplier < 0)) {
	return (double)-Sample_rate_factor / (double)Sample_rate_multiplier ;
	}
    else if ((Sample_rate_factor < 0) && (Sample_rate_multiplier > 0)) {
	return (double)-Sample_rate_multiplier / (double)Sample_rate_factor ;
	}
    else if ((Sample_rate_factor < 0) && (Sample_rate_multiplier < 0)) {
	return (double)Sample_rate_multiplier / (double)Sample_rate_factor ;
	}
    }

/*-------------------------------------------------------------------------*\
    UPDATE FSDH TIME - compute ending time of data block using info
		       in the fsdh header.
\*-------------------------------------------------------------------------*/
void update_fsdh_time( p_fsdh, ds )
DATA_HEADER * p_fsdh ;
DATA_STATE * ds ;

{
    LONG delta_secs, delta_frac_secs ; ULONG delta;
    double delta_time ; /* change in time in 0.0001 seconds */

    /*-----------------------------------------------------------*\
	Setup delta time in seconds & fraction of seconds
    \*-----------------------------------------------------------*/

    delta_time = ((double)p_fsdh->Number_of_samples*10000.0 / ds->sample_rate) + ds->left_over;

    delta = (ULONG) delta_time;

    ds->left_over = delta_time - (double)delta;

    delta_secs = (ULONG)delta_time/10000 ;
    delta_frac_secs = (LONG) (delta_time-((double)delta_secs*10000.0)) ;

    /*-----------------------------------------------------------*\
	bubble delta_time through btime structure
	assigning times to Record start times.
    \*-----------------------------------------------------------*/
    delta_frac_secs += p_fsdh->Record_start_time.frac_secs ;
    p_fsdh->Record_start_time.frac_secs = delta_frac_secs % 10000 ;

    delta_secs += (delta_frac_secs - p_fsdh->Record_start_time.frac_secs)/10000 ;
    delta_secs += p_fsdh->Record_start_time.seconds ;
    p_fsdh->Record_start_time.seconds = delta_secs % 60 ;

    delta_secs = p_fsdh->Record_start_time.minutes + delta_secs / 60 ;
    p_fsdh->Record_start_time.minutes = delta_secs % 60 ;

    delta_secs = p_fsdh->Record_start_time.hours + delta_secs / 60 ;
    p_fsdh->Record_start_time.hours = delta_secs % 24 ;

    delta_secs = p_fsdh->Record_start_time.day + delta_secs / 24 ;

    p_fsdh->Record_start_time.day = delta_secs % (isaleap(p_fsdh->Record_start_time.year) ? 367 : 366);

	if (p_fsdh->Record_start_time.day == 0)	// rolled over to a new year
	{
		p_fsdh->Record_start_time.day = 1;
		p_fsdh->Record_start_time.year++;
	}

    }

/*-------------------------------------------------------------------------*\
    FINISH_RECORD - fill in the record fixed section data header.
\*-------------------------------------------------------------------------*/
finish_record( fp, p_seed_data_records, p_fsdh, ds )
FILE *fp;
int *p_seed_data_records;
DATA_HEADER * p_fsdh ;
DATA_STATE * ds ;
{
	char tmp[10];

	DATA_HEADER tmp_fsdh;

	char blks[ds->num_cont_blk_bytes];

	/* copy record header */
	if (verbose) {
		display_header(p_fsdh);
	}
	
	if (get_word_order() == 0)	// intel
	{
		ds->x0 = swap_4byte(ds->x0);
		ds->xN = swap_4byte(ds->xN);	
	}

	p_seed_data_records[ds->record_offset+1] = ds->x0 ;
	p_seed_data_records[ds->record_offset+2] = ds->xN ;

	if (get_word_order() == 0)	// intel
	{
         	ds->x0 = swap_4byte(ds->x0);
		ds->xN = swap_4byte(ds->xN);
	}


	ds->get_new_x0 = TRUE ;

	sprintf(tmp, "%06d", ds->num_data_rec + 1);

	memcpy(p_fsdh->SequenceNumber, tmp, strlen(tmp));

	memcpy(blks, ds->cont_blk, ds->num_cont_blk_bytes);
	memcpy((char *)&tmp_fsdh, (char *)p_fsdh, sizeof(DATA_HEADER));

	if (get_word_order() == 0)      // intel
        {
         
		swap_DATA_HEADER(&tmp_fsdh);

		// blockettes were already swapped
		blockette_swap_back((char *)blks);

	}

	memcpy((char *)p_seed_data_records+p_fsdh->First_blockette, 
			(char *)blks, ds->num_cont_blk_bytes);

	memcpy((char *)p_seed_data_records, (char *)&tmp_fsdh, sizeof(DATA_HEADER));

	// memcpy((char *)p_seed_data_records+p_fsdh->First_blockette, 
	//		(char *)ds->cont_blk, ds->num_cont_blk_bytes);

	// memcpy((char *)p_seed_data_records, (char *)p_fsdh, sizeof(DATA_HEADER));

	    /*-----------------------------------------------------------*\
       		Write data record ...
	    \*-----------------------------------------------------------*/

	if (fwrite (p_seed_data_records, ds->data_rec_length, 1, fp) != 1)
	{
		fprintf (stderr, "\tWARNING (output_steim):  ");
		fprintf (stderr, "failed to properly write STEIM data to output file.\n");
		fprintf (stderr, "\tExecution continuing.\n");
		return(-1);
	}


	/*-----------------------------------------------------------*\
	Set up _DATA_STATE for next record ...
	\*-----------------------------------------------------------*/

	ds->num_data_rec++;

 	/* start first frame of next record */
	ds->seed_frame = 0;

	ds->seed_index = 3;   /* leave room for w0, x0, xN */
	ds->w0 = 0 ;

	/*-----------------------------------------------------------*\
       	Set up FSDH for next record ...
    	\*-----------------------------------------------------------*/
	update_fsdh_time(p_fsdh, ds );
	p_fsdh->Number_of_samples = 0 ;

	return(0);

}

/*-------------------------------------------------------------------------*\
    ADD_WORD - append a compressed word to the data record.
\*-------------------------------------------------------------------------*/
Add_word(fp, p_seed_data_records, p_fsdh, wk, ck, ds )
FILE *fp;
int *p_seed_data_records;
DATA_HEADER * p_fsdh ;
LONG wk ;
LONG ck ;
DATA_STATE * ds ;
{
	char *ptr;
	int p1, p2, p3, p4;

	int byteswap = get_word_order() == 0;	// 0 is intel

    	ds->w0 |= ck << ( 2 * ( 15 - ds->seed_index ) ) ;

	if (byteswap)
	{
		wk = swap_4byte(wk);
	}

	p_seed_data_records[ds->record_offset+(ds->seed_frame*16)+ds->seed_index] = wk ;
	ds->seed_index++;

	if (ds->seed_index > 15)
	{

		/*-----------------------------------------------------------*\
		   Finish a frame ...
		\*-----------------------------------------------------------*/
		if (byteswap)
			ds->w0 = swap_4byte(ds->w0);

		p_seed_data_records[ds->record_offset + (ds->seed_frame*16)] = ds->w0 ;

		/*-----------------------------------------------------------*\
			Start next frame ...
		\*-----------------------------------------------------------*/
		ds->seed_index = 1;
		ds->seed_frame ++;
		ds->w0 = 0;
		if (ds->seed_frame >= ds->frames_per_record)
		{
			if (-1 == finish_record(fp, p_seed_data_records, p_fsdh, ds ))
				return(-1);
	    	}
	}

	return(0);
}

/*--------------------------------------------------------------------------*/

int swap_DATA_HEADER(DATA_HEADER *data_header)

{

	data_header->Record_start_time.year = 
			swap_2byte(data_header->Record_start_time.year);

        data_header->Record_start_time.day = 
			swap_2byte(data_header->Record_start_time.day);

        data_header->Record_start_time.frac_secs = 
			swap_2byte(data_header->Record_start_time.frac_secs);

        data_header->Number_of_samples = 
			swap_2byte(data_header->Number_of_samples);

        data_header->Sample_rate_factor = 
			swap_2byte(data_header->Sample_rate_factor);

        data_header->Sample_rate_multiplier = 
			swap_2byte(data_header->Sample_rate_multiplier);

        data_header->Time_correction = 
			swap_4byte(data_header->Time_correction);

        data_header->Beginning_of_data = 
			swap_2byte(data_header->Beginning_of_data);

        data_header->First_blockette = 
			swap_2byte(data_header->First_blockette);


	return 0;

}

/*--------------------------------------------------------------------------*\


/*--------------------------------------------------------------------------*\
     STEIM COMPRESSION -
\*--------------------------------------------------------------------------*/
int Steim_comp(fp,
		p_dbuf,
		p_fsdh,
		Number_of_samples,
		data_rec_length,
		p_seed_data_records,
		num_cont_blk, num_cont_blk_bytes, cont_blk,
		num_once_blk, num_once_blk_bytes, once_blk)
FILE *fp;
double *p_dbuf;
DATA_HEADER * p_fsdh;
int Number_of_samples ;
int data_rec_length;
int *p_seed_data_records;
int num_cont_blk;		/* number of continuous blockettes */
int num_cont_blk_bytes;	/* number of bytes in continuous blockette */
char *cont_blk;			/* pointer to blockettes in every data record */
int num_once_blk;		/* number of one shot blockettes */
int num_once_blk_bytes;	/* number of bytes in one shot blockette */
char *once_blk;			/* pointer to blockettes in first data record */

{
	LONG d[4] ;
	ULONG tossed = 0 ;
	ULONG stat[4];
	LONG ck ;
	LONG wk;
	int token, i ;
	DATA_HEADER new_fsdh ;   /* buffer for new fixed section data headers */
	DATA_STATE ds ;
	ULONG dbuf_index ;
	ULONG range = Number_of_samples ;
	LONG dif ;

    	int state = _START_STATE;


    /*-----------------------------------------------------------*\
       Build the seed data record initial state
    \*-----------------------------------------------------------*/

	ds.sample_rate = eval_rate(p_fsdh->Sample_rate_factor,p_fsdh->Sample_rate_multiplier);

	if (num_cont_blk_bytes != 0) if (((struct data_blk_100 *)cont_blk)->hdr.type == 100)
	ds.sample_rate = ((struct data_blk_100 *)cont_blk)->sample_rate;

	ds.frames_per_record = (data_rec_length - p_fsdh->Beginning_of_data)/64;

	ds.record_offset = p_fsdh->Beginning_of_data / sizeof(int);

	ds.seed_frame = 0;

	ds.seed_index = 3;		/* points past initial w0, x0, xN */

	ds.w0 = 0 ;

	ds.num_data_rec = 0;

	ds.data_rec_length = data_rec_length ;

	ds.left_over = 0.0;

    /*-----------------------------------------------------------*\
       Initialize x sub -1 for use in computing the forward
       and reverse integration constants x sub 0 and x sub N.
    \*-----------------------------------------------------------*/
    ds.get_new_x0 = TRUE ;
    ds.xN = 0 ;

    ds.num_cont_blk  		= num_cont_blk;	/* # continuous blockettes */

    ds.num_cont_blk_bytes 	= num_cont_blk_bytes;	/* # bytes in continuous blocks */
    ds.cont_blk      		= cont_blk;		/* pointer to blockettes in every data record */
    ds.num_once_blk  		= num_once_blk;	/* # of one shot blockettes */
    ds.num_once_blk_bytes 	= num_once_blk_bytes;	/* number of bytes in one shot blockette */
    ds.once_blk      		= once_blk;		/* pointer to blockettes in first data record */

    /*-----------------------------------------------------------*\
       Build the blockette next_blk_byte fields
    \*-----------------------------------------------------------*/

    for (i=0; i<num_once_blk; i++)
    {
        if (((struct data_blk_100 *)once_blk)->hdr.next_blk_byte == 0) break;
	else ((struct data_blk_100 *)once_blk)->hdr.next_blk_byte += 48 + num_cont_blk_bytes;
    }

    /*-----------------------------------------------------------*\
       Copy all the values from the passed fsdh, then set
       values affected by the steim compression.
    \*-----------------------------------------------------------*/
    memcpy((char *)&new_fsdh, (char *)p_fsdh, 48 ) ;

    /* Now set static values in the new fsdh */

    /* new_fsdh.Data_header_indicator	 = input.type; */

    new_fsdh.Reserved_bytes_A		 = ' ' ;
/*  
 *  commented out so it does not overwrite the
 *  network code
 *  memcpy(new_fsdh.Reserved_bytes_B, "  ", 2); 
 */
    new_fsdh.Number_of_samples		 = 0 ;	   /* Gets updated later */

    new_fsdh.Number_of_blockettes_follow = num_cont_blk + num_once_blk;

    if (num_cont_blk || num_once_blk) 
		new_fsdh.First_blockette = p_fsdh->First_blockette;
    else 
		new_fsdh.First_blockette = 0 ;

    new_fsdh.Beginning_of_data = 
		p_fsdh->Beginning_of_data /* *ds.seed_frame */;

    for (i=0; i<4; stat[i++]=0);

    if (verbose) {
	printf("starting up the compressor ..\n");
	display_header(p_fsdh);
	}

	for (dbuf_index=0; dbuf_index<range; dbuf_index++ ) 
	{

		dif = ((int)p_dbuf[dbuf_index]) - ds.xN ;
		ds.xN = (int)p_dbuf[dbuf_index] ;

		if (ds.get_new_x0) 
		{
			ds.x0 = (int)p_dbuf[dbuf_index] ;
	    		ds.get_new_x0 = FALSE ;
	    	}


		/*-----------------------------------------------------------*\
		   Check for a one-byte dif - _D1_
		\*-----------------------------------------------------------*/
		if ((dif <= 127) && (dif >= -128)) 
		{
		    char cx = (char)dif;
		    token = 0 ;
	  	}

	/*-----------------------------------------------------------*\
	   Check for a two-byte dif - _D2_
	\*-----------------------------------------------------------*/
		else if ((dif <= 32767) && (dif > -32768)) {
		    short sx = (short)dif;
		    token = 1 ;
	    	}

	/*-----------------------------------------------------------*\
	   Must be a four-byte dif - _D4_
	\*-----------------------------------------------------------*/
		else {
		    token = 2 ;
		}

	/*-----------------------------------------------------------*\
	   Make the transition ...
	\*-----------------------------------------------------------*/
	{  

		/* (begin scope local variable tran_index) */
		int tran_index = state * 3 + token ;

		if (transition[tran_index].unget)
		{
			dbuf_index -= transition[tran_index].unget;
			ds.xN = (int)p_dbuf[dbuf_index] ;
		}

		if (-1 < transition[tran_index].dif_index) {
			d[transition[tran_index].dif_index] = dif ;
	    	}

		/* The state assignment must be done last. */
		state = transition[tran_index].new_state ;

	}  /* (end scope local variables) */

	/*-----------------------------------------------------------*\
	   Got to a final state, put values into data section ...
	\*-----------------------------------------------------------*/
	if (final[state]) {
	    switch (state) {
		case _D4_f:
		    /* one 4-byte difference (one 32 bit sample) */
		    wk = d[0];
		    ck = 3 ;
		    new_fsdh.Number_of_samples += 1 ;

		    break;

		case _D2_D2_f:
		    /* two 2-byte differences (two 16 bit samples) */

		    	wk = ((d[0]&0xFFFFL) << 16) | (d[1] & 0xFFFFL) ;
		    	// wk = ((d[1]&0xFFFFL) << 16) | (d[0] & 0xFFFFL) ;
		    	ck = 2 ;
		    	new_fsdh.Number_of_samples += 2 ;
		    	break;

		case _D1_D1_D1_D1_f:
		    /* four 1-byte differences (four 8 bit samples) */
		    wk = ((d[0]&0xFFL) << 24) | ((d[1]&0xFFL) << 16) | ((d[2]&0xFFL) << 8) | (d[3]&0xFFL) ;

		    ck = 1 ;
		    new_fsdh.Number_of_samples += 4 ;
		    break;
		}

	    stat[ck]++;
	    state = _START_STATE ;

	    if (-1 == Add_word(fp, p_seed_data_records, &new_fsdh, wk, ck, &ds )) return(-1);

	    }

	}

    /*-----------------------------------------------------------*\
       Ran out of input, decide what to do with
       the data already in the buffer ...
    \*-----------------------------------------------------------*/

    if (!final[state]) {
	switch (state) {
	    case _START_STATE:
				/* nothing !*/
				break;

	    case _D1:
				ck = 3;
				stat[ck]++;
				wk = d[0];
			
				new_fsdh.Number_of_samples += 1 ;
				break;

	    case _D2:
				ck = 3;
				stat[ck]++;
				wk = d[0];

				new_fsdh.Number_of_samples += 1 ;
				break;

	    case _D1_D1:
				ck = 2;
				stat[ck]++;

			 	wk = ((d[0]&0xFFFFL) << 16) | (d[1] & 0xFFFFL);

				new_fsdh.Number_of_samples += 2 ;
				break;

	    case _D1_D1_D1:
				ck = 2;
				stat[ck]++;

				wk = ((d[0]&0xFFFFL) << 16) | (d[1] & 0xFFFFL);

				new_fsdh.Number_of_samples += 2 ;

				/*:: call to Add_word may finish a record. */
 				ds.xN = p_dbuf[range-2] ;

				if (-1 == Add_word(fp, 
						   p_seed_data_records, 
						   &new_fsdh, wk, ck, &ds )) 
					return(-1);

				ck = 3;
				stat[ck]++;
				wk = d[2];

				new_fsdh.Number_of_samples += 1 ;

				/*:: Get new X0 value if necessary.    */
 				/*:: Explicitly set xN to last value.  */
 				/*:: We may be starting a new record.  */
 				if (ds.get_new_x0) {
 					ds.x0 = p_dbuf[range-1] ;
 					ds.get_new_x0 = FALSE ;
 				}
 				ds.xN = p_dbuf[range-1];

				break;

	    }		/* switch */

		if (-1 == Add_word(fp, p_seed_data_records, 
				&new_fsdh, wk, ck, &ds )) 
			return(-1);
	}			/* if */

    	if (new_fsdh.Number_of_samples) {
		/*-----------------------------------------------------------*\
		   Finish the frame ...
		\*-----------------------------------------------------------*/

		if (get_word_order() == 0)
			ds.w0 = swap_4byte(ds.w0);

		p_seed_data_records[ds.record_offset+(ds.seed_frame*16)] = ds.w0 ;

		/*-----------------------------------------------------------*\
		   Finish the record ...
		\*-----------------------------------------------------------*/
		finish_record(fp, p_seed_data_records, &new_fsdh, &ds ) ;
	}

	/* display_results */
	if (verbose) 
	{
		statistics( stat, tossed, Number_of_samples );
	}

	return ds.num_data_rec ;
}
