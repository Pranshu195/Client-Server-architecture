#include"header.h"
#include"request.h"
#include"decl.h"
int createfifo()
{
	int ret;
	int i;
	for(i=0 ; i<2 ; i++)
	{	
		msgid[i] = msgget((key_t)(i+11),0666|IPC_CREAT);
		if(msgid[i] == -1)
		{
			perror("msgget");
			exit(EXIT_FAILURE);
		}
	}
	for( i=0 ; i<2 ; i++)
	{
		shmid[i] = shmget((key_t)(11+i),sizeof(Request),0666|IPC_CREAT);
		if(shmid[i] == -1)
		{
			perror("shmget");
			exit(EXIT_FAILURE);
		}
	}
	return 0;
}

