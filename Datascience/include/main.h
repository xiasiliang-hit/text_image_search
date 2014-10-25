#ifndef MAIN_H
#include "stdio.h"
#include "parallel.h"
#include <stdio.h>
#include<fstream>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <queue>
#include <string>
#include <vector>
#include <set>
#include<opencv2/highgui/highgui.hpp>
#include<opencv/cv.h>
#include <unistd.h>


#define N_MACHINE 3
#define MASTER_RANK 3
#define MAX_PENDING_QUERY 100 //the maximum number of pending queries allowed in the Incomming_Query_Queue
#define MAX_MPI_MSG_SIZE 1000000 //1MB
#define TAG_MPI_JOB 1
#define TAG_MPI_RESPONSE 2
#define TAG_MPI_PING 3

typedef unsigned char uchar;
typedef struct QUERY_QUEUE_STRUCT{
    int id; //unique id of the query.
    uchar* image; //image[i*widthStep+j*channels+k] -> k-th channel of pixel (i,j)
    int height; //height of image
    int width; //width of image
    int widthStep; //width step.
    int channels; //number of channels, 1 for grayscale image, 3 for color image.
    std::vector<std::string> text_key_words;
    int sockfd; //the socket connection corresponding to the query.
    clock_t ts_enqueue; //time stamp entering the queue.
    clock_t ts_recv_by_slave; //time stamp received by processing.
    clock_t ts_begin_process; //time stamp at the beginning of processing.
    clock_t ts_after_process; //time stamp after processing.
}QUERY_QUEUE_STRUCT;

typedef struct MERGED_SCORE_STRUCT{
    int item_id; //the id of the item.
    float score; //the score of the item.
}MERGED_SCORE_STRUCT;

typedef struct RESPONSE_ARR_STRUCT{
    int id; //id of the query.
    mutable clock_t ts_enqueue; //time stamp entering the queue.
    mutable clock_t ts_schuduled; //time stamp being scheduled.
    mutable clock_t ts_finished; //time stamp finish.
    mutable int num_machines; //number of machines used for calculating the query.
    mutable int num_results; //number of results received.
    mutable string errtxt; //if there is any error, errtxt will be set.
    std::vector<MERGED_SCORE_STRUCT> merged_scores; //the scores computed by worker machines.
    bool operator<(const RESPONSE_ARR_STRUCT& other) const {
        return id < other.id;
    }
} RESPONSE_ARR_STRUCT;


typedef struct MACHINE_STATUS{
    float n_pending_jobs;
    float jobs_per_second;
    int rank; //0,1,... the rank value assigned by MPI.
    float network_speed; //[packet/second]the network communication speed between master machine and itself. 
}MACHINE_STATUS;

/*This thread accepts queries from php via TCP, and insert the queries into incoming queue.*/
void* Thread_Accept_Query(void* params);

/*This thread Takes one query from the Incomming_Query_Queue, and assigns it to the machines according to their workloads.*/
void* Thread_Scheduler(void* params);

/*This thread collects responses from worker machines.*/
void* Thread_Response_Collector(void* params);


/*This thread merge and sort the scores and return result to web user.*/
void* Thread_Merge_Score(void* params);

/*This thread waits for job from master and insert the job into a pending queue*/
void* Thread_Wait_Job_From_Master(void* params);

/*This thread actually implements the query: text+image. Return scores.*/
void* Thread_Slave_Do_Query(void* params);

/*serialize objects into a block of memory for network transmission
 * query[In]: the query to be serialized.
 * n_workers[In]: the number of machines used for computing this query.
 * buffer[Out]: the buffer memory to store the output serialized data.
 * buf_size[In]: the size of the buffer provided.
 * pkt_size[Out]: the size of encoded packet [pkt_size <= buf_size].
 * Return true if success, return false otherwise.
 */
bool Encode_MPI_Query_Packet(const QUERY_QUEUE_STRUCT& query, const int& n_workers, char* buffer, const int& buf_size, int& pkt_size);

/*reconstruct objects from a block of memory
 * query[Out]: the decoded query structure.
 * part[Out]: 0,1,...,n_workers-1. Represents which part this machine should take care of.
 * n_workers[Out]: the total number of machines participating in this query computation.
 * data[In]: the data received from MPI to be decoded.
 * Return true if success, return false otherwise.
 */
bool Decode_MPI_Query_Packet(QUERY_QUEUE_STRUCT& query, int& part, int& n_workers, const char* data);

/*Encode a response into packet for transmission from worker to master.
 * item_id[In]: a list of item id.
 * item_score[In]: a list of item score.
 * time_elapsed[In]: the total time in second used for waiting & processing.
 * buffer[Out]: the encoded data
 * buf_size[In]: the maximum size of the buffer.
 * pkt_size[Out]: the actual size of the encoded packet.
 * qid[In]: the query id of this job.
 * Return true is success, false otherwise.
 */
bool Encode_MPI_Response_Packet(const vector<int>& item_id, const vector<float>& item_score, const double& time_elapsed,const double& processing_time, char* buffer, const int& buf_size, int&pkt_size, const int& qid);

/*Decode a response packet.
 * item_id[Out]: a list of item id.
 * item_score[Out]: a list of item score.
 * time_elapsed[Out]: the total time in second used for waiting & processing.
 * buffer[Out]: the decoded data
 * buf_size[In]: the maximum size of the buffer.
 * pkt_size[Out]: the actual size of the decoded packet.
 * qid[Out]: the query id of this job.
 * Return true is success, false otherwise.
 */
bool Decode_MPI_Response_Packet(vector<int>& item_id, vector<float>& item_score, double& time_elapsed, double&processing_time, char* buffer, const int& buf_size, int&pkt_size, int& qid);

int get_unique_query_id(); //this function gets a unique query id



#endif
