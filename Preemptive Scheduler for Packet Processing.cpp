/*------------PROJECT : Preemptive Scheduling for Packet Processing------------*/

//		*************************************************
//		*												*
//		*												*
//		*		Jatin Kesnani	(K21-3204)				*
//		*												*
//		*												*
//		*************************************************

#include <iostream>
#include <vector>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <iomanip>
#define BOLD_CYAN "\e[1;36m"
#define RESET "\033[0m"

using namespace std;

int sleepcount = 0;
int TimeQuantum;
bool visited[100];
int queue[100], front = 0, rear = 0;
static int index=0;
int n;
int x = 0;
float total_waiting_time = 0;
float total_turnaround_time = 0;
float avg_waiting_time = 0;
float avg_turnaround_time = 0;
int GANTTindex[100], k=0, l=0, GANTTpid[100];
int current_time = 0;
bool check = false;
int anothervar = -1;
int PID[100], PAT[100], PBT[100], PFT[100];
int TAT[100], WT[100];
int RT[100];
sem_t mutex;
sem_t completed;

struct Packet{
    int pid;
    int arrival_time;
    int burst_time;
    int waiting_time;
    int turnaround_time;
    int completion_time;
    int remaining_time;
    int priority;
};

vector<Packet> packets;

void Text_Animation(string a){
	for(int i=0 ; a[i]!='\0' ; i++){
		for(long int j=0 ; j<10000000/2; j++){}
		cout<<a[i];
	}
}

bool checkshortest(Packet *p)
{
    bool shortest_remaining_time = true;
    for (int i = 0; i < packets.size(); i++) {
        if (packets[i].remaining_time > 0 && packets[i].arrival_time <= current_time && i != p->pid - 1) {
            if (packets[i].remaining_time < p->remaining_time)
            {
                shortest_remaining_time = false;
                break;
            }
            else if (packets[i].remaining_time == p->remaining_time && packets[i].arrival_time < p->arrival_time)
            {
                shortest_remaining_time = false;
                break;
            }
        }
    }
    return shortest_remaining_time;
}

bool checklargest(Packet *p)
{
    bool largest_remaining_time = true;
    for (int i = 0; i < packets.size(); i++)
	{
        if (packets[i].remaining_time > 0 && packets[i].arrival_time <= current_time && i != p->pid - 1)
	{
            if (packets[i].remaining_time > p->remaining_time)
            {
                largest_remaining_time = false;
                break;
            }
            else if (packets[i].remaining_time == p->remaining_time && packets[i].arrival_time < p->arrival_time)
            {
                largest_remaining_time = false;
                break;
            }
        }
    }
    return largest_remaining_time;
}

bool check_packet_arrival()
{
    bool arrived = false;
    for (int i = 0; i < packets.size(); i++)
	{
        if (packets[i].remaining_time > 0 && packets[i].arrival_time <= current_time)
        {
            arrived = true;
            break;
        }
    }
    return arrived;
}

void* srtf_algorithm(void* arg)
{
    Packet* packet = (Packet*) arg;
    cout << "Packet: P" << packet->pid << " has arrived! Thread created for P" << packet->pid << "\n";
    packet->remaining_time = packet->burst_time;
    sleepcount--;
    sleep(sleepcount);
    
	while (packet->remaining_time > 0)
    	{	
        	if(packet->arrival_time <= current_time)
        	{
            		sem_wait(&mutex);
           		bool flag = checkshortest(packet);
            		if(flag == true)
            		{
                		packet->remaining_time--;
                		current_time++;
				packet->completion_time = current_time;
            		}
            		sem_post(&mutex);
        	}
        	else
		{
           		if(!check_packet_arrival())
           		{
           			sem_wait(&mutex);
                		current_time++;
                		sem_post(&mutex);
           		}
        	}
    	}
	
	packet->turnaround_time = packet->completion_time - packet->arrival_time;
    	packet->waiting_time = packet->turnaround_time - packet->burst_time;
    
	sem_post(&completed);
    	return NULL;
}

void* lrtf_algorithm(void* arg)
{
    Packet* packet = (Packet*) arg;
    cout << "Packet: P" << packet->pid << " has arrived! Thread created for P" << packet->pid << "\n";
    packet->remaining_time = packet->burst_time;
    sleepcount--;
    sleep(sleepcount);
    while (packet->remaining_time > 0)
    {
        if(packet->arrival_time <= current_time)
        {
            sem_wait(&mutex);
            bool flag = checklargest(packet);
            if(flag == true)
            {
                packet->remaining_time--;
                current_time++;
                packet->completion_time = current_time;
            }
            sem_post(&mutex);
        }
        else
	{
           if(!check_packet_arrival())
	   {
           	sem_wait(&mutex);
                current_time++;
                sem_post(&mutex);
           }
        }
    }
 
    packet->turnaround_time = packet->completion_time - packet->arrival_time;
    packet->waiting_time = packet->turnaround_time - packet->burst_time;

    sem_post(&completed);
    return NULL;
}

