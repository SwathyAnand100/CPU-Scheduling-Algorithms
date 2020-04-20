#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<pthread.h>
#include<limits.h>
#include<stdbool.h>
#include<time.h>
int process[3][100];
int run2[100]={0};
int* rt;
int choice,i=0,n=0,j=0,ind=0,timequantum;
sem_t out,in,rdyque,blkque;

struct Queue
{
    int front, rear, size;
    unsigned capacity;
    int *array;
}*readyqueue,*blockedqueue,*ganttchart;

struct Queue* createQueue(unsigned capacity) 
{ 
    struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue)); 
    queue->capacity = capacity; 
    queue->front = queue->size = 0;  
    queue->rear = capacity - 1;  // This is important, see the enqueue 
    queue->array = (int*) malloc(queue->capacity * sizeof(int)); 
    return queue; 
} 



// Queue is full when size becomes equal to the capacity
int isFull(struct Queue* queue)
{ return (queue->size == queue->capacity);  }

// Queue is empty when size is 0
int isEmpty(struct Queue* queue)
{  return (queue->size == 0); }



void enqueue(struct Queue* queue, int item) 
{
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1)%queue->capacity; 
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
}


// Function to remove an item from queue.  
// It changes front and size 
int dequeue(struct Queue* queue) 
{ 
    if (isEmpty(queue)) 
        return INT_MIN; 
    int item = queue->array[queue->front]; 
    queue->front = (queue->front + 1)%queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

void dequeue_ele(struct Queue * queue,int item)
{
	int arr[50]={0},k=0,itr,element=0;
	while(isEmpty(queue)==0)
	{
		element=dequeue(queue);
		if(element==item)
		{
			itr=k;
			break;
		}
		arr[k]=element;
		k=k+1;
	}
	for(int m=0;m < k;m++)
	{
		enqueue(queue,arr[m]);
	}
}
void print(struct Queue *queue)
{
	int x;
	x=queue->front;
	printf("\n\n");
	for (int k=0;k < queue->size;k++)
	{
		 if (isEmpty(queue)) 
        		return INT_MIN;
    		int item = queue->array[x];
		printf("P%d | ",item); 
    		x = (x + 1)%queue->capacity;
        }
	printf("\n");
}


void *ready_state(int* arg)
{
	int m=0;
	printf("\n");
	while(1)
	{
		sem_wait(&rdyque);
		if(i!=m)
		{
			m=i;
			enqueue(readyqueue,i);
			printf("\nReady Queue");
			print(readyqueue);
		}
		sem_post(&rdyque);
	}
}



void * run_statefifo(int * arg)
{
	int pno,ptime=0,f1=0,run=0;
	printf("\n");
	while(1)
	{
		sem_wait(&rdyque);
		if (isEmpty(readyqueue)==0)
		{	pno=dequeue(readyqueue);
			run=0;
										//printf("pno %d\n",pno);
			f1=0;
			sem_post(&rdyque);
			sem_wait(&out);
			for(int m=0;m<n;m++)
			{
				if(run2[m]==pno)
				{
					run=2;
					break;
				}
			}
			sem_post(&out);							//printf("run %d\n",run);
			sem_wait(&in);
			ptime=process[run][(pno-1)];
			sem_post(&in);
			if(ptime!=0)
			{
										//printf("ptime %d\n",ptime);
				while(ptime>0)
					ptime--;
				sem_wait(&in);
				process[run][(pno-1)]=0;
				sem_post(&in);
			}
			if(run==0)
                        {
        	                sem_wait(&blkque);
                                enqueue(blockedqueue,pno);
				printf("\nBlocked queue");
                                print(blockedqueue);
                                sem_post(&blkque);
                                f1=0;
                        }
                        if(f1==0 && run==2)
                        {
                                f1=1;
                                printf("\nP%d terminate",pno);
	                }
		}
		else
			sem_post(&rdyque);
	}
}


void * run_statesjfnp(int * arg)
{
	int pno,item,ptime,smallest,run=0,r,f1=0,frnt,f2=0;
	printf("\n");
	while(1)
	{
		if(ind==1)
		{
			sem_wait(&rdyque);
			sem_wait(&in);
			if(isEmpty(readyqueue)==0)
			{
				frnt=readyqueue->front;
				pno=readyqueue->array[frnt];                                       //printf("\npno %d",pno);
				if(process[0][(pno-1)] != 0)
					smallest=process[0][(pno-1)];
				else
					smallest=99999999;
				run=0;
				sem_post(&in);

				for(int s=0; s < readyqueue->size; s++)
				{
					if(isEmpty(readyqueue))
						break;
					item = readyqueue->array[frnt];
                			frnt = (frnt + 1)%readyqueue->capacity;
					r=0;
					sem_wait(&out);
					for(int t=0;t<n;t++)
					{
						if(item==run2[t])
						{
							r=2;
							break;
						}
					}
					sem_post(&out);
					sem_wait(&in);
					if(process[r][item-1] < smallest && process[r][item-1]!=0)
					{
						pno=item;
						smallest=process[r][item-1];
						run=r;
					}
					sem_post(&in);
				}
				ptime=smallest;
				if(isEmpty(readyqueue)==0)
				{
					dequeue_ele(readyqueue,pno);
					sem_post(&rdyque);
					if(ptime!=0)
					{
						while(ptime>0)
							ptime--;
						sem_wait(&in);
						process[run][(pno-1)]=0;
						sem_post(&in);
					}
					if(run==0)
					{
						sem_wait(&blkque);
						enqueue(blockedqueue,pno);
						printf("\nBlocked queue");
						print(blockedqueue);
						sem_post(&blkque);
						f1=0;
					}
					if(f1==0 && run==2)
                        		{
        	                		f1=1;
                                		printf("\nP%d terminate\n",pno);
                        		}
				}
				else
					sem_post(&readyqueue);
			}
			else
			{
				sem_post(&rdyque);
				sem_post(&in);
			}
		}
	}
}


void * run_statesjfp(int * arg)
{
	int pno,item,ptime,smallest,run=0,r,f1=0,frnt,f2=0;
	printf("\n");
	while(1)
	{
		if(ind==1)
		{
			sem_wait(&rdyque);
			sem_wait(&in);
			if(isEmpty(readyqueue)==0)
			{
				frnt=readyqueue->front;
				pno=readyqueue->array[frnt];                                       //printf("\npno %d",pno);
				if(process[0][(pno-1)] != 0)
					smallest=process[0][(pno-1)];
				else
					smallest=99999999;
				run=0;
				sem_post(&in);

				for(int s=0; s < readyqueue->size; s++)
				{
					if(isEmpty(readyqueue))
						break;
					item = readyqueue->array[frnt];
                			frnt = (frnt + 1)%readyqueue->capacity;
					r=0;
					sem_wait(&out);
					for(int t=0;t<n;t++)
					{
						if(item==run2[t])
						{
							r=2;
							break;
						}
					}
					sem_post(&out);
					sem_wait(&in);
					if(process[r][item-1] < smallest && process[r][item-1]!=0)
					{
						pno=item;
						smallest=process[r][item-1];
						run=r;
					}
					sem_post(&in);
				}
				ptime=smallest;
				if(isEmpty(readyqueue)==0)
				{
					dequeue_ele(readyqueue,pno);
					sem_post(&rdyque);
					ptime--;
					sem_wait(&in);
					process[run][(pno-1)]=ptime;
					sem_post(&in);
					if(ptime==0 && run==0)
					{
						sem_wait(&blkque);
						enqueue(blockedqueue,pno);
						printf("\nBlocked queue");
						print(blockedqueue);
						sem_post(&blkque);
					}
					else if(ptime!=0)
					{
						sem_wait(&rdyque);
						enqueue(readyqueue,pno);
						printf("\nReady queue");
						print(readyqueue);
						sem_post(&rdyque);
					}
					else if (ptime==0 && run==2)
					{
						printf("\nP%d terminate\n",pno);
					}
				}
				else
					sem_post(&rdyque);
			}
			else
			{
				sem_post(&rdyque);
				sem_post(&in);
			}
		}
	}
}

void *run_stateRR(int * arg)
{
	int pno,ptime,f1=0,run=0;
	while(1)
	{
		sem_wait(&rdyque);
		if(isEmpty(readyqueue)==0)
		{
			pno=dequeue(readyqueue);
			run=0;
			sem_post(&rdyque);
			sem_wait(&out);
			for(int t=0;t < n;t++)
			{
				if(run2[t]==pno)
				{
					run=2;
					break;
				}
			}
			sem_post(&out);
			sem_wait(&in);
			ptime=process[run][(pno-1)];
			sem_post(&in);
			for(int s=0;s < timequantum && ptime>0;s++)
				ptime--;
			sem_wait(&in);
			process[run][(pno-1)]=ptime;
			sem_post(&in);
			if(ptime==0 && run==0)
			{
				sem_wait(&blkque);
				enqueue(blockedqueue,pno);
				printf("\nBlocked queue");
				print(blockedqueue);
				sem_post(&blkque);
			}
			else if(ptime!=0)
			{
				sem_wait(&rdyque);
				enqueue(readyqueue,pno);
				printf("\nReady queue");
				print(readyqueue);
				sem_post(&rdyque);
			}
			else if (ptime==0 && run==2)
			{
				printf("\nP%d terminate\n",pno);
			}
		}
		else
			sem_post(&rdyque);
	}
}



void * block_state(int *arg)
{
	int pno,ptime;
	printf("\n");
	while(1)
	{
		sem_wait(&blkque);
		if(isEmpty(blockedqueue)==0)
		{
			pno=dequeue(blockedqueue);
											//printf("\nbpno %d",pno);
			sem_post(&blkque);
			sem_wait(&in);
			ptime=process[1][(pno-1)];
											//printf("\nbptime %d",ptime);
			sem_post(&in);
			if(ptime!=0)
			{
				while(ptime>0)
					ptime--;
				sem_wait(&in);
				process[1][(pno-1)]=0;
				sem_post(&in);
			}
			sem_wait(&rdyque);
			enqueue(readyqueue,pno);
			printf("\nReady Queue");
			print(readyqueue);
			sem_post(&rdyque);
			sem_wait(&out);
			run2[n]=pno;
			n=n+1;
			sem_post(&out);
		}
		else
			sem_post(&blkque);
	}
}


void main()
{
	system("clear");
	int r1,io,r2,flag;
	printf("Enter the algorithm.no:\n");
	printf("1.First in First out \n");
	printf("2.Shortest Job First Non-Preemptive \n");
	printf("3.Shortest Job Fifst  Preemptive\n");
	printf("4.Round Robin \n");
	printf("\nEnter your option:");
	scanf("%d",&choice);
	if(choice==4)
	{
		printf("\nEnter the time quantum:");
		scanf("%d",&timequantum);
	}
	sem_init(&in,0,1);
	sem_init(&out,0,0);
	sem_init(&rdyque,0,0);
	sem_init(&blkque,0,0);
	readyqueue=createQueue(100);
	blockedqueue=createQueue(100);
	pthread_t ready_thread,run_thread,blocked_thread;
	switch(choice)
	{
	case 1:
		pthread_create(&blocked_thread,NULL,block_state,process);
		pthread_create(&run_thread,NULL,run_statefifo,process);
		pthread_create(&ready_thread,NULL,ready_state,process);
		flag=0;
		break;
	case 2:
		pthread_create(&blocked_thread,NULL,block_state,process);
		pthread_create(&run_thread,NULL,run_statesjfnp,process);
                pthread_create(&ready_thread,NULL,ready_state,process);
                flag=0;
                break;
	case 3:
                pthread_create(&blocked_thread,NULL,block_state,process);
                pthread_create(&run_thread,NULL,run_statesjfp,process);
                pthread_create(&ready_thread,NULL,ready_state,process);
                flag=0;
                break;

	case 4:
		pthread_create(&blocked_thread,NULL,block_state,process);
                pthread_create(&run_thread,NULL,run_stateRR,process);
                pthread_create(&ready_thread,NULL,ready_state,process);
                flag=0;
		break;
	default:
		printf("\nInvalid entry");
		exit(0);
	}
	printf("\nEnter the process time(CPU time | I/O time | CPU time):\n");
	while(1)
	{
		scanf("%d %d %d",&r1,&io,&r2);
		sem_wait(&in);
		process[j][i]=r1;printf("%d",process[j][i]);
		j=j+1;
		process[j][i]=io;
		j=j+1;
		process[j][i]=r2;
		i=i+1;
		j=0;
		ind=1;
		sem_post(&in);
		if(flag==0)
		{
			flag=1;
			sem_post(&rdyque);
			sem_post(&blkque);
			sem_post(&out);
		}
	}
}
