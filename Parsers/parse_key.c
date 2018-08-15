#include "ddl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE -1
#define FALSE 0
 
/* get string token */
char *token(input, delim)
char *input;
int delim;
{
	char *p;
 
	if (*input == '\0') return(NULL);
	else
		{
		p = strchr(input, delim);
		if (p != NULL)
			{
			*p = '\0';
			}
		return(input);
		}
}
 
/* test for numeric string */
char *is_num_str(string)
char *string;
{
	while (*string != '\0')
		{
		if (isdigit(*string)) string++;
		else return(string);
		}
	return(NULL);
}
 
/* get integer value */
char *get_int(input, value, delim)
char *input;
int *value;
int delim;
{
	char *p, *p1, temp[50];
	int i, sign, base;
 
	strcpy(temp, input);
	p = token(temp, delim);
	if (p != NULL)
		{
		if (*p == '-')
			{
			sign = -1;
			p++;
			}
		else sign = 1;
 
		if (*p == '%')
			{
			base = 10;
			p++;
			}
		else if (*p == '#')
			{
			base = 2;
			p++;
			}
		else base = 0;
		
		if ((p1 = is_num_str(p)) == NULL) /* string all numeric */
			{
			i = atoi(p);
			if (base != 0)
				{
				*value = 1;
				while (i > 0)
					{
					*value = *value * base;
					i -= 1;
					}
				}
			else
				*value = i;
			*value = *value * sign;
 
			i = strlen(temp);
			if (*(input+i) == '\0')
				return(input+i);
			else
				return(input+i+1);
			}
		else
			{
			*p1 = 0;
			if (strlen(p) != 0)
				{
				i = atoi(p);
				if (base != 0)
					{
					*value = 1;
					while (i > 0)
					{
						*value = *value * base;
						i -= 1;
						}
					}
				else
					*value = i;
				*value = *value * sign;
				i = strlen(temp);
				return(input+i);
				}
			else
				{
				printf("ERROR integer decode, %s\n", input);
				return(NULL);
				}
			}
		}
	else
		{
		return(NULL);
		}
}
 
 
char *get_pair(input, value1, value2, delim1, delim2)
char *input;
int *value1;
int *value2;
int delim1;
int delim2;
{
	char *p, temp[100];
	int i, l;
 
	strncpy(temp, input, 100);
	p = token(temp, delim2);
	if (p != NULL)
		{
		p = get_int(temp, value1, delim1);
		if (NULL == get_int(p, value2, delim1))
			*value2 = -1;
 
		i = strlen(temp);
		if (*(input+i) == '\0')
			return(input+i);
		else
			return(input+i+1);
		}
	else
		{
		*value1 = *value2 = -32768;
		return(NULL);
		}
}
 
struct prim_struct *parse_ddl_key(input, family_type)
char *input;
int family_type;

