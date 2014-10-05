
#ifndef COMMON_H
#define COMMON_H

#include<memory.h>
#include<cmath>
#include<algorithm>
#include<vector>
#include<string>
#include <ctime>
#include <fstream>
#include <map>

#include <iostream>
#include <vector>
#include <string>
#include "stdio.h"

#include <sstream>
#include <utility>

#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

extern clock_t begin_ts; //used for measuring time elapsed.

const int sz_dict = 548;
const int sz_total_index = 14836;

const bool debug = false;

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
void tic();
double toc();
#endif
