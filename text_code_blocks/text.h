#ifndef TEXT_H
#define TEXT_H
#include "common.h"
#include "porter.h"

using namespace std;

class TEXT{
public:

	//int sz_dict; /*Number of different words in the dictionary*/
	std::vector<double> freqs; /*1xsz_dict array, Normalized frequence of words*/
	std::vector< vector<int> >inverted_index; /*inverted_intex[i] contains an array of item ids that containing the i-th word*/
	std::vector<string> words; /*word->code mapping*/
    std::vector<string> titles;

/*
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {

        ar & inverted_index;
        ar & words;
        ar & freqs;
        ar & titles;

        //ar & test_vector;
        ar & test_vector;
    }
*/


    std::vector<int> test_vector;
    //int* parray ;
	//	int sz_total_index;
	//	int word[sz_dict];
/*
    std::vector< vector<string> > t_vector;
    string array [100];

    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & (t_vector);
        ar & (array);
    }
*/


//    static vector<string> total_word;
    static string total_title[sz_title + 1] ;
    static string total_word[sz_dict + 1];
    static double total_freq[sz_dict + 1];

    std::vector<int> index;

    int static pointer[sz_dict + 1];
    int static total_index[sz_total_index + 1];

	TEXT();
	//TEXT(ifstream &is);

	~TEXT();

	//void serialize(ofstream &os);
	int Initialize(std::string path_to_mapper, std::string path_to_index, std::string path_to_freq);
	/*Initialize the text search engine
	* path_to_mapper: the path to the 'mapper.txt' file on hard drive.
	* path_to_index:  the path to the 'index.txt'  file on hard drive.
	* path_to_freq:   the path to the 'freq.txt'   file on hard drive.
	* return value:
	*	0 -- success
	*	-1 -- fail
	*/

	vector< pair<int, double> > SearchText(vector<std::string> keywords, const int& num_candidates);
	/*Search items by keywords
	* keywords[in]: a vector array of keyworks to search.
	* num_candidates[in]: the number of items need to returned.
	* items[out]: a list of ids of items that best match the keywords.
	* scores[out]: the matching scores corresponding to the items. Higher score is better.
	* return value:
	*	0  -- success
	*	-1 -- fail
	*/

    void read_dict();
    void read_index();
    void read_freq();
    void read_title();

    /*  read all text files
    */
    void read_txt();



    void dump();  //obsolete
    void load_bin();  //obsolete

    //wirte index to binary file
    void write_index_bin();

    //read index from binary file
    void read_index_bin();

	void write_word_bin();
	void write_freq_bin();

	//get the beginning position and ending postion in index array, given the code of keyword
    int get_row(int row_num, int& begin, int& end);

    //merge rows for different rows
    //heads: array of indice(code of keyword) that specifies the rows to be merged
    vector< pair<int, double> >  multiway_merge_single(vector<int> heads);

    //merge the result from differnt threads
    vector< pair<int, double> > merge_grades(vector< vector< pair<int, double> > > v);

    //void error_msg(char* function, char* msg, char* para = "");

    //process query string, tokenize, lower case, stem
    //return code of keywords
    vector<int> process_query(string str);

    string stem_python_to_c(string str);
};
#endif
