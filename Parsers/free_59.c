/*===========================================================================*/
/* SEED reader     |            free_type59                |  station header */
/*===========================================================================*/
/*
	Name:		free_type59
*/

#include "rdseed.h"

void free_type59 (type59)
struct type59 *type59;
{
	struct type59 *t;

	while (type59 != NULL)
	{
		if (type59->start)
			free(type59->start);

		if (type59->end != NULL) 
			free(type59->end);

		t = type59->next;

		free((char *) type59);

		type59 = t;

	}

}
