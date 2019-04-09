#include <stdio.h>
#include <stdlib.h>
#include <string.h> // memset
#include <pthread.h> 
#include <semaphore.h>
#include <time.h>

#define TOTAL_SEATS 3

#define SLEEP_MAX_TEACHER 5



  

int room_chair[3];
int waiting_stu = 0;  //number of waiting studs
int next_seat_position = 0;
int next_teach_position = 0;


sem_t sem_students;   //sematphores
sem_t sem_TA;    //sematphores



pthread_mutex_t mutex;



void rand_sleep(void);
void* ta_teaching();
void* studs_programming(void* stu_id);


int main(int argc, char **argv){

	//thread(s)
	pthread_t *studs;
	pthread_t ta;
	
	int* stu_ids;
	int stu_num;
	
	//index
	int i;

	
	printf("How many students are there --> ");
	scanf("%d", &stu_num);

	//initialize
	studs = (pthread_t*)malloc(sizeof(pthread_t) * stu_num);
	stu_ids = (int*)malloc(sizeof(int) * stu_num);

	memset(stu_ids, 0, stu_num);

	sem_init(&sem_students,0,0);
	sem_init(&sem_TA,0,1);
	
	//set random
	srand(time(NULL));

	//sem_init(&  mutex,0,1);
	pthread_mutex_init(&  mutex,NULL);

	//create thread
	pthread_create(&ta,NULL,ta_teaching,NULL);

	//create threads
	for(i=0; i<stu_num; i++)
	{
		stu_ids[i] = i+1;
		pthread_create(&studs[i], NULL, studs_programming, (void*) &stu_ids[i]);
	} 
	
	pthread_join(ta, NULL);
	
	for(i=0; i<stu_num;i++)
	{
		pthread_join(studs[i],NULL);
	}	
	
	return 0;
}

void* studs_programming(void* stu_id)
{
	int id = *(int*)stu_id;

	printf("[stu] student %d is programming\n",id);		
	
	while(1)
	{
		rand_sleep();

		//sem_wait(&  mutex);
		pthread_mutex_lock(&  mutex);

		if(waiting_stu < TOTAL_SEATS)	
		{
			room_chair[next_seat_position] = id;
			waiting_stu++;
			
			printf("	[stu] student %d is waiting\n",id);
			printf("waiting studs --> [1] %d [2] %d [3] %d\n",room_chair[0],room_chair[1],room_chair[2]);
			next_seat_position = (next_seat_position+1) % TOTAL_SEATS;
			
			//sem_post(&  mutex);
			pthread_mutex_unlock(&  mutex);
	
			//wakeup ta
			sem_post(&sem_students);
			sem_wait(&sem_TA);
		}
		else //no more room_chairs
		{
			//sem_post(&  mutex);
			pthread_mutex_unlock(&  mutex);
			
			printf("[stu] no more room_chairs!!!!. student %d is programming\n",id);		
		}
	}				
}

void* ta_teaching()
{
	while(1)
	{
		sem_wait(&sem_students);	
		
		//sem_wait(&  mutex);
		pthread_mutex_lock(&  mutex);
		
		printf("		[ta] TA is teaching student %d\n",room_chair[next_teach_position]);	
		room_chair[next_teach_position]=0;
		waiting_stu--;
		
		printf("waiting studs --> [1] %d [2] %d [3] %d\n",room_chair[0],room_chair[1],room_chair[2]);
		next_teach_position = (next_teach_position + 1) % TOTAL_SEATS;
		
		rand_sleep();

		printf("		[ta] teaching finish!!!!\n");	

		//sem_post(&  mutex);
		pthread_mutex_unlock(&  mutex);

		sem_post(&sem_TA);
	}	
}

void rand_sleep(void){
	int time = rand() % SLEEP_MAX_TEACHER + 1;
	sleep(time);
}
