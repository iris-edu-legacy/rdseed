/*===========================================================================*/
/* SEED reader     |              free_type2000            |  blockette 2000 */
/*===========================================================================*/
/*
	Name:		free_type_2000
*/

#include "rdseed.h"

void free_type_2k(struct data_blk_2000 *p)
{

        int i;
 
        for (i = 0; i < p->number_header_flds; i++)
                free(p->data_header_flds[i]);
 
        free(p->data_header_flds);
 
        free(p);
}
 
