
/*===========================================================================*/
/* SEED reader     |              log_errors			   |         utility */
/*===========================================================================*/
/*
 *	Name:		myfprintf, open_log_file, stderr_to_file, close_log_file	
 *	Purpose:	capture the program's stderr messages and duplicate the 
 *				output to a disk file.
 *	Usage:		void myfprintf(...); 
 *
 *	Input:		variable parameter list
 *	Output:		none
 *	Warnings:	none
 *	Errors:		none
 *	Called by:	anything
 *	Calls to:	none
 *	Algorithm:	none.
 *	Notes:		replaces fprintf by #defining fprintf as myfprintf, see
 *				rdseed.h
 *				This was done to redirect fprintf(stderr,...) to a file.
 *				If no stderr processing, then send to fprintf().
 *	Problems:	None
 *	Language:	C, hopefully ANSI standard
 *	Author:		Chris Laughbon	
 *	Revisions:	
 * 
 */

/* ---------------------------- Includes -------------------------------- */

#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/param.h>
#include <pwd.h>
#include <time.h>



/* ---------------------------- Defines --------------------------------- */
#define ERR_FILE_EXT	".stderr.msg"
#define ERR_FILE_NAME	"rdseed.err_log"
#define RDSEED_ALERT_FNAME "rdseed.alert_log"
#define TRUE 1
#define FALSE 0
#define BOOL int

/* ---------------------------- Global Variables ------------------------ */

int rdseed_alert = 0;

/* ---------------------------- Local Variables ------------------------- */
static BOOL error_log_inited 	= FALSE;
static BOOL header_added	= FALSE;

static char input_fname[MAXPATHLEN];

/* ----------------------------- External Variables --------------------- */

extern char Rdseed_Cwd[];

/* ---------------------------------------------------------------------- */


void stderr_to_file();


void myfprintf(FILE *fp, char *fmt, ...)
{
	va_list args;

	va_start(args,fmt);

	vfprintf(fp, fmt, args);

	fflush(fp);

	/* if output to stderr, then add to log file */

/*	if (error_log_inited && fp == stderr)
		stderr_to_file(fmt, ap);  */

	va_end(args);

	return;

}

void save_myfprintf(FILE *fp, ...)
{
	va_list ap;
	va_list aq;

	char *fmt;

	va_start(ap, fp);

	va_copy(aq, ap);
	
	fmt = va_arg(ap, char *);

	vfprintf(fp, fmt, ap);

	fflush(fp);

	/* if output to stderr, then add to log file */
	if (error_log_inited && fp == stderr)
		stderr_to_file(aq);

	va_end(ap);
	va_end(aq);

	return;

}

/* ----------------------------------------------------------------------- */

void stderr_to_file(va_list ap)


{
	char * fmt;

	FILE *fptr;
	
	char now_dir[MAXPATHLEN];

	getcwd(now_dir, MAXPATHLEN);

	if (strcmp(now_dir, Rdseed_Cwd))
		chdir(Rdseed_Cwd);

	fptr = fopen(input_fname, "a");

	if (!header_added)
		add_header(fptr);

	fmt = va_arg(ap, char *);

	vfprintf(fptr, fmt, ap);

	fclose(fptr);

	if (strcmp(now_dir, Rdseed_Cwd)) 
                chdir(now_dir);

	return;

}

/* ----------------------------------------------------------------------- */
void init_error_file()

{

	/* check to make sure file is writable.
	 * If not, then write to /tmp. Still problems?, then
	 * continue but no error logging.
	 */

	strcpy(input_fname, ERR_FILE_NAME);
	if (!is_writable("."))
	{
		fprintf(stderr, "WARNING: Unable to write to current directory for the error message file!\n");

		fprintf(stderr, "Using /tmp directory instead.\n");
	
		if (!is_writable("/tmp")) 
		{
			fprintf(stderr, "WARNING: Unable to write to /tmp for the error message file!\n");
 
        		fprintf(stderr, "Error file processing is cancelled for this run.\n");

			error_log_inited = FALSE;

			return;
		}

		sprintf(input_fname, "/tmp/%s", ERR_FILE_NAME);

	}

	header_added = FALSE;

	/* Got this far...alls well */

	error_log_inited = TRUE;
	
}


/* ------------------------------------------------------------------------ */

BOOL is_writable(fname)
char *fname;

{
	struct stat s;
                
    if (stat(fname, &s) == -1)
		return FALSE; 
	
    return (s.st_mode & S_IWUSR);

}

/* ------------------------------------------------------------------------ */
BOOL add_header(FILE *fptr)

{
	/* create opening line with run start time and date */

	struct timeval tp; 
	struct timezone tz; 
 
   	struct passwd *pw_ent; 
 
   	gettimeofday(&tp, &tz);  
        
	pw_ent = getpwuid(getuid());
	
	fprintf(fptr, "\n==================================================\n");
	fprintf(fptr, "Error logging startup\nDate: %s\nBy: %s\n", ctime((const time_t *)&(tp.tv_sec)), pw_ent->pw_name);

	header_added = TRUE;
 
	return(TRUE);
	
}

/* ------------------------------------------------------------------------ */
int rdseed_alert_msg_out(msg)
char *msg;

{
	FILE *fptr;

	char alert_fname[MAXPATHLEN];

        struct timeval tp;
        struct timezone tz;
        struct tm *tm;
        char buf[200];

	
        gettimeofday(&tp, &tz);
	tm = localtime((const time_t *)&(tp.tv_sec));
        strftime(buf, sizeof(buf), "%m.%d.%y", tm);

        sprintf(alert_fname, "%s.%s", RDSEED_ALERT_FNAME, buf);

	fptr = fopen(alert_fname, "a+");

	if (fptr == NULL)
	{
		fprintf(stderr, "Error - unable to open alert message file!\n");

		perror("rdseed:rdseed_alert_msg_out():");

		fprintf(stderr, "Alert message >>%s\n", msg);

		return 0;

	}


	if (fprintf(fptr, "%s\n", msg) == -1)
	{
		fprintf(stderr, "Error - unable to log alert message to file!\n");
                perror("rdseed:rdseed_alert_msg_out():"); 
 
                fprintf(stderr, "Alert message >>%s\n", msg);
 
                return 0; 

	}

	fclose(fptr);

	/* main() will look at this to remind the user to scan file, on exit */
	rdseed_alert = 1;

	return 1;

}

/* ------------------------------------------------------------------------ */


