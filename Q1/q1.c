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
    int mid = (l+r)/2;
    if(size <= 5)
    insertion_sort(a,l,r);
    else
    {
        pid_t left_pid,right_pid;
        left_pid = fork();
        if(left_pid < 0)
        {
            printf("left child couldnot be created\n");
            printf("exiting ....\n");
            exit(1);
        }
        else if(left_pid == 0)
        {
            mergesort(a,l,mid);
            exit(1);
        }
        else
        {
            right_pid = fork();
            if(right_pid < 0)
            {
                printf("right child couldnot be created\n");
                printf("exiting .....\n");
                exit(1);
            }
            else if(right_pid == 0)
            {
                mergesort(a,mid+1,r);
                exit(1);
            }
            
        }
        int status;
        waitpid(left_pid,&status,0);
        waitpid(right_pid,&status,0);
        merge(a,l,mid,r);
    }



}
void print(int a[],int n)
{
    for(int i=0;i<n;i++)
    printf("%d ",a[i]);
    printf("\n");
}

int main()
{
    int id;
    key_t key = IPC_PRIVATE;
    int* shared_mem;
    int n;
    printf("Enter the size of the testing array\n");
    scanf("%d",&n);
    size_t size = n*sizeof(int);
    //create the shared memory space
    id = shmget(key,size,IPC_CREAT | 0666);
    if(id < 0)
    {
        printf("Error in getting the shared memory \n");
        perror("error :");
        printf("Exiting ....\n");
        exit(1);
    }

    //assign the shared memory created
    shared_mem = shmat(id,NULL,0);
    if(shared_mem == (int*)-1)
    {
        perror("error :");
        printf("Exiting ...\n");
        exit(1);

    }
    printf("Enter the array\n");
    for(int i=0;i<n;i++)
    scanf("%d",&shared_mem[i]);
    mergesort(shared_mem,0,n-1);
    //print(shared_mem,n);
    int ans = check(shared_mem,n);
    
    if(ans == 1)
    printf("array sorted\n");
    else
    printf("array not sorted\n");
    


    //detach the created memory
    int d = shmdt(shared_mem);
    if(d == -1)
    {
        perror("error");
        printf("exiting .....\n");
        exit(1);
    }
    
    //delete the shared memory
    int s = shmctl(id,IPC_RMID,NULL);
    if(s == -1)
    {
        perror("error");
        printf("exiting ......\n");
        exit(1);
    }
    
    return 0;
}
