#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ctype.h>


char strlst(string)
char *string;
{
	while (*string != '\0') { if (*(string+1) == '\0') return(*string); string++;}
	return('\0');
}

int trim(s)
char *s;

{
        char *c;
 
        while (isspace(*(s + strlen(s) - 1)))    
                *(s + strlen(s) - 1) = '\0';  
 
        c = s; 
 
        while (*c == ' ' && (*c != '\0'))
                *c++ = *(c + 1);    
 
        strcpy(s, c); 
 
	return 1;

}

/* ---------------------------------------------------------------------- */
int str_isnum(s)
char *s;

{
	while(*s)
		if (!isdigit(*s++))
			return 0;

	return 1;

}

/* ---------------------------------------------------------------------- */
void pad_it(char *s, int l)
{

        int sl;

        int ii;

	sl = strlen(s);

        if (sl < l)
                for (ii = 0; ii < l - sl; ii++)
                        strcat(s, "0");
}


/* ---------------------------------------------------------------------- */