{
	char *p, temp[100];
	struct prim_struct *prim, *head, *current;
	int i,j,k;
	int ignore, p_flag;
 
	head = current = NULL;
	p_flag = FALSE;
	strcpy(temp, input);
	p = strtok(temp, " ");
	ignore = FALSE;
	while (p != NULL)
	{
		if (!ignore)
			prim = (struct prim_struct *)malloc(sizeof(struct prim_struct));
		if (prim == NULL)
		{
			printf("ERROR - Malloc failure!!!\n");
			exit(2);
		}

		memset((char *)prim, 0, sizeof(struct prim_struct));

		prim->type = *p;
		prim->next = NULL;
		ignore = FALSE;
		switch(*p)
		{
		case 'W':
			{
			p++;
			if (NULL == (p=get_int(p, &(prim->data.ext_byte.num_bytes), ',')))
			{
				printf("ERROR in W primitive, Decoder Key %s\n", input);
				return(NULL);
			}
			prim->data.ext_byte.order = (unsigned int *) calloc(prim->data.ext_byte.num_bytes, sizeof(int));
			for (i=0; i<prim->data.ext_byte.num_bytes; i++)
				prim->data.ext_byte.order[i] = i;
			i = 0;
			while (NULL != (p=get_int(p, &(prim->data.ext_byte.order[i]), ',')))
			{
				i++;
			}
			if ((i!=0) && (i != prim->data.ext_byte.num_bytes))
			{
				printf("ERROR in W primitive, Decoder Key %s\n", input);
				return(NULL);
			}
 
			break;
			}
		case 'B':
		{
			char *save;

			p++;
			if (NULL == (p=get_int(p, &(prim->data.ext_bits.num_bits), ',')))
			{
				printf("ERROR in B Decoder Key %s\n", input);
				return(NULL);
			}
			if (NULL == (p=get_int(p, &(prim->data.ext_bits.t), ',')))
			{
				break;
			}
 
			save = p;
			i = 0;
			while (NULL != (p=get_pair(p,	&j, &k, '-', ','))) 
				i++;
			prim->data.ext_bits.num_order = i;
 
			if (i != 0)
			{
				prim->data.ext_bits.order1 = (unsigned int *) calloc(i, sizeof(int));
				prim->data.ext_bits.order2 = (unsigned int *) calloc(i, sizeof(int));
 
				p = save;
				for (i=0; i<prim->data.ext_bits.num_order; i++)
				{
					p=get_pair(p, 
					&(prim->data.ext_bits.order1[i]), 
					&(prim->data.ext_bits.order2[i]), 
					'-', ',');
				}
			}
			break;
			}
		case 'D':
		{
			p++;
			if (NULL == (p=get_pair(p, 
				        &(prim->data.ext_data.bits[0]),
					&(prim->data.ext_data.bits[1]),
					'-', ':')))
			{
				break;
			}
			if (NULL == (p=get_int(p, &(prim->data.ext_data.scale),  ':')))
			{
				prim->data.ext_data.scale = 1;
				prim->data.ext_data.code = 0;
				prim->data.ext_data.offset = 0;
				break;
			}
			if (NULL == (p=get_int(p, &(prim->data.ext_data.code),   ':')))
			{
				prim->data.ext_data.code = 0;
				prim->data.ext_data.offset = 0;
				break;
			}
			if (NULL == (p=get_int(p, 
					&(prim->data.ext_data.offset), ':')))
			{
				prim->data.ext_data.offset = 0;
				break;
			}
			break;
		}

		case 'C':
		{
			p++;
			if (NULL == (p=get_int(p, 
				           &(prim->data.comp_sign.type), ',')))
			{
				prim->data.comp_sign.type = 2;
				prim->data.comp_sign.mult = 0;
				break;
			}
			if (NULL == (p=get_int(p, 
					&(prim->data.comp_sign.mult), ',')))
			{
				prim->data.comp_sign.mult = 0;
				break;
			}
			break;
		}
 
		case 'S':
		{
			p++;
			if (p_flag)
			{
				if (NULL == (p=get_int(p, 
					&(prim->data.control_code.bits), ',')))
				{
					printf("ERROR in S Decoder Key %s\n", input);
					return(NULL);
				}
				if (NULL == (p=get_int(p, &(prim->data.control_code.start), ',')))
				{
					printf("ERROR in S Decoder Key %s\n", input);
					return(NULL);
					break;
				}
				if (NULL == (p=get_int(p, &(prim->data.control_code.skip), ',')))
				{
					prim->data.control_code.skip = 0;
					break;
				}
				break;
			}
			else
			{
				if (NULL == (p=get_int(p, &(prim->data.sign_bit.bit), ',')))
				{
					printf("ERROR in S Decoder Key %s\n", input);
					return(NULL);
				}
				if (NULL == (p=get_int(p, &(prim->data.sign_bit.mult), ',')))
				{
					prim->data.sign_bit.mult = 0;
					break;
				}
			}
			break;
		}
 
		case 'A':
		{
			p++;
			if (NULL == (p=get_int(p, &(prim->data.bias_sign.bias), ',')))
			{
				printf("ERROR in A Decoder Key %s\n", input);
				return(NULL);
			}
			break;
		}
 
		case 'Y':
			{
			p++;
			if (NULL == (p=get_int(p, &(prim->data.repeat_fields.count), ',')))
			{
				printf("ERROR in Y Decoder Key %s\n", input);
				return(NULL);
			}
			break;
		}
 
		case 'X':
		{
			p++;
			break;
		}

		case 'O':
		{
			p++;
			if (NULL == (p=get_int(p, &(prim->data.bit_order.type), ',')))
			{
				prim->data.bit_order.type = 0;
			}
			break;
		}
 
		case 'J':
		{
			p++;
			if (NULL == (p=get_int(p, &(prim->data.bit_direction.bit), ',')))
			{
				printf("ERROR in J Decoder Key %s\n", input);
				return(NULL);
			}
			break;
		}
 
		case 'M':
		{
			p++;
			if (NULL == (p=get_int(p, &(prim->data.multiplexed.code), ',')))
			{
				printf("ERROR in M Decoder Key %s\n", input);
				return(NULL);
			}
			break;
		}

		case 'I':
		{
			p++;

			/* must take a look at the famile type 
			 * to determine if it is integer or 
			 * indirection
			 */

			if (family_type == 50) /* steim */
				p ++;	
			else
			if (NULL == (p=get_int(p, &(prim->data.interleave.flag), ',')))
			{
				printf("ERROR in I Decoder Key %s\n", input);
				return(NULL);
			}
			break;
		}
 
		case 'L':
		{
			p++;
			if (NULL == (p=get_int(p, &(prim->data.interleave.size), ',')))
			{
				printf("ERROR in L Decoder Key %s\n", input);
				return(NULL);
			}
			break;
		}
 
		case 'P':
		{
			char *save;
 
			p++;
			save = p;
			i = 0;
			while (NULL != (p=get_pair(p, &j, &k, ':', ','))) 
				i++;
			prim->data.gain_code.num_codes = i;
			if (i == 0)
			{
				printf("ERROR in P Decoder Key %s\n", input);
				return(NULL);
			}
			else
			{
				prim->data.gain_code.code = (int *) calloc(i, sizeof(int));
				prim->data.gain_code.mult = (int *) calloc(i, sizeof(int));
			}
 
			p = save;
			for (i=0; i<prim->data.gain_code.num_codes; i++)
			{
				p=get_pair(p, &(prim->data.gain_code.code[i]), &(prim->data.gain_code.mult[i]), ':', ',');
			}
			p_flag = TRUE;
			break;
		}

		case 'E':
		{
			p++;
			if (NULL == (p=get_int(p, &(prim->data.exponent.base), ':')))
			{
				printf("ERROR in E Decoder Key %s\n", input);
				return(NULL);
			}
			if (NULL == (p=get_int(p, &(prim->data.exponent.bias), ':')))
			{
				prim->data.exponent.bias = 0;
				prim->data.exponent.mult = 1;
				prim->data.exponent.offset = 0;
				break;
			}
			if (NULL == (p=get_int(p, &(prim->data.exponent.mult), ':')))
			{
				prim->data.exponent.mult = 1;
				prim->data.exponent.offset = 0;
				break;
			}
			if (NULL == (p=get_int(p, &(prim->data.exponent.offset), ':')))
			{
				prim->data.exponent.offset = 0;
				break;
			}
			break;
		}
 
		case 'H':
		{
			p++;
			break;
		}
 
		case 'Z':
		{
			p++;
			if (NULL == (p=get_int(p, &(prim->data.zero_code.exponent), ':')))
			{
				printf("ERROR in Z Decoder Key %s\n", input);
				return(NULL);
			}
			if (NULL == (p=get_int(p, &(prim->data.zero_code.mantissa), ':')))
			{
				prim->data.zero_code.mantissa = 0x80000000;
				break;
			}
			break;
		}
 
		case 'F':
		{
			p++;
			if (NULL == (p=get_int(p, &(prim->data.forward_constant.type), ',')))
			{
				printf("ERROR in F Decoder Key %s\n", input);
				return(NULL);
			}
			break;
		}
 
		case 'R':
		{
			p++;
			if (NULL == (p=get_int(p, &(prim->data.reverse_constant.type), ',')))
			{
				printf("ERROR in R Decoder Key %s\n", input);
				return(NULL);
			}
			break;
		}
 
		case 'N':
		{
			p++;
			if (NULL == (p=get_int(p, &(prim->data.control_number.number), ',')))
			{
				printf("ERROR in N Decoder Key %s\n", input);
				return(NULL);
			}
			break;
		}
 
		case 'T':
		{
			p++;
			if (NULL == (p=get_int(p, &(prim->data.control_key.code), ',')))
			{
				printf("ERROR in T Decoder Key %s\n", input);
				return(NULL);
			}
			break;
		}
 
		case 'K':
		{
			p++;
			if (NULL == (p=get_int(p, &(prim->data.sub_control_key.code), ',')))
			{
				printf("ERROR in K Decoder Key %s\n", input);
				return(NULL);
			}
			break;
		}

		case 'G':
		{
			p++;
			if (NULL == (p=get_int(p, &(prim->data.compress_frame.length), ',')))
			{
				printf("ERROR in G Decoder Key %s\n", input);
				return(NULL);
			}
			break;
		}

/*		default:
				{
				p++;
				printf("ERROR in invalid Decoder Key %c\n", *p);
				ignore = TRUE;
				}
*/
		}

		if (!ignore)
		{
			if (head == NULL)
			{
				head = prim;
				current = prim;
			}
			else
			{
				current->next = prim;
				current = prim;
			}
		}

		p = strtok(NULL, " ");

	}

	return(head);

}
