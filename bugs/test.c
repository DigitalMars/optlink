#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1610

float a[N][N], b[N][N];

/* NASTY PROBLEM: if array c is not declared 'static', any reference to it in the code breaks
DigitalMars linking!!*/
//static
 float c[2000][2000];

int main()
{
    int i, j, k;
    float *pa, *pb, *pc;
    float tot = 0.0;

    pa=a[0];
    pb=b[0];
    for(i=0; i<5; ++i)
    {
	*pa++ = rand();
	*pb++ = rand();
    }

    pc=c[0];
    tot += *pc++;
    return 0;
}
