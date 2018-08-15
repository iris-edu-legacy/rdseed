/*===========================================================================*/
/* SEED reader     |             free_type12               |   volume header */
/*===========================================================================*/
/*
	Name:		free_type12
P*/

#include "rdseed.h"

void free_type12 ()
{
	int i;									
	

	for (i = 0; i < type12.number_spans; i++)
	{
		free(type12.timespan[i].bos);
		free(type12.timespan[i].eos);		
	}

	if (type12.timespan != NULL) 
		free((char *)type12.timespan);

	type12.timespan = NULL;
	type12.number_spans = 0;

}