void* priority_algorithm(void* arg)
{
    Packet* packet = (Packet*) arg;
    cout << "Packet: P" << packet->pid << " has arrived! Thread created for P" << packet->pid << "\n";
    packet->remaining_time = packet->burst_time;
    sleepcount--;
    sleep(sleepcount);
    
    while(1){
    	int HPPIndex = -1;
    	int HPP = 2147483647;
    	bool iscompleted = true;
    	for(int i=0 ; i<n ; i++){
    		if(packets[i].remaining_time > 0){
    			iscompleted = false;
                if(packets[i].arrival_time <= current_time){
                	sem_wait(&mutex);
                    if(packets[i].priority < HPP){
                        HPP = packets[i].priority;
                        HPPIndex = i;
                        check = true;
                    }
                    sem_post(&mutex);
                }
    		}
    	}
    	if(iscompleted){
    		break;
	}
    	if(check == false){
    		sem_wait(&mutex);
            current_time++;
            if(GANTTpid[l-1] == -1){
            	GANTTindex[k-1] = current_time;
			}
			else{
				GANTTindex[k] = current_time;
		        k++;
		        GANTTpid[l] = -1;
		        l++;
			}
            sem_post(&mutex);
            continue;
        }
    	packets[HPPIndex].remaining_time--;
        current_time++;
		if(packets[HPPIndex].remaining_time == 0){
    		check = false;
			packets[HPPIndex].completion_time = current_time;
			packets[HPPIndex].turnaround_time = packets[HPPIndex].completion_time - packets[HPPIndex].arrival_time;
	        packets[HPPIndex].waiting_time = packets[HPPIndex].turnaround_time - packets[HPPIndex].burst_time;
			GANTTindex[k] = current_time;
    		k++;
    		GANTTpid[l] = packets[HPPIndex].pid;
    		l++;
		}
		if(anothervar != HPPIndex && packets[anothervar].remaining_time != 0){
    		if(anothervar != -1){
				GANTTindex[k] = current_time-1;
				k++;
    			GANTTpid[l] = packets[anothervar].pid;
				l++;	
			}
		}
        anothervar = HPPIndex;
        sem_post(&completed);
	}
    return NULL;
}

void* rr_algorithm(void* arg)
{
    Packet* packet = (Packet*) arg;
    cout << "Packet: P" << packet->pid << " has arrived! Thread created for P" << packet->pid << "\n";
    packet->remaining_time = packet->burst_time;
    sleepcount--;
    sleep(sleepcount);
    while (packet->remaining_time > 0){
	    index = queue[front];
	    front++;
	    if(packets[index].remaining_time == packets[index].burst_time){
	  		if(current_time < packets[index].arrival_time){
	  			sem_wait(&mutex);
	  			current_time = packets[index].arrival_time;
	  			GANTTindex[k] = current_time;
		        k++;
		        GANTTpid[l] = -1;
		        l++;
	  			sem_post(&mutex);
			}	
		}
		if(packets[index].remaining_time-TimeQuantum > 0){
			sem_wait(&mutex);
	        packets[index].remaining_time -= TimeQuantum;
	        current_time += TimeQuantum;
	        GANTTindex[k] = current_time;
	        k++;
	        GANTTpid[l] = packets[index].pid;
	        l++;
	        sem_post(&mutex);
	    }
	    else{
			current_time += packets[index].remaining_time;
			GANTTindex[k] = current_time;
	        k++;
	        GANTTpid[l] = packets[index].pid;
	        l++;
			packets[index].remaining_time = 0;
			packets[index].completion_time = current_time;
			packets[index].turnaround_time = packets[index].completion_time - packets[index].arrival_time;
			packets[index].waiting_time = packets[index].turnaround_time - packets[index].burst_time;
			
		    sem_post(&completed);	
		}
		for(int i=1 ; i<n ; i++){
	    	if(packets[i].remaining_time>0 && packets[i].arrival_time<=current_time && visited[i]==false){
	    		queue[++rear] = i;
	    		visited[i] = true;
	    	}
		}
		if(packets[index].remaining_time>0){
	    	queue[++rear] = index;
		}
		if(front>rear){
	      	for(int i=1 ; i<n ; i++){
	        	if(packets[i].remaining_time>0){
	          		queue[++rear] = i;
	          		visited[i] = true;
	        		break;
	    		}
	    	}
	  	}	 
	}
    return NULL;
}

