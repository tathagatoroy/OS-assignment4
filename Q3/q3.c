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

//structure for all musicians/singer
typedef struct m1
{
    int id;
    pthread_t tid;
    pthread_mutex_t m;
    pthread_cond_t cv;
    int state ; //0 starting,1 waiting, 2 performing in stage ,3 -valid for singers,joining a musician, 5 done performance
    //6 ,7,8 for tshirt things
    char ins; //instrument
    int a; // arrival time

}m1;

m1** all ;//stores all musicians and singers;

int k,a,c,e,t1,t2,t;
int range;  //range of performance
int mus[1024];//1 singer // 2 acc // 3 ele // 4 both
int stage[1024]; //1 acc singer // 2 ele singer // 3 acc music // 4 ele mus
char name[1024][50];  //stores name ,assume no more than 1024 musicians/singers
char temp[1024]; //stores instruments
// structure of stages
typedef struct s1
{
    int id;
    int state ;  // 0 - empty , 2 -> a non-singer is performing, 10->solo singer, 100+ a singer wants to join a non-singer 99-duet
    pthread_mutex_t m;
    pthread_cond_t cv;
    pthread_t tid;
}s1;
s1** ss;
sem_t b1;  //number of stages where a singer can join already existing musician
sem_t c1;  // number of coordinators is free
sem_t a1;   //number musician/singer waiting for acoustic stage
sem_t e1;  //number musician/singer waiting for electric stage 
int size;
//initialise all singers
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
//initialise all musician who can use both acoustic and electric stage
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
//initialise all musician who can all use only accoustic stage
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

//initialise all electric stage only musicians
void ele_init(int id,m1* z,char c,int arr)
{
  z->state = 0;
  z->id = id;
  mus[id] = 3;
  z->a = arr;
  pthread_cond_init(&(z->cv),NULL);
  pthread_mutex_init(&(z->m),NULL);
}

//initiallise all accoustic stage
void stage1(int id,s1* z)
{
    z->id = id;
    z->state = 0;
     pthread_cond_init(&(z->cv),NULL);
  pthread_mutex_init(&(z->m),NULL);
  stage[id] = 1;
  

}
//initialise all electric stage
void stage2(int id,s1* z)
{
    z->id = id;
    z->state = 0;
     pthread_cond_init(&(z->cv),NULL);
  pthread_mutex_init(&(z->m),NULL);
  stage[id] = 2;
  

}
//initialise some global stuff
void init()
{
    sem_init(&b1,0,0);
    sem_init(&a1,0,0);
    sem_init(&e1,0,0);
    sem_init(&c1,0,c);
    range=t2-t1+1;
    size = a+e;
//    printf("a = %d, e = %d, size = %d ,range = %d, t2 = %d, t1 = %d\n",a,e,size,range,t2,t1);
}

//timer function 
struct timespec timer(int u)
{
    struct timespec tr;
    clock_gettime(CLOCK_REALTIME,&tr);
  //  tr.tv_nsec += u*1000000000;
    tr.tv_sec += u;
   // tr.tv_nsec = tr.tv_sec%1000000000;
    return tr;
}



