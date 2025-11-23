/*
 * Ternarnye_operac.c
 * 
 * 	expression1 ? expression2 : expression3;
	● сначала вычисляется выражение expression1
	● если его значение отлично от нуля, то результатом всей операции
	является значение выражения expression2
	● иначе – значение выражения expression3.
	a ? b : c ? d : e эквивалентно (a ? b : ( c ? d : e))
 */


#include <stdio.h>
#include <inttypes.h>


int main()
{
	int a,b, max;
	//scanf("%d%d",&a,&b);
	//if (a>b)
	//	max = a;
	//else
	//	max = b;
	//printf("%d\n",max);
	
	// короткая запись if else
	//условная операция
	
//	scanf("%d%d",&a,&b);						
//	max = a>b ? a : b;
//	printf("%d",max);

	// еще одна запись
	
//	scanf("%d%d",&a,&b);
//	a>b ? printf("%d", a): printf("%d", b);
	
	// еще одна запись
	
	scanf("%d%d",&a,&b);
	printf("%d", a>b ? a : b);
	
	return 0;
}

