/*===========================================================================*/
/* SEED reader     |              output_resp              |    subprocedure */
/*===========================================================================*/
/*
	Name:	output_resp
	Purpose:write a channel response file for current channel from the SEED
            header tables.
	Usage:	void output_resp ();
				output_resp ();
	Input:	none (gets its data from globally-available tables and files)
	Output:	none (writes a response file; files are named by
		beginning time, station, and component; for example,
		1988.023.15.34.08.2800.ANMO.SPZ.RESP is the filename for a
		seismogram from year 1988, Julian day 23, 15:34:08.2800 UT,
		recorded at station ANMO from component SPZ.  Writes message
		to stderr describing the response file being written.
	Externals:data_hdr - a table containing information about this seismogram
		current_station - a pointer to the SEED header tables for the
		station at which this seismogram was recorded
	   	current_channel - a pointer to the SEED header tables for the
		channel of the station at which this seismogram was recorded
	Warnings:unable to open response file for writing
   				failure to properly write the response data
	Errors:	none
	Called by:process_data
	Calls to:none
	Algorith:

	Problems:none known
	References:Halbert, S. E., R. Buland, and C. R. Hutt (1988).  Standard for
			the Exchange of Earthquake Data (SEED), Version V2.0,
			February 25, 1988.  United States Geological Survey,
			Albuquerque Seismological Laboratory, Building 10002,
			Kirtland Air Force Base East, Albuquerque, New Mexico
			87115.  82 pp.
			O'Neill, D. (1987).  IRIS Interim Data Distribution Format
			(SAC ASCII), Version 1.0 (12 November 1987).  Incorporated
			Research Institutions for Seismology, 1616 North Fort Myer
			Drive, Suite 1440, Arlington, Virginia 22209.  11 pp.
			Tull, J. (1987).  SAC User's Manual, Version 10.2, October 7,
			1987.  Lawrence Livermore National Laboratory, L-205,
			Livermore, California 94550.  ??? pp.
	Language:	C, hopefully ANSI standard
	Author:		Allen Nance, from skeleton by Dennis O'Neill
	Revisions: 	tjm  09/12/1995  - added resp support for evalresp
			03/30/1999 Stephane Zuzlewski - Added support for blockette 62.

*/

#include "rdseed.h"								/* SEED tables and structures */
#include "version.h"
#include "resp_defs.h"

void output_resp ()
{
	char buffer[30];	/* output file name */
	int i;										/* counter */

	chdir(output_dir);

/*                 +=======================================+                 */
/*=================|  Find selected station channels       |=================*/
/*                 +=======================================+                 */
	for (current_station = type50_head; current_station != NULL;
		current_station = current_station->next)
	{
		if (chk_station(current_station->station))
		{
			if ((type10.version >= 2.3) && 
					!chk_network(current_station->network_code))
				continue;

			for (current_channel=current_station->type52_head; current_channel!=NULL;
				 current_channel=current_channel->next)
			{
				if (chk_channel(current_channel->channel))
				{
					print_resp ();
				}
			}
		}
	}
}
	
