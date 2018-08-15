/*===========================================================================*/
/* SEED reader     |               free_type58             |  station header */
/*===========================================================================*/
/*
	Name:		free_type58
*/

#include "rdseed.h"

void free_type58 (type58)
struct type58 *type58;
{
	int i;

	if (type58 == NULL)
		return;

	free_type58(type58->next);

	for (i = 0; i < type58->number_calibrations; i++)
	{
		if (type58->calibration[i].time)
			free((char *)(type58->calibration[i].time));
	}

	if (type58->calibration)
		free((char *)(type58->calibration));

	free((char *)type58);

}
