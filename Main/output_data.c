/*===========================================================================*/
/* SEED reader     |              output_data              |    subprocedure */
/*===========================================================================*/
/*
	Name:		output_data
	Purpose:	decode selected data output format

	Usage:		void output_data (data_hdr, offset);

	Input:		none (gets its data from globally-available tables and files)
	Output:		none 

	Externals:	output_format - numerics data output format selection.
	Warnings:	none
				failure to properly write the seismic data
	Errors:		none
	Called by:	time_span_out
	Calls to:	none
	Algorithm:	Call subroutine for selected data output format.
					
	Problems:	none known
	References:	Halbert, S. E., R. Buland, and C. R. Hutt (1988).  Standard for
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
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
				09/19/89  Dennis O'Neill  corrected output string length
				07/30/91  Allen Nance     decode multiple formats
*/

#include "rdseed.h"	/* SEED tables and structures */



void output_data (data_hdr, offset)
struct data_hdr *data_hdr;
int offset;
{
	if (data_hdr->nsamples == 0)
		return;

	switch (output_format)
	{
		case 0: output_sac(data_hdr, offset, 0); break;
		case 1: output_ah (data_hdr, offset); break;
		case 2: output_css(data_hdr, offset); break;
		case 3: output_mini(data_hdr, offset); break;
		case 4: break;	/* skip output seed, not done here, try process_data */
		case 5: output_sac(data_hdr, offset, 1); break;
		case 6: output_segy(data_hdr, offset); break;
		case 7: output_simple(data_hdr, offset, 0); break;
		case 8: output_simple(data_hdr, offset, 1); break;
	}

	data_hdr->nsamples = 0;

}
