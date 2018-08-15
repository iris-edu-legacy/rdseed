/*===========================================================================*/
/* SEED reader     |             cmdlineproc               |         utility */
/*===========================================================================*/
/*
	Name:	cmdlineproc
	Purpose:process the command line; options have leading "-" (see Notes)
		Serves as a replacement for "getopt", which is not always 
		available.
	Usage:	#define MAXOPTS [some number]       = max # of options
		#define STRLEN [some number]        = max argument string length
		struct optstruct
		{
			int number_options;             =  # options found
			int next_arg;                   =  argv index to next arg
			char option[MAXOPTS];           = list of allowed options
			char option_present[MAXOPTS];   = TRUE if option present
			char argument[MAXOPTS][STRLEN]; = argument if present
		};

	main (argc, argv)
			int argc;
			char *argv[];
		{
			struct optstruct *options;
			struct optstruct *cmdlineproc ();
			char optlist[some number];
			strcpy (optlist, "abf:o:");
			options = cmdlineproc (argc, argv, optlist);
			if (options != NULL) <process options; see notes>
				if (options == NULL) <handle input errors >
			}
			See Notes.
	Input:	argc = count of arguments from command line
		argv = command line
		optlist = list of permitted option keys (single characters);
			a character followed by a colon indicates that the option
		takes an argument
	Output:	a structure containing option keys (options->option[]); 
		option-present flags (options->option_present[]);
		arguments as appropriate (options->argument[]);
		number of options in optlist (options->number_options);
		and the argv index of the first argv which is not part of
		an option/argument set (this allows indefinite-length 
				command lines, to permit input of lists of items)
	Externals:none
	Messages:none
	Warnings:spurious argument found before last option/argument set.
		E.g., assuming a program "pgm" takes options "a" and "b", and
		option "b" takes an argument but "a" does not:
		pgm -a albert -b bernie
			has spurious argument "albert", which is ignored.
	Errors:	duplication of option key character in commandline
		argument missing where one was required
		invalid options key character present
		For all errors a NULL is returned to the caller to allow the
		caller to present a usage message
	Fatals:	none
	Called by:main
	Calls to:none (warning and error messages are printed by subprocedures
			included in this file)
	Algorithm:given option list, initialize option/argument structure;
		find strings in command line beginning with "-" and parse them
		into option key characters, setting an option-found flag in
		the option/argument structure.  As each option is registered,
		if the option requires an argument recover the next available
		non-option string as the argument for that option.  Return
		the option/argument structure, including an index into argv
		so the calling program can process any remaining arguments.
	Notes:	Typical use of this routine would be:
		if ((options = cmdlineproc (argc, argv, "abc:d:")) != NULL)
		{
		for (i = 0; i < options->number_options; i++)
		{
			if (options->option_present[i] == TRUE)
			{
			switch (options->option[i])
			{
				case 'a':
				case 'b':
				case 'c':
				case 'd':
				default:
			}
		}
		}
		}
		else /+ error occurred in cmdlineproc +/
		{
			printf_usage_message ();
			exit (0);
		}
			for (i = options->next_arg; i < argc; i++)
			{
			do_something_with_end_of_line_arguments ();
			}

	Definitions:  the option/argument section of the command line
		contains options and their associated arguments, if any; it
		may be only the first part of the command line after the
		program name, if the program requires some list of input
		arguments other than controlling options, or it may comprise
		the entire command line after the program name.  Options are
		single characters which may be grouped together, are delineated
		by a leading "-", and which act as switches in the program.
		Arguments are strings which do not begin with a "-".

		Given a program called "pgm" which used "cmdlineproc" as shown
		above.  The program accepts options "a", "b", "c", and "d";
		options "c" and "d" require arguments, while the other options
		take no arguments.  Presume also that some post-option/argument
		section values must be specified on the command line.  Note that
		options may be specified in any order.

		The program may be called with any of the following sequences.
		The option "c" is associated with argument "charlie", and
		option "d" takes argument "diane".
		prompt> pgm -a -b -c charlie -d diane other_args ...
		prompt> pgm -abcd charlie diane other_args ...
		prompt> pgm other_args ...              (no options used)
		prompt> pgm -ac charlie other_args ...  (some options used)
		prompt> pgm -cd charlie diane -ab other_args ...
			or even (unfortunately)
		prompt> pgm -ac charlie -d -b diane other_args ...

		If the program is invoked with an extra argument in the
		option/argument section, for example
		prompt> pgm -ab bernie -c charlie -d diane other_args ...
		prompt> pgm -abd diane bernie -c charlie other_args ...
		the string "bernie" will be ignored except for the printing
		of a warning.

		Any of the following will generate error messages, and should
		be treated by the main program as fatal errors.
		prompt> pgm -aa other_args ...        (repeat arg)
		prompt> pgm -abc -d charlie           (no arg for "d")
		prompt> pgm -abc -d diane             (also no arg for "d")

	Problems:The case in which a program takes other arguments can lead to a
		condition which will NOT generate an error message, but is
		incorrect usage.  That's when the user leaves out an argument:
			prompt> pgm -abcd charlie other_args ...
		No argument is present for option "d", but the first element
		of "other_args" will be used (see Algorithm).
	References:none
	Contract:IRIS SEED reader, based on previous work
	Language:C, more or less ANSI standard
	Revisions:25 August 1989  Dennis O'Neill  initial release
*/

