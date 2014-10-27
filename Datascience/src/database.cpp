#include "database.h"

DB::DB()
{
    pEn  = new Engine(TXT);
}

int DB::get_row(const int& item_id, int*& code, int*& count){


    int num = pEn->get_row(item_id, code, count);
    return num;

/*    Engine img(IMAGE);

    int* t_row = NULL;
    int* img_row = NULL;

    int t_count = 0;
    int img_count = 0;

    txt.get_row(item_id, t_row, t_count);
    img.get_row(item_id, img_row, img_count);

    int* pr = (int*)malloc(sizeof(int)* (t_count+img_count));
    memcpy(pr, t_row, t_count);
    memcpy(pr+t_count, img_row, img_count);

    code = pr;
    count  = t_count + img_count;
*/
    //Engine fusion();
    //txt.get_row();


    return 0;
}

void DB::get_weight(double* weights){
    weights = pEn->get_freqs();
}


int DB::get_nb_items(){
    return pEn -> n_candidate;
}


int DB::get_nb_codes(){
    return pEn->n_NDUP;
}
