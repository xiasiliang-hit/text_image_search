#ifndef PORTER_INCLUDED
#define PORTER_INCLUDED

#include "common.h"

int cons(int i);
int m();
int vowelinstem();
int doublec(int j);
int cvc(int i);
int ends(char * s);
void setto(char * s);
void r(char * s);
void step1ab();
void step1c();
void step2();
void step3();
void step4();
void step5();
int stem(char * p, int i, int j);
void increase_s();
void stemfile(FILE * f);

string stem_my(string s);

//stem_my is invoked
//lower case, filter special characters, stem
vector<string> preprocess(string str);

#endif // PORTER_INCLUDED
