/*===========================================================================*/
/* SEED reader     |               free_type62             |  station header */
/*===========================================================================*/
/*
	Name:		free_type62
*/

#include "rdseed.h"

void free_type62 (type62)
struct type62 *type62;
{
	free((char *)(type62->coefficient));
	free((char *) type62);
}
