#include "text.h"
#include "common.h"
#include "main.h"
#include "porter.h"
#include <mpi.h>

using namespace std;
using namespace cv;

/*GLOBAL VARIABLES*/
queue<QUERY_QUEUE_STRUCT>Incomming_Query_Queue;
set<RESPONSE_ARR_STRUCT>Response_Array;
pthread_mutex_t mutex_incoming_queue = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_response_array = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_machine_status = PTHREAD_MUTEX_INITIALIZER;
MACHINE_STATUS machines[N_MACHINE];
int RANK; //my rank.
 pthread_t Tid_Thread_Accept_Query;
 pthread_t Tid_Thread_Wait_Job_From_Master;
 pthread_mutex_t lock_thread_net; 



void log_status(MACHINE_STATUS* status) {
    char buf[3];
    for (int i = 0; i < N_MACHINE; i++) {
        sprintf(buf,"%d",i+1);
        FILE* fp = fopen((string("/home/dihong/AWS-site/log/machine_status_")+buf+".txt").c_str(),"w+");
        if(fp){
            fprintf(fp,"Pending Jobs:%.1f\nNetwork Speed:%.2f\nJob Speed:%.2f",status[i].n_pending_jobs,status[i].network_speed,status[i].jobs_per_second);
            fclose(fp);
        }
    }
}
/****************************************************************************************************/
/*Threads*/
void* Thread_Accept_Query(void* params){
    /*This thread accepts queries from php via TCP, and insert the queries into incoming queue.*/
    int sockfd, newsockfd, portno;
    char buffer[11000]; //200x200 image.
    struct sockaddr_in serv_addr, cli_addr;
    int  n,sz_text,sz_image;
    /* First call to socket() function */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        perror("[Error] Thread_Accept_Query: opening socket");
        exit(1);
    }
    /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 0;//59886;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
 
    /* Now bind the host address using bind() call.*/
    int option = 1;
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
         MPI_Finalize();
         perror("[Error] Thread_Accept_Query: binding");
         exit(1);
    }
    if(setsockopt(sockfd,SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(char*)&option,sizeof(option)) < 0)
    {
        printf("setsockopt failed\n");
        close(sockfd);
        MPI_Finalize(); exit(1);
    }
    listen(sockfd,10); //at most 10 connections in the waiting queue.
    socklen_t clilen = sizeof(cli_addr);
    printf("[%d] Thread_Accept_Query starts up. Now waiting for connections...\n",RANK);
    while (1) {
        /* Accept actual connection from the client */
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); //block
        if (newsockfd < 0) {
            perror("[Error] Thread_Accept_Query: accept");
            continue;
        }
        else{
            //printf("Accepted incoming connection from: %s\n",inet_ntoa(cli_addr.sin_addr));
        }
        /*If there is not enough buffer in the queue, reject it*/
        pthread_mutex_lock(&mutex_incoming_queue);
        if(Incomming_Query_Queue.size()>=MAX_PENDING_QUERY){
            pthread_mutex_unlock( &mutex_incoming_queue);
            vislog("The system is currently busy, please try again later.\n");
            close(newsockfd);
            continue;
        }
        pthread_mutex_unlock( &mutex_incoming_queue);
        /* If connection is established then start communicating */
        bzero(buffer, sizeof(buffer));
        n = recv(newsockfd, buffer, sizeof(buffer), 0);
        if (n < 0) {
            perror("[Error] Thread_Accept_Query: reading from socket");
            continue;
        }else if(n>=sizeof(buffer)-1){
            vislog("Image size must be no more than 10 kb.\n");
            continue;
        }
        n -= 2;
        //printf("Size of the message: %d\n", n);
        sz_text = strlen(buffer);
        sz_image = n - sz_text - 1;
        //printf("text:%d, image:%d\n",sz_text,sz_image);
        vislog("----------------------------------------\n");
        QUERY_QUEUE_STRUCT str;
        str.sockfd = newsockfd;
        string txt = "Text:  input = "+string(buffer);
        txt += ", stemmed = ";
        if (sz_text > 0) {
            str.text_key_words = preprocess(buffer);
            
            for (int i = 0; i < str.text_key_words.size(); i++)
                txt += str.text_key_words[i]+" ";
            vislog(txt+"\n");
        }
        if (sz_image > 0) {
            CvMat A = Mat(1, n, CV_8UC1, buffer + sz_text + 1);
            IplImage* img_ptr = cvDecodeImage(&A, true);
            str.channels = img_ptr->nChannels;
            str.image = (uchar*) img_ptr->imageData;
            str.height = img_ptr->height;
            str.width = img_ptr->width;
            str.widthStep = img_ptr->widthStep;
            char buf[40];
            sprintf(buf,"Image: height = %d, width = %d\n",str.height,str.width);
            vislog(buf);
            //cvShowImage("Win", img_ptr);
            //cvWaitKey(0);
        } else
            str.image = 0; //no image.
        str.id = get_unique_query_id();
        str.ts_enqueue = tic();
        pthread_mutex_lock( &mutex_incoming_queue );
        Incomming_Query_Queue.push(str);
        pthread_mutex_unlock( &mutex_incoming_queue );
        /* Write a response to the client */
        /*n = send(newsockfd, "I got your message", 18, 0);
        if (n < 0) {
            perror("[Error] Thread_Accept_Query: writing to socket");
            MPI_Finalize(); exit(1);
        }
         close(newsockfd);*/
    }
    return 0;
}

