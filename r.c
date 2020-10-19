#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <wait.h>
#include <semaphore.h>
#include<string.h>
#include<time.h>

#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define CYAN "\033[1;36m"
#define BLUE "\033[1,34m"
#define DEFAULT "\033[0m"
#define CLEAR "\033[2J\033[1;1H"

typedef struct m1
{
    int id;
    pthread_t tid;
    pthread_mutex_t m;
    pthread_cond_t cv;
    int state ;
    char ins;
    int a;

}m1;

m1** all;

int k,a,c,e,t1,t2,t;
int range;
int mus[1024];//1 singer // 2 acc // 3 ele // 4 both
int stage[1024]; //1 acc singer // 2 ele singer // 3 acc music // 4 ele mus
char name[1024][50];

typedef struct s1
{
    int id;
    int state ;
    pthread_mutex_t m;
    pthread_cond_t cv;
    pthread_t tid;
}s1;
s1** ss;
sem_t b1;
sem_t c1;
sem_t a1;
sem_t e1;
int size;
void sing_init(int id,m1* z,char c,int arr)
{
  z->state = 0;
  z->id = id;
  mus[id] = 1;
  pthread_cond_init(&(z->cv),NULL);
  pthread_mutex_init(&(z->m),NULL);
  z->ins = c;
  z->a = arr;

}
void both_init(int id,m1* z,char c,int arr)
{
  z->state = 0;
  z->id = id;
  mus[id] = 4;
  pthread_cond_init(&(z->cv),NULL);
  pthread_mutex_init(&(z->m),NULL);
  z->ins = c;
  z->a = arr;
}
void acc_init(int id,m1* z,char c,int arr)
{
  z->state = 0;
  z->id = id;
  mus[id] = 2;
  pthread_cond_init(&(z->cv),NULL);
  pthread_mutex_init(&(z->m),NULL);
  z->ins = c;
  z->a = arr;
}

void ele_init(int id,m1* z,char c,int arr)
{
  z->state = 0;
  z->id = id;
  mus[id] = 3;
  z->a = arr;
  pthread_cond_init(&(z->cv),NULL);
  pthread_mutex_init(&(z->m),NULL);
}
void stage1(int id,s1* z)
{
    z->id = id;
    z->state = 0;
     pthread_cond_init(&(z->cv),NULL);
  pthread_mutex_init(&(z->m),NULL);
  stage[id] = 1;
  

}
void stage2(int id,s1* z)
{
    z->id = id;
    z->state = 0;
     pthread_cond_init(&(z->cv),NULL);
  pthread_mutex_init(&(z->m),NULL);
  stage[id] = 2;
  

}
void init()
{
    sem_init(&b1,0,0);
    sem_init(&a1,0,0);
    sem_init(&e1,0,0);
    sem_init(&c1,0,c);
    range=t2-t1+1;
    size = a+e;
}
struct timespec timer(int u)
{
    struct timespec tr;
    clock_gettime(CLOCK_REALTIME,&tr);
    tr.tv_nsec += u*1000000000;
    tr.tv_sec = tr.tv_nsec/1000000000;
    tr.tv_nsec = tr.tv_sec%1000000000;
    return tr;
}



