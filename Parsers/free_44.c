/*===========================================================================*/
/* SEED reader     |              free_type44              |  station header */
/*===========================================================================*/
/*
	Name:		free_type44
*/

#include "rdseed.h"

void free_type44 ()
{
	struct type44 *type44, *t; 
	int i;

	type44 = type44_head;
	while( type44 != NULL)
	{
		if (type44->name)
			free(type44->name);

		if (type44->numerator)
			free(type44->numerator);

		if (type44->denominator)
			free(type44->denominator);

		t = type44->next;

		free((char *)type44);

		type44 = t;
	}

	type44_head = NULL;

}