void SRTFGanttChart(){
	int Complete = 0, Current_Time = 0, minimum = 9999999;
    int Smallest = 0;
    bool Check = false;
	int Anothervar = -1;
	
	GANTTindex[k] = Current_Time;
	k++;
	while(Complete != n){
        for(int j=0 ; j<n ; j++){
            if((PAT[j]<=Current_Time) && (RT[j]<minimum) && (RT[j] > 0)){
                minimum = RT[j];
                Smallest = j;
                Check = true;
            }
        }
        if(Check == false){
            Current_Time++;				
        	if(GANTTpid[l-1] == -1){
            		GANTTindex[k-1] = Current_Time;
		}
			else{
				GANTTindex[k] = Current_Time;
		        k++;
		        GANTTpid[l] = -1;
		        l++;
			}
            continue;
        }
        RT[Smallest]--;
        minimum = RT[Smallest];
        if(minimum == 0){
            minimum = 9999999;
		}
        if(RT[Smallest] == 0){
            Complete++;
            Check = false;
            PFT[Smallest] = Current_Time + 1;
            GANTTindex[k] = PFT[Smallest];		
			k++;
    		GANTTpid[l] = PID[Smallest];
			l++;	
        }
        Current_Time++;
        if(Anothervar != Smallest && RT[Anothervar] != 0){
    		if(Anothervar != -1){
	    		GANTTindex[k] = Current_Time-1;
				k++;
    			GANTTpid[l] = PID[Anothervar];
				l++;	
			}
		}
        Anothervar = Smallest;
    }
}

void LRTFGanttChart(){
	int Complete = 0, Current_Time = 0, maximum = -9999999;
    int Smallest = 0;
    bool Check = false;
	int Anothervar = -1;
	
	GANTTindex[k] = Current_Time;
	k++;
	while(Complete != n){
        for(int j=0 ; j<n ; j++){
            if((PAT[j]<=Current_Time) && (RT[j]>maximum) && (RT[j] > 0)){
                maximum = RT[j];
                Smallest = j;
                Check = true;
            }
        }
        if(Check == false){
            Current_Time++;				
        	if(GANTTpid[l-1] == -1){
            	GANTTindex[k-1] = Current_Time;
			}
			else{
				GANTTindex[k] = Current_Time;
		        k++;
		        GANTTpid[l] = -1;
		        l++;
			}
            continue;
        }
        RT[Smallest]--;
        maximum = RT[Smallest];
        if(maximum == 0){
            maximum = -9999999;
		}
        if(RT[Smallest] == 0){
            Complete++;
            Check = false;
            PFT[Smallest] = Current_Time + 1;
            GANTTindex[k] = PFT[Smallest];		
			k++;
    		GANTTpid[l] = PID[Smallest];
			l++;	
        }
        Current_Time++;
        if(Anothervar != Smallest && RT[Anothervar] != 0){
    		if(Anothervar != -1){
	    		GANTTindex[k] = Current_Time-1;
				k++;
    			GANTTpid[l] = PID[Anothervar];
				l++;	
			}
		}
        Anothervar = Smallest;
    }
}

