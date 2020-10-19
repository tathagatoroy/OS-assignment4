### README AND LOGIC FOR QUESTION

There are 3 structures for the three threads
structure for the companies

``` c
typedef struct Company
{
int id; 
pthread_t tid;
double x;   // success rate
int state; //manufacturing or waiting
//1 . start manufacturing
//3 .  manufactured ,waiting for a zone to be delivered to; 

int total;  // total vaccine produced in one session

//lock and cond_var for all company variables
pthread_mutex_t m_state;
pthread_cond_t cv_state;
}Company;
```
structure for the  student

``` c
typedef struct Student
{
    int id;
    int cnt; // keeps count of the number of shots recieved
    pthread_t tid;
    int state ;  //vaccinating ,done, or waiting
    // 0 is waiting
    // 1 is assigned to zone;
    // 2 3 attempts done go home;
    // 3 waiting for antibody test;
    // 4 vaccinated ;
    int zone;   //currently assigned zone
    double x;    // success rate of current shot
    pthread_mutex_t m_state;
    pthread_cond_t cv_state;
    int size;   // size of number of zones
}Student;
````
structure for zones

```` c
 usetypedef struct Zones
{
    int id ;
    int cnt;    // current number of vaccines
    int slot;   //slots generated
    pthread_mutex_t m_cnt;
    pthread_cond_t cv_cnt;
    pthread_t tid;
    Company** comp;
    Student** stud;
    int sleep ;
    int n;   // #companies
    int o;   //#students
    int last;  //last company which delivered
    double x;
}Zones;
Zones** zone; // global 
`````

each of them has a mutex lock and conditional variable to wait .
we also have 2 global variables wait1 and left to count the number of students waiting and number students who havent been sent home or college
both with their mutex and lock

the thread function for pharmacy

````c
//this is the function for company threads
void* start_manufacturing(void* inp)
{
    
    sleep(1);
    Company* arg = (Company*)inp;
   // printf("Company id %d ...\n",arg->id);
    int tt = 1;
   while(tt == 1)
   {
    //   printf("inside\n");
    //random variables generation
           int r = rand()%(5-1+1)+1;
           int p = rand()%(20-10+1)+10;
           int w = rand()%(5-2+1)+2;

        //total vaccines manufactured
        pthread_mutex_lock(&(arg->m_state));
           arg->total = p*r;
           printf(CYAN "Pharmaceutical Company %d is preparing %d batches of vaccine which have success probabilily %lf\n" DEFAULT,arg->id,r,arg->x);
          
          // manufacturing
           sleep(w);
           // manufacturing done


           printf(RED "Pharmaceutical Company %d has prepared %d batches of vaccines which has success probability %lf\n" DEFAULT,arg->id,r,arg->x);
           
           arg->state = 3;
           // conditionally wait until state is one,then resume manufacturing
           while(true)
           {
               if(arg->state == 1)
           {
               printf(CYAN "All vaccines previously manufacture by company %d is used up,resuming manufacturing\n" DEFAULT,arg->id);
               break;
           }
               
               else
               {
                  /// if(arg->state < 0)
              // printf(BLUE "Pharmaceutical Company %d is delivering a vaccine batch to vaccination zone %d which has success probability %lf\n" DEFAULT,arg->id,-arg->state-1,arg->x);
               pthread_cond_wait(&(arg->cv_state),&(arg->m_state));
               }
           }

           //if all students successfully vaccinated or sent home, end loop
           pthread_mutex_unlock(&(arg->m_state));
           pthread_mutex_lock(&(m_left));
           if(left == 0)
           tt = 0;
           pthread_mutex_unlock(&(m_left));

    }
    return NULL;

   
}

````
each thread begins by starting manufacturing p*r vaccines and lets the zones know that vaccines is ready and waits  while the a zone signals the company and signals the company,it only resumes manufacturing when it is signaled that all previously manufactured vaccines have been used up.this is done using pthread_cond_wait function

There are 3 simple initialisation function each for one particular type of thread.global variables are initialised in main

