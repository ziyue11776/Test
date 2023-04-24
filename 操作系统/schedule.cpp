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
	int tid;			//������;
	int arrivetime;		//����ʱ��
	int runtime;		//����ʱ��
	int priority;		//���ȼ� 
	int endtime;		//����ʱ�� 
	int starttime;		//��ʼʱ��
	float turntime;		//��תʱ�� 
	float weight_turntime; //��Ȩ��תʱ�� 
	int state;			//���ý���״̬ 
	float response;		//��Ӧ�� 
	int waittime;		//�ȴ�ʱ�� 
	 
}TCB[30]; 


//Function to initial virtual PCB
void t_init()
{
    int n;
    //srand(time(NULL));		//�������������������ǰʱ����Ϊ���Ӵ��ݸ�srand�������Ա�ÿ�����ж������ɲ�ͬ����������� 
    for(n =0;n<Thread_Num;n++)
    {
    	TCB[n].tid = n + 1;//���̴߳��������Ϊ�������id 
  
		printf("���������%d�ĵ���ʱ�䣬����ʱ�䣬���ȼ���\n",TCB[n].tid);//�Ӽ�����������̵ĵ���ʱ�䣬����ʱ�䣬���ȼ� 
   
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
void print_all()//��� 
{
	int i,j;
	float sum_turn=0;
	float sum_weight=0;
	float avg_turn;
	float avg_weight;
	printf("%-10s%-10s%-10s%-10s%-10s%-10s\t\n","������","����ʱ��","����ʱ��","���ʱ��","��תʱ��","��Ȩ��תʱ��"); 
	//����������˳������
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
	printf("ƽ����תʱ��Ϊ%.2f\n",avg_turn);
	printf("ƽ����Ȩ��תʱ��Ϊ%.2f\n",avg_weight);
	
}
//First come first service schedule function
void FCFS()
{
    int i,j;
    int time = 0;
	printf("----------------------FCFS----------------------\n");
    // ���յ���ʱ������
    for (i = 0; i < Thread_Num -1; i++) {
        for (j = i + 1; j < Thread_Num; j++) {
            if (TCB[i].arrivetime > TCB[j].arrivetime) {
                struct VirtualPCB temp = TCB[i];
                TCB[i] = TCB[j];
                TCB[j] = temp;
            }
        }
    }
    printf("����˳��Ϊ��");
    // ִ������
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
	int t=0;//�ж��м������̱�ִ�� 
    printf("----------------------SJF----------------------\n");
    printf("����˳��Ϊ��");
	//ѡȡ��С����ʱ����±� 
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
	//���ж���timeʱ����˭���Ȼ����ݷ���ʱ��������˳�򣬶�û�У�time++���ж� 
	print_all();
	for(i=0;i<Thread_Num;i++){
		TCB[i].state=0;
	} //�㷨��������ִ��״̬��Ϊ0 
}
//Priority schedule function
void Priority()
{
   int i,j;
	int time = 0;
	int t=0;//�ж��м������̱�ִ�� 
    printf("----------------------Priority----------------------\n");
	printf("����˳��Ϊ��");

	while(t<Thread_Num){
    	int index=Thread_Num+1;
    	int min_priority = 999;//����ԽС���ȼ�Խ�� 
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
	} //�㷨��������ִ��״̬��Ϊ0 
}

void HRRN()
{
   //���ȼ�����Ӧ�ȣ��ҵ������Ӧ�Ƚ��� 
   //���ڻ�δ����Ľ��̣�����Ӧ����Ϊ0
   printf("----------------------HRRN----------------------\n");
   int i,j;
   int time=0;
   int block_q=0;	//�Ѿ���ִ����Ľ��̶��� 
	printf("����˳��Ϊ��");
   while(block_q<Thread_Num){
   		int t_time=time; 
   		//������Ӧ�� 
	   for(i=0;i<Thread_Num;i++){
	   		if(TCB[i].state==0){
				if(TCB[i].arrivetime<=time){
					TCB[i].waittime = time - TCB[i].arrivetime;
					TCB[i].response = (TCB[i].waittime + TCB[i].runtime) / (float)(TCB[i].runtime);//����ת�� 
				}else{
					TCB[i].response = 0;
				}
			}else{
				TCB[i].response = -1;
			}
	   } 
	   //�ҵ������Ӧ�ȵĽ��� 
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
	} //�㷨��������ִ��״̬��Ϊ0 	 
}

//Main thread execute function to create children threads
void *Children(void *arg)
{
    int ret[Thread_Num]; // �����̴߳����ķ���ֵ
    pthread_t tid[Thread_Num]; // �߳�ID����
    pthread_mutex_init(&Device_mutex, NULL); // ��ʼ��������
    int i, j;

    for (i = 0; i < Thread_Num; i++)
    {
        // Ϊÿ���̷߳����µı��������ⲻͬ�߳�֮���໥Ӱ��
        int* thread_num_ptr = (int*)malloc(sizeof(int));
        if (thread_num_ptr == NULL) {
            printf("Failed to allocate memory for thread %d", i + 1);
            exit(1);
        }
        *thread_num_ptr = i + 1;

        // �����߳�
        ret[i] = pthread_create(&tid[i], NULL, &t_print, thread_num_ptr);
        if (ret[i] != 0) {
            printf("Failed to create thread %d\n", i + 1);
            exit(1);
        }

        // �ȴ�һ��ʱ�䣬�����߳�ͬʱ���������������
        sleep(1);
    }

    // �ȴ������߳�ִ�����
    for (j = 0; j < Thread_Num; j++) {
        if (pthread_join(tid[j], NULL) != 0) {
            printf("Failed to join thread %d\n", j + 1);
        }
    }

    // ���ٻ�����
    pthread_mutex_destroy(&Device_mutex);

    pthread_exit(0);
}
int main()
{
    int ret1;
	int d;
	printf("�����������Ŀ��");
	scanf("%d",&Thread_Num);
	printf("---------------���̳�ʼ��---------------\n");
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
    		printf("��������!\n");
    		break;
	}
	return 0;
}