void* Thread_Scheduler(void* params){
    int sort_by_pending_jobs_val[N_MACHINE];
    int sort_by_pending_jobs_ind[N_MACHINE];
    QUERY_QUEUE_STRUCT query;
    RESPONSE_ARR_STRUCT response;
    MACHINE_STATUS status_snapshot[N_MACHINE];
    int i,j,pkt_size;
    char logtxt[200];
    float best_assignment_cost, best_nb_machines, new_cost, max_network_speed;
    char buffer [MAX_MPI_MSG_SIZE]; //used to send query to worker machines.
    //debug
    pthread_mutex_lock(&mutex_machine_status);
    log_status(machines);
    pthread_mutex_unlock(&mutex_machine_status);
    MPI_Request	send_request[N_MACHINE];
    MPI_Status Stat[N_MACHINE];
    while (1) {
        pthread_mutex_lock(&mutex_incoming_queue);
        if (Incomming_Query_Queue.size() == 0) {  //no query currently.
            pthread_mutex_unlock(&mutex_incoming_queue);
            usleep(1000);
            continue;
        }
        pthread_mutex_unlock( &mutex_incoming_queue);
        /*Determine whether and how to take care of the next query*/
        pthread_mutex_lock(&mutex_machine_status);
        memcpy(status_snapshot,machines,sizeof(MACHINE_STATUS)*N_MACHINE);
        pthread_mutex_unlock(&mutex_machine_status);
        for(i=0;i<N_MACHINE;i++)
            sort_by_pending_jobs_val[i] = status_snapshot[i].n_pending_jobs;
        quick_sort(sort_by_pending_jobs_val,N_MACHINE,sort_by_pending_jobs_ind,sort_by_pending_jobs_val,false); //sort ascending.
        best_assignment_cost = (1.0+status_snapshot[sort_by_pending_jobs_ind[0]].n_pending_jobs)/(status_snapshot[sort_by_pending_jobs_ind[0]].jobs_per_second);
        best_nb_machines = 1;
        max_network_speed = status_snapshot[0].network_speed;
        for(i=2;i<=N_MACHINE;i++){
            new_cost = (1.0/i+status_snapshot[sort_by_pending_jobs_ind[0]].n_pending_jobs)/(status_snapshot[sort_by_pending_jobs_ind[0]].jobs_per_second);
            for(j =1;j<i;j++)
                if((1+status_snapshot[sort_by_pending_jobs_ind[j]].n_pending_jobs)/(status_snapshot[sort_by_pending_jobs_ind[j]].jobs_per_second)>new_cost)
                    new_cost = (1.0/i+status_snapshot[sort_by_pending_jobs_ind[j]].n_pending_jobs)/(status_snapshot[sort_by_pending_jobs_ind[j]].jobs_per_second);
            if(new_cost<best_assignment_cost){
                best_assignment_cost = new_cost;
                best_nb_machines = i;
            }
            if(status_snapshot[i].network_speed>max_network_speed)
                max_network_speed = status_snapshot[i].network_speed;
        }
        if(best_assignment_cost>5.0/max_network_speed){
            continue; //the machines are busy currently: not to take the next query.
        }
        /*assign the next query to worker machines*/
        pthread_mutex_lock(&mutex_incoming_queue);
        query = Incomming_Query_Queue.front(); //take the first element in the queue.
        Incomming_Query_Queue.pop(); //remove the first element.
        pthread_mutex_unlock( &mutex_incoming_queue);
        response.id = query.id;
        response.num_machines = best_nb_machines;
        response.ts_enqueue = query.ts_enqueue;
        if(!Encode_MPI_Query_Packet(query, best_nb_machines, buffer, MAX_MPI_MSG_SIZE, pkt_size)){
            response.errtxt = "Encode_MPI_Packet failed.";
        }else{
            sprintf(logtxt,"Query id = %d has been assigned to machine(s):",query.id);
            string txt = logtxt;
            for(i=0;i<best_nb_machines;i++){
                pkt_size = 100; //debug
                MPI_Isend(buffer, pkt_size, MPI_CHAR, status_snapshot[sort_by_pending_jobs_ind[i]].rank, TAG_MPI_JOB, MPI_COMM_WORLD,send_request+i);
                pthread_mutex_lock(&mutex_machine_status);
                machines[sort_by_pending_jobs_ind[i]].n_pending_jobs++;
                pthread_mutex_unlock(&mutex_machine_status);
                sprintf(logtxt, " %d", status_snapshot[sort_by_pending_jobs_ind[i]].rank);
                txt += logtxt;
            }
            txt += "\n";
            vislog(txt);
            response.ts_schuduled = tic();
            MPI_Waitall(best_nb_machines, send_request, Stat); //wait all messages sending done.
        }
        pthread_mutex_lock(&mutex_response_array);
        Response_Array.insert(response);
        pthread_mutex_unlock(&mutex_response_array);
        //debug
        pthread_mutex_lock(&mutex_machine_status);
        log_status(machines);
        pthread_mutex_unlock(&mutex_machine_status);
    }
}
void* Thread_Response_Collector(void* params) {
    int i, dataWaitingFlag, pkt_size,qid;
    MPI_Request	recv_request;
    MPI_Status Stat;
    RESPONSE_ARR_STRUCT response, * ptr_response;
    set<RESPONSE_ARR_STRUCT>::iterator it_response;
    char logtxt[200];
    char buffer [MAX_MPI_MSG_SIZE]; //used to send query to worker machines.
    vector<int> item_id;
    vector<float> item_score;
    double time_elapsed, rtt, processing_time;
    clock_t ts;
    while (1) {
        for (i = 0; i < N_MACHINE; i++) { //check whether there is any message from peers.
            MPI_Iprobe(i, TAG_MPI_RESPONSE, MPI_COMM_WORLD, &dataWaitingFlag, MPI_STATUS_IGNORE);
            if (dataWaitingFlag) { //Receive a packet
                ts = tic();
                /*Decode the packet*/
                MPI_Irecv(buffer, MAX_MPI_MSG_SIZE, MPI_CHAR, i, TAG_MPI_RESPONSE, MPI_COMM_WORLD, &recv_request);
                MPI_Wait(&recv_request,&Stat); //wait until recv done.
                MPI_Get_count(&Stat, MPI_CHAR, &pkt_size);
                item_id.clear();
                item_score.clear();
                if(!Decode_MPI_Response_Packet(item_id, item_score, time_elapsed, processing_time, buffer, MAX_MPI_MSG_SIZE, pkt_size, qid)) {
                    continue;
                }
                /*Look up the corresponding response*/
                response.id = qid;
                pthread_mutex_lock(&mutex_response_array);
                it_response = Response_Array.find(response);
                pthread_mutex_unlock(&mutex_response_array);
                if(it_response==Response_Array.end()) {
                    printf("[Error]: cannot find response in Response_Array.");
                    MPI_Finalize(); exit(-1);
                }
                /*Update response*/
                it_response->num_results++;
                it_response->ts_finished = toc();
                if (it_response->num_results == it_response->num_machines) {//done: create a thread to merge the result and return to the web user.
                    pthread_t Tid_Thread_Merge_Score;
                    ptr_response = new RESPONSE_ARR_STRUCT;
                    *ptr_response = *it_response;
                    Response_Array.erase (it_response);
                    if (pthread_create(&Tid_Thread_Merge_Score, NULL, &Thread_Merge_Score, ptr_response) != 0) {
                        printf("[Error] System_Initialization: fail to create Thread_Merge_Score thread.");
                    }
                }
                rtt = toc(it_response->ts_schuduled) - toc(ts) - time_elapsed; //round trip time [transmission].
                pthread_mutex_lock(&mutex_machine_status);
                machines[Stat.MPI_SOURCE].network_speed = 0.7 * machines[Stat.MPI_SOURCE].network_speed + 0.3 * 2.0 / rtt;
                machines[Stat.MPI_SOURCE].n_pending_jobs -= 1.0 / it_response->num_machines;
                machines[Stat.MPI_SOURCE].jobs_per_second = 1.0 / it_response->num_machines / (processing_time);
                pthread_mutex_unlock(&mutex_machine_status);
            }
        }
    }
}
void* Thread_Merge_Score(void* params){
    RESPONSE_ARR_STRUCT* response = (RESPONSE_ARR_STRUCT*)params;
}

