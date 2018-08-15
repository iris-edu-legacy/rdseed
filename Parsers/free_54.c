/*===========================================================================*/
/* SEED reader     |               free_type54             |  station header */
/*===========================================================================*/
/*
	Name:		free_type54
*/

#include "rdseed.h"

void free_type54 (type54)
struct type54 *type54;
{
	if (type54 == NULL)
		return;

	free_type54(type54->next);

	if (type54->transfer)
		free(type54->transfer);

	if (type54->numerator)
		free((char *)(type54->numerator));

	if (type54->denominator)
		free((char *)(type54->denominator));

	free((char *)type54);

	return;

}
