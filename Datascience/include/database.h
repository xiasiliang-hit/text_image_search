#ifndef DATABASE_H
#define	DATABASE_H


#include "common.h"
#include "Engine.h"
class DB{
private:
    Engine* pEn = NULL;

public:

    DB();

    /*GET_ROW: Get a list of "codes" and the corresponding "counts" of a product.
     * item_id[In]: the product id, begin from 0.
     * code[Out]: the pointer to the beginning address of item codes [sorted ascendingly].
     * count[Out]: the pointer to the beginning address of code counts.
     * Return value:
     *      successful: the length of 'code' and 'count' [they have the same length].
     *      failed: 0 [you should validate 'item_id' is greater or equal to 0 and smaller than the total number of items in the database].
     */
    int get_row(const int& item_id, int*& code, int*& count);

    /*GET_WEIGHT: Get the weights of "codes" (TEXT+IMAGE).
     * weigths[Out]: the weight values.
     */
    void get_weight(double* weights);

    /*GET_NB_ITEMS: Get the total number of items int he database.
     * Return value: number of items.
     */
    int get_nb_items();

    /*GET_NB_ITEMS: Get the total number of items int he database.
     * Return value: number of codes (TEXT+IMAGE).
     */
    int get_nb_codes();
};


#endif	/* DATABASE_H */

