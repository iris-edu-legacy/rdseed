#include <stdio.h>
#include "rdseed.h"

static int cnt = 0;

/* static cnter = 0;*/
void decode_usnsn(data_ptr, data_hdr)
int *data_ptr;
struct input_data_hdr *data_hdr;


{

/* FILE *fptr; */


	int i, error, n = -1, eod, overflow;

	int *decomp;

/* fptr = fopen("usnsn.out", "a+"); */

	decomp = (int *)malloc(sizeof(int)*data_hdr->nsamples);

	memset((char *)decomp, 0, sizeof(int)*data_hdr->nsamples);

	if (byteswap)
	{
		for (i = 0; i < (LRECL - data_hdr->bod); i += sizeof(int))
			*data_ptr = swap_4byte(*(data_ptr + i));
	}

	error = dcmprs(data_hdr->nsamples, &n, 
			decomp, 
			&eod, &overflow,
			data_ptr);	

	for (i = 0; i <= n; i++)
	{
		*seismic_data_ptr = (double)decomp[i];

/* fprintf(fptr, "%d : %6.2f\n", cnt, *seismic_data_ptr); */
/* cnt++; */

		seismic_data_ptr++;


	}

/* fclose(fptr); */

	free(decomp);

	return;
}

/* ------------------------------------------------------------------------- */
#define	  NBK	     4096 
#define   NST	        7

static int prnt = 1;
int ipt,nct,ifr,iovr,ldcmprs;
int npt,id0;

static int nib[3][16] = {4, 4, 4, 6, 6, 8, 8,10,10,12,14,16,20,24,28,32,
                         4, 8,12, 4, 8, 4, 8, 4, 8, 4, 4, 4, 4, 4, 4, 4,
                         2, 4, 6, 3, 6, 4, 8, 5,10, 6, 7, 8,10,12,14,16};


int mask[16] = {0x00000003,0x0000000F,0x0000003F,0x000000FF,
                 0x000003FF,0x00000FFF,0x00003FFF,0x0000FFFF,
                 0x0003FFFF,0x000FFFFF,0x003FFFFF,0x00FFFFFF,
                 0x03FFFFFF,0x0FFFFFFF,0x3FFFFFFF,0xFFFFFFFF};
int isgn[16] = {0x00000002,0x00000008,0x00000020,0x00000080,
                 0x00000200,0x00000800,0x00002000,0x00008000,
                 0x00020000,0x00080000,0x00200000,0x00800000,
                 0x02000000,0x08000000,0x20000000,0x80000000};
int msgn[16] = {0xFFFFFFFC,0xFFFFFFF0,0xFFFFFFC0,0xFFFFFF00,
                 0xFFFFFC00,0xFFFFF000,0xFFFFC000,0xFFFF0000,
                 0xFFFC0000,0xFFF00000,0xFFC00000,0xFF000000,
                 0xFC000000,0xF0000000,0xC0000000,0x00000000};

int dcmprs(maxx,n,idat,eod,ovr,icmp)
/*
   Dcmprs decompresses a series previously compressed by cmprs and 
   cmfin.  On each call to dcmprs, one compression record, provided in 
   icmp[], is decompressed into output array idat[max].  On the first 
   call, n must be set less than 0.  On successive calls, n will be the 
   maintained by dcmprs to be the C array index of the last point 
   decompressed (the number of points decompressed into idat so far 
   minus one).  Eod will be set to 1 if the entire time series has 
   been finished (0 otherwise).  Ovr will be set to 1 if more series 
   was available than would fit into idat (0 otherwise).
*/
int maxx,*n,*eod,*ovr;
int idat[],icmp[];
{
   int nn,fin,ln,j,lm;
   int ia0;

/* Get the forward integration contstant and the number of samples in the
   record.  Initialize internal variables. */
   ipt = 1;
   gnible(icmp,&ia0,&ipt,32,1,1,1);
   gnible(icmp,&npt,&ipt,16,1,1,0);
   ifr = 0;
   ipt = NST;
   nct = ipt-1;
   id0 = ia0;
   iovr = 0;
   ldcmprs = 1;

   if(*n < 0)
/* If this is the first record, set the first data point to be the 
   forward integration constant. */
   {
      *n = 0;
      idat[*n] = ia0;
   }
   else
/* If this is not the first record, check the internal consistency of 
   the new forward integration constant. */
      if(idat[*n] != ia0)
      {
         if(prnt)
          printf("########## ia0 mismatch ########## idat=%d ia0=%d\n",
          idat[*n],ia0);
         ldcmprs = 0;
      }
   lm = *n+npt;

   for(;;)
   {
/* Unpack each frame in turn. */
      ifr = ifr+1;
      unpacknsn(maxx-*n-1,&nn,&idat[*n+1],&fin,ovr,eod,icmp);
/* If we were in danger of an integer overflow clean up and get out. */
      if(iovr != 0)
      {
         ln = (lm <= maxx) ? lm : maxx;
         for(j = *n+1; j < ln; j++) idat[j] = 0;
         *n = ln-1;
         fin = 1;
      }
      else
         *n = *n+nn+1;
/* Bail out if the output buffer is full or if this was the last frame. */
      if(maxx-*n <= 1) *ovr = 1;
      if(*ovr != 0 || fin != 0) return(ldcmprs);
   }
}

