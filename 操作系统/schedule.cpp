#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<pthread.h>
#include<time.h>

int Thread_Num;

pthread_mutex_t Device_mutex ;
//Virtual PCB of threads 
struct VirtualPCB
{
	int tid;			//进程名;
	int arrivetime;		//到达时间
	int runtime;		//服务时间
	int priority;		//优先级 
	int endtime;		//结束时间 
	int starttime;		//开始时间
	float turntime;		//周转时间 
	float weight_turntime; //带权周转时间 
	int state;			//设置进程状态 
	float response;		//响应比 
	int waittime;		//等待时间 
	 
}TCB[30]; 


//Function to initial virtual PCB
void t_init()
{
    int n;
    //srand(time(NULL));		//用于生成随机数，将当前时间作为种子传递给srand函数，以便每次运行都能生成不同的随机数序列 
    for(n =0;n<Thread_Num;n++)
    {
    	TCB[n].tid = n + 1;//用线程创建序号作为虚拟进程id 
  
		printf("请输入进程%d的到达时间，服务时间，优先级：\n",TCB[n].tid);//从键盘输入各进程的到达时间，服务时间，优先级 
   
    	scanf("%d%d%d",&TCB[n].arrivetime,&TCB[n].runtime,&TCB[n].priority);
    }
}

//Threads run function
void *t_print(void *arg)
{
    int n = *(int *)arg;
    while(1)
    {
        pthread_mutex_lock(&Device_mutex);
        printf("Thread_%-2d: ",n);
        printf("tid:%-2d arrivetime:%-2d runtime:%-2d priority:%-2d \n",TCB[n-1].tid,TCB[n-1].arrivetime,TCB[n-1].runtime,TCB[n-1].priority);
        pthread_mutex_unlock(&Device_mutex);
        sleep(1);
        break;
    }
    pthread_exit(0);
}
void print_all()//输出 
{
	int i,j;
	float sum_turn=0;
	float sum_weight=0;
	float avg_turn;
	float avg_weight;
	printf("%-10s%-10s%-10s%-10s%-10s%-10s\t\n","进程名","到达时间","服务时间","完成时间","周转时间","带权周转时间"); 
	//按进程输入顺序排序
	for(i=0;i<Thread_Num-1;i++){
		for(j=i+1;j<Thread_Num;j++){
			if(TCB[i].tid>TCB[j].tid){
			 	struct VirtualPCB temp = TCB[i];
                TCB[i] = TCB[j];
                TCB[j] = temp;	
			}
		}
	} 
	
	for(i=0;i<Thread_Num;i++){
		sum_turn += TCB[i].turntime;
		sum_weight += TCB[i].weight_turntime;
		printf("%-10d%-10d%-10d%-10d%-10.2f%-10.2f\t\n",
		TCB[i].tid,TCB[i].arrivetime,TCB[i].runtime,TCB[i].endtime,TCB[i].turntime,TCB[i].weight_turntime);
	}
	avg_turn = sum_turn / Thread_Num;
	avg_weight = sum_weight / Thread_Num;
	printf("平均周转时间为%.2f\n",avg_turn);
	printf("平均带权周转时间为%.2f\n",avg_weight);
	
}
//First come first service schedule function
void FCFS()
{
    int i,j;
    int time = 0;
	printf("----------------------FCFS----------------------\n");
    // 按照到达时间排序
    for (i = 0; i < Thread_Num -1; i++) {
        for (j = i + 1; j < Thread_Num; j++) {
            if (TCB[i].arrivetime > TCB[j].arrivetime) {
                struct VirtualPCB temp = TCB[i];
                TCB[i] = TCB[j];
                TCB[j] = temp;
            }
        }
    }
    printf("调度顺序为：");
    // 执行任务
    for(i=0;i<Thread_Num;i++)
    {
        if(time < TCB[i].arrivetime){
        	time=TCB[i].arrivetime;
		}
		TCB[i].starttime = time;
		time += TCB[i].runtime;
		TCB[i].endtime = time;
        TCB[i].turntime = TCB[i].endtime - TCB[i].arrivetime; 
        TCB[i].weight_turntime = TCB[i].turntime / TCB[i].runtime;
		printf("%d-",TCB[i].tid);
    }
    printf("end\n");
    print_all();
}
//Shortest job first schedule function
void SJF()
{
	int i,j;
	int time = 0;
	int t=0;//判断有几个进程被执行 
    printf("----------------------SJF----------------------\n");
    printf("调度顺序为：");
	//选取最小服务时间的下标 
    while(t<Thread_Num){
    	int index=Thread_Num+1;
    	int min_time = 999;
    	for(i=0;i<Thread_Num;i++){
    		if(TCB[i].state==0 &&TCB[i].runtime >0 &&TCB[i].arrivetime<=time && TCB[i].runtime<min_time){
    			min_time = TCB[i].runtime;
    			index = i;
			}
		}
		if(index==Thread_Num+1){
			time++;
		}else{
			TCB[index].starttime = time;
			time += TCB[index].runtime;
			TCB[index].endtime = time;
			TCB[index].turntime = TCB[index].endtime - TCB[index].arrivetime; 
            TCB[index].weight_turntime = TCB[index].turntime / TCB[index].runtime; 
            TCB[index].state = 1;
            t++; 
            printf("%d-",TCB[index].tid);
		}
	}
	printf("end\n");
	//先判断在time时刻有谁到达，然后根据服务时间来决定顺序，都没有，time++再判断 
	print_all();
	for(i=0;i<Thread_Num;i++){
		TCB[i].state=0;
	} //算法结束后将其执行状态置为0 
}
//Priority schedule function
void Priority()
{
   int i,j;
	int time = 0;
	int t=0;//判断有几个进程被执行 
    printf("----------------------Priority----------------------\n");
	printf("调度顺序为：");

	while(t<Thread_Num){
    	int index=Thread_Num+1;
    	int min_priority = 999;//数字越小优先级越高 
    	for(i=0;i<Thread_Num;i++){
    		if(TCB[i].state==0 &&TCB[i].arrivetime<=time && TCB[i].priority<min_priority){
    			min_priority = TCB[i].priority;
    			index = i;
			}
		}
		if(index==Thread_Num+1){
			time++;
		}else{
			TCB[index].starttime = time;
			time += TCB[index].runtime;
			TCB[index].endtime = time;
			TCB[index].turntime = TCB[index].endtime - TCB[index].arrivetime; 
            TCB[index].weight_turntime = TCB[index].turntime / TCB[index].runtime; 
            TCB[index].state = 1;
            t++;
            printf("%d-",TCB[index].tid);
		}
	}
	printf("end\n");
	print_all();
	for(i=0;i<Thread_Num;i++){
		TCB[i].state=0;
	} //算法结束后将其执行状态置为0 
}

