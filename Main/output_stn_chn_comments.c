
/*=================================================================*/
/* SEED reader    |         output_resp         |     subprocedure */
/*=================================================================*/
/*
	Name:	output_stn_chn_comments.c
	Purpose: writing Blockette 51s and 59s

	Usage:	void output_stn_comments()
		void output_chn_comments()

	Input:	none (gets its data from globally-available tables and files)
	Output:	comment blockettes

	Externals:
		current_station - a pointer to the SEED header 
				  tables for the station 
	   	current_channel - a pointer to the SEED header tables 
				  for the channel of the station

	Warnings: failure to properly write the comments (I/O) 
	Errors:	none
	Called by: main
	Calls to:none
	Algorithm: none

	Problems:none known
	References:

*/
#include <sys/param.h>

#include "rdseed.h"	/* SEED tables and structures */ 
#include "version.h" 
#include "resp_defs.h"


/* ----------------------------------------------------------------- */

void output_stn_comments()
{

	char orig_dir[MAXPATHLEN];

	getcwd(orig_dir, MAXPATHLEN);

	chdir(output_dir);


	for (current_station = type50_head; 
			current_station != NULL;
			current_station = current_station->next)
	{

		if (current_station->type51_head == NULL)
			fprintf(outputfile, "%sNo comments for station %s, network %s\n", 
				com_strt,
				current_station->station, 
				type10.version >= 2.3 ?
					current_station->network_code :
					"N/A");
		else
			print_type51(1);

	}

	chdir(orig_dir);

}

/* -------------------------------------------------------------------- */ 
void output_chn_comments()
{
	char orig_dir[MAXPATHLEN];

	getcwd(orig_dir, MAXPATHLEN);

	chdir(output_dir);
	
        for (current_station = type50_head;
                        current_station != NULL;
                        current_station = current_station->next)
        {

		for (current_channel=current_station->type52_head; 
				current_channel!=NULL;
                                current_channel=current_channel->next)
		{
			if (current_channel->type59_head == NULL)
			{
				fprintf(outputfile, "%sNo comments for channel %s, location %s for station %s, network %s\n", 
					com_strt,
					current_channel->channel, 
				strcmp(current_channel->location, "") != 0 ?
					current_channel->location: "??"
, 
					current_station->station,
					type10.version >= 2.3 ? 
						current_station->network_code :
						"??");

				
			}
			else
                		print_type59(current_channel->type59_head, 1); 

		}
 
 
        }

	chdir(orig_dir);

	return;

}
       
/* ------------------------------------------------------------------ */
 
void output_stn_chn_comments()
{
	char orig_dir[MAXPATHLEN];

	getcwd(orig_dir, MAXPATHLEN);

	chdir(output_dir);
	
        for (current_station = type50_head;
                        current_station != NULL;
                        current_station = current_station->next)
        {
                if (current_station->type51_head == NULL)
                        fprintf(outputfile, 
				"%sNo comments for station %s, network %s\n",
                                com_strt, 
				current_station->station, 
				type10.version >= 2.3 ?
					current_station->network_code :
					"N/A");
		else
                	print_type51(1); 

                for (current_channel=current_station->type52_head;
                                current_channel!=NULL;
                                current_channel=current_channel->next)
                {
			if (current_channel->type59_head == NULL)
			{
				fprintf(outputfile, 
"%sNo comments for channel %s, location %s for station %s, network %s\n", 
					com_strt,
					current_channel->channel, 
				strcmp(current_channel->location, "") != 0 ?
					current_channel->location : "??",
					current_station->station,
					type10.version >= 2.3 ?
						current_station->network_code :
						"N/A");

			}
			else
                        	print_type59(current_channel->type59_head, 1);

		}
 
        }

	chdir(orig_dir);

	return;

}