void* staging1(void* inp)
{
    
    printf("electrical stage\n");
    sleep(1);
    s1* arg = (s1*)inp;

    while(true)
    {
        int x = -1;
        while(true)
        {
            pthread_mutex_lock(&arg->m);
            printf("stage id is ready\n");
          int r = sem_trywait(&a1);
          if(r == 1)
          {
              for(int i=0;i<k;i++)
              {
                  if(mus[i]!=3)
                  {
                      pthread_mutex_lock(&all[i]->m);
                      if(all[i]->state == 1)
                      {
                          if(mus[i]==1)
                         { arg->state  = 10;
                           sem_wait(&e1);
                         }
                          else 
                          arg->state = 2;
                          all[i]->state = 2;
                          x = i;
                          pthread_cond_signal(&all[i]->cv);
                          break;

                      }
                      pthread_mutex_unlock(&all[i]->m);
                  }

              }
              if(x!=-1)
              break;
          }
          /*pthread_mutex_lock(&left);
          if(left == 0)
          {
              pthread_mutex_unlock(&left);
              pthread_mutex_unlock(&arg->m);
              return NULL;
          }*/
        }

        //now we have a musician
          int num = rand()%range+t1;
          struct timespec tr = timer(num);
          int err ;
          printf(CYAN "%s is starting in accoustic stage %d for time %d\n" DEFAULT,name[x],num,arg->id);
          
          if(arg->state == 10)
          {

              sleep(num);

              printf(RED "%s has finished his performance in accoustic stage %d\n" DEFAULT,name[x],arg->id);
              pthread_mutex_lock(&all[x]->m);
              all[x]->state = 5;
              pthread_cond_signal(&all[x]->cv);
              pthread_mutex_unlock(&all[x]->m);
              arg->state = 0;
          }
          else
          {
              sem_post(&b1);
              int dd = 1;
              int ii = -1;
              while(dd == 1)
              {
                  int e = pthread_cond_timedwait(&arg->cv,&arg->m,&tr);
                  if(arg->state >= 100)
                  {
                       ii = arg->state - 100; 
                      printf(YELLOW "%s has joined %s in accoustic stage %d ,time extended by 2 sec\n" DEFAULT,name[ii],name[x],arg->id);
                      tr.tv_sec += 2;
                  }
                  else if(err == ETIMEDOUT)
                  {
                      if(ii == -1)
                      {
                          sem_wait(&b1);
                          printf(RED "%s has ended his performance in accoustic stage %d\n" DEFAULT,name[x],arg->id);
                          
                      }
                      else 
                      {
                          printf(RED "%s and %s has ended their performance in accoustic stage %d" DEFAULT,name[x],name[ii],arg->id);
                          pthread_mutex_lock(&all[ii]->m);
                          all[ii]->state = 5;
                          pthread_cond_signal(&all[ii]->cv);
                          pthread_mutex_unlock(&all[ii]->m);
                      }
                          pthread_mutex_lock(&all[x]->m);
                          all[x]->state = 5;
                          pthread_cond_signal(&all[x]->cv);
                          pthread_mutex_unlock(&all[x]->m);
                          arg->state = 0;
                          break;
                  }
              }
          }
      pthread_mutex_unlock(&arg->m);
    }

}
void* staging2(void* inp)
{
    printf("accoustic stage\n");
    sleep(1);
    s1* arg = (s1*)inp;

    while(true)
    {
        int x = -1;
        while(true)
        {
            pthread_mutex_lock(&arg->m);
            printf("stage %d is ready\n",arg->id);
          int r = sem_trywait(&e1);
          if(r == 1)
          {
              for(int i=0;i<k;i++)
              {
                  if(mus[i]!=2)
                  {
                      pthread_mutex_lock(&all[i]->m);
                      if(all[i]->state == 1)
                      {
                          if(mus[i]==1)
                          {
                              arg->state  = 10;
                              sem_wait(&a1);
                          }
                          else 
                          arg->state = 2;
                          all[i]->state = 2;
                          x = i;
                          pthread_cond_signal(&all[i]->cv);
                          break;

                      }
                      pthread_mutex_unlock(&all[i]->m);
                  }

              }
              if(x!=-1)
              break;
          }
         /* pthread_mutex_lock(&left);
          if(left == 0)
          {
              pthread_mutex_unlock(&left);
              pthread_mutex_unlock(&arg->m);
              return NULL;
          }*/
        }

        //now we have a musician
          int num = rand()%range+t1;
          struct timespec tr = timer(num);
          int err ;
          printf(CYAN "%s is starting in electric stage %d for time %d\n" DEFAULT,name[x],num,arg->id);
          
          if(arg->state == 10)
          {
              sleep(num);
              printf(RED "%s has finished his performance in electric stage %d\n" DEFAULT,name[x],arg->id);
              pthread_mutex_lock(&all[x]->m);
              all[x]->state = 5;
              pthread_cond_signal(&all[x]->cv);
              pthread_mutex_unlock(&all[x]->m);
              arg->state = 0;
          }
          else
          {
              sem_post(&b1);
              int dd = 1;
              int ii = -1;
              while(dd == 1)
              {
                  int e = pthread_cond_timedwait(&arg->cv,&arg->m,&tr);
                  if(arg->state >= 100)
                  {
                       ii = arg->state - 100; 
                      printf(YELLOW "%s has joined %s in electric stage %d ,time extended by 2 sec\n" DEFAULT,name[ii],name[x],arg->id);
                      tr.tv_sec += 2;
                  }
                  else if(err == ETIMEDOUT)
                  {
                      if(ii == -1)
                      {
                          sem_wait(&b1);
                          printf(RED "%s has ended his performance in electric stage %d\n" DEFAULT,name[x],arg->id);
                          
                      }
                      else 
                      {
                          printf(RED "%s and %s has ended their performance in electric stage %d" DEFAULT,name[x],name[ii],arg->id);
                          pthread_mutex_lock(&all[ii]->m);
                          all[ii]->state = 5;
                          pthread_cond_signal(&all[ii]->cv);
                          pthread_mutex_unlock(&all[ii]->m);
                      }
                          pthread_mutex_lock(&all[x]->m);
                          all[x]->state = 5;
                          pthread_cond_signal(&all[x]->cv);
                          pthread_mutex_unlock(&all[x]->m);
                          arg->state = 0;
                          break;
                  }
              }
          }
          pthread_mutex_unlock(&arg->m);

    }

}
void* singing(void* input)
{
    printf("singer  have not yet arrived\n");
    int rr = rand()%3 +1;
    m1* arg = (m1*)input;
    sleep(rr+5);
    pthread_mutex_lock(&arg->m);
    printf(GREEN "%s has arrived\n" DEFAULT,name[arg->id]);
    arg->state = 1;
    sem_post(&a1);
    sem_post(&e1);
    while(true)
    {
        int d = sem_trywait(&b1);
        if(d == 1)
        {
            for(int i=0;i<size;i++)
            {
                pthread_mutex_lock(&ss[i]->m);
                if(ss[i]->state == 1)
                {
                    ss[i]->state = 100 + arg->id;
                    arg->state = 3;
                    pthread_cond_signal(&ss[i]->cv);
                    pthread_mutex_unlock(&ss[i]->m);
                    break;
                }
                pthread_mutex_unlock(&ss[i]->m);
            

            }
            if(arg->state == 3)
            {
                while(true)
                {
                    if(arg->state == 5)
                    {
                        pthread_mutex_unlock(&arg->m);
                        return NULL;
                    }
                    else
                pthread_cond_wait(&arg->cv,&arg->m);
                }
            }
        }
      struct timespec tu = timer(t);
        int dd = 1;
        while(dd == 1)
        {
            int er = pthread_cond_timedwait(&arg->cv,&arg->m,&tu);
            if(arg->state == 5)
            {
                pthread_mutex_unlock(&arg->m);
                return NULL;
            }
            else if(er == ETIMEDOUT)
            {
                printf(RED "Singer %s leaves due to impatience\n" DEFAULT,name[arg->id]);
                pthread_mutex_unlock(&arg->m);
                return NULL;

            }
        }
    }

    
}

