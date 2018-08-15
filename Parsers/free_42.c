/*===========================================================================*/
/* SEED reader     |              free_type42              |  station header */
/*===========================================================================*/
/*
	Name:		free_type42
*/

#include "rdseed.h"

void free_type42 ()
{
	struct type42 *type42, *t; 
	int i;

	type42 = type42_head;
	while( type42 != NULL)
	{
		free(type42->name);
		free(type42->coefficient);

		t = type42->next;
		free((char *)type42);
		type42 = t;
	}
	type42_head = NULL;

}
