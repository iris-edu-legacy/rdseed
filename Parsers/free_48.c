/*===========================================================================*/
/* SEED reader     |              free_type48              |  station header */
/*===========================================================================*/
/*
	Name:		free_type48
*/

#include "rdseed.h"

void free_type48 ()
{
	struct type48 *type48, *t;
	int i;

	type48 = type48_head;
	while (type48 != NULL)
	{	
		if (type48->name)
			free(type48->name);

		for (i = 0; i < type48->number_calibrations; i++)
		{
			if (type48->calibration[i].time)
				free(type48->calibration[i].time);
		}

		if (type48->calibration)
			free ((char *)(type48->calibration));

		t = type48->next;

		free((char *) type48);

		type48 = t;

	}
	type48_head = NULL;
}