/*===========================================================================*/
print_resp ()
{
	FILE *outfile;			/* output file pointer */
	char outfile_name[100];		/* output file name */
	int i;										/* counter */
	struct response *response;	/* looping vbl */
	char *blkt_id1="B050",*blkt_id2="B052";     /* blockette id strings */

/*                 +=======================================+                 */
/*=================|  build name for and open output file  |=================*/
/*                 +=======================================+                 */

	sprintf (outfile_name, "RESP.%s.%s.%s.%s",
		current_station->network_code ? current_station->network_code: "",
		current_station->station,
		current_channel->location,
		current_channel->channel);

	printf("Writing RESPONSE file: %s\n", outfile_name);

	if ((outfile = fopen (outfile_name, "a")) == NULL)
	{
		fprintf (stderr, "\tWARNING (output_resp):  ");
		fprintf (stderr, "Output file %s is not available for writing.\n", outfile_name);

		perror("output_resp()");

		fprintf (stderr, "\tExecution continuing.\n");
		return;
	}

	if (fprintf(outfile, "%s<< IRIS SEED Reader, Release %s >>\n%s\n", com_strt, VERSION, com_strt) == -1)
	{
		fprintf (stderr, "\tWARNING (output_resp):  ");
                fprintf (stderr, "Output file %s is not available for writing.\n", outfile_name);
 
                perror("output_resp()");
 
                fprintf (stderr, "\tExecution continuing.\n");

		fclose(outfile);

                return;

	}

	if (fprintf(outfile,"%s======== CHANNEL RESPONSE DATA ========\n", com_strt) == -1)
	{
                fprintf (stderr, "\tWARNING (output_resp):  "); 
                fprintf (stderr, "Output file %s is not available for writing.\n", outfile_name); 
  
                perror("output_resp()"); 
 
                fprintf (stderr, "\tExecution continuing.\n");
 
                fclose(outfile);
 
                return;
 
        }
 

	if (fprintf(outfile,"%s%s%2.2d     Station:     %s\n", 
			blkt_id1,fld_pref,3,current_station->station) == -1)
        {
                fprintf (stderr, "\tWARNING (output_resp):  "); 
                fprintf (stderr, "Output file %s is not available for writing.\n", outfile_name); 
  
                perror("output_resp()"); 
 
                fprintf (stderr, "\tExecution continuing.\n");
 
                fclose(outfile);
 
                return;
 
        }
 
        if(fprintf(outfile,"%s%s%2.2d     Network:     %s\n",
                    blkt_id1,fld_pref,16,
                    current_station->network_code ? current_station->network_code : "??") == -1)
	{
                fprintf (stderr, "\tWARNING (output_resp):  "); 
                fprintf (stderr, "Output file %s is not available for writing.\n", outfile_name); 
  
                perror("output_resp()"); 
 
                fprintf (stderr, "\tExecution continuing.\n");
 
                fclose(outfile);
 
                return;
 
        }

	if (fprintf(outfile, "%s%s%2.2d     Location:    %s\n",
                        blkt_id2, fld_pref, 3,
				strcmp(current_channel->location, "") != 0 ?  
					current_channel->location : "??") == -1)

	{
		fprintf (stderr, "\tWARNING (output_resp):  ");

		fprintf (stderr, "Output file %s is not available for writing.\n", outfile_name);
 
                perror("output_resp()");
 
                fprintf (stderr, "\tExecution continuing.\n");
 
                fclose(outfile);
 
                return;

	}

	if (fprintf(outfile,"%s%s%2.2d     Channel:     %s\n",
			blkt_id2,fld_pref,4,current_channel->channel) == -1)
	{
                fprintf (stderr, "\tWARNING (output_resp):  "); 
                fprintf (stderr, "Output file %s is not available for writing.\n", outfile_name); 
  
                perror("output_resp()"); 
 
                fprintf (stderr, "\tExecution continuing.\n");
 
                fclose(outfile);
 
                return;
 
        }
 
	if (fprintf(outfile,"%s%s%2.2d     Start date:  %s\n",
			blkt_id2,fld_pref,22,current_channel->start) == -1)
        {
                fprintf (stderr, "\tWARNING (output_resp):  "); 
                fprintf (stderr, "Output file %s is not available for writing.\n", outfile_name); 
  
                perror("output_resp()"); 
 
                fprintf (stderr, "\tExecution continuing.\n");
 
                fclose(outfile);
 
                return;
 
        }
 

	if (fprintf(outfile,"%s%s%2.2d     End date:    ",
			blkt_id2,fld_pref,23) == -1)
        {
                fprintf (stderr, "\tWARNING (output_resp):  "); 
                fprintf (stderr, "Output file %s is not available for writing.\n", outfile_name); 
  
                perror("output_resp()"); 
 
                fprintf (stderr, "\tExecution continuing.\n");
 
                fclose(outfile);
 
                return;
 
        }
 


	if (current_channel->end == NULL) 
	{
		if (fprintf(outfile,"No Ending Time\n") == -1)
        	{
                	fprintf(stderr, "\tWARNING (output_resp):  "); 
                	fprintf(stderr, "Output file %s is not available for writing.\n", outfile_name); 
  
                	perror("output_resp()"); 
 
                	fprintf(stderr, "\tExecution continuing.\n");
 
                	fclose(outfile);
 
                	return;
        	}
	} 
	else 
	if (*(current_channel->end) == '\0') 
	{
		if (fprintf(outfile,"No Ending Time\n") == -1)
	        {
                	fprintf(stderr, "\tWARNING (output_resp):  "); 
                	fprintf(stderr, "Output file %s is not available for writing.\n", outfile_name); 
  
                	perror("output_resp()"); 
 
                	fprintf(stderr, "\tExecution continuing.\n");
 
                	fclose(outfile);
 
                	return;
 
        	}
	} 
	else 
	if (fprintf(outfile,"%s\n", current_channel->end) == -1)
	{
                fprintf (stderr, "\tWARNING (output_resp):  "); 
                fprintf (stderr, "Output file %s is not available for writing.\n", outfile_name); 
  
                perror("output_resp()"); 
 
                fprintf (stderr, "\tExecution continuing.\n");
 
                fclose(outfile);
 
                return;
 
        }
 

	if (fprintf(outfile,"%s=======================================\n", com_strt) == -1)
        {
                fprintf (stderr, "\tWARNING (output_resp):  "); 
                fprintf (stderr, "Output file %s is not available for writing.\n", outfile_name); 
  
                perror("output_resp()"); 
 
                fprintf (stderr, "\tExecution continuing.\n");
 
                fclose(outfile);
 
                return;
 
        }
 
	/* write out responses */
   for (response = current_channel->response_head; response != NULL; response = response->next)
	{
		if (response->type == 'P') print_type53 (outfile,response->ptr.type53);
		else if (response->type == 'C') print_type54 (outfile,response->ptr.type54);
		else if (response->type == 'L') print_type55 (outfile,response->ptr.type55);
		else if (response->type == 'G') print_type56 (outfile,response->ptr.type56);
		else if (response->type == 'D') print_type57 (outfile,response->ptr.type57);
		else if (response->type == 'S') print_type58 (outfile,response->ptr.type58);
		else if (response->type == 'R') print_type60 (outfile,response->ptr.type60);
		else if (response->type == 'F') print_type61 (outfile,response->ptr.type61);
		else if (response->type == 'O') print_type62 (outfile,response->ptr.type62);
		else 
		{
			fprintf (stderr, "WARNING [print_response]:  ");
			fprintf (stderr, "unknown response type %c encountered.\n", 
				response->type);
			fprintf (stderr, "\tExecution continuing.\n");
		}
	}

/*                 +=======================================+                 */
/*=================|          close the output file        |=================*/
/*                 +=======================================+                 */

	fclose (outfile);

}