void* Thread_Wait_Job_From_Master(void* params){
    char buffer [MAX_MPI_MSG_SIZE]; //used to receive query from master.
    int part, n_workers, pkt_size;
    QUERY_QUEUE_STRUCT* query = 0;
    MPI_Request	recv_request;
    MPI_Status Stat;
    pthread_t Tid_Thread_Slave_Do_Query;
    clock_t ts;
    printf("[%d] Thread_Wait_Job_From_Master starts up.\n", RANK);
    while (1) {
        MPI_Irecv(buffer, MAX_MPI_MSG_SIZE, MPI_CHAR, MASTER_RANK, TAG_MPI_JOB, MPI_COMM_WORLD, &recv_request); //nonblocking
        MPI_Wait(&recv_request,&Stat); //wait until done.
        ts = tic();
        printf("[%d] received job with id = %d from master.\n",RANK,query->id);
        continue;
        MPI_Get_count(&Stat, MPI_CHAR, &pkt_size);
        query = new QUERY_QUEUE_STRUCT;
        if (!Decode_MPI_Query_Packet(*query, part, n_workers, buffer)) { //decode the packet.
            printf("[%d] Decode_MPI_Query_Packet failed.\n",RANK);
            continue;
        }
        query->ts_recv_by_slave = ts;
        
        /*Create a thread do the query.*/
        if (pthread_create(&Tid_Thread_Slave_Do_Query, NULL, &Thread_Slave_Do_Query, query) != 0) {
            printf("[Error] System_Initialization: fail to create Thread_Wait_Job_From_Master thread.");
            MPI_Finalize(); exit(-1);
        }
    }
}