`````c



//initialise company variables
void init_company(int id,double x,Company* z)
{
    z->id = id;
    z->x = x;
    z->state = 1;  //tells company to start manufacturing once initialised
    z->total = 0;
    pthread_mutex_init(&(z->m_state),NULL);
    pthread_cond_init(&(z->cv_state),NULL);
}

//initialise student variables
void init_student(int id,Student* z,int m)
{
    z->id = id;
    z->state = 0;  //waiting 
    z->cnt = 0;   // no shots yet
    z->zone = -1;  
    z->x = 0.0;
    z->size = m;
    pthread_mutex_init(&(z->m_state),NULL);
    pthread_cond_init(&(z->cv_state),NULL);


}

//initialise zone variables
void init_zone(int id,Zones* z,Company** comp,int n,Student** stud,int o)
{
z->id = id;
z->comp = comp;
z->stud = stud;
z->n = n;
z->o = o;
z->slot = 0;
z->sleep = 0;
pthread_mutex_init(&(z->m_cnt),NULL);
pthread_cond_init(&(z->cv_cnt),NULL);
z->last = -1;
z->cnt = 0;
z->x = 0.0;

}
 ```

now the function for zone which is the most complicated

````c

// function for thread zones

void* start_vaccination(void* inp)
{
    
    sleep(1);
    Zones* arg = (Zones*)inp;
    int tt = 1;
    while(tt == 1)
    {
        int i=0;
        //assign a company to the zone
      while(true)
      {
         //checks if company is has a batch  ready .i,e state = 3 
          pthread_mutex_lock(&(arg->comp[i]->m_state));
          if(arg->comp[i]->state != 3)
          pthread_mutex_unlock(&(arg->comp[i]->m_state));
          else
          {
              //company i is ready and assigned
              pthread_mutex_lock(&(arg->m_cnt));
             arg->last = i;  // assign company
             arg->x = arg->comp[i]->x;
             arg->comp[i]->state = 2;  //tells company to wait while vaccinations are being used
             printf(YELLOW "Pharmaceutical Company %d is delivering a batch to the vaccination zone %d with success probability %lf\n" DEFAULT,i,arg->id,arg->comp[i]->x);
             
             //ON THE WAY TO DELIVERY
             sleep(2);
             printf(RED "Pharmaceutical Company %d has delivered to Vaccination Zone %d,Resuming Vaccinations now\n" DEFAULT,i,arg->id);
             arg->cnt = arg->comp[i]->total; // zone now have all vaccinations of the company
             printf(RED "Now Vaccination zone %d has %d vaccines \n" DEFAULT,arg->id,arg->cnt);
             arg->comp[i]->total = 0;  //company have zero vaccination left
             //lets the company know that its batch is delivered
             pthread_cond_signal(&(arg->comp[i]->cv_state));  //signals the company to know delivery done
             pthread_mutex_unlock(&(arg->comp[i]->m_state));
             pthread_mutex_unlock(&(arg->m_cnt));
             //breaks because zone now have vaccination
             break;
          }
          //finds other companies to see if they have vaccination
          i=(i+1)%arg->n;
      }


      // NOW OUR VACCINATION ZONE HAS arg->cnt Vaccines;
      while(true)
      {
       // printf("inside generation\n");
        pthread_mutex_lock(&(arg->m_cnt));

printf(RED "current zone %d has %d vaccines with success probability %lf\n" DEFAULT,arg->id,arg->cnt,arg->x);
        // if all vaccines have been used
        if(arg->cnt <= 0)
        {
         //   printf("inside inside\n");
            //break if no vaccine is left

            //useless check
            arg->cnt =0;  
            //useless condition to ensure no nonsense stuff happend
            if(arg->last != -1)
            {
           // return the to find a zone given vaccines are no more

            pthread_mutex_lock(&(arg->comp[arg->last]->m_state));
            //last zone start manufacturing
            arg->comp[arg->last]->state = 1;  //let the company know so that it restart manufacturing
            printf(BLUE "Vaccination Zone %d has run out of vaccines\n" DEFAULT,arg->id);
            pthread_cond_signal(&(arg->comp[arg->last]->cv_state)); //signal the company thread to wake up
            pthread_mutex_unlock(&(arg->comp[arg->last]->m_state));
        
            }
            pthread_mutex_unlock(&(arg->m_cnt));
        //break because no vaccines left    
            break;
        }
       // printf("Not inside inside\n");
        //NOW WE HAVE KNOW WE HAVE NON-ZERO VACCINES LEFT;
        printf(CYAN "SLOT GENERATION BY ZONE %d\n",arg->id);
          int mini = 1;
        //  printf("waiting for lock20\n");
          pthread_mutex_lock(&m_wait);
        //  printf("lock recieved\n");
          int maxi = 8;
        //  printf("wait1 %d cnt %d\n",wait1,arg->cnt);
          if(maxi > wait1)
          maxi = wait1;
          if(maxi > arg->cnt)
          maxi = arg->cnt;
          if(maxi < 1)
          maxi = 1;

          pthread_mutex_unlock(&m_wait);
          //printf("unlocked7\n");
          //no_of_slots = arg->slot
          arg->slot = rand()%(maxi-mini+1)+mini;
          int temp = arg->slot;
          sleep(1); //slot generation
          //ready
          printf(YELLOW "Vaccination Zone %d is ready to vaccinate with %d slots\n" DEFAULT,arg->id,arg->slot);
          

          //SLOTS ASSIGNMENT
  //conditionally wait till slots are assigned
  //pthread_mutex_unlock(&arg->m_cnt);
  int j = 0;
  int f = 0;
  int list[arg->o];
  for(int i1=0;i1<arg->o;i1++)
  list[i1]=0;
          while(true)
          {
              pthread_mutex_lock(&m_wait);
              if(wait1 == 0 && f!=0)
              {
                  printf(RED "No student is currently waiting ,slot assignment for zone %d done\n" DEFAULT,arg->id);

                  pthread_mutex_unlock(&m_wait);
                  break;

              }
              pthread_mutex_unlock(&m_wait);
              if(arg->slot == 0)
              {
                  printf(YELLOW "all slots filled for zone %d,slot assignment done\n" DEFAULT,arg->id);
                  break;
              }
             pthread_mutex_lock(&(arg->stud[j]->m_state));
             if(arg->stud[j]->state == 0)
             {
                 list[j] = 1;
                 f++;
                 arg->stud[j]->state = 1;
                 arg->stud[j]->zone = arg->id;
                 arg->stud[j]->x = arg->x;
                 pthread_mutex_lock(&(arg->comp[arg->last]->m_state));
                 arg->stud[j]->zone = arg->comp[arg->last]->x;
                 pthread_mutex_unlock(&(arg->comp[arg->last]->m_state));
                 pthread_mutex_unlock(&(arg->stud[j]->m_state));
                 pthread_mutex_lock(&m_wait);
                 wait1--;
                 pthread_mutex_unlock(&m_wait);
                 arg->slot --;
                 printf(CYAN "Student %d has been assigned vaccination zone %d\n" DEFAULT,j,arg->id);

            
             }
             pthread_mutex_unlock(&(arg->stud[j]->m_state));
             j=(j+1)%arg->o;

               
               
             // no slot left ,can start vaccination
             
             //wait for slots to be filled or no person to be waiting
             // slot are assigned once no more slots are left or no more students are waiting

          }
          //SLOT ALLOTMENT DONE VACCINATION BEGINS
          printf(RED "Vaccination zone %d entering Vaccination Phase\n" DEFAULT,arg->id);

          //VACCINATION ONGOING
          sleep(1);
          for(int g=0;g<arg->o;g++)
          {
              if(list[g]==1)
              {
                  pthread_mutex_lock(&(arg->stud[g]->m_state));
                   printf(YELLOW "Student %d on vaccination zone %d has been vaccinated which has success probability %lf\n" DEFAULT,g,arg->id,arg->stud[i]->x);
                   arg->stud[g]->state = 3;
                   arg->stud[g]->cnt += 1;
                   pthread_cond_signal(&(arg->stud[g]->cv_state));
                   pthread_mutex_unlock(&(arg->stud[g]->m_state));
                   arg->cnt -= 1;
                   list[g] = 0;
              }
            /*pthread_mutex_lock(&(arg->stud[i]->m_state));
            //if student i is assigned this slot and they get vaccinated
            if(arg->stud[i]->zone == arg->id && arg->stud[i]->state == 1)
            {
              arg->stud[i]->state = 3; //vaccination done,waiting for antibody
              pthread_mutex_lock(&(arg->comp[arg->last]->m_state));
              arg->stud[i]->x = arg->comp[arg->last]->x; // storing success rate
              arg->stud[i]->cnt+=1;  //attempted vaccination update
              arg->cnt -= 1;   //no of vaccines used decrease
              pthread_mutex_unlock(&(arg->comp[arg->last]->m_state));
              printf(YELLOW "Student %d on vaccination zone %d has been vaccinated which has success probability %lf\n" DEFAULT,i,arg->id,arg->stud[i]->x);
              pthread_cond_signal(&(arg->stud[i]->cv_state));//let the student know you have vaccinated him*/

            }
          printf(YELLOW "VACCINATION FOR ZONE %d\n",arg->id);
          pthread_mutex_unlock(&(arg->m_cnt));
            //pthread_mutex_unlock(&(arg->stud[i]->m_state));
          }
          pthread_mutex_unlock(&(arg->m_cnt));
          
      
 
    pthread_mutex_lock(&(m_left));
    // if no one is left,no point in zones asking for vaccines
    if(left == 0)
    {
        pthread_mutex_unlock(&(m_left));
        tt = 0;
        break;
    }
    pthread_mutex_unlock(&(m_left));


    }
    return NULL;
}


