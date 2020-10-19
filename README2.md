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