void* Thread_Slave_Do_Query(void* params) {
    QUERY_QUEUE_STRUCT* query = (QUERY_QUEUE_STRUCT*) params;
    query->ts_begin_process = tic();
    /* DO QUERY HERE
     * 
     *  QUERY
     * 
     * 
     */
    
    query->ts_after_process = tic();
    
    /* SEND RESULT BACK TO MASTER
     * 
     *  SEND RESULT
     * 
     * 
     */
    /*Done*/
}

void* Thread_Slave_Wait_For_Ping(void* params){
	pthread_mutex_lock(&lock_thread_net);
    printf("[%d] Thread_Slave_Wait_For_Ping starts up.\n",RANK);
    char buf[MAX_MPI_MSG_SIZE];
	MPI_Request	send_request;
    MPI_Request	recv_request;
    MPI_Status Stat;
    MPI_Irecv(buf, MAX_MPI_MSG_SIZE, MPI_CHAR, MASTER_RANK, TAG_MPI_PING, MPI_COMM_WORLD, &recv_request);//nonblocking.
    MPI_Wait(&recv_request,&Stat); //wait until done.
    MPI_Isend(buf, 1, MPI_CHAR, MASTER_RANK, TAG_MPI_PING, MPI_COMM_WORLD, &send_request);
    MPI_Wait(&send_request,&Stat); //wait all messages sending done.
	sleep(2);
	pthread_mutex_unlock(&lock_thread_net);
}

/*Procedures*/

bool Encode_MPI_Query_Packet(const QUERY_QUEUE_STRUCT& query, const int& n_workers, char* buffer, const int& buf_size, int& pkt_size){
    return true;
}

bool Decode_MPI_Query_Packet(QUERY_QUEUE_STRUCT& query, int& part, int& n_workers, const char* data){
    return true;
}

bool Encode_MPI_Response_Packet(const vector<int>& item_id, const vector<float>& item_score, const double& time_elapsed,const double& processing_time, char* buffer, const int& buf_size, int&pkt_size, const int& qid){
    return true;
}
bool Decode_MPI_Response_Packet(vector<int>& item_id, vector<float>& item_score, double& time_elapsed, double&processing_time, char* buffer, const int& buf_size, int&pkt_size, int& qid){
    return true;
}

int get_unique_query_id(){
    static int initial_id = 0;
    initial_id = (initial_id+1) % (MAX_PENDING_QUERY*N_MACHINE);
    return initial_id;
}