#include <stdio.h>
#include <string.h>
#include "rdseed.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

static struct optstruct options;		/* options/argument structure */

struct optstruct *cmdlineproc (argc, argv, argument_list)
int argc;					/* command line arg count */
char *argv[];					/* command line arg values */
char argument_list[];				/* list of valid options */
{
	void error_invalid_opt ();		/* call if invalid opt found */
	void error_noarg_found ();		/* call if no arg found */
	void error_dup_opt ();			/* call if duplicat opt found */
	void warn_spurious_arg ();		/* call if extra arg found */

	int argctr;				/* index: next arg to examine */

	int cmdlinectr;				/* index: argv current posn */
	int j;					/* miscellaneous use counter */
	int k;					/* miscellaneous use counter */
	int l;					/* miscellaneous use counter */
	int opt_found;				/* option-found flag */
	int arg_found;				/* argument-found flag */

/*                 +=======================================+                 */
/*=================| Init options structure, count options |=================*/
/*                 +=======================================+                 */

	for (j = 0, k = 0; j < strlen (argument_list); j++)
	{
		if (argument_list[j] != ':')
		{
			options.option[k] = argument_list[j];
			options.option_present[k] = FALSE;
			if ((j+1 <= strlen (argument_list)) && (argument_list[j+1] == ':'))
				options.argument[k][0] = TRUE;
			else options.argument[k][0] = FALSE;
			options.number_options = ++k;
		}
	}

/*                 +=======================================+                 */
/*=================|   sort thru cmd line, find options    |=================*/
/*                 +=======================================+                 */

	argctr = 0;
	options.next_arg = 0;

	/* sort through the command line, associate options with arguments */
	/* cmdlinectr points to current command line string being processed */
	/* argctr points to current cmdline string or next unprocessed argument */
	for (cmdlinectr = 1; cmdlinectr < argc; cmdlinectr++)
	{
		argctr = (argctr > cmdlinectr) ? argctr : cmdlinectr;

		/* identify option characters in the command line, process them */
		if (argv[cmdlinectr][0] == '-')
		{
			/* set end of option/argument section ptr after this opt */
			options.next_arg = cmdlinectr + 1;

			/* option string found; process each option, maybe look for arg */
			for (j = 1; j < strlen (argv[cmdlinectr]); j++) 
			{
				/* search options list for a match to option in command line */
				opt_found = -1;

				for (k = 0; k < options.number_options; k++)
				{
					/* if a match is found in the options tbl, notice, break */
					if (argv[cmdlinectr][j] == options.option[k])
					{
						opt_found = k;
						if (!options.option_present[k])
							options.option_present[k] = TRUE;
						else
						{
							error_dup_opt (argc, argv, argv[cmdlinectr][j]);
							return (NULL);
						}
						break; /* out of for (k ...) loop */
					}
				}

				/* if cmd line option not in options list, error in usage */
				if (opt_found == -1)
				{
					error_invalid_opt (argv[cmdlinectr][j]);
					return (NULL);
				}

/*                 +=======================================+                 */
/*=================| find argument for option if expected  |=================*/
/*                 +=======================================+                 */

				/* locate next non-option string, take that as argument */
				/* remember where we left off, start there next time */
				if (options.argument[opt_found][0])
				{
					arg_found = FALSE;
					for (k = argctr; k < argc; k++)
					{
						if (argv[k][0] != '-')
						{
							strcpy (options.argument[opt_found], argv[k]);	
							arg_found = TRUE;
							break; /* out of for (k ... loop */
						}
					}
					argctr = k + 1;

					/* set end of option/argument section ptr after this arg */
					options.next_arg = argctr;

					/* if argument was expected but not found, usage error */
					if (!arg_found)
					{
						/* 
						 * This function is real cute, Dennis, unfortunately
						 * it doesn't work properly if an option can have
						 * both arguments or no arguments such as for the
						 * -d option. Therefore, exclude -d
						 */
						if( strcmp(argv[k-1], "-d") )
						{
							error_noarg_found (argc, argv, &options);
							return (NULL);
						}
					}

				}
				/* end of argument-association loop */

			}
		}
		/* end of option-string processing loop */

/*                 +=======================================+                 */
/*=================| process unexpected interior arguments |=================*/
/*                 +=======================================+                 */

		/* if unexpected argument found in interior of command line,
			send warning message, continue */
		/* ignore extra arguments after last option/argument pair */
		/* if current string is an option string, don't report */
		else /* possible invalid argument string found */
		{
			if (argctr > cmdlinectr); /* forget it; already processed */
			else
			{
				/* look for another option string in balance of command line */
				for (j = cmdlinectr; j < argc; j++)
				{
					if (argv[j][0] == '-')
					{
						warn_spurious_arg (argc, argv, argv[cmdlinectr]);
						break;
					}
				}
			} 
		}

	}
	/* end of command-line processing loop */
							
	return (&options);
}

