/*===========================================================================*/
/* SEED reader     |             init_data_hdr             |    process data */
/*===========================================================================*/
/*
	Name:		init_data_hdr
	Purpose:	initialize internal SEED data header prior to use
	Usage:		void init_data_hdr ();
				init_data_hdr ();
	Input:		none
	Output:		sets values in data header structure to nulls
	Externals:	none
	Warnings:	none
	Errors:		memory allocation failure
	Called by:	main
	Calls to:	none
	Algorithm:	give values to variables in structures
	Notes:		none
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
*/

#include "rdseed.h"
#include <stdlib.h>

void init_data_hdr ()
{

/*                 +=======================================+                 */
/*=================|     allocate space for data header    |=================*/
/*                 +=======================================+                 */

	/* allocate space for a data_hdr record */
	if ((data_hdr = (struct data_hdr *) malloc (sizeof (struct data_hdr)))
		== NULL)
		allocation_error ("init_data_hdr");
	
/*                 +=======================================+                 */
/*=================|   initialize information for header   |=================*/
/*                 +=======================================+                 */

	/* station information */
	if ((data_hdr->station  = malloc (6)) == NULL) allocation_error ("init_data_hdr");
	if ((data_hdr->location = malloc (4)) == NULL) allocation_error ("init_data_hdr");
	if ((data_hdr->channel = malloc  (4)) == NULL) allocation_error ("init_data_hdr");

	data_hdr->station[0] = '\0';
	data_hdr->location[0] = '\0';
	data_hdr->channel[0] = '\0';

	/* time information */
	data_hdr->time.year    = 0;
	data_hdr->time.day     = 0;
	data_hdr->time.hour    = 0;
	data_hdr->time.minute  = 0;
	data_hdr->time.second  = 0;
	data_hdr->time.fracsec = 0;

	/* numerical information */
	data_hdr->nsamples     = 0;
	data_hdr->sample_rate  = 0;

	/* flags */
	data_hdr->activity_flags = '\0';
	data_hdr->io_flags       = '\0';
	data_hdr->data_quality_flags = '\0';

	/* time information for continuity tests */
	last_time.year    = 0;
	last_time.day     = 0;
	last_time.hour    = 0;
	last_time.minute  = 0;
	last_time.second  = 0;
	last_time.fracsec = 0;
	last_nsamples     = 0;
	last_sample_rate  = 0;

}