void* player(void* inp)
{
    printf("musician  have not yet arrived\n");
    int rr = rand()%3+5;
    m1* arg = (m1*)inp;
    sleep(rr);
    pthread_mutex_lock(&arg->m);
    arg->state = 1;
    if(mus[arg->id] == 2)
    sem_post(&a1);
    if(mus[arg->id] == 3)
    sem_post(&e1);
    printf(GREEN "musician %s has arrived\n" DEFAULT,name[arg->id]);
    struct timespec tu = timer(t);
        int dd = 1;
        while(dd == 1)
        {
            int er = pthread_cond_timedwait(&arg->cv,&arg->m,&tu);
            if(arg->state == 5)
            {
                break;
            }
            else if(er == ETIMEDOUT)
            {
                printf(YELLOW "musician %s leaves due to impatience\n" DEFAULT,name[arg->id]);
                pthread_mutex_unlock(&arg->m);
                return NULL;

            }
        }
        printf(CYAN "%s is waiting for Tshirt\n" DEFAULT,name[arg->id]);
        sem_wait(&c1);
        printf(GREEN "%s is collecting the Tshirt\n" DEFAULT,name[arg->id]);
        sleep(2);
        printf(YELLOW "%s has collected the tshirt, he is exiting now\n" DEFAULT,name[arg->id]);
        sem_post(&c1);
        return NULL;
}
int main()
{

    int k,a,e,c,t1,t2,t;
    printf("enter date\n");
    scanf("%d %d %d %d %d %d %d",&k,&a,&e,&c,&t1,&t2,&t);
    printf("initiation\n");
    init();
  //  for(int i=0;i<1024;i++)

    ss = (s1**)malloc(sizeof(s1*)*(size));
    all = (m1**)malloc(sizeof(m1*)*(k));
   /* for(int i=0;i<a;i++)
    {
        ss[i] = (s1*)malloc(sizeof(s1));
        stage1(i,ss[i]);
        pthread_create(&(ss[i]->tid),NULL,staging1,(void*)ss[i]);


    }
    

    for(int i=a;i<size;i++)
    {
        ss[i]= (s1*)malloc(sizeof(s1));
        stage2(i,ss[i]);
        pthread_create(&(ss[i]->tid),NULL,staging2,(void*)ss[i]);
    }*/
    char temp[k];
    printf("enter the musicians\n");
    for(int i=0;i<k;i++)
    {
       // char x[100];
        int arr;
        char c;
        scanf("%s %c %d",name[i],&c,&arr);
      
         temp[i]=c;
        all[i] = (m1*)malloc(sizeof(m1));
        if(c == 'g' || c == 'p')
        both_init(i,all[i],c,arr);
        else if(c == 'v')
        acc_init(i,all[i],c,arr);
        else if(c == 'b')
        ele_init(i,all[i],c,arr);
        else
        sing_init(i,all[i],c,arr);
       /* if(c == 's')
        pthread_create(&(all[i]->tid),NULL,singing,(void*)all[i]);
        else
        pthread_create(&(all[i]->tid),NULL,player,(void*)all[i]);*/
        
    }
    printf("threads of musicians\n");
    for(int i=0;i<k;i++)
    {
         if(temp[i] == 's')
        pthread_create(&(all[i]->tid),NULL,singing,(void*)all[i]);
        else
        pthread_create(&(all[i]->tid),NULL,player,(void*)all[i]);
    }
    printf("stages of acoustic\n");
    for(int i=0;i<a;i++)
    {
        ss[i] = (s1*)malloc(sizeof(s1));
        stage1(i,ss[i]);
        pthread_create(&(ss[i]->tid),NULL,staging1,(void*)ss[i]);


    }
    
    printf("stages of electruc\n");
    for(int i=a;i<size;i++)
    {
        ss[i]= (s1*)malloc(sizeof(s1));
        stage2(i,ss[i]);
        pthread_create(&(ss[i]->tid),NULL,staging2,(void*)ss[i]);
    }
    for(int i=0;i<k;i++)
    pthread_join(all[i]->tid,NULL);

    for(int i=0;i<k;i++)
    pthread_mutex_destroy(&all[i]->m);
    for(int i=0;i<size;i++)
    pthread_mutex_destroy(&ss[i]->m);
    return 0;
}


