#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

char buffer[5];
int head  = 0; 
int tail  = 0;
int count = 0;
int done  = 0;

pthread_mutex_t lock     = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full  = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

void *producer(void *arg);
void *consumer(void *arg);

int main()
{
	pthread_t prod, cons;
	pthread_create(&prod, NULL, producer, NULL);
	pthread_create(&cons, NULL, consumer, NULL);
	pthread_join(prod, NULL);
	pthread_join(cons, NULL);
	return 0;
}

void *producer(void *arg)
{
    int ch;

	FILE *fp = fopen("assignment2.txt", "r");
	if (!fp)
    { 
        perror("fopen"); 
        pthread_exit(NULL); 
    }

	while ((ch = fgetc(fp)) != EOF)
	{
		pthread_mutex_lock(&lock);

		while (count == 5)
        {
			pthread_cond_wait(&not_full, &lock);
        }

		buffer[tail] = (char)ch;
		tail = (tail + 1) % 5;
		count++;
		pthread_cond_signal(&not_empty);
		pthread_mutex_unlock(&lock);
	}
	fclose(fp);
	pthread_mutex_lock(&lock);

	done = 1;

	pthread_cond_signal(&not_empty);
	pthread_mutex_unlock(&lock);
	return NULL;
}

void *consumer(void *arg)
{
	char c;
	while (1) 
    {
		pthread_mutex_lock(&lock);
		while (count == 0 && !done)
        {
			pthread_cond_wait(&not_empty, &lock);
        }

		if (count == 0 && done) 
        {
			pthread_mutex_unlock(&lock);
			break;
		}
		c = buffer[head];
		head = (head+1) % 5;
		count--;

		pthread_cond_signal(&not_full);
		pthread_mutex_unlock(&lock);
		putchar(c);
	}
	putchar('\n');
	return NULL;
}

