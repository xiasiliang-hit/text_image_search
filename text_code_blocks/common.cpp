#include "common.h"

using namespace std;

clock_t begin_ts = clock();

/*
bool comp_int_double(const pair<int, double>& l, const pair<int, double>& r) {
    return l.second > r.second;
}

bool comp_int_int_small(const pair<int, int>& l, const pair<int, int>& r) {
    return l.first > r.first;
}
*/

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

int binarySearch(string array[], int lowerbound, int upperbound, string key)
{
       int position;
       int comparisonCount = 1;    //count the number of comparisons (optional)

       // To start, find the subscript of the middle position.
       position = ( lowerbound + upperbound) / 2;

       while((array[position] != key) && (lowerbound <= upperbound))
       {
              comparisonCount++;
              if (array[position] > key)               // If the number is > key, ..
             {                                                       // decrease position by one.
                    upperbound = position - 1;
             }
             else
            {                                                        // Else, increase position by one.
                   lowerbound = position + 1;
             }
             position = (lowerbound + upperbound) / 2;
       }
      if (lowerbound <= upperbound)
      {
          /*
            cout<< "The number was found in array subscript "<< position<<endl<<endl;
            cout<< "The binary search found the number after " << comparisonCount
                   << " comparisons.\n";
                   */
            // printing the number of comparisons is optional
            return position;
       }
       else
            cout << "keywords not found in dict:" << key << endl;
            return -1;
            /*
             cout<< "Sorry, the number is not in this array.  The binary search made "
                   <<comparisonCount << " comparisons.";
                   */
       //return;  // you may also consider returning the subscript
}
