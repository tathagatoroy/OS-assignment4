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

struct musician
{
    int state ;
    pthread_t tid;
    //1 not yet arrived
    //2 waiting 
    //3 solo
    //4 with singer
    //5 waiting for tshirt
    //6 tshirt collected
    // 7 exited
    int id ;
    pthread_mutex_t m;
    pthread_cond_t cv;
    int accoustic;
    int electric;
    int sing;
    int performed;
}musician;


int* ac;
char** name;
int* el;
int* cor
pthread_mutex_t c;
pthread_cond_t c1
sem_t c2;
pthread_mutex_t a;
pthread_mutex_t e;
pthread_cond_t a1;
pthread_cond_t e1;
pthread_mutex_t s;
pthread_cond_t s1;
sem_t s2;
sem_t a2;
sem_t c2;

void init_musician(int id,char c,musician* m,char c[])
{
    if(c == 's')
    {
        m->sing = 1;
        m->accoustic = 1;
        m->electric = 1;

    }
    else
    {
        m->sing = 0;
        if(c == 'p' || c == 'g' || c=='v')
        m->accoustic = 1;
        else m->accoustic = 0;
        if(c == 'p' || c == 'g' || c == 'b')
        m->electric = 1;
        else m->electric = 0;
}
m->id = id;
m->state = 0;
m->performed = 0;
pthread_mutex_init(&(m->m),NULL);
pthread_cond_init(&(m->cv),NULL);
}

void* play_music(void* input)
{
    musician* arg = (musician*)input;
    int r = rand()%(3);
    sleep(r);
    pthread_mutex_lock(&(arg->m));
    arg->state = 1;
    printf(GREEN "%s has arrived\n" DEFAULT,namelist[]
}
int k,a,e,c,t1,t2,t;
int range;

void init(int a3,int c3,int e3)
{
    pthread_mutex_init(&a,NULL);
    pthread_mutex_init(&c,NULL);
    pthread_mutex_init(&e,NULL);
    pthread_mutex_init(&s,NULL);

    pthread_cond_init(&a1,NULL);
    pthread_cond_init(&c1,NULL);
    pthread_cond_init(&e1,NULL);
    pthread_cond_init(&s1,NULL);

    sem_init(&a2,0,a3);
    sem_init(&c2,0,c3);
    sem_init(&e2,0,e3);
    sem_init(&s2,0,0);
    range = t2-t1+1;

}
void* play_music(void* input)
{
    musician* arg = (musician*)input;
    int r = rand()%(3);
    sleep(r);
    pthread_mutex_lock(&(arg->m));
    arg->state = 1;
    printf(GREEN "%s has arrived\n" DEFAULT,namelist[arg->id]);
    if(arg->singer == 1)
    {
        //check if there is an empty acoustic
        int ret = sem_trywait(&a2)
        //locked\n
        if(ret == 1)
        {
            pthread_timestruc_t to;
            clock_gettime(CLOCK_REALTIME,&to);
            int p_time = rand()%range+t1;
            to.tv_sec += p_time;
            while(FALSE)
            {
                int u = pthread_cond_timedwait(&arg->cv,&arg->m,&to);
                if(u == ETIMEDOUT);
            }
        }
    }

}
int main()
{


cin>>k>>a>>e>>c>>t1>>t2>>t;
name = (char**)malloc(sizeof(char*)*k);
ac = (int*)malloc(sizeof(int)*a);
el = (int*)malloc(sizeof(int)*e);
cor = (int*)malloc(sizeof(int)*c);
init();
}

