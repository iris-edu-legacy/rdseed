/*===========================================================================*/
/* SEED reader     |              macros.h                 |     header file */
/*===========================================================================*/
/*
	Name:		macros.h
	Purpose:	header file containing commonly-used macro definitions
	Usage:		#include "macros.h"
	Input:		not applicable
	Output:		not applicable
	Externals:	not applicable
	Warnings:	not applicable
	Errors:		not applicable
	Called by:	anything
	Calls to:	none
	Algorithm:	various; see the macros
	Notes:		none
	Problems:	none known
	References:	none
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
*/

/* if this header is already included, don't include it again */
#ifndef MACROS_INCLUDED
#define MACROS_INCLUDED 1

/*                 +=======================================+                 */
/*=================|            Macro definitions          |=================*/
/*                 +=======================================+                 */

/* Type-independant absolute value for C standard types */
#ifndef abs
#define ABS(x) ((x)<0?-(x):(x))
#endif

/* Type-independant maximum value for C standard types */
#ifndef max
#ifndef MAX
#define MAX(x,y) ((x)>(y)?(x):(y))
#endif
#endif

/* Type-independant minimum value for C standard types */
#ifndef min
#ifndef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif
#endif

/* True/false definitions */
#ifndef TRUE
#define FALSE 0
#define TRUE !FALSE
#endif

/* NULL pointer */
#ifndef NULL
#define NULL 0
#endif

/* add a new element onto the end of a linked list */
/* requires a ptr in structure called "next" to point to next element */
/* before use, head and tail are both NULL */
#define append_linklist_element(new, head, tail) \
	new->next = NULL; \
	if (head != NULL) tail->next = new; \
	else head = new; \
	tail = new;

/*                 +=======================================+                 */
/*=================|        End of the header file         |=================*/
/*                 +=======================================+                 */

/* the next line should be the last line of the header file */
#endif