unpacknsn(maxx,n,idat,fin,ovr,eod,icmp)
/*
   Subroutine unpacknsn unpacks data out of compression frame ifr into 
   array idat[max].  On return, idat will contain n+1 decompressed data 
   points.  If the series ended during the compression frame, fin will 
   be set to nonzero.  If there was more data in the compression frame 
   than will fit into idat, ovr will be set to nonzero.  If this is the 
   last frame of the time series, eod will be set to nonzero.
*/
int maxx,*n,*fin,*ovr,*eod;
int idat[],icmp[];
{
   int key[2],ict,lpt,ian;
   int js,kpt,j,jb,ln;

/* Initialize output flags. */
   *fin = 0;
   *ovr = 0;
   *eod = 0;

/* Unpack the frame key fields. */
   gnible(icmp,key,&ipt,4,2,2,0);

/* If the integration constant is over 2**30 or we are using 32-bit 
   differences we better bail. */
   if(id0 >= 1073741824 || key[0] >= 15 || key[1] >= 15)
   {
      if(prnt)
       printf("## impending integer overflow ## id0=%d keys=%d %d ipt=%d\n",
       id0,key[0],key[1],ipt);
      ldcmprs = -1;
      iovr = 1;
      return;
   }

/* Initialize some counters. */
   js = 0;
   kpt = 0;

/* Loop over the data fields in the frame. */
   for(j=0; j<2; j++)
   {
/* Bail out if the output buffer is full. */
      if(js >= maxx)
      {
         *ovr = 1;
         break;
      }
      jb = key[j];
/* Set the number of samples to unpack to get to the end of the data 
   field, the end of the samples in the record, or the end of the 
   output buffer, whichever comes first. */
      ln = (nib[1][jb] <= maxx-js) ? nib[1][jb] : maxx-js;
      ln = (ln <= npt) ? ln : npt;
/* Unpack the data. */
      gnible(icmp,&idat[js],&ipt,nib[0][jb],ln,nib[1][jb],1);
/* Update pointers and counters. */
      js = js+ln;
      npt = npt-ln;
      kpt = kpt+ln;
/* End of the record trap. */
      if(npt <= 0)
      {
         *fin = 1;
         if(j < 1) ipt = ipt+nib[2][0];
         break;
      }
   }

/* Fiddle the record buffer pointer so that trailer information may be 
   found. */
   *n = js-1;

/* Integrate the first differences to recover the input time series. */
   if(*n >= 0)
   {
      idat[0] = idat[0]+id0;
      if(*n >= 1)
      {
         for(j=1; j<=*n; j++) 
	{
/*
if (idat[j] == 0)
{
	printf("cnter=%d\n", cnter);
}
	cnter++;
*/

		idat[j] = idat[j]+idat[j-1];
/* printf("cnter=%d - sample=%d\n", cnter, idat[j]); */

	}

      }
   }
/* Reset id0 for next time. */
   id0 = idat[*n];

   if(*ovr != 0 || (ifr%7 != 0 && *fin ==0)) return;

/* Check the end of block back pointer. */
   nct = ipt-nct;
   gnible(icmp,&ict,&ipt,8,1,1,0);
   if(ict != nct)
   {
      if(prnt)
       printf("########## nct mismatch ########## ict=%d nct=%d ipt=%d\n",
       ict,nct,ipt);
      if(ldcmprs!=-1) ldcmprs = -4;
   }
   nct = ipt-1;
   if(*fin == 0 || ipt > NBK-4) return;
   gnible(icmp,&lpt,&ipt,8,1,1,0);
   if(lpt == 0) return;

/* For the last record of the series, check consistency. */
   *eod = 1;
/* Check that the number of samples in the last frame is as expected. */
   if(kpt != lpt)
   {
      if(prnt)
       printf("########## kpt mismatch ########## kpt=%d lpt=%d ipt=%d\n",
       kpt,lpt,ipt);
      if(ldcmprs>=0) ldcmprs = -5;
   }
   ipt = NBK-3;
   gnible(icmp,&ian,&ipt,32,1,1,1);
/* Check that the reverse integration constant is as expected. */
   if(idat[*n] != ian)
   {
      if(prnt)
       printf("########## ian mismatch ########## ian=%d idat[n]=%d\n",
       ian,idat[*n]);
      if(ldcmprs>=0) ldcmprs = -6;
   }
   return;
}


