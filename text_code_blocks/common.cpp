#include"common.h"

using namespace std;

clock_t begin_ts = clock();

bool comp_int_double(const pair<int, double>& l, const pair<int, double>& r) {
    return l.second > r.second;
}


bool mycompfunc_double(const pair<double, int>& l, const pair<double, int>& r) {
    return l.first > r.first;
}

bool mycompfunc_int(const pair<int, int>& l, const pair<int, int>& r) {
    return l.first > r.first;
}

bool mycompfunc_float(const pair<float, int>& l, const pair<float, int>& r) {
    return l.first > r.first;
}

void quick_sort(float* arr, int N, int* order, float* sorted, bool descend) {
    vector< pair<float, int> > WI;
    pair<float, int> val_ind;
    for (int i = 0; i < N; i++) {
        val_ind.first = arr[i]; //value.
        val_ind.second = i; //index.
        WI.push_back(val_ind);
    }
    sort(WI.begin(), WI.end(), mycompfunc_float);
    if (descend)
        for (int i = 0; i < N; i++) {
            if (sorted) sorted[i] = WI[i].first;
            order[i] = WI[i].second;
        } else
        for (int i = N - 1; i >= 0; i--) {
            if (sorted) sorted[i] = WI[i].first;
            order[i] = WI[i].second;
        }
}


void quick_sort(double* arr, int N, int* order, double* sorted, bool descend) {
    vector< pair<double, int> > WI;
    pair<double, int> val_ind;
    for (int i = 0; i < N; i++) {
        val_ind.first = arr[i]; //value.
        val_ind.second = i; //index.
        WI.push_back(val_ind);
    }
    sort(WI.begin(), WI.end(), mycompfunc_double);
    if (descend)
        for (int i = 0; i < N; i++) {
            if (sorted) sorted[i] = WI[i].first;
            order[i] = WI[i].second;
        } else
        for (int i = N - 1; i >= 0; i--) {
            if (sorted) sorted[i] = WI[i].first;
            order[i] = WI[i].second;
        }
}

void quick_sort(int* arr, int N, int* order, int* sorted, bool descend) {
    vector< pair<int, int> > WI;
    pair<int, int> val_ind;
    for (int i = 0; i < N; i++) {
        val_ind.first = arr[i]; //value.
        val_ind.second = i; //index.
        WI.push_back(val_ind);
    }
    sort(WI.begin(), WI.end(), mycompfunc_int);
    if (descend)
        for (int i = 0; i < N; i++) {
            if (sorted) sorted[i] = WI[i].first;
            order[i] = WI[i].second;
        } else
        for (int i = N - 1; i >= 0; i--) {
            if (sorted) sorted[i] = WI[i].first;
            order[i] = WI[i].second;
        }
}

void tic(){
	begin_ts = clock();
}
double toc(){
	return double(clock() - begin_ts) / CLOCKS_PER_SEC;
}

