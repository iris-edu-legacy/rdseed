/*===========================================================================*/
/* SEED reader     |              free_type41              |  station header */
/*===========================================================================*/
/*
	Name:		free_type41
*/

#include "rdseed.h"

void free_type41 ()
{
	struct type41 *type41, *t; 
	int i;

	type41 = type41_head;
	while( type41 != NULL)
	{
		if (type41->name)
			free(type41->name);

		if (type41->numerator != NULL)
			free(type41->numerator);

		t = type41->next;

		free((char *)type41);

		type41 = t;

	}
	type41_head = NULL;

}
