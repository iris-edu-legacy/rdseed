/*===========================================================================*/
/* SEED reader     |               free_type45             |  station header */
/*===========================================================================*/
/*
	Name:		free_type45
*/

#include "rdseed.h"

void free_type45 ()
{
	struct type45 *type45, *t;

	type45 = type45_head;
	while (type45 != NULL)
	{	
		if (type45->name)
			free(type45->name);

		if (type45->response)
			free(type45->response);

		t = type45->next;

		free((char *)type45);

		type45 = t;

	}
	type45_head = NULL;
}
