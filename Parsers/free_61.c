/*===========================================================================*/
/* SEED reader     |               free_type61             |  station header */
/*===========================================================================*/
/*
	Name:		free_type61
*/

#include "rdseed.h"

void free_type61 (type61)
struct type61 *type61;
{
	if (type61 == NULL)
		return;

	if (type61->name)
		free((char *)(type61->name));

	if (type61->numerator)
		free((char *)(type61->numerator));

	free((char *) type61);
}
