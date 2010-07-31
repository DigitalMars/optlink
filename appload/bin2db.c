#include <STDIO.H>
#include <IO.H>
#include <STDLIB.H>
#include <DOS.H>
#define BUFLEN 0x4000	//Multiple of 16 only
#define MAXLEN 128
#define HEXOUT(y)	((y) &0xf)>9 ? (((y) &0xf)+0x37) : (((y) &0xf) +0x30)

unsigned char buffer[BUFLEN];

int nn,buffer_fill(unsigned char my_buffer[BUFLEN],FILE *ptr);
long numtoread;

int main(int argc, char *argv[])
{
	FILE *fileptr, *outfile;
	int LENGTH,dec,i, j, bytes_left;
   long	skip;
	unsigned char bin_string[MAXLEN*5+3];
	unsigned char *ptr,*bptr;

	printf("\nBIN2DB (DPMI)  Copyright (C) SLR Systems, Inc. 1993 Rel. 0.02à\n");
	if(argc<3)
		{ printf("\nFormat ->BIN2DB infile outfile [linect(max %Xh)] [offset] [length]\n",MAXLEN,MAXLEN); exit(1); }
	if((fileptr=fopen(argv[1],"rb"))==NULL)
		{ fprintf(stderr,"\nCannot open %s",argv[1]); exit(1); }
	if((outfile=fopen(argv[2],"w+b"))==NULL)
		{ fprintf(stderr,"\nCannot open %s",argv[2]); exit(1); }
   if(argc<4)
   	LENGTH=1;
   else
      sscanf(argv[3],"%i",&LENGTH);
   if(argc>4)
   {
      sscanf(argv[4],"%I",&skip);
      if(filelength(fileno(fileptr))<skip)
			{ fprintf(stderr,"\nCannot seek to %04lXh(%lu), offset too large",skip,skip); exit(1); }
      fseek(fileptr,skip,SEEK_SET);
   }
   nn=0;
	if (argc>5)
   	sscanf(argv[5],"%I",&numtoread);
   else
      nn=1;

   if (LENGTH>MAXLEN)
		LENGTH=MAXLEN;
 	while ((bytes_left=buffer_fill(buffer,fileptr)) > 0)
	{
		ptr = buffer;
		while (bytes_left > 0)
		{
			i= (bytes_left < LENGTH) ? bytes_left : LENGTH;
			bptr=bin_string;
			for(j=0 ; j<i ; j++)
			{
            *bptr++='0';
				*bptr++=HEXOUT(*ptr >> 4);
				*bptr++=HEXOUT(*ptr);
				*bptr++='h';
				if (j<(i-1))
         		*bptr++=',';
				ptr++;
			}
			bytes_left-=i;
			*bptr = '\0';
			fprintf(outfile,"	db %s\r\n",bin_string);
		};
	};
	fclose(fileptr); fclose(outfile);
	return(0);
}

// Buffer Read Function
int buffer_fill(unsigned char my_buffer[BUFLEN],FILE *ptr)
{
	int bytes_read;
   if (nn)
    	numtoread=BUFLEN;
   else
      if (numtoread<1)
         return 0;
   bytes_read=fread(my_buffer,1,(BUFLEN<numtoread) ? BUFLEN : numtoread,ptr);
   numtoread-=bytes_read;
	return bytes_read;
}