void HRRN()
{
   //首先计算响应比，找到最大响应比进程 
   //对于还未到达的进程，将响应比置为0
   printf("----------------------HRRN----------------------\n");
   int i,j;
   int time=0;
   int block_q=0;	//已经被执行完的进程队列 
	printf("调度顺序为：");
   while(block_q<Thread_Num){
   		int t_time=time; 
   		//计算响应比 
	   for(i=0;i<Thread_Num;i++){
	   		if(TCB[i].state==0){
				if(TCB[i].arrivetime<=time){
					TCB[i].waittime = time - TCB[i].arrivetime;
					TCB[i].response = (TCB[i].waittime + TCB[i].runtime) / (float)(TCB[i].runtime);//类型转换 
				}else{
					TCB[i].response = 0;
				}
			}else{
				TCB[i].response = -1;
			}
	   } 
	   //找到最高响应比的进程 
		int max=0;
		for(i=0;i<Thread_Num;i++){
			if(TCB[i].response>TCB[max].response){
				max=i;
			}   		
		}
		if(TCB[max].response!=0){
			TCB[max].starttime = time;
			time += TCB[max].runtime;
			TCB[max].endtime = time;
			TCB[max].turntime = TCB[max].endtime - TCB[max].arrivetime; 
	        TCB[max].weight_turntime = TCB[max].turntime / TCB[max].runtime; 
	        TCB[max].state = 1;
	        block_q++;
	        printf("%d-",TCB[max].tid);
		}
		if(t_time == time){
			time++;
		}
	}
	printf("end\n");
	print_all();
	for(i=0;i<Thread_Num;i++){
		TCB[i].state=0;
	} //算法结束后将其执行状态置为0 	 
}

//Main thread execute function to create children threads
void *Children(void *arg)
{
    int ret[Thread_Num]; // 保存线程创建的返回值
    pthread_t tid[Thread_Num]; // 线程ID数组
    pthread_mutex_init(&Device_mutex, NULL); // 初始化互斥锁
    int i, j;

    for (i = 0; i < Thread_Num; i++)
    {
        // 为每个线程分配新的变量，避免不同线程之间相互影响
        int* thread_num_ptr = (int*)malloc(sizeof(int));
        if (thread_num_ptr == NULL) {
            printf("Failed to allocate memory for thread %d", i + 1);
            exit(1);
        }
        *thread_num_ptr = i + 1;

        // 创建线程
        ret[i] = pthread_create(&tid[i], NULL, &t_print, thread_num_ptr);
        if (ret[i] != 0) {
            printf("Failed to create thread %d\n", i + 1);
            exit(1);
        }

        // 等待一段时间，避免线程同时启动导致输出混乱
        sleep(1);
    }

    // 等待所有线程执行完毕
    for (j = 0; j < Thread_Num; j++) {
        if (pthread_join(tid[j], NULL) != 0) {
            printf("Failed to join thread %d\n", j + 1);
        }
    }

    // 销毁互斥锁
    pthread_mutex_destroy(&Device_mutex);

    pthread_exit(0);
}
int main()
{
    int ret1;
	int d;
	printf("请输入进程数目：");
	scanf("%d",&Thread_Num);
	printf("---------------进程初始化---------------\n");
    t_init();
    pthread_t tid1;
    ret1 = pthread_create(&tid1,NULL,&Children,NULL);//Create main thread
    if(ret1 == 0) 
    {
        printf("Main Thread ok!\n");
        sleep(Thread_Num);
    }
    else{ 
        printf("Thread failed!\n");
    }  
    printf("choose 1=FCSF 2=SPF 3=Priority 4=HRRN\n");
    scanf("%d",&d);
    switch(d){
    	case 1 :
			FCFS();
			break;
    	case 2 :
			SJF();
			break;
    	case 3 :
			Priority();
			break;
    	case 4 :
			HRRN();
			break;
    	default:
    		printf("输入有误!\n");
    		break;
	}
	return 0;
}