// function for threads representing accoustic stage
void* staging1(void* inp)
{
    
    //printf("electrical stage\n");
    sleep(1);
    s1* arg = (s1*)inp;

    while(true)
    {
        int x = -1;  //stores the assigned musician(assigned first)
        pthread_mutex_lock(&arg->m);
        //printf("Stage %d is ready which is accoustic\n",arg->id);
        while(true)
        {
            //pthread_mutex_lock(&arg->m);
           // printf("stage id is ready\n");
           // look for musician willing to perform in accoustic stage
          int r = sem_trywait(&a1);
          if(r == 0)
          {
             // printf("locked\n");
             //find thhe musician you know exist
              for(int i=0;i<k;i++)
              {
                  if(mus[i]!=3)
                  {
                      pthread_mutex_lock(&all[i]->m);
                      if(all[i]->state == 1)
                      {
                          //found him/her
                          //singer if
                          if(mus[i]==1)
                         { arg->state  = 10;
                           sem_wait(&e1);
                         }
                          else //not a singer
                          arg->state = 2;
                          all[i]->state = 2;
                          x = i;
                          //let him/her know stage is assigned
                          pthread_cond_signal(&all[i]->cv);
                          pthread_mutex_unlock(&all[i]->m);
                          break;

                      }
                      pthread_mutex_unlock(&all[i]->m);
                  }

              }
              //stage not empty,no need to look anymore
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
          int err = 100 ;
          printf(CYAN "%s %c is starting in accoustic stage %d for  %d secs\n" DEFAULT,name[x],temp[x],arg->id,num);
          
          //singer ,therefore no one else can join
          if(arg->state == 10)
          {

            //performance going on
              sleep(num);

              printf(RED "%s has finished his performance in accoustic stage %d\n" DEFAULT,name[x],arg->id);
              pthread_mutex_lock(&all[x]->m);
              all[x]->state = 5;
              pthread_cond_signal(&all[x]->cv);//let singer know his performance is done
              pthread_mutex_unlock(&all[x]->m);
              arg->state = 0; //stage is available agains
          }
          else
          {
              //musician singer can join
              sem_post(&b1);
              int dd = 1;
              int ii = -1;
              int e = 100;
              while(dd == 1)
              {
                  //int e = pthread_cond_timedwait(&arg->cv,&arg->m,&tr);
                  //singer joins
                  if(arg->state >= 100)
                  {
                       ii = arg->state - 100;
                       arg->state = 99; 
                      printf(YELLOW "%s has joined %s in accoustic stage %d ,time extended by 2 sec\n" DEFAULT,name[ii],name[x],arg->id);
                      tr.tv_sec += 2;
                  }
                  //performance done
                  else if(e == ETIMEDOUT)
                  {
                      //if alone
                      if(ii == -1)
                      {
                          sem_wait(&b1);
                          printf(RED "%s has ended his performance in accoustic stage %d\n" DEFAULT,name[x],arg->id);
                          
                      }
                      //if duet
                      else 
                      {
                          printf(RED "%s and %s has ended their performance in accoustic stage %d\n" DEFAULT,name[x],name[ii],arg->id);
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
                  //performance going on
                  else
                  {
                      e = pthread_cond_timedwait(&arg->cv,&arg->m,&tr);
                  }
              }
          }
      pthread_mutex_unlock(&arg->m);
    }

}
//function for electric stage, same as accoustic
//not gonna comment again
void* staging2(void* inp)
{
    //printf("accoustic stage\n");
    sleep(1);
    s1* arg = (s1*)inp;

    while(true)
    {
        int x = -1;
        pthread_mutex_lock(&arg->m);
        //printf("stage %d is ready which is electric\n",arg->id);
        while(true)
        {
            //pthread_mutex_lock(&arg->m);
            //printf("stage %d is ready\n",arg->id);
          int r = sem_trywait(&e1);
          if(r == 0)
          {
             // printf("locked\n");
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
                          //printf("signalled ")
                          pthread_cond_signal(&all[i]->cv);
                          pthread_mutex_unlock(&all[i]->m);
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
          printf(CYAN "%s %c is starting in electric stage %d for  %d secs\n" DEFAULT,name[x],temp[x],arg->id,num);
          
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
              //printf("here electric\n");
              int e = 100;
              while(dd == 1)
              {
                  //int e = pthread_cond_timedwait(&arg->cv,&arg->m,&tr);
                  if(arg->state >= 100)
                  {
                       ii = arg->state - 100;
                       arg->state = 99; 
                      printf(YELLOW "%s has joined %s in electric stage %d ,time extended by 2 sec\n" DEFAULT,name[ii],name[x],arg->id);
                      tr.tv_sec += 2;
                  }
                  else if(e == ETIMEDOUT)
                  {
                     // printf("Timedout\n");
                      if(ii == -1)
                      {
                          sem_wait(&b1);
                          printf(RED "%s has ended his performance in electric stage %d\n" DEFAULT,name[x],arg->id);
                          
                      }
                      else 
                      {
                          printf(RED "%s and %s has ended their performance in electric stage %d\n" DEFAULT,name[x],name[ii],arg->id);
                          pthread_mutex_lock(&all[ii]->m);
                          all[ii]->state = 5;
                          pthread_cond_signal(&all[ii]->cv);
                          pthread_mutex_unlock(&all[ii]->m);
                      }
                    //  printf("x : %d\n",x);
                          pthread_mutex_lock(&all[x]->m);
                      //    printf("recieved lock\n");
                          all[x]->state = 5;
                          pthread_cond_signal(&all[x]->cv);
                          pthread_mutex_unlock(&all[x]->m);
                          arg->state = 0;
                          break;
                  }
                  else 
                  {
                      e =  pthread_cond_timedwait(&arg->cv,&arg->m,&tr);
                  }
              }
          }
          pthread_mutex_unlock(&arg->m);

    }

}
//function for all threads representing singers
void* singing(void* input)
{
    //printf("singer  have not yet arrived\n");
    //int rr = rand()%3 +1;
    m1* arg = (m1*)input;
    //sleep(rr+5);
    pthread_mutex_lock(&arg->m);
    sleep(arg->a);
    printf(GREEN "%s has arrived\n" DEFAULT,name[arg->id]);
    arg->state = 1;// ready to start
    //can work in both accoustic and electric stage
    sem_post(&a1);
    sem_post(&e1);
    //look for already occupied stage that the singer join
    //reason : design choice ,easy to implement,no reason it should prioritise it over empty stage
    //assumed it to be instantaneos
    while(true)
    {
        // if there any occupied accoustic/electric stage by non-singer;
        int d = sem_trywait(&b1);
        if(d == 0)
        {
            //there is someone singer can join
            //printf("singer locked\n");
            for(int i=0;i<size;i++)
            {
                pthread_mutex_lock(&ss[i]->m);
                //found the stage where he/she can join
                if(ss[i]->state == 2)
                {
                    ss[i]->state = 100 + arg->id;
                    arg->state = 3;
                    pthread_cond_signal(&ss[i]->cv);
                    pthread_mutex_unlock(&ss[i]->m);
                    //performance going on
                    while(true)
                {
                    if(arg->state == 5)
                    {
                        //performance done,return thread
                        pthread_mutex_unlock(&arg->m);
                        return NULL;
                    }
                    else
                    {
                        //else wait for performance to wait
                        int z= arg->id;
          //               printf("singer %s goes to sleep for performing \n",name[z]);
                pthread_cond_wait(&arg->cv,&arg->m);
            //    printf("singer %s wakes up with state %d \n",name[z],arg->state);
                    }
                }
                //no such stage available ,hence break
                    break;
                }
                pthread_mutex_unlock(&ss[i]->m);
            

            }
            
                /*while(true)
                {
                    if(arg->state == 5)
                    {
                        pthread_mutex_unlock(&arg->m);
                        return NULL;
                    }
                    else
                    {
                        int z= arg->id;
                pthread_cond_wait(&arg->cv,&arg->m);
                printf("singer %s goes to sleep for performing \n",name[z]);
                    }
                }*/
            }
        
      struct timespec tu = timer(t);
        int dd = 1;
        int er = 100;
        //timed wait for a stage to signal its free
        while(dd == 1)
        {
            //int er = pthread_cond_timedwait(&arg->cv,&arg->m,&tu);
            //perfomance done,return thread
            if(arg->state == 5)
            {
                pthread_mutex_unlock(&arg->m);
                return NULL;
            }
            //timed out,no stage found
            //impatiently leave
            else if(arg->state == 2)
            {
                break;
            }
            else if(er == ETIMEDOUT)
            {
                printf(RED "Singer %s leaves due to impatience\n" DEFAULT,name[arg->id]);
                arg->state = 5;
                pthread_mutex_unlock(&arg->m);
                return NULL;

            }
            //wait for a stage to signal its availability
            else
            er = pthread_cond_timedwait(&arg->cv,&arg->m,&tu);
        }
        //stage is assigned
        if(arg->state == 2 )
        {
            while(true)
            {
                //performance done
                if(arg->state == 5)
                {
                    pthread_mutex_unlock(&arg->m);
                    return NULL;
                }
                //wait for performance to be over
                else pthread_cond_wait(&(arg->cv),&(arg->m));
            }
        }
    }

    
} 


//thread for a non-singing musician
void* player(void* inp)
{
    //printf("musician  have not yet arrived\n");
    int rr = rand()%3+5;
    m1* arg = (m1*)inp;
    //sleep();
    pthread_mutex_lock(&arg->m);
    sleep(arg->a);
    arg->state = 1;// ready,waiting for stage
    if(mus[arg->id] == 2 || mus[arg->id] == 4)
    sem_post(&a1); //looking for accoustic stage
    if(mus[arg->id] == 3 || mus[arg->id] == 4)
    sem_post(&e1); // looking for electric
    
    printf(GREEN "musician %s %c has arrived and waiting for a stage\n" DEFAULT,name[arg->id],temp[arg->id]);
    struct timespec tu = timer(t);
        int dd = 1;
        int ff = 1;
        int er  = 100;
        //wait till it is signaled availability of stage
        while(dd == 1)
        {
            // performance done
            //break
            if(arg->state == 5)
            {
      //          printf("musician done performing\n");
                break;

            }
            //timed out
            //no stage available 
            // return thread,no t-shirt for this one
            else if(er == ETIMEDOUT)
            {
                printf(YELLOW "musician %s leaves due to impatience\n" DEFAULT,name[arg->id]);
                arg->state = 7;
                pthread_mutex_unlock(&arg->m);
                return NULL;

            }
            //else stage assigned,yah
            else if(arg->state == 2)
            {
                ff = 1;
            //    printf("musician %d has recieved stage\n",arg->id);
                break;
            }
            //conditionally timed_wait
            else 
            {
        //        printf("musician %d goes to sleep \n",arg->id);
               er = pthread_cond_timedwait(&arg->cv,&arg->m,&tu);
          //     printf("musician %d is awoken by signals , state %d \n",arg->id,arg->state);
               
            }
        }
        //stage given
        if(ff == 1)
        {
            //performance going on
            while(true)
            {
                //performance done
                if(arg->state == 5)
                {
                    break;
                }
                else
                {
                    //waiting for performance to end
                    pthread_cond_wait(&arg->cv,&arg->m);
                }
            }
        }
        
        //tshirt business pretty simple to explain
        if(c !=0)
        {
        printf(CYAN "%s is waiting for Tshirt\n" DEFAULT,name[arg->id]);
        sem_wait(&c1);
        printf(GREEN "%s is collecting the Tshirt\n" DEFAULT,name[arg->id]);
        sleep(2);
        printf(YELLOW "%s has collected the tshirt, he is exiting now\n" DEFAULT,name[arg->id]);
        }
        else
        printf(RED "music club forgot to assign coordinators,no tshirt for anyone\n" DEFAULT);
        arg->state = 7;
        pthread_mutex_unlock(&arg->m);
        sem_post(&c1);
        
        return NULL;
}
int main()
{

    //int k,a,e,c,t1,t2,t;
   // printf("enter date\n");
    scanf("%d %d %d %d %d %d %d",&k,&a,&e,&c,&t1,&t2,&t);
    // printf("a = %d, e = %d, size = %d ,range = %d, t2 = %d, t1 = %d\n",a,e,size,range,t2,t1);
   // printf("initiation\n");
    init();
  //  for(int i=0;i<1024;i++)
     srand(time(0));
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
   // char temp[k];
    //printf("enter the musicians\n");
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
    //printf("stages of acoustic\n");
    for(int i=0;i<a;i++)
    {
        ss[i] = (s1*)malloc(sizeof(s1));
        stage1(i,ss[i]);
        pthread_create(&(ss[i]->tid),NULL,staging1,(void*)ss[i]);


    }
    
   // printf("stages of electruc\n");
   //printf("%d %d\n",a,size);
    for(int i=a;i<size;i++)
    {
        ss[i]= (s1*)malloc(sizeof(s1));
        stage2(i,ss[i]);
        pthread_create(&(ss[i]->tid),NULL,staging2,(void*)ss[i]);
    }
    //printf("threads of musicians\n");
    sleep(5);
    for(int i=0;i<k;i++)
    {
         if(temp[i] == 's')
        pthread_create(&(all[i]->tid),NULL,singing,(void*)all[i]);
        else
        pthread_create(&(all[i]->tid),NULL,player,(void*)all[i]);
    }
    /*printf("stages of acoustic\n");
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
    }*/
    for(int i=0;i<k;i++)
    pthread_join(all[i]->tid,NULL);

    for(int i=0;i<k;i++)
    pthread_mutex_destroy(&all[i]->m);
    for(int i=0;i<size;i++)
    pthread_mutex_destroy(&ss[i]->m);
    return 0;
}