/*===========================================================================*/

void error_invalid_opt (option)
char option;
{
	fprintf (stderr, "ERROR [command line]:  ");
	fprintf (stderr, "invalid command line option \"%c\" found.\n", option);
	return;
}

/*===========================================================================*/

void error_noarg_found (argc, argv, options)
int argc;
char *argv[];
struct optstruct *options;
{
	int l;

	fprintf (stderr, "ERROR [command line]:  ");
	fprintf (stderr, "too few arguments for specified options.\n");
	fprintf (stderr, "\tCommand line was:\n");

	fprintf (stderr, "\t");
	for (l = 0; l < argc; l++) fprintf (stderr, "%s ", argv[l]);
	fprintf (stderr, "\n");

	fprintf (stderr, "\tOptions expecting arguments ");
	fprintf (stderr, "and associated arguments:\n");
	fprintf (stderr, "\tOption\tAssociated argument\n");
	for (l = 0; l < options->number_options; l++)
	{
		if (options->argument[l][0])
		{
			fprintf (stderr, "\t%c", options->option[l]);
			if (options->option_present[l])
			{	
				if (options->argument[l][0] >= ' ')
					fprintf (stderr, "\t%s\n", options->argument[l]);
				else
					fprintf (stderr, "\tno argument found\n");
			}
			else
			{
				fprintf (stderr, "\tnot used this time or ");
				fprintf (stderr, "not yet encountered when error occurred\n");
			}
		}
	} /* argument error printing; uses l as counter */

	return;
}

/*===========================================================================*/

void error_dup_opt (argc, argv, option)
int argc;
char *argv[];
char option;
{
	int l;

	fprintf (stderr, "ERROR [command line]:  ");
	fprintf (stderr, "argument \"%c\" present more than once.\n", option);
	fprintf (stderr, "\tCommand line was:\n");

	fprintf (stderr, "\t");
	for (l = 0; l < argc; l++) fprintf (stderr, "%s ", argv[l]);
	fprintf (stderr, "\n");

	return;
}

/*===========================================================================*/

void warn_spurious_arg (argc, argv, string)
int argc;
char *argv[];
char *string;
{
	int j;
	fprintf (stderr, "WARNING [command line]:  ");
	fprintf (stderr, "unexpected argument \"%s\" ", string);
	fprintf (stderr, "found, ignored.\n");
	fprintf (stderr, "\tCommand line was:\n");
	fprintf (stderr, "\t");
	for (j = 0; j < argc; j++) fprintf (stderr, "%s ", argv[j]);
	fprintf (stderr, "\n");
	fprintf (stderr, "\tExecution continuing.\n");
	return;
}
