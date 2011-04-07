#include <stdio.h>
#include <math.h>
double pi= 3.1415926;
int main()
{
	double angle=1;
	int i = 0;
	for( i = 0; i < 90;i++)
	{
	
		double a = ((double)pi) / 180;
		printf("sin %d = %0.2f\n",i,sin(a));
	}
}