```` 

first it tries to detect a company with ready vaccines.once it detects such a company it signals the company to deliver it
once all vaccines are delivered  it goes to the next part

till the zone has non -zero vaccines left it does the following

generates slot;
while there are slots left and students are waiting 
the zone actively looks to find students who are yet to be vaccinated
one such students are found,the student is signaled to let it know it has been assigned slot
once no more slots can be filled vaccination phase begins
 zone vaccinates all such students and signals the sleeping student threads to let it know it vaccination done
once this phase of vaccination is finished,it restarts with slot generation
once all vaccines are finished ,it signals the company to let it know it has to restart manufacturing and the zone actively looks for the next zone.

 function for students

 ```` c
 // for students
void* getting_vaccinated(void* inp)
{
    sleep(4);
    Student* arg = (Student*)inp;
    int tt = 1;
    while(tt == 1)
    {
     pthread_mutex_lock(&(arg->m_state));
       // if state !=4 -> successfully vaccinated if arg->cnt >=3 ,go home,cant come to college
          if(arg->state !=4 && arg->cnt >= 3)
          {
              printf(RED "Student %d has his 3 attempts completed but unsuccesful,he/she is going home\n" DEFAULT,arg->id);
              pthread_mutex_lock(&(m_left));
              left = left -1;  // 1 less left
              pthread_mutex_unlock(&(m_left));
              if(arg->state == 0)// if waiting,reduce number of waiting
              {
                  //printf("waiting for lock1\n");
                  pthread_mutex_lock(&(m_wait));
                 // printf("lock1\n");
                  wait1= wait1-1;
                  pthread_mutex_unlock(&(m_wait));
                 // printf("unlocked15\n");
              }
              pthread_mutex_unlock(&(arg->m_state));

              return NULL;
          }
          else if(arg->state == 4)
          {
              printf(RED "Student %d has been succesfully vaccinated\n" DEFAULT,arg->id);
               pthread_mutex_lock(&(m_left));
               left = left -1;  // one less left
              pthread_mutex_unlock((&m_left));
              pthread_mutex_unlock(&(arg->m_state));

              return NULL;
          }
     
     //Student is ready to be Vaccinated
    else if(arg->state == 0)
    {
     printf(CYAN "student  %d is ready to be vaccinated for the %d time\n" DEFAULT,arg->id,arg->cnt+1);
     sleep(1);
     //He/she is ready to be assigned a slot;
     printf(CYAN "student %d is waiting to be alloted a vaccination zone\n" DEFAULT,arg->id);
     int i=0;
     //looking for slot
     while(true)
     {
         if(arg->state == 3)
         {
             
             printf(RED "student %d has been vaccinated in vaccination zone %d with success probability %lf\n" DEFAULT,arg->id,arg->zone,arg->x);
             break;
         }
         else
         {
             pthread_cond_wait(&(arg->cv_state),&(arg->m_state));
         }
         
     }
     if(arg->state == 3)
     {
         double pos = (double)rand()/(double)RAND_MAX;
         if(pos <= arg->x)
        {   printf(RED "student %d has tested positive for antibodies\n" DEFAULT,arg->id);
            printf(CYAN "student %d is going to college ,he has vaccinated succesful,he is safe\n" DEFAULT,arg->id);
         pthread_mutex_lock(&m_left);
         left--;
         pthread_mutex_unlock(&m_left);
         pthread_mutex_unlock(&(arg->m_state));
         return NULL;
        }
        else
        {
            printf(YELLOW "student %d has tested negative for antibodies\n" DEFAULT,arg->id);
            arg->state = 0;
            pthread_mutex_unlock(&arg->m_state);
            pthread_mutex_lock(&m_wait);
            wait1+=1;
            pthread_mutex_unlock(&m_wait);
        }

     }
    }
    }
    return NULL;
}


```c

the code initially checks whether the student is already vaccinated or has already failed 3 times
once it is satisfied that it is not the case,the student thread conditionally waits till he is signaled that he has been assigned the
a slot.He only wakes up once vaccination is completed.(signaled by zone)

after that the thread simulates a antibodies test
if positive it returns
else
it repeats the whole process described in the function

the simulation ends once all the student threads have returned