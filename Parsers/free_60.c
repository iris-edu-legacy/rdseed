/*===========================================================================*/
/* SEED reader     |              free_type60              |  station header */
/*===========================================================================*/
/*
	Name:		free_type60
*/

#include "rdseed.h"

void free_type60 (type60)
struct type60 *type60;
{
	int i, ii;

	if (type60 == NULL)
		return;

	free_type60(type60->next);

	for (i = 0; i < type60->number_stages; i++)
	{
			if (type60->stage[i].response)
				free((char *)(type60->stage[i].response));	
	}

	free((char *)type60->stage); 
	free((char *) type60);	

	return;

}
