#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <memory.h>
#include <cmath>
#include <algorithm>
#include <vector>
#include <queue>
#include <string>
#include <ctime>
#include <fstream>
#include <map>

#include <iostream>

#include "stdio.h"
//#include "lib/english_stem.h"

#include <sstream>
#include <utility>

#define text_candidate 1470
#define text_NDUP 15501

/*
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
*/

using namespace std;

extern clock_t begin_ts; //used for measuring time elapsed.

const int sz_dict = 529;  //number of dict
const int sz_title = 1470;  //number of titles
const int sz_total_index = 13626;  //size of array storing inverted index

const int n_thread = 6;
const int n_machine = 3;
const int n_query_returned = 10;  //specify the number of results to be returned

const bool debug = false;

typedef struct {
    int unique_i;

    int row_begin;
    int row_end;
} R_INFO;

typedef struct {

    int title_image_code;
    int feature_repeat;
} INDEX_ITEM;



class CompareFirstInt {
public:
    bool operator()(pair<int, int>& l, pair<int, int>& r)
    {
       return l.first > r.first;
    }
};


class CompareSecondDouble {
public:
    bool operator()(pair<int, double>& l, pair<int, double>& r)
    {
       return l.first < r.first;
    }
};

/*
const string prefix = "./model/";
const string indexfilename = prefix + "index.txt";
string freqfilename = prefix + "freq.txt";
string wordfilename = prefix + "mapper.txt;";

string indexfilename_bin = prefix +  "index.bin";
string freqfilename_bin = prefix + "freq.bin";
string wordfilename_bin = prefix + "mapper.bin";
*/



void quick_sort(float* arr, int N, int* order, float* sorted = 0, bool descend = true);
void quick_sort(int* arr, int N, int* order, int* sorted = 0, bool descend = true);
void quick_sort(double* arr, int N, int* order, double* sorted = 0, bool descend = true);
clock_t tic();
double toc();
double toc(clock_t bts);

int binarySearch(string array[], int lowerbound, int upperbound, string key);


void vislog(std::string str);

#endif