void output_old_resp ()
{
	char buffer[30];	  				/* output file name */
	int i;										/* counter */

/*                 +=======================================+                 */
/*=================|  Find selected station channels       |=================*/
/*                 +=======================================+                 */

	/* Print a warning that this RESP file format will no longer be 
	 * available after Sept 1, 1996 */

	fprintf (stderr, "\n\t\tWARNING!  THIS RESP FILE FORMAT WILL NO LONGER BE SUPPORTED\n");
	fprintf (stderr, "\t\tAFTER 1 SEPT, 1996.  IT WILL BE REPLACED BY RESP FILES\n");
	fprintf (stderr, "\t\tWHICH FOLLOW THE FORMAT USED BY EVALRESP V3.0.  THE CHANGES\n");
	fprintf (stderr, "\t\tMADE TO THIS FORMAT MAY EFFECT SOME PARSING ROUTINES THAT\n");
	fprintf (stderr, "\t\tUSE THESE FILES.  PLEASE REFER TO THE ACCOMPANYING DESCRIPTION\n");
	fprintf (stderr, "\t\tIN THE FILE 'RESP_FILE.CHANGES' IN THE RDSEEDv%s DIRECTORY\n\n",
			 VERSION);

	
	for (current_station = type50_head; current_station != NULL;
		current_station = current_station->next)
	{
		if (chk_station(current_station->station))
		{
			if ((type10.version >= 2.3) && 
					!chk_network(current_station->network_code))
				continue;

			for (current_channel=current_station->type52_head; current_channel!=NULL;
				 current_channel=current_channel->next)
			{
				if (chk_channel(current_channel->channel))
				{
					print_old_resp ();
				}
			}
		}
	}
}
	
