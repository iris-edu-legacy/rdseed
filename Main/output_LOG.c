
/*===========================================================================
 * SEED reader     |              output_LOG               |    subprocedure 
 *===========================================================================
 *
 *	Name:		output_LOG
 *	Purpose:	
 *	Usage:		void output_LOG();
 *				output_LOG(char *);
 *	Input:		pointer to current LOG
 *	Output:		none (writes a ascii file; 
 *			1988.01.23.15.34.08.ANMO.LOG is the filename for a
 *			log file for station ANMO at the indicated time
 *	Called by:	process_data();
 *	Author:		C L.
 *
 *	Revisions:	
 *		
*/

#include "rdseed.h"		/* SEED tables and structures */
#include <stdio.h>
#include <sys/param.h>


extern int Seed_flag;

void dump_LOG_rec(log_rec, num_bytes, log_time, stn, chn, net)
char *log_rec;
int num_bytes;
struct time *log_time;
char *stn;
char *chn;
char *net;

{
	FILE 	*outfile;		/* output file pointer */
	char 	outfile_name[100];	/* output file name */
	
	char orig_dir[MAXPATHLEN];

	getcwd(orig_dir, MAXPATHLEN);

	chdir(output_dir);

	sprintf (outfile_name, "%04d.%03d.%02d.%02d.%02d.%04d.%s.%s.%s",
			log_time->year,
			log_time->day,
			log_time->hour,
			log_time->minute,
			log_time->second,
			log_time->fracsec,
			net == 0 ? "na" : net,
			stn,
			chn);

	if ((outfile = fopen (outfile_name, "w")) == NULL)
	{
		fprintf (stderr,"\tERROR (output_LOG):  ");
		fprintf (stderr,"Output file %s is not available for writing.\n",outfile_name);
		perror("rdseed");
		fprintf (stderr, "\tExecution continuing.\n");

		chdir(orig_dir);

		return;	
	}

	/* describe the file being written */

	printf ("Writing %s: %s: %s\n",
			net == 0 ? "na" : net,
			stn,
			"LOG");


	if (fwrite(log_rec, num_bytes, 1, outfile) != 1)
	{
		fprintf (stderr,"\tERROR (output_LOG):  ");
                fprintf (stderr,"Unable to write to file %s\n", outfile_name); 
                perror("rdseed"); 
                fprintf (stderr, "\tExecution continuing.\n");
	}

	/* append a \n */
	fprintf(outfile, "\n");

	fclose(outfile);

	chdir(orig_dir);

	return;

}
	
