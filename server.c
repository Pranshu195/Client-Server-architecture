#include"header.h"
#include"request.h"
#include"decl.h"
Request r1;
int count,count1;
int msgid[2],shmid[2],semid[2];
int set_semaphore(int);
void* request_handle(void*);
union semun
{
        int val;                        //value for SETVAL
        struct semid_ds *buf;           //buffer for IPC_STAT & IPC_SET
        unsigned short *array;          //array for GETALL and SETALL
        struct seminfo *__buf;          //buffer for IPC_INFO
        void *__pad;
};
pthread_t thread_ID;
pthread_mutex_t mutex[2];
pthread_attr_t atr;
int j = 0;
int main()
{
	int i;
	int ret;
	init();
	(*crfo)();								//shared mem and message queues created
	for(i = 0; i<4 ; i++)
		semid[i] = semget((key_t)(i+11),1,0666|IPC_CREAT);		//semaphores created

	for(i = 0; i<4 ; i++)
		set_semaphore(semid[i]);					//semaphores set

	for(i = 0; i<2 ; i++)
		pthread_mutex_init(&mutex[i],NULL);				//mutex created

	pthread_attr_setdetachstate(&atr,PTHREAD_CREATE_DETACHED);
	pthread_attr_init(&atr);
	ret = pthread_create(&thread_ID,&atr,request_handle,0);

	while(1)
	{
		if(r1.flag == 1)
		{
			pthread_attr_setdetachstate(&atr,PTHREAD_CREATE_DETACHED);
			pthread_attr_init(&atr);
			ret = pthread_create(&thread_ID,&atr,request_handle,0);
			printf("thread created\n");
			if(ret != 0)
			{
				perror("pthread_create");
				exit(EXIT_FAILURE);
			}
			r1.flag = 0;
		}
	}
	exit(EXIT_SUCCESS);
}
void* request_handle(void* arg)
{
	pthread_mutex_lock(&mutex[0]);
	sleep(1);
	printf("In the Thread \n");
	result *res,res1;
	Request *r;
	void *shared_mem1 = (void*)0;
	void *shared_mem2 = (void*)0;
	int ret;
	
	ret = msgrcv(msgid[0],&r1,sizeof(Request)-sizeof(long),0,0);
	printf("recieved operator = %c\n",r1.oper);
	sleep(1);
//	printf("yo2\n");

	shared_mem2 = shmat(shmid[1],(void*)0,0);
	r = (Request*)shared_mem2;
	r->pid = r1.pid;
	r->oper = r1.oper;
	r->opr1 = r1.opr1;
	r->opr2 = r1.opr2;

//	printf("yo3\n");
	switch(r1.oper)
	{
		case '+':
			ret = fork();
			if(ret == 0)
				execl("./pc1","pc1",NULL);
			break;
		case '-':
			ret = fork();
			if(ret == 0)
				execl("./pc2","pc2",NULL);
			break;
		case '*':
			ret = fork();
			if(ret == 0)
				execl("./pc3","pc3",NULL);
			break;
		default:
			break;
	}
	ret = msgrcv(msgid[1],&res1,sizeof(result)-sizeof(long),0,0);
	sleep(1);
	printf("result recieved\n");
//	printf("yo4\n");
	pthread_mutex_unlock(&mutex[0]);
	pthread_mutex_lock(&mutex[1]);
	shared_mem1 = shmat(shmid[0],(void*)0,0);
	res = (result*)shared_mem1;
	res->res = res1.res;
	kill(res1.pidr,SIGINT);
	printf("%d: result sent %d\n",j++,res->res);
	sleep(1);
//	printf("yo5\n");
	wait(&count1);
	pthread_mutex_unlock(&mutex[1]);
	pthread_exit("end");
}
int set_semaphore(int sem_id)
{
        union semun sem;
        sem.val = 1;
        if((semctl(sem_id,0,SETVAL,sem)) == -1)
        {
                perror("semctl");
                return 0;
        }
        return 1;
}

