#include"header.h"
#include"request.h"
#include"decl.h"

void get_result(int);
int del_semaphore(int);
int semaphore_p(int);
int semaphore_v(int);
int semid[2];

Request r;
result *res;
void *shared_mem = (void*)0;
int msg,shm,count;

int main()
{
	int i,a,b;
	msg = msgget((key_t)11,0666|IPC_CREAT);				//msg queue created
	shm = shmget((key_t)11,sizeof(Request),0666|IPC_CREAT);		//shared mem created 

	for(i = 0 ; i<2 ; i++)
		semid[i] = semget((key_t)(11+i),1,0666|IPC_CREAT);		//semaphore created 

	signal(SIGINT,get_result);						//SIGINT assigned to get result

	semaphore_p(semid[0]);
	r.pid = getpid();
	printf("enter the request\n");
	scanf("%d%d",&a,&b);
	r.oper = '+';
	r.opr1 = a;
	r.opr2 = b;
	r.flag = 1;
	printf("msg sent\n");
	sleep(1);
	count = msgsnd(msg,&r,sizeof(Request)-sizeof(long),0);
	semaphore_v(semid[0]);
	sleep(1);
	pause();
	exit(EXIT_SUCCESS);
}
void get_result(int a)
{
	shared_mem = shmat(shm,(void*)0,0);
	res = (result*)shared_mem;
	sleep(1);
	printf("%s:addition = %d\n",__FILE__,res->res);
	sleep(1);
}
int semaphore_p(int sem_id)
{
        struct sembuf buf;
        buf.sem_num = 0;
        buf.sem_op = -1;
        buf.sem_flg = SEM_UNDO;
        if((semop(sem_id, &buf,1)) == -1)
        {
                perror("semop");
                return 0;
        }
        return 1;
}
int semaphore_v(int sem_id)
{
        struct sembuf buf;
        buf.sem_num = 0;
        buf.sem_op = 1;
        buf.sem_flg = SEM_UNDO;
        if((semop(sem_id, &buf,1)) == -1)
        {
                perror("semop");
                return 0;
        }
        return 1;
}

