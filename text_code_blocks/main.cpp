#include "stdio.h"
#include "text.h"
#include "common.h"

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


int main(){
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

test3();
}


