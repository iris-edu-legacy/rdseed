/*===========================================================================*/
/* SEED reader     |            parse_type2000             |     data block  */
/*===========================================================================*/
/*
	Name:		parse_type2000
	Purpose:	parse a data record header for blockette 2000s.
	Usage:		void parse_type_2k ();
				char *input_data_ptr;
				parse_type_2k(input_data_ptr);

	Input:		pointer to beginning of data record header
	Output:		pointer to blk 2000 structure	
	Externals:	none
	Warnings:	none
	Errors:		none
	Called by:	process_data
	Calls to:	none
	Algorithm:	search through data record blockettes for blk 2000
	Notes:		none
	Problems:	none known
	References:	none
	Language:	C, hopefully ANSI standard
	Author:		C Laughbon	
	Revisions:	
*/

#include "rdseed.h"


extern int byteswap;

struct data_blk_2000 *parse_type_2k(buff)
char *buff;	/* ptr to 2000 blk record */



{

	struct data_blk_2000 *p;

        char *ch_ptr1, *ch_ptr2;
        int i;

        int *ii;

        short *l;

        p = (struct data_blk_2000 *)buff;

        if ((p->data_header_flds = (char **)malloc(sizeof(char *) * p->number_header_flds)) == NULL)
	{
		free(p);
		return NULL;
	}
 
        for (i = 0; i < p->number_header_flds; i++)
        {
                /* add one to include the tilde */
 
                ch_ptr2 = strchr(ch_ptr1, '~') + 1;

                if ((p->data_header_flds[i] = 
			(char *)malloc((ch_ptr2 - ch_ptr1 ) + 1)) == NULL)
		{
			free(p->data_header_flds);
			free(p);
			return NULL;
		}

                strncpy(p->data_header_flds[i], ch_ptr1, ch_ptr2-ch_ptr1);
 
                p->data_header_flds[i][ch_ptr2-ch_ptr1] = 0;
 
                ch_ptr1 = ch_ptr2;
	}

	if ((p->opaque_buff = (unsigned char *)malloc(p->blk_length - 
						p->opaque_offset)) == NULL)
	{
		for (i = 0; i < p->number_header_flds; i++)
			free(p->data_header_flds[i]);

		free(p->data_header_flds);
		free(p);
		return NULL;
	}

	for (i = p->opaque_offset; i < p->blk_length; i++)
		p->opaque_buff[i - p->opaque_offset] = buff[i];
	
	return p;

}

/* -------------------------------------------------------------------- */