gnible(ib,ia,ns,nb,n,nrun,sgn)
/*
   Gnible gets n consecutive nibbles of length nb bits from byte array 
   ib beginning at byte ib[ns] and puts them into integer*4 array ia[].  
   No bits are disturbed in ib.  If sgn != 0, high order bits in ia 
   are sign extended from the sign bit of the nibble.  If sgn == 0, 
   the nibble is taken to be unsigned and high order bits in ia are 
   cleared.  Ns is updated to point to the next unprocessed byte in ib 
   assuming that nrun nibbles had been processed (rather than n).  Note 
   that even length nibbles up to 32-bits work except for 30-bits.
*/
char ib[];
int ia[];
int *ns,nb,n,nrun,sgn;
{
   char ja[4];
   int *ka;
   int kb,isw,mb,mbe,krun,kshf,ishf,ke,npt;
   int k,i,j;

/* Initialize some constants. */
   ka = (int *)ja;
   kb = nb/2-1;
   isw = (kb%4)+1;
   mb = 4-(kb+5-isw)/4;
   npt = *ns+(nrun*nb)/8;
   *ns -= 1;   /* Bump ns down for the C array indexing convention. */

   switch (isw)
   {
      case 1:   /* 2, 10, 18, and 26-bit nibbles */
         krun = 4;
         goto cs2;

      case 2:   /* 4, 12, 20, and 28-bit nibbles */
         krun = 2;
cs2:
         kshf = 2*isw;
/* Take the data in groups of krun. */
         for(k = 0; k < n; k = k+krun)
         {
            ishf = 8;
            ke = (k+krun-1<n)?k+krun-1:n-1;

	    /* Unpack each word in this group. */

            for(i = k; i <= ke; i++)
            {

/* Copy the bytes in this nibble. */

               *ns -= 1;
               for(j = mb; j <= 3; j++) 
		{
			ja[j] = ib[++(*ns)];
		}

/* Shift the nibble into place. */

               ishf = ishf-kshf;
               *ka = *ka>>ishf;

/* Extend or clear the sign bits as needed. */

               if((*ka&isgn[kb])!=0 & sgn!=0) 
			ia[i] = (*ka|msgn[kb]);
               else    
			ia[i] = (*ka&mask[kb]);
/*
if (ia[i] == 0)
	printf("found it - cnter=%d\n", cnter);

cnter++;
*/

            }

/* Each group ends on a byte boundary, so adjust ns. */

            *ns += 1;
         }
         break;

      case 3:   /* 6, 14, 22, and 30-bit nibbles */
         kshf = 2*isw;
/* Take the data in groups of 4. */
         for(k = 0; k < n; k = k+4)
         {
            ishf = 8;
            ke = (k+3<n)?k+3:n-1;
/* Unpack each word in this group. */
            for(i = k; i <= ke; i++)
            {
               ishf = ishf-kshf;
               if(ishf < 0)
/* In this case, the second and third words of the group take an extra byte. */
               {
                  mbe = mb-1;
                  ishf = ishf+8;
               }
               else mbe=mb;
/* Copy the bytes in this nibble. */
               *ns -= 1;
               for(j = mbe; j <= 3; j++) ja[j] = ib[++(*ns)];
/* Shift the nibble into place. */
               *ka = *ka>>ishf;
/* Extend or clear the sign bits as needed. */

               if((*ka&isgn[kb])!=0 & sgn!=0) 
			ia[i] = (*ka|msgn[kb]);
               else                           
			ia[i] = (*ka&mask[kb]);
/*
 if (ia[i] == 0)
	printf("found it cnter=%d\n", cnter);

cnter++;
*/


            }
/* Each group ends on a byte boundary, so adjust ns. */
            *ns += 1;
         }
         break;

      case 4:   /* 8, 16, 24, and 32-bit nibbles */
         *ns -= 1;
/* Loop over each input word. */
         for(i = 0; i < n; i++)
         {
            for(j = mb; j <= 3; j++) ja[j] = ib[++(*ns)];
/* Extend or clear the sign bits as needed. */
               if((*ka&isgn[kb])!=0 & sgn!=0) ia[i] = (*ka|msgn[kb]);
               else                           ia[i] = (*ka&mask[kb]);
/*
 if (ia[i] == 0)
	printf("found it - cnter=%d\n", cnter);

cnter++;
*/


         }
         break;
   }
/* Adjust ns back to the FORTRAN convention. */
   *ns = npt;

   return;
}


dcmpbr()
/*
   Dcmpbr toggles the print flag controlling Dcmprs diagnostic output.
*/
{
   if(prnt) prnt = 0;
   else prnt = 1;
   return;
}

dcmper(ierr)
/*
   Dcmper prints out a Dcmprs diagnostic based on the status flag returned 
   by Dcmprs.
*/
int ierr;
{
   switch (ierr)
   {
      case  1:                /* Success. */
         break;
      case  0:
         printf("IA0 mismatch in Dcmprs.\n");
         break;
      case -1:
         printf("Integer overflow in Dcmprs.\n");
         break;
      case -4:
         printf("NCT mismatch in Dcmprs.\n");
         break;
      case -5:
         printf("KPT mismatch in Dcmprs.\n");
         break;
      case -6:
         printf("IAN mismatch in Dcmprs.\n");
         break;
      default:
         printf("Unknown error in Dcmprs (%d).\n",ierr);
         break;
   }
   return;
}