/*===========================================================================*/
print_old_resp ()
{
	FILE *outfile;								/* output file pointer */
	char outfile_name[100];	  				/* output file name */
	int i;										/* counter */
	struct response *response;				/* looping vbl */
	char *blkt_id1="B050",*blkt_id2="B052";     /* blockette id strings */

/*                 +=======================================+                 */
/*=================|  build name for and open output file  |=================*/
/*                 +=======================================+                 */

	sprintf (outfile_name, "RESP.%s.%s.%s",
		current_station->network_code ? current_station->network_code: "",
		current_station->station,
		current_channel->channel);

	if ((outfile = fopen (outfile_name, "a")) == NULL)
	{
		fprintf (stderr, "\tWARNING (output_data):  ");
		fprintf (stderr, "Output file %s is not available for writing.\n",
			outfile_name);
		fprintf (stderr, "\tExecution continuing.\n");
		return;
	}

	fprintf(outfile, "<< IRIS SEED Reader, Release %s >>\n\n", 
			VERSION);

	fprintf(outfile,"======== CHANNEL RESPONSE DATA ========\n");
	fprintf(outfile,"Station:     %s\n",
			current_station->station);
	fprintf(outfile,"Channel:     %s\n",
			current_channel->channel);
	fprintf(outfile,"Start date:  %s\n",
			current_channel->start);
	fprintf(outfile,"End date:    ");
	if (current_channel->end == NULL) fprintf(outfile,"No Ending Time\n");
	else if (*(current_channel->end) == '\0') fprintf(outfile,"No Ending Time\n");
	else fprintf(outfile,"%s\n", current_channel->end);
	fprintf(outfile,"=======================================\n");

	/* write out responses */
   for (response = current_channel->response_head; response != NULL; response = response->next)
	{
		if (response->type == 'P') old_print_type53 (outfile,response->ptr.type53);
		else if (response->type == 'C') old_print_type54 (outfile,response->ptr.type54);
		else if (response->type == 'L') old_print_type55 (outfile,response->ptr.type55);
		else if (response->type == 'G') old_print_type56 (outfile,response->ptr.type56);
		else if (response->type == 'D') old_print_type57 (outfile,response->ptr.type57);
		else if (response->type == 'S') old_print_type58 (outfile,response->ptr.type58);
		else if (response->type == 'R') old_print_type60 (outfile,response->ptr.type60);
		else if (response->type == 'F') old_print_type61 (outfile,response->ptr.type61);
		else if (response->type == 'O') old_print_type62 (outfile,response->ptr.type62);
		else 
		{
			fprintf (stderr, "WARNING [print_response]:  ");
			fprintf (stderr, "unknown response type %c encountered.\n", 
				response->type);
			fprintf (stderr, "\tExecution continuing.\n");
		}
	}
	fclose(outfile);

/*                 +=======================================+                 */
/*=================|          close the output file        |=================*/
/*                 +=======================================+                 */

	fclose (outfile);

}
