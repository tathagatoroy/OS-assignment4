#include<stdio.h>
#include<stdlib.h> 
#include<time.h>
int main()
{
    int size = 1000;
    printf("%d\n",size);
    for(int i=0;i<size;i++)
    printf("%d ",rand());
    return 0;

}