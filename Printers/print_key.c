#include "ddl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#define TRUE 1
#define FALSE 0
 
struct prim_struct *print_ddl_key(head)
struct prim_struct *head;
{
	char *p, temp[100];
	struct prim_struct *prim;
	int i,j,k;
	int p_flag;
 
	p_flag = FALSE;
	prim = head;
	while (prim != NULL)
		{
		switch(prim->type)
			{
			case 'W':
				{
				printf("W%d", prim->data.ext_byte.num_bytes);
				for (i=0; i<prim->data.ext_byte.num_bytes; i++)
					printf(",%d", prim->data.ext_byte.order[i]);
				printf(" ");
				break;
				}
			case 'B':
				{
				printf("B%d,%d",
					prim->data.ext_bits.num_bits,
					prim->data.ext_bits.t);
				for (i=0; i<prim->data.ext_bits.num_order; i++)
					{
					printf(",%d",
						prim->data.ext_bits.order1[i]);
					if (prim->data.ext_bits.order2[i] >= 0)
						printf("-%d", prim->data.ext_bits.order2[i]);
					}
				printf(" ");
				break;
				}
			case 'D':
				{
				printf("D%d",
					prim->data.ext_data.bits[0]);
				if (prim->data.ext_data.bits[1] >= 0)
					printf("-%d", prim->data.ext_data.bits[1]);
				printf(":%d", prim->data.ext_data.scale);
				printf(":%d", prim->data.ext_data.code);
				printf(":%d ", prim->data.ext_data.offset);
				break;
				}
 
			case 'C':
				{
				printf("C%d", prim->data.comp_sign.type);
				printf(",%d ", prim->data.comp_sign.mult);
				break;
				}
 
			case 'S':
				{
				if (p_flag) /* control code decode */
					{
					printf("S%d", prim->data.control_code.bits);
					printf(",%d", prim->data.control_code.start);
					printf(",%d ", prim->data.control_code.skip);
					break;
					}
				else /* sign bit decode */
					{
					printf("S%d", prim->data.sign_bit.bit);
					printf(",%d ", prim->data.sign_bit.mult);
					}
				break;
				}
 
			case 'A':
				{
				printf("A%d ", prim->data.bias_sign.bias);
				break;
				}
 
			case 'Y':
				{
				printf("Y%d ", prim->data.repeat_fields.count);
				break;
				}
 
			case 'X':
				{
				printf("X ");
				break;
				}
 
			case 'O':
				{
				printf("O%d ", prim->data.bit_order.type);
				break;
				}
 
			case 'J':
				{
				printf("J%d ", prim->data.bit_direction.bit);
				break;
				}
 
			case 'M':
				{
				printf("M%d ", prim->data.multiplexed.code);
				break;
				}
 
			case 'I':
				{
				printf("I%d ", prim->data.interleave.flag);
				break;
				}
 
			case 'L':
				{
				printf("L%d ", prim->data.interleave.size);
				break;
				}
 
			case 'P':
				{
				printf("P");
				for (i=0; i<prim->data.gain_code.num_codes; i++)
					{
					printf("%d",
						prim->data.gain_code.code[i]);
					if (prim->data.gain_code.mult[i] >= 0)
						printf(":%d,", prim->data.gain_code.mult[i]);
					}
				printf(" ");
				p_flag = TRUE;
				break;
				}
 
			case 'E':
				{
				printf("E%d", prim->data.exponent.base);
				printf(":%d", prim->data.exponent.bias);
				printf(":%d", prim->data.exponent.mult);
				printf(":%d ", prim->data.exponent.offset);
				break;
				}
 
			case 'H':
				{
				printf("H");
				break;
				}
 
			case 'Z':
				{
				printf("Z%d", prim->data.zero_code.exponent);
				printf(":%d ", prim->data.zero_code.mantissa);
				break;
				}
 
			case 'F':
				{
				printf("F%d ", prim->data.forward_constant.type);
				break;
				}
 
			case 'R':
				{
				printf("R%d ", prim->data.reverse_constant.type);
				break;
				}
 
			case 'N':
				{
				printf("N%d ", prim->data.control_number.number);
				break;
				}
 
			case 'T':
				{
				printf("T%d ", prim->data.control_key.code);
				break;
				}
 
			case 'K':
				{
				printf("K%d ", prim->data.sub_control_key.code);
				break;
				}
 
			case 'G':
				{
				printf("G%d ", prim->data.compress_frame.length);
				break;
				}
			default:
				{
				printf("ERROR in invalid Decoder Key %c\n", prim->type);
				}
			} /* end primitive code switch */
 
		prim = prim->next;
		}
	printf("\n");
}
