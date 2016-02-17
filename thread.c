#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

/* This is our thread function.  It is like main(), but for a thread*/
void *port_listener(void *arg)
{
	char *str;
	int i = 0;

	str=(char*)arg;

	while(1)
	{
		usleep(1000);
		printf("%s\n", str);

	}

	return NULL;
}

int main(void)
{
	pthread_t pth[2];	// this is our thread identifier
	int i = 0;

	pthread_create(&pth[0],NULL,port_listener,"0000");
	pthread_create(&pth[1],NULL,port_listener,"1111");

	while(i < 100)
	{
		usleep(1);
		printf("main is running...\n");
		++i;
	}

	printf("main waiting for threads to terminate...\n");
	pthread_join(pth[0],NULL);
	pthread_join(pth[1],NULL);

	return 0;
}
