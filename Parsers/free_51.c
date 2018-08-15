/*===========================================================================*/
/* SEED reader     |             free_type51               |  station header */
/*===========================================================================*/
/*
	Name:		free_type51
*/

#include "rdseed.h"

void free_type51 (type51)
struct type51 *type51;
{
	struct type51 *t;

	while (type51 != NULL)
	{

		if (type51->start)
			free(type51->start);

		if (type51->end != NULL) 
			free(type51->end);

		t = type51->next;

		free((char *) type51);

		type51 = t;
	}

}
