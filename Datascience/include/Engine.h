#ifndef ENGINE_H_INCLUDED
#define ENGINE_H_INCLUDED


#include "common.h"
#include <stdio.h>
#include <stdlib.h>


#define TXT "TXT"
#define IMAGE "IMAGE"
#define COMBINE "COMBINE"
//#define n_dict 1470
//#define n_NDUP 15501



class Engine
{
private:


    string image_txt_type = "";
    vector<string> g_words;
    vector<double> g_freqs;


    int* COUNT_NDUP = NULL;
    int* ENCODE_NDUP = NULL;
    int* pointer = NULL;

    void combine(Engine& text, Engine& image);


    void read_para();
    void write_para();
    //total_features
    //total_

public:
    int n_candidate = 0;
    int n_NDUP = 0;

    Engine(string image_txt_type);
    Engine(Engine& text, Engine& image);

    ~Engine();
    static void test();

    int get_n_candidate() {return n_candidate;}
    int get_n_NDUP() {return n_NDUP;}

    void build_dict();
    /*void write_dict();
    void read_dict();
    */
    void encode_ndup();
    //void encode(string);
    void write_bin();
    void read_bin();
    int get_row(const int& row, int*& code, int*& count);

    double* get_freqs();
};
#endif // ENGINE_H_INCLUDED