int main(){
	cout << BOLD_CYAN;
	cout<<"\n\n\n\n\n";
	Text_Animation("\t\t\t\t _________________________________________________________________________\n");
	Text_Animation("\t\t\t\t|                                                                         |\n");
	Text_Animation("\t\t\t\t|                                                                         |\n");
	Text_Animation("\t\t\t\t|                                                                         |\n");
	Text_Animation("\t\t\t\t|   ~~~~~~ Project : Preemptive Scheduling for Packet Processing ~~~~~~   |\n");
	Text_Animation("\t\t\t\t|                                                                         |\n");
	Text_Animation("\t\t\t\t|_________________________________________________________________________|\n");
	Text_Animation("\t\t\t\t|                                                                         |\n");
	Text_Animation("\t\t\t\t|                                                                         |\n");
	Text_Animation("\t\t\t\t|                     Prepared By : Jatin Kesnani                         |\n");
	Text_Animation("\t\t\t\t|                                                                         |\n");
	Text_Animation("\t\t\t\t|_________________________________________________________________________|\n");
	cout<<"\n\t\t\t\t\t";
	
	cout << RESET;
	cout << "\n---------------------------------------------------------\n";
	cout << "|\tSELECT PREEMPTIVE SCHEDULING ALGORITHM\t\t|\n";
	cout << "---------------------------------------------------------\n";
	cout << "|\t\t[1]	SRTF\t\t\t\t|\n";
	cout << "|\t\t[2]	PRIORITY\t\t\t|\n";
	cout << "|\t\t[3]	LRTF\t\t\t\t|\n";
	cout << "|\t\t[4]	ROUND ROBIN\t\t\t|\n";
	cout << "|-------------------------------------------------------|\n";
	cout << "\n\t\tCHOOSE: ";
	cin >> x;
	
	while(x < 1 || x > 4)
	{
		cout << "\t\tInvalid input!\n";
		cout << "\t\tCHOOSE AGAIN: ";
		cin >> x;
	}
	
	if(x == 1)
	{
		cout << "\nEnter the Number of Packets: ";
	    cin >> n;
	    sleepcount = n;
	    sem_init(&mutex, 0, 1);
	    sem_init(&completed, 0, 0);
	
	    for (int i = 0; i < n; i++)
		{
	        Packet p;
	        p.pid = i + 1;
	        PID[i] = p.pid;
	        cout << "Enter Arrival Time and Processing Time for Packet " << i + 1 << ": ";
	        cin >> p.arrival_time >> p.burst_time;
	        p.remaining_time = p.burst_time;
	        PAT[i] = p.arrival_time;
	        PBT[i] = p.burst_time;
	        RT[i] = p.remaining_time;
	        packets.push_back(p);
	    }
	    
	    cout<<endl;
	    cout<<"----------------------------------"<<endl;
		cout<<"| Packet | Arrival | Processsing |"<<endl;
		cout<<"|   ID   |  Time   |    Time     |"<<endl;
		cout<<"----------------------------------"<<endl;
		for(int i=0 ; i<n ; i++){
			if(packets[i].pid >= 10){
				cout<<"|   P"<<packets[i].pid<<"  |"<<setw(5)<<packets[i].arrival_time<<"    |"<<setw(7)<<packets[i].burst_time<<"      |"<<endl;
			}
			else{
	    		cout<<"|   P"<<packets[i].pid<<"   |"<<setw(5)<<packets[i].arrival_time<<"    |"<<setw(7)<<packets[i].burst_time<<"      |"<<endl;
			}
	    }
	    cout<<"----------------------------------"<<endl;
	    
	    pthread_t thread_id[n];
	    cout << endl;
	    for (int i = 0; i < n; i++)
		{
	        pthread_create(&thread_id[i], NULL, srtf_algorithm, (void*) &packets[i]);
	    }
	
	    int completed_processes = 0;
	    while (completed_processes < n)
		{
	        sem_wait(&completed);
	        completed_processes++;
	    }
	    
	    sem_destroy(&mutex);
	    sem_destroy(&completed);
	    
	    cout<<endl;
	    cout<<"--------------------------------------------------------------------"<<endl;
		cout<<"| Packet | Arrival | Processing | Finishing | Turnaround | Waiting |"<<endl;
		cout<<"|   ID   |  Time   |    Time    |   Time    |    Time    |  Time   |"<<endl;
		cout<<"--------------------------------------------------------------------"<<endl;
	    for (int i = 0; i < n; i++) {
			if(packets[i].pid >= 10){
				cout<<"|   P"<<packets[i].pid<<"  |"<<setw(5)<<packets[i].arrival_time<<"    |"<<setw(7)<<packets[i].burst_time<<"     |"<<setw(6)<<packets[i].completion_time<<"     |"<<setw(7)<<packets[i].turnaround_time<<"     |"<<setw(5)<<packets[i].waiting_time<<"    |"<<endl;
			}
			else{
				cout<<"|   P"<<packets[i].pid<<"   |"<<setw(5)<<packets[i].arrival_time<<"    |"<<setw(7)<<packets[i].burst_time<<"     |"<<setw(6)<<packets[i].completion_time<<"     |"<<setw(7)<<packets[i].turnaround_time<<"     |"<<setw(5)<<packets[i].waiting_time<<"    |"<<endl;
			}
	        total_waiting_time += packets[i].waiting_time;
	        total_turnaround_time += packets[i].turnaround_time;
	    }
		cout<<"--------------------------------------------------------------------"<<endl;
		
	    avg_waiting_time = total_waiting_time / n;
	    avg_turnaround_time = total_turnaround_time / n;
	    
	    cout<<endl;
	    cout << "Average Waiting Time    : " << avg_waiting_time << endl;
	    cout << "Average Turnaround Time : " << avg_turnaround_time << endl;
	    
	    SRTFGanttChart();
	    
	    for(int i=0 ; i<n-1; i++){
	    	for(int j=i+1 ; j<n; j++){
				if(packets[j].arrival_time < packets[i].arrival_time){
					Packet Temp = packets[i];
					packets[i] = packets[j];
					packets[j] = Temp;
				}
				else if(packets[j].arrival_time == packets[i].arrival_time){
					if(packets[j].pid < packets[i].pid){
						Packet Temp = packets[i];
						packets[i] = packets[j];
						packets[j] = Temp;
					}
				}
			}
		}
		
	    cout<<endl;
		cout<<"\t\t--> GANTT Chart <--\n ";
		for(int i=0 ; i<l ; i++){
			if(GANTTpid[i] >= 10 || i==l-1){
				cout<<"------";
			}
			else{
				cout<<"-----";
			}
		}
		cout<<endl;
		cout<<" |";
		for(int i=0 ; i<l ; i++){
			if(GANTTpid[i] == -1){
				cout<<" ~~ |";
			}
			else{
				cout<<" P"<<GANTTpid[i]<<" |";
			}
		}
		cout<<endl;
		cout<<" ";
		for(int i=0 ; i<l ; i++){
			if(GANTTpid[i] >= 10 || i==l-1){
				cout<<"------";
			}
			else{
				cout<<"-----";
			}
		}
		cout<<endl;
		for(int i=0 ; i<k ; i++){
			if(i==0){
				cout<<" "<<GANTTindex[i];
			}
			else if(GANTTpid[i-1] >= 10){
				cout<<setw(6)<<GANTTindex[i];
			}
			else{
				cout<<setw(5)<<GANTTindex[i];
			}
		}
		cout<<endl;
	}
	if(x == 2)
	{
		cout << "\nEnter the Number of Packets: ";
	    cin >> n;
	    sleepcount = n;
	    sem_init(&mutex, 0, 1);
	    sem_init(&completed, 0, 0);
	
	    for (int i = 0; i < n; i++)
	    {
	        Packet p;
	        p.pid = i + 1;
	        cout << "Enter Arrival Time, Processing Time and Priority for Packet " << i + 1 << ": ";
	        cin >> p.arrival_time >> p.burst_time >> p.priority;
	        p.remaining_time = p.burst_time;
	        packets.push_back(p);
	    }
	    
	    cout<<endl;
	    cout<<"--------------------------------------------"<<endl;
		cout<<"| Packet | Arrival | Processing |  Packet  |"<<endl;
		cout<<"|   ID   |  Time   |    Time    | Priority |"<<endl;
		cout<<"--------------------------------------------"<<endl;
		for(int i=0 ; i<n ; i++){
			if(packets[i].pid >= 10){
				cout<<"|   P"<<packets[i].pid<<"  |"<<setw(5)<<packets[i].arrival_time<<"    |"<<setw(7)<<packets[i].burst_time<<"     |"<<setw(6)<<packets[i].priority<<"    |"<<endl;
			}
			else{
	    		cout<<"|   P"<<packets[i].pid<<"   |"<<setw(5)<<packets[i].arrival_time<<"    |"<<setw(7)<<packets[i].burst_time<<"     |"<<setw(6)<<packets[i].priority<<"    |"<<endl;
			}
	    }
	    cout<<"--------------------------------------------"<<endl;
	    
	    
	    cout << endl;
	    GANTTindex[k] = current_time;
		k++;
	    pthread_t thread_id[n];
	    
	    for (int i = 0; i < n; i++)
	    {
	        pthread_create(&thread_id[i], NULL, priority_algorithm, (void*) &packets[i]);
	    }
	
	    int completed_processes = 0;
	    while (completed_processes < n)
	    {
	        sem_wait(&completed);
	        completed_processes++;
	    }
	
	    sem_destroy(&mutex);
	    sem_destroy(&completed);
	    cout<<endl;
	    cout<<"-------------------------------------------------------------------------------"<<endl;
		cout<<"| Packet | Arrival | Processing |  Packet  | Finishing | Turnaround | Waiting |"<<endl;
		cout<<"|   ID   |  Time   |    Time    | Priority |   Time    |    Time    |  Time   |"<<endl;
		cout<<"-------------------------------------------------------------------------------"<<endl;
	    for (int i = 0; i < n; i++) {
			if(packets[i].pid >= 10){
				cout<<"|   P"<<packets[i].pid<<"  |"<<setw(5)<<packets[i].arrival_time<<"    |"<<setw(7)<<packets[i].burst_time<<"     |"<<setw(5)<<packets[i].priority<<"     |"<<setw(6)<<packets[i].completion_time<<"     |"<<setw(7)<<packets[i].turnaround_time<<"     |"<<setw(5)<<packets[i].waiting_time<<"    |"<<endl;
			}
			else{
				cout<<"|   P"<<packets[i].pid<<"   |"<<setw(5)<<packets[i].arrival_time<<"    |"<<setw(7)<<packets[i].burst_time<<"     |"<<setw(5)<<packets[i].priority<<"     |"<<setw(6)<<packets[i].completion_time<<"     |"<<setw(7)<<packets[i].turnaround_time<<"     |"<<setw(5)<<packets[i].waiting_time<<"    |"<<endl;
			}
	        total_waiting_time += packets[i].waiting_time;
	        total_turnaround_time += packets[i].turnaround_time;
	    }
		cout<<"-------------------------------------------------------------------------------"<<endl;
		
	    avg_waiting_time = total_waiting_time / n;
	    avg_turnaround_time = total_turnaround_time / n;
	    
	    cout<<endl;
	    cout << "Average Waiting Time    : " << avg_waiting_time << endl;
	    cout << "Average Turnaround Time : " << avg_turnaround_time << endl;
	    
	    cout<<endl;
		cout<<"\t\t--> GANTT Chart <--\n ";
		for(int i=0 ; i<l ; i++){
			if(GANTTpid[i] >= 10 || i==l-1){
				cout<<"------";
			}
			else{
				cout<<"-----";
			}
		}
		cout<<endl;
		cout<<" |";
		for(int i=0 ; i<l ; i++){
			if(GANTTpid[i] == -1){
				cout<<" ~~ |";
			}
			else{
				cout<<" P"<<GANTTpid[i]<<" |";
			}
		}
		cout<<endl;
		cout<<" ";
		for(int i=0 ; i<l ; i++){
			if(GANTTpid[i] >= 10 || i==l-1){
				cout<<"------";
			}
			else{
				cout<<"-----";
			}
		}
		cout<<endl;
		for(int i=0 ; i<k ; i++){
			if(i==0){
				cout<<" "<<GANTTindex[i];
			}
			else if(GANTTpid[i-1] >= 10){
				cout<<setw(6)<<GANTTindex[i];
			}
			else{
				cout<<setw(5)<<GANTTindex[i];
			}
		}
		cout<<endl;
	}
	if(x == 3)
	{
		cout << "\nEnter the Number of Packets: ";
	    cin >> n;
	    sleepcount = n;
	    sem_init(&mutex, 0, 1);
	    sem_init(&completed, 0, 0);
	
	    for (int i = 0; i < n; i++)
		{
	        Packet p;
	        p.pid = i + 1;
	        PID[i] = p.pid;
	        cout << "Enter Arrival Time and Processing Time for Packet " << i + 1 << ": ";
	        cin >> p.arrival_time >> p.burst_time;
	        p.remaining_time = p.burst_time;
	        PAT[i] = p.arrival_time;
	        PBT[i] = p.burst_time;
	        RT[i] = p.remaining_time;
	        packets.push_back(p);
		}
	    
	    cout<<endl;
	    cout<<"---------------------------------"<<endl;
		cout<<"| Packet | Arrival | Processing |"<<endl;
		cout<<"|   ID   |  Time   |    Time    |"<<endl;
		cout<<"---------------------------------"<<endl;
		for(int i=0 ; i<n ; i++){
			if(packets[i].pid >= 10){
				cout<<"|   P"<<packets[i].pid<<"  |"<<setw(5)<<packets[i].arrival_time<<"    |"<<setw(7)<<packets[i].burst_time<<"     |"<<endl;
			}
			else{
	    		cout<<"|   P"<<packets[i].pid<<"   |"<<setw(5)<<packets[i].arrival_time<<"    |"<<setw(7)<<packets[i].burst_time<<"     |"<<endl;
			}
	    }
	    cout<<"---------------------------------"<<endl;
	    
	   
	    pthread_t thread_id[n];
	    cout << endl;
	    for (int i = 0; i < n; i++)
		{
	        pthread_create(&thread_id[i], NULL, lrtf_algorithm, (void*) &packets[i]);
	    }
	
	    int completed_processes = 0;
	    while (completed_processes < n)
		{
	        sem_wait(&completed);
	        completed_processes++;
	    }
	    
	    sem_destroy(&mutex);
	    sem_destroy(&completed);
	    
	    cout<<endl;
	    cout<<"--------------------------------------------------------------------"<<endl;
		cout<<"| Packet | Arrival | Processing | Finishing | Turnaround | Waiting |"<<endl;
		cout<<"|   ID   |  Time   |    Time    |   Time    |    Time    |  Time   |"<<endl;
		cout<<"--------------------------------------------------------------------"<<endl;
	    for (int i = 0; i < n; i++) {
			if(packets[i].pid >= 10){
				cout<<"|   P"<<packets[i].pid<<"  |"<<setw(5)<<packets[i].arrival_time<<"    |"<<setw(7)<<packets[i].burst_time<<"     |"<<setw(6)<<packets[i].completion_time<<"     |"<<setw(7)<<packets[i].turnaround_time<<"     |"<<setw(5)<<packets[i].waiting_time<<"    |"<<endl;
			}
			else{
				cout<<"|   P"<<packets[i].pid<<"   |"<<setw(5)<<packets[i].arrival_time<<"    |"<<setw(7)<<packets[i].burst_time<<"     |"<<setw(6)<<packets[i].completion_time<<"     |"<<setw(7)<<packets[i].turnaround_time<<"     |"<<setw(5)<<packets[i].waiting_time<<"    |"<<endl;
			}
	        total_waiting_time += packets[i].waiting_time;
	        total_turnaround_time += packets[i].turnaround_time;
	    }
		cout<<"--------------------------------------------------------------------"<<endl;
		
	    avg_waiting_time = total_waiting_time / n;
	    avg_turnaround_time = total_turnaround_time / n;
	    
	    cout<<endl;
	    cout << "Average Waiting Time    : " << avg_waiting_time << endl;
	    cout << "Average Turnaround Time : " << avg_turnaround_time << endl;
	    
	    LRTFGanttChart();
	    
	    for(int i=0 ; i<n-1; i++){
	    	for(int j=i+1 ; j<n; j++){
				if(packets[j].arrival_time < packets[i].arrival_time){
					Packet Temp = packets[i];
					packets[i] = packets[j];
					packets[j] = Temp;
				}
				else if(packets[j].arrival_time == packets[i].arrival_time){
					if(packets[j].pid < packets[i].pid){
						Packet Temp = packets[i];
						packets[i] = packets[j];
						packets[j] = Temp;
					}
				}
			}
		}
		
	    cout<<endl;
		cout<<"\t\t--> GANTT Chart <--\n ";
		for(int i=0 ; i<l ; i++){
			if(GANTTpid[i] >= 10 || i==l-1){
				cout<<"------";
			}
			else{
				cout<<"-----";
			}
		}
		cout<<endl;
		cout<<" |";
		for(int i=0 ; i<l ; i++){
			if(GANTTpid[i] == -1){
				cout<<" ~~ |";
			}
			else{
				cout<<" P"<<GANTTpid[i]<<" |";
			}
		}
		cout<<endl;
		cout<<" ";
		for(int i=0 ; i<l ; i++){
			if(GANTTpid[i] >= 10 || i==l-1){
				cout<<"------";
			}
			else{
				cout<<"-----";
			}
		}
		cout<<endl;
		for(int i=0 ; i<k ; i++){
			if(i==0){
				cout<<" "<<GANTTindex[i];
			}
			else if(GANTTpid[i-1] >= 10){
				cout<<setw(6)<<GANTTindex[i];
			}
			else{
				cout<<setw(5)<<GANTTindex[i];
			}
		}
		cout<<endl;
	}
    if(x == 4){
    	cout << "Enter the Number of Packets: ";
	    cin >> n;
		cout << "Enter Time Quantum: ";
	    cin >> TimeQuantum;
	    sleepcount = n;
	    sem_init(&mutex, 0, 1);
	    sem_init(&completed, 0, 0);
	    
	    
		for (int i = 0; i < n; i++) {
	        Packet p;
	        p.pid = i + 1;
	        cout << "Enter Arrival Time and Processing Time for Packet " << i + 1 << ": ";
	        cin >> p.arrival_time >> p.burst_time;
	        p.remaining_time = p.burst_time;
	        packets.push_back(p);
	        visited[i] = false;
	    }
	    
	    cout<<endl;
	    cout<<"---------------------------------"<<endl;
		cout<<"| Packet | Arrival | Processing |"<<endl;
		cout<<"|   ID   |  Time   |    Time    |"<<endl;
		cout<<"---------------------------------"<<endl;
		for(int i=0 ; i<n ; i++){
			if(packets[i].pid >= 10){
				cout<<"|   P"<<packets[i].pid<<"  |"<<setw(5)<<packets[i].arrival_time<<"    |"<<setw(7)<<packets[i].burst_time<<"     |"<<endl;
			}
			else{
	    		cout<<"|   P"<<packets[i].pid<<"   |"<<setw(5)<<packets[i].arrival_time<<"    |"<<setw(7)<<packets[i].burst_time<<"     |"<<endl;
			}
	    }
	    cout<<"---------------------------------"<<endl;
	    
	    for(int i=0 ; i<n-1; i++){
	    	for(int j=i+1 ; j<n; j++){
				if(packets[j].arrival_time < packets[i].arrival_time){
					Packet Temp = packets[i];
					packets[i] = packets[j];
					packets[j] = Temp;
				}
				else if(packets[j].arrival_time == packets[i].arrival_time){
					if(packets[j].pid < packets[i].pid){
						Packet Temp = packets[i];
						packets[i] = packets[j];
						packets[j] = Temp;
					}
				}
			}
		}
	   	queue[0] = 0;
	   	visited[0] = true;
	   	GANTTindex[k] = current_time;
		k++;
	    pthread_t thread_id[n];
	    for (int i = 0; i < n; i++) {
	        pthread_create(&thread_id[i], NULL, rr_algorithm, (void*) &packets[i]);
	    }
	
	    int completed_processes = 0;
	    while (completed_processes < n) {
	        sem_wait(&completed);
	        completed_processes++;
	    }
	
	    sem_destroy(&mutex);
	    sem_destroy(&completed);
	    
		cout<<endl;
	    cout<<"--------------------------------------------------------------------"<<endl;
		cout<<"| Packet | Arrival | Processing | Finishing | Turnaround | Waiting |"<<endl;
		cout<<"|   ID   |  Time   |    Time    |   Time    |    Time    |  Time   |"<<endl;
		cout<<"--------------------------------------------------------------------"<<endl;
	    for (int i = 0; i < n; i++) {
			if(packets[i].pid >= 10){
				cout<<"|   P"<<packets[i].pid<<"  |"<<setw(5)<<packets[i].arrival_time<<"    |"<<setw(7)<<packets[i].burst_time<<"     |"<<setw(6)<<packets[i].completion_time<<"     |"<<setw(7)<<packets[i].turnaround_time<<"     |"<<setw(5)<<packets[i].waiting_time<<"    |"<<endl;
			}
			else{
				cout<<"|   P"<<packets[i].pid<<"   |"<<setw(5)<<packets[i].arrival_time<<"    |"<<setw(7)<<packets[i].burst_time<<"     |"<<setw(6)<<packets[i].completion_time<<"     |"<<setw(7)<<packets[i].turnaround_time<<"     |"<<setw(5)<<packets[i].waiting_time<<"    |"<<endl;
			}
	        total_waiting_time += packets[i].waiting_time;
	        total_turnaround_time += packets[i].turnaround_time;
	    }
		cout<<"--------------------------------------------------------------------"<<endl;
		
	    avg_waiting_time = total_waiting_time / n;
	    avg_turnaround_time = total_turnaround_time / n;
	    
	    cout<<endl;
	    cout << "Average Waiting Time    : " << avg_waiting_time << endl;
	    cout << "Average Turnaround Time : " << avg_turnaround_time << endl;
		
		cout<<endl;
		cout<<"\t\t--> GANTT Chart <--\n ";
		for(int i=0 ; i<l ; i++){
			if(GANTTpid[i] >= 10 || i==l-1){
				cout<<"------";
			}
			else{
				cout<<"-----";
			}
		}
		cout<<endl;
		cout<<" |";
		for(int i=0 ; i<l ; i++){
			if(GANTTpid[i] == -1){
				cout<<" ~~ |";
			}
			else{
				cout<<" P"<<GANTTpid[i]<<" |";
			}
		}
		cout<<endl;
		cout<<" ";
		for(int i=0 ; i<l ; i++){
			if(GANTTpid[i] >= 10 || i==l-1){
				cout<<"------";
			}
			else{
				cout<<"-----";
			}
		}
		cout<<endl;
		for(int i=0 ; i<k ; i++){
			if(i==0){
				cout<<" "<<GANTTindex[i];
			}
			else if(GANTTpid[i-1] >= 10){
				cout<<setw(6)<<GANTTindex[i];
			}
			else{
				cout<<setw(5)<<GANTTindex[i];
			}
		}
		cout<<endl;
	}
	cout<<endl;
    return 0;
}

