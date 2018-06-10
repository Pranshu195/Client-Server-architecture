#include"header.h"
#include"request.h"
#include"decl.h"

int semaphore_p(int);
int semaphore_v(int);

int main()
{
	Request *r1;
	result res1;
	int i;
	void* shared_mem = (void*)0;
	int msgid,shmid,semid[2],ret;
	
	for( i=0 ; i<2 ; i++)
		semid[i] = semget((key_t)(i+13),1,0666|IPC_CREAT);

	msgid = msgget((key_t)12,0666|IPC_CREAT);
	shmid = shmget((key_t)12,sizeof(Request),0666|IPC_CREAT);

	semaphore_p(semid[0]);
	shared_mem = shmat(shmid,(void*)0,0);
	r1 = (Request*)shared_mem;
//	printf("%s: recieved operator %c\n",__FILE__,r1->oper);
	res1.pidr = r1->pid;
	res1.res = r1->opr1 * r1->opr2;
	semaphore_v(semid[0]);
	
	semaphore_p(semid[1]);
	ret = msgsnd(msgid,&res1,sizeof(result)-sizeof(long),0);
//	printf("%s: result sent %d\n",__FILE__,res1.res);
	semaphore_v(semid[1]);
	
	exit(EXIT_SUCCESS);
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


