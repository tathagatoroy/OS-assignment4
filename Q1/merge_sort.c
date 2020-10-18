#include<stdio.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
int check(int a[],int n)
{
    int tt = 1;
    int last = a[0];
    for(int i=0;i<n;i++)
    {
        if(a[i] >= last)
        {
           last = a[i];
        }
        else
        {
            tt = 0;
            break;
        }
    }
    return tt;

}
void print(int a[],int n)
{
    for(int i=0;i<n;i++)
    printf("%d ",a[i]);
    printf("\n");
}
void insertion_sort(int a[],int l,int r)
{
    for(int i=l+1;i<=r;i++)
    {
        int key = a[i];
        int start = i-1;

        while(start>=l && a[start] > key)
        {
          //  printf("%d\n",start);
            a[start+1]=a[start];
            start--;
        }
        a[start+1]=key;
    }
}
void merge(int a[],int l,int mid,int r)
{
   int l_size = mid-l+1;
   int r_size = r-(mid+1)+1;
   int left_temp[l_size];
   int right_temp[r_size];
   for(int i=l;i<=mid;i++)
   left_temp[i-l]=a[i];
   for(int i=mid+1;i<=r;i++)
   right_temp[i-mid-1]=a[i];
   int i=0;
   int j=0;
   int k=l;
   while(i<l_size && j<r_size)
   {
       if(left_temp[i]<right_temp[j])
       {
           a[k]=left_temp[i];
           k++;
           i++;
       }
       else
       {
           a[k]=right_temp[j];
           k++;
           j++;
       }
   }
   while(i<l_size)
   {
       a[k]=left_temp[i];
       k++;
       i++;
   }
   while(j<r_size)
   {
       a[k]=right_temp[j];
       j++;
       k++;
   }
}
void mergesort(int a[],int l,int r)
{
    int size = r-l+1;
    if(size <= 5)
    {
        insertion_sort(a,l,r);
    }
    else
    {
       int mid = (l+r)/2;
       mergesort(a,l,mid);
       mergesort(a,mid+1,r);
       merge(a,l,mid,r);
    }
}
int main()
{
   // printf("Enter the size of the array\n");
    int n;
    scanf("%d",&n);
    int a[n];
    for(int i=0;i<n;i++)
    scanf("%d",&a[i]);
    mergesort(a,0,n-1);
    //print(a,n);
    if(check(a,n) == 1)
    printf("sorted\n");
    else
    printf("unsorted\n");
    return 0;
}