#include "stdio.h"
#include "text.h"
#include "common.h"
#include "parallel.h"
#include "porter.h"

using namespace std;

const string objfilename_bin = "text.bin";

void dump(TEXT& t)
{
    std::ofstream ofs(objfilename_bin.c_str());
    // create class instance
    // save data to archive
    {
        boost::archive::text_oarchive oa(ofs);
        oa << t;
        ofs.close();
    }

    //write_index_bin();
    //write_freq_bin();
    //write_word_bin();
}


void load_bin(TEXT& t)
{
    std::ifstream ifs(objfilename_bin.c_str());
    {
        if (!ifs)
        {
            cout << "error:" << objfilename_bin << endl;
        }

        boost::archive::text_iarchive ia(ifs);
        // read class state from archive
        ia >> t;
        ifs.close();
    }
}
/*
void test2()
{
    TEXT t;
    //t.t_vector = new vector<int> {1,2,3,99};

    vector<string> a;
    a.push_back("new here 1");

    vector<string> b;
    b.push_back ("hello 2");
    b.push_back("hello 22");

    std::ofstream ofs("./test.bin");
    // create class instance
    // save data to archive
    {
        boost::archive::text_oarchive oa(ofs);
        oa << t;
        ofs.close();

    }

    TEXT t2;
    std::ifstream ifs("./test.bin");
    {
        if (!ifs)
        {
            cout << "error";
        }


        boost::archive::text_iarchive ia(ifs);
        // read class state from archive
        ia >> t2;
        ifs.close();
    }

    cout << t2.t_vector[0][0] << endl;
    cout << t2.t_vector[1][1] << endl;

    cout << t.array[0] << endl;
    cout << t.array[99] << endl;
}
*/

void test3()
{
    TEXT t;
    t.read_txt();
    dump(t);

    vector<string> keywords;
    keywords.push_back("a");
    keywords.push_back("basic");

    TEXT t2;
    load_bin(t2);
    vector< pair<int, double> >r = t2.SearchText(keywords, 100);

    for (vector< pair<int, double> >::iterator it = r.begin(); it <= r.end()-1; it++)
    {
        int i = (it)->first;

        cout << "----------";
        cout << i << endl;
        cout << t2.titles.at(i) << endl;
    }

/*
    cout << t2.inverted_index[0][0] << endl;
    int x = t2.inverted_index.size() -1;
    vector<int> a = t2.inverted_index[x];
    cout << a[0] << endl;
    cout << a[1] << endl;
    cout << a[2] << endl;

    cout << "----" << endl;
    cout << t2.titles[0];
    cout << t2.titles[547];
    cout << "----" << endl;
    cout << t2.freqs[0];
    cout << t2.freqs[547];
    cout << "----" << endl;
    cout << t2.words[0];
    cout << t2.words[547];
*/

}


/*
void test()
{
	TEXT t;



	//t.read_dict();
	//t.read_index();
    //t.write_index_bin();
    //t.read_index_bin();


    //t.read_freq();
    t.read_dict();

    int i = sz_dict -1 ;
    int head = t.pointer[i];
    int end = t.pointer[i+1];


    for (i=0; i<=sz_dict-1; i++)
    {
        cout << "###";
        cout << t.total_freq[i] << endl;
        cout << t.total_word[i] << endl;
    }


    for (int j = head; j <= end - 1; j++  )
    {
        //cout << "pinrt index "<< endl;
        //int ele = t.index.at(j);

        //cout << ele << " ";
    }

	//t.build_index_array();

}
*/
void parallel_thread_main(void* params)
{
	vector<int> my_rows = ((PARALLEL_THREAD_PARAM*)params) -> unique_is;
	TEXT t;
	((PARALLEL_THREAD_PARAM* )params) -> result =  t.multiway_merge_single(my_rows);
}


int main(){


string s = "Unotux Boys Suits Formal Wedding Eggplant Purple Satin Bow Tie from Baby to Teen";


/*
for (vector<string>::iterator it = str_v.begin(); it <= str_v.end()-1; it++)
{
    string s = stem_my(*it);

    processed.push_back(s);
}
*/





//cout << stem_my("words") << endl;

    TEXT t;
    t.read_txt();
    t.write_index_bin();
    //t.write_freq_bin();
    t.write_word_bin();
    t.read_index_bin();


    vector<int> preprocessed = t.process_query(s);

    vector< vector<int> > assigned_i(n_machine);
/*
//manager machine
    if (true)
    {
        //vector<int> preprocessed;  //preprocessed_id

        //assign rows
        for (int i = 0; i <= preprocessed.size() - 1; i++)
        {
            assigned_i.at(i % n_machine).push_back(i);
        }

        //t.merge_grades from all machines;
        vector< vector< pair<int, double> > > received;
        vector< pair<int, double> > merged = t.merge_grades(received);

        std::nth_element(merged.begin(), merged.begin() + n_query_returned, merged.end(), CompareSecondDouble);

        double g = (*(merged.begin() + n_query_returned)).second;

        for (vector< pair<int, double> >::iterator it = merged.begin(); it <= merged.end() -1; it++)
        {
            if ((*it).second >= g)
                cout << (*it).first << endl;
        }

    }
*/
/*
    //slave node
    if (true)
    {
        vector< vector< pari<int. double> > > rst;
        //parameters for threads
        vector<void*> v_params;


        for (int i = 0; i <= n_thread - 1; i++)
        {
//            static const int arr[] = {1, 2, 3, 4};
//            vector<int> begins (arr, arr + sizeof(arr) / sizeof(arr[0]) );


            //get his own tasks $$$s
            vector<int> begins = assigned_i.at(current_node);

            PARALLEL_THREAD_PARAM* param = new PARALLEL_THREAD_PARAM();
            param -> unique_is = begins;

            v_params.push_back(param);
        }

        Parallel pthreads;
        pthreads.Run(parallel_thread_main, v_params);

        for (int j = 0; j <= n_thread - 1; j++)
        {
            vector< pair<int, double> > v  = ((PARALLEL_THREAD_PARAM*)v_params.at(j)) -> result;
            rst.push_back(v);

            cout << "------in main" << j << endl;
            for (int k = 0; k <= v.size() -1; k++)
            {
                cout << v[k].first << endl;
                cout << v[k].second << endl;
            }
        }

        //send rst;
    }
*/


	//string path_to_mapper = "model/mapper.txt";
	//string path_to_index = "model/index.txt";
	//string path_to_freq = "model/freq.txt";
	//Initialize the TEXT object.
	/*
	TEXT* text = new TEXT();
	if(int code = text->Initialize(path_to_mapper,path_to_index,path_to_freq)){
		printf("Error: failed to initialize TEXT. Error code was %d\n",code);
		return -1;
	}
	//Search Text.
	//Show results.
	*/

//string myString;

    /*
    string word("documentation");
    stemming::english_stem<string> StemEnglish;
    StemEnglish(word);
*/






    /*
    int N= 100000000;

    t.test_vector.resize(N, 99);

    //t.parray =  (int*)malloc(sizeof(int) * N);

    tic();
    dump(t);
    clock_t cost  = toc();


	cout << cost << endl;

    tic();
    load_bin(t);
    cost = toc();

cout << "load" << endl;

cout << cost <<endl;


tic();
FILE* f = fopen("test2.bin", "wb");
fwrite (&t.test_vector[0] , sizeof(int), N, f);
cost  = toc();

cout << "using fwrite" << endl;
cout << cost << endl;

tic();
 f = fopen("test2.bin", "r");
fread (&t.test_vector[0], sizeof(int), N, f);
 cost  = toc();

cout << "using fread" << endl;
cout << cost << endl;
*/

    //test3();

/*
    TEXT t;


    t.read_txt();

    t.write_index_bin();
    //t.write_freq_bin();
    t.write_word_bin();


    t.read_index_bin();


    vector<int> heads1;
    heads1.push_back(1);
    heads1.push_back(2);
    heads1.push_back(3);
    heads1.push_back(4);


    vector< pair<int, double> >v1;
    v1 = t.multiway_merge_single(heads1);


    vector<int> heads2;
    heads2.push_back(3);
    heads2.push_back(4);


    vector< pair<int, double> > v2;
    v2 = t.multiway_merge_single(heads2);


    vector< vector < pair<int, double> > > v3;
    v3.push_back(v1);
    v3.push_back(v2);


    vector< pair<int, double> > merged = t.merge_grades(v3);

    if (debug){
        cout << "--after meger grades";
        for (int i = 0; i<= merged.size() - 1; i++)
        {
               cout << (merged[i].first) <<endl;
                cout << (merged[i].second) << endl;
        }
    }
    //cout << "";

    */
}