void System_Initialization_Master(){
    /* THIS FUNCTION INITIALIZES THE MASTER SYSTEM.*/

    /*1) Create a thread to accept incoming query.*/
    FILE* fp = fopen("/home/dihong/AWS-site/log/log.txt","w+");
    if(fp) fclose(fp); //empty log
    if(pthread_create(&Tid_Thread_Accept_Query, NULL, &Thread_Accept_Query, 0)!=0){
        printf("[Error] System_Initialization: fail to create Thread_Accept_Query thread.");
        MPI_Finalize(); exit(-1);
    }
    /*2) Initialize machine status*/
    for(int i = 0;i<N_MACHINE;i++){
        machines[i].n_pending_jobs = 0;
        machines[i].rank = i;
        machines[i].jobs_per_second = 1.0;
        machines[i].network_speed = 1.0;
    }
    /*3) Create a thread to collect responses from worker machines.*/
    pthread_t Tid_Thread_Response_Collector;
    if(pthread_create(&Tid_Thread_Response_Collector, NULL, &Thread_Response_Collector, 0)!=0){
        printf("[Error] System_Initialization: fail to create Thread_Response_Collector thread.");
        MPI_Finalize(); exit(-1);
    }else{
        printf("[%d] Thread_Response_Collector starts up.\n",RANK);
    }
    /*4) Create a thread to schedule queries.*/
    pthread_t Tid_Thread_Scheduler;
    if(pthread_create(&Tid_Thread_Scheduler, NULL, &Thread_Scheduler, 0)!=0){
        printf("[Error] System_Initialization: fail to create Tid_Thread_Scheduler thread.");
        MPI_Finalize(); exit(-1);
    }else{
        printf("[%d] Thread_Scheduler starts up.\n",RANK);
    }
    /*5) Initialize network speed*/
    char buf[MAX_MPI_MSG_SIZE];
    int num_replied = 0, i, dataWaitingFlag;
    MPI_Status Stat[N_MACHINE];
    MPI_Request	send_request[N_MACHINE];
    MPI_Request recv_request;
    for(i=0;i<N_MACHINE;i++){  //send ping signals to slaves.
        MPI_Isend(buf, MAX_MPI_MSG_SIZE, MPI_CHAR, i, TAG_MPI_PING, MPI_COMM_WORLD,send_request+i); //block until message received by destination.
    }
    MPI_Waitall(N_MACHINE, send_request, Stat); //wait all sending done.
	printf("Initialize network speed: All sending done.\n");
    while (num_replied < N_MACHINE) {
        for (i = 0; i < N_MACHINE; i++){
            MPI_Iprobe(i, TAG_MPI_PING, MPI_COMM_WORLD, &dataWaitingFlag, MPI_STATUS_IGNORE);
            if (dataWaitingFlag) { //Receive a feedback
                MPI_Irecv(buf, MAX_MPI_MSG_SIZE, MPI_CHAR, i, TAG_MPI_PING, MPI_COMM_WORLD, &recv_request); //nonblocking.
                MPI_Wait(&recv_request,Stat); //wait until recv done.
                printf("Ping replied from %d\n",Stat[0].MPI_SOURCE);
                num_replied++;
            }
        }
    }
    printf("[%d] Network initialization down.\n",RANK);
}

void System_Initialization_Slave(){
    /* THIS FUNCTION INITIALIZES THE SLAVE SYSTEM.*/
	/*0) Mutex initialization*/
	if (pthread_mutex_init(&lock_thread_net, NULL) != 0)
	{
	        printf("\n mutex init failed\n");
	        exit(1);
	}
    /*1) Wait for ping signals from master*/
    pthread_t Tid_Thread_Slave_Wait_For_Ping;
    if(pthread_create(&Tid_Thread_Slave_Wait_For_Ping, NULL, &Thread_Slave_Wait_For_Ping, 0)!=0){
        printf("[Error] System_Initialization: fail to create Thread_Slave_Wait_For_Ping thread.");
        MPI_Finalize(); exit(-1);
    }
    /*2) Create a thread to wait for job from master.*/
    if(pthread_create(&Tid_Thread_Wait_Job_From_Master, NULL, &Thread_Wait_Job_From_Master, 0)!=0){
        printf("[Error] System_Initialization: fail to create Thread_Wait_Job_From_Master thread.");
        MPI_Finalize(); exit(-1);
    }
}

int main(int argc, char *argv[]) {
    /*MPI initialization.*/
	
    int num_processors;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processors);
    if (num_processors != N_MACHINE+1) {
        printf("[Error] num_processors!=N_MACHINE %d:%d\n",num_processors,N_MACHINE);
        MPI_Finalize();
        return -1;
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &RANK);
    printf("Enter main. My rank is: %d\n",RANK);
	MPI_Finalize();
    /*if(RANK==MASTER_RANK){  //master
        System_Initialization_Master();
        pthread_join(Tid_Thread_Accept_Query, NULL);
    }else{
        System_Initialization_Slave();
        pthread_join(Tid_Thread_Wait_Job_From_Master, NULL);
    }
    printf("[%d] is now exiting.\n",RANK);
    fflush(stdout);
    MPI_Finalize();*/
}
