
/*===========================================================================*/
/* SEED reader     |               print_type71               |     time header */
/*===========================================================================*/
/*
	Name:		print_type71
	Purpose:	print the hypocenter information table to the standard output
	Usage:		void print_type71 ();
				print_type71 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type71_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_timeh
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type71_head is the first member
				of the list
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
				04/26/93  Allen Nance     added fields 12,13,14 for v2.3
                02/17/93  CL    nullpointer protect fprintfs
				10/13/95  TJM   prefix strings added to make consistent with RESP files
*/

#include "rdseed.h"
#include "resp_defs.h"

void print_type71 ()
{
	struct type71 *type71;							/* looping vbl */
	int i;										/* counter */
	char str[80];
	char *blkt_id="B071";						/* blockette id string */

	if (type71_head != NULL)
	{
		fprintf(outputfile,"%sHypocenter information:\n",com_strt);

		for (type71 = type71_head; type71 != NULL; type71 = type71->next)
		{
			fprintf(outputfile,"%s%s%2.2d     \t%-40s%s\n",
					blkt_id,fld_pref,3,"Event origin time:", 
					type71->origin_time ? type71->origin_time : "(null)");

			sprintf(str, "Hypocenter source lookup:    %4d", type71->source_code);

			fprintf(outputfile,"%s%s%2.2d     \t%-40s",
					blkt_id,fld_pref,4,str);

			find_type32(outputfile,type71->source_code);

			fprintf(outputfile,"%s%s%2.2d     \t%-40s%f\n",
					blkt_id,fld_pref,5,"Event latitude:", type71->latitude);
			fprintf(outputfile,"%s%s%2.2d     \t%-40s%f\n",
					blkt_id,fld_pref,6,"Event longitude:", type71->longitude);
			fprintf(outputfile,"%s%s%2.2d     \t%-40s%f\n",
					blkt_id,fld_pref,7,"Event depth (km):", type71->depth);
			for (i = 0; i < type71->number_magnitudes; i++)
			{
				fprintf(outputfile,"%s%s%2.2d     \t%-40s%f\n", 
							blkt_id,fld_pref,9,"Magnitude", 
							type71->magnitude[i].magnitude);

				fprintf(outputfile,"%s%s%2.2d     \t%-40s%s\n", 
						blkt_id,fld_pref,10,"  Type for above:",  
						type71->magnitude[i].type ? type71->magnitude[i].type : "(null)");

				sprintf(str, "  Source lookup:    %4d", type71->magnitude[i].source_code);

				fprintf(outputfile,"%s%s%2.2d     \t%-40s",
						blkt_id,fld_pref,11,str);

				find_type32(outputfile,type71->magnitude[i].source_code);

			}
			if (type10.version >= 2.3)
			{
				fprintf(outputfile,"%s%s%2.2d     \t%-40s%d\n", 
						blkt_id,fld_pref,12,"Flinn-Engdahl Seismic Region:", 
						type71->seismic_region);
				
				fprintf(outputfile,"%s%s%2.2d     \t%-40s%d\n", 
						blkt_id,fld_pref,13,"Flinn-Engdahl Seismic Location:", 
						type71->seismic_location);

				fprintf(outputfile,"%s%s%2.2d     \t%-40s%s\n", 
						blkt_id,fld_pref,14,"Flinn-Engdahl Standard Name:", 
						type71->region_name ? type71->region_name : "(null)");
			}

			fprintf(outputfile,"%s\n",com_strt);

			if (type71->type72_head != NULL) print_type72 (type71->type72_head);

		}
	}
}
