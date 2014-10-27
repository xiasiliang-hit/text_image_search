#include "text.h"
#include "common.h"

using namespace std;

const string prefix = "./model/";
const string indexfilename = prefix + "index.txt";
const string freqfilename = prefix + "freq.txt";
const string wordfilename = prefix + "mapper.txt";
const string titlefilename = prefix + "TITLE";

const string errorfilename = "error.txt";

const string indexfilename_bin = prefix +  "index.bin";
const string freqfilename_bin = prefix + "freq.bin";
const string wordfilename_bin = prefix + "mapper.bin";

const string pointerfilemane_bin = prefix + "pointer.bin";

const string newdictfilename = prefix + "newmaper.text";

bool comp_int_double(const pair<int, double>& l, const pair<int, double>& r);
bool comp_int_int_small(const pair<int, int>& l, const pair<int, int>& r);

string TEXT::total_word[sz_dict + 1];
//vector<string> TEXT::total_word(sz_dict + 1);
string TEXT::total_title[sz_title + 1] ;
double TEXT::total_freq[sz_dict + 1];

int TEXT::pointer[sz_dict + 1];
int TEXT::total_index[sz_total_index + 1];


TEXT::TEXT(){
    /*
    string total_title[sz_title + 1];
    string total_word[sz_dict + 1];
    double TEXT::total_freq[sz_dict + 1];

    int TEXT::pointer[sz_dict + 1];
    int TEXT::total_index[sz_total_index + 1];
*/

//sz_dict = 0;
	//inverted_index = 0;
}


TEXT::~TEXT(){
//	if(inverted_index) delete inverted_index;
}


void TEXT::read_txt()
{
    read_dict();
    read_freq();
    read_index();
    read_title();
}




/*

int TEXT::Initialize(std::string path_to_mapper, std::string path_to_index, std::string path_to_freq){
	puts("---------------------------");
	printf("TEXT intialization ... ");
	tic();
	//read freq.txt
	FILE* fp = fopen(path_to_freq.c_str(),"r");
	if(!fp) return -1; //failed to open file.
	char buffer [1000000];
	int number;
	double sum = 0, mean_word_freq = 0, std_word_freq = 0;
	while(fgets (buffer , sizeof(buffer) , fp) != NULL ){
		sscanf (buffer,"%d",&number);
		word_freq.push_back(number);
		sum += number;
	}
	fclose(fp);
	sz_dict = word_freq.size();
	mean_word_freq = sum/sz_dict;
	for(int i = 0;i<sz_dict;i++){
		std_word_freq += pow(word_freq[i]-mean_word_freq,2);
		word_freq[i] /= sum;
	}
	std_word_freq = sqrt(std_word_freq);
	//read mapper.txt
	fp = fopen(path_to_mapper.c_str(),"r");
	if(!fp) return -2; //failed to open file.
	number = 0;
	while(fgets (buffer , sizeof(buffer) , fp) != NULL ){
		mapper[buffer] = number++;
	}
	//read index.txt
	fp = fopen(path_to_index.c_str(),"r");
	if(!fp) return -3; //failed to open file.
	inverted_index = new vector<int> [sz_dict];
	int cnt = 0;
	while(fgets (buffer , sizeof(buffer) , fp) != NULL  && cnt<sz_dict){
		if(sizeof(buffer)-5<strlen(buffer)) return -4; //out of memory.
		while(sscanf(buffer,"%d",&number) != EOF){
			printf("%d ",number);
		}
		//inverted_index[cnt].push_back(number);
		cnt++;
		printf("\n");
	}
	//done
	printf("Elapsed time is %.2f seconds.\n",toc());
	printf("Size of dictionary: %d\n",sz_dict);
	printf("Words frequency distribution: mean = %.6f, std = %.6f\n",mean_word_freq,std_word_freq);
	printf("Size of mapper: %d\n",(int)mapper.size());
	puts("---------------------------");
	return 0;
}
*/


vector< pair<int, double> >TEXT::SearchText( vector<string> keywords, const int& num_candidates){
    //vector<int> t_indexs;

    vector< pair<int, double> > result;
    vector<int> t_indexs;  //index of title for grading
    vector<double> scores;


    for (vector<string>::iterator it = keywords.begin(); it <= keywords.end() - 1; it++)
    {
        map<int, double> grades;

        string word = *it;
        vector<string>::iterator p = find(words.begin(), words.end(), word);

        if (p != words.end())
        {
            int offset = p - words.begin();  //index of dict

            vector<int> current = inverted_index[offset];
            for (vector<int>::iterator it = current.begin(); it <= current.end() - 1; it++)
            {
                int index = *it;  // index of title

                double g = (double)(1.0f/freqs.at(offset));
                /*
                map<int, double>::iterator imap = grades.find(index);

                if (imap != grades.end())
                {
                    imap -> second += g;
                }
                else
                {
                    grades.push_back(pair<int, double>(index, g));
                }

                sort(grades.begin(), grades.end(), &comp_int_double);
*/

                //find if the title index exists, if not, add; otherwise upadate grade
                vector<int>::iterator p = find(t_indexs.begin(), t_indexs.end(), index);
                if (p != t_indexs.end())
                {
                    int offset = p - t_indexs.begin();
                    scores.at(offset) += g;
                }
                else
                {
                    t_indexs.push_back(index);
                    scores.push_back(g);
                }



/*
                int num = 0;
                for (map<int, double>::iterator jmap = grades.begin(); jmap != grades.end(); jmap++)
                {
                    num++;
                    if (num <= num_candidates)
                    {
                        t_indexs.push_back(num);
                    }
                    else
                    {

                    }
                    //cout << titles[jmap->first] << endl;
                }
                */
            }
        }
        else
        {
            if (debug)
            {
                cout << "exception:keyword:" + word + " not found" << endl;
            }
            else
            {}
        }

    }

    int sz = t_indexs.size();

    int* p_index_index = (int*)malloc(sizeof(int) * sz);
    double* p_sorted = (double*)malloc(sizeof(double) * sz);


    quick_sort(&(*scores.begin()), sz, p_index_index, p_sorted, false);
    int count = 0;

    for (int* p = p_index_index; p <= p_index_index + sz - 1; p++)
    {
        int pos = *p;
        int real_i = t_indexs.at(pos);


        int offset = p - p_index_index;

        if (count <= num_candidates - 1)
        {
            result.push_back(pair<int, double>(real_i, scores.at(offset)));
        }
        else
        {}

        string t = titles.at(real_i);

        cout << "result" << endl;
        cout << t << endl;

        count++;
    }

    free(p_index_index);
    free(p_sorted);

    return result;
}

/*
void TEXT::serialize(ofstream &os)
{
	int vsize = sz_total_index;

	os.write((char*)&vsize, sizeof(vsize));
	os.write((vcahr*)&index[0][0], vsize * sizeof(int));

}

void TEXT::TEXT(ifstream &is)
{
	int vsize;
	is.read((char*)&vsize, sizeof(vsize));

	index.resize( vsize);
	is.read((char*)&v[0][0], visze * sizeof(int));
}
*/


void TEXT::read_dict()
{
    cout << "f:read_dict" << endl;

    ifstream fin;
    fin.open(wordfilename.c_str());

    int i = 0;
    string word = "";


    while (fin >> word)
    {
        word = stem_my(word);

        //words.push_back(word);
        total_word[i] = (word);
        i++;
    }
    fin.close();

    /*
    ofstream ofs;
    ofs.open(newdictfilename.c_str());
    for (int i = 0; i<= sz_dict - 1; i++ )
    {

        ofs << total_word[i] << endl;
    }


    ofs.close();
    */
}

void TEXT::read_freq()
{
    cout << "f:read_freq" << endl;

    ifstream fin;
    fin.open(freqfilename.c_str());

    int i = 0;
    string word = "";
    double freq = 0.0f;

    while (fin >> word >> freq )
    {
        //freqs.push_back(freq);
        total_freq[i] = (freq);
        i++;
    }
    fin.close();
}

void TEXT::read_index()
{
    //File* f = fopen(indexfilename.c_str(), "r");
	ifstream fin(indexfilename.c_str());
	int i = 0;
    int count = 0;

    if (!fin)
    {
        cout << "error" << indexfilename << endl;
    }

    string s = "";
	while(getline(fin, s))
	{
	    //last line
        if (s == "")
        {
            continue;
        }

	    pointer[i] = count;
        s = s + " ";

		string split =  " ";
		string token = "";
		int j = 0;

		size_t head = 0;
		size_t end = s.find(split, head);
		cout << s << endl;

        if (i==sz_dict && debug)
        {
            cout << "line:548";
        }

        vector<int> current_word;
		while (end != string::npos) {
			token = s.substr(head, end);

			int num = atoi(token.c_str());

            TEXT::total_index[count] = num;
//            current_word.push_back(num);
			head = end + 1;
			end = s.find(split, head);
			//j++;
            count++;
			//	cout <<"j:"<< j << endl;
		}

//        inverted_index.push_back(current_word);
		i++;
	}

    fin.close();
    /* debug
	for (int k =0; k<= index.size()-1;k++)
		{
			for (int j =0; j<= index.size() -1; j++)
				{

					cout << index[i][j];
				}
    }
    */


    cout << "sz_dict: " << sz_dict << endl;
    cout << "actual size of dict: " << i << endl;

    cout << "sz_total_index: " << sz_total_index << endl;
    cout << "actual size of index: " << count << endl;
}

void TEXT::read_title()
{
    cout << "f:read_title" << endl;

    ifstream fin;
    fin.open(titlefilename.c_str());

    int i = 0;
    string word = "";

    if (!fin)
    {
        cout << "error:" << titlefilename << endl;
    }

    string title = "";
    while (getline(fin, title))
    {
        if (title == "")
        {
            cout << "error if not last line" << endl;
            break;
        }

        total_title[i] = title;
        //titles.push_back(title);
        i++;

        if (i == 1098)
            cout << i << endl;
    }
    fin.close();

    cout << "sz_title: " << sz_title << endl;
    cout << "actual size of title: " << i << endl;
}

void TEXT::write_index_bin()
{
	FILE* fout;

	if (fout = fopen(indexfilename_bin.c_str(), "wb"))
	{
	    fwrite(TEXT::total_index, sizeof(int), sz_total_index + 1, fout);
	    fclose(fout);
    }
    else
    {
        //error_msg("write_index_bin".c_str(), indexfilename_bin.c_str(), "".c_str());
    }

    if (fout = fopen(pointerfilemane_bin.c_str(), "wb"))
    {
        fwrite(pointer, sizeof(int), sz_dict + 1, fout);
        fclose(fout);
    }

    else
    {
        //error_msg("write_index_bin".c_str(), pointerfilemane_bin.c_str(), "".c_str());
    }
}


/*
	int count = 0;
	for (int i = 0; i <= sz_dict - 1; i++){

		current = inverted_index[i];
		for(std::vector<int>::iterator it = current.begin(); it != current.end(); it++) {
			//int index = *it;
			count++;
		}
	}

	buffer = (int*) malloc(sizeof(int) * count);

	for (int i = 0; i<= sz_dict -1; i++){

		for(std::vector<int>::iterator it = current.begin(); it != current.end(); it++) {
			int index = *it;

			buffer
		}
	}


	for(std::vector<int>
		 vector)
		 */


void TEXT::read_index_bin()
{
    FILE* fin;

    if (fin = fopen(indexfilename_bin.c_str(), "r"))
	{
	    fread(TEXT::total_index, sizeof(int), sz_total_index + 1, fin);
	    fclose(fin);
    }
    else
    {
        cout << "error: " + indexfilename_bin << endl;
    }

    if (fin = fopen(pointerfilemane_bin.c_str(), "r"))
    {
        fread(pointer, sizeof(int), sz_dict + 1, fin);
        fclose(fin);
    }

    else
    {
        cout << "error: " + pointerfilemane_bin << endl;
    }
}

/*
void TEXT::write_word_bin()
{
    FILE* fin;

    if (fin = fopen(wordfilename_bin.c_str(), "wb"))
	{
	    fwrite(total_word, sizeof(int), sz_total_index + 1, fin);
	    fclose(fin);
    }
    else
    {
        cout << "error: " + indexfilename_bin << endl;
    }

    if (fin = fopen(pointerfilemane_bin.c_str(), "r"))
    {
        fread(pointer, sizeof(int), sz_dict + 1, fin);
        fclose(fin);
    }

    else
    {
        cout << "error: " + pointerfilemane_bin << endl;
    }
}
*/

//return a row of index
//beign will be pointer to the beignning of the row, end will be pointed the end of the row
int TEXT::get_row(int row_num, int& begin, int& end)
{
    int length = 0;
    if (row_num >= 0 && row_num <= sz_dict - 1 )
    {
        begin = pointer[row_num];
        end = pointer[row_num + 1];

        length = end - begin;
    }
    else
    {
        //error_msg("get_row", "index out of range", "");
    }
    return length;
}


vector< pair<int, double> >  TEXT::multiway_merge_single(vector<int> heads)
{
    vector< pair<int, double> >result;

    vector<R_INFO> row_infos;

    for (vector<int>::iterator it = heads.begin(); it <= heads.end()-1; it++)
    {
        int row_num = *it;

        int begin = 0;
        int end = 0;

        int len = get_row(row_num, begin, end);

        R_INFO r_info = {
            row_num,

            begin,
            end,
        };
        row_infos.push_back(r_info);
    }


    std::priority_queue< pair<int, int>, vector< pair<int, int> >, CompareFirstInt > pq;
    for (vector<R_INFO>::iterator it = row_infos.begin(); it <= row_infos.end() - 1; it++)
    {
        int begin = it -> row_begin;
        int end = it -> row_end;
        int unique_i = it -> unique_i;

        for (int i = begin; i<= end - 1; i++)
        {
            int content = TEXT::total_index[i];

            pq.push(pair<int, int>(content, unique_i));
        }
    }

    while (!pq.empty())
    {
        int top = pq.top().first;
        int unique_i = pq.top().second;

        double wgt = 1.0f/double(total_freq[unique_i]);

        if (!result.empty())
        {
            if (result.back().first == top  )
            {
                result.back().second += wgt;
            }

            else
            {
                result.push_back(pair<int, double>(top, wgt));
            }
        }
        else
        {
            result.push_back(pair<int, double>(top, wgt));
        }

        pq.pop();
    }


    if (debug)
    {
        cout << "--vector returned in muiltway merge" << endl;
        for (int i = 0; i<= result.size()-1; i++)
        {
            cout << (result[i].first) << endl;
            cout << (result[i].second) << endl;
        }
    }

    return result;
}

vector< pair<int, double> > TEXT::merge_grades(vector< vector< pair <int, double> > > v)
{
    vector< pair<int, double> >result;

    priority_queue< pair<int, int>, vector< pair<int, int> >, CompareFirstInt > pq;

    for (int i = 0; i <= v.size()-1; i++)
    {
        for (int j = 0; j<= v[i].size()-1; j++)
        {
            pq.push(v[i][j]);
        }
    }

    while (!pq.empty())
    {
        if (!result.empty())
        {
            int top = pq.top().first;
            double grade = pq.top().second;

            if (result.back().first == top)
            {
                result.back().second += grade;
            }
            else
            {
                result.push_back(pq.top());
            }
        }
        else
        {
            result.push_back(pq.top());
        }
        pq.pop();
    }

    return result;
}



vector<int> TEXT::process_query(string str)
{
    vector<string> preprocessed_str  = preprocess(str);
    vector<int> result(preprocessed_str.size());
/*
    cout << preprocessed_str[0] << endl;
    cout << preprocessed_str[1] << endl;
    cout << preprocessed_str[2] << endl;
    cout << preprocessed_str[3] << endl;
*/

    for (vector<string>::iterator it = preprocessed_str.begin(); it <= preprocessed_str.end()-1; it++)
    {
        //cout << *it << endl;
        string str(*it);

        int pos = //binary_search(total_word.begin(), total_word.end(), str);
    binarySearch(total_word, 0, sz_dict, str);

        //int pos = 0;
        if (pos != -1)
        {
            //pos = p - TEXT::total_word;
            result.push_back(pos);
        }

        else
        {}
    }

    return result;
}


string TEXT::stem_python_to_c(string str)
{
    return stem_my(str);
}



/*
void error_msg(char* function, char* msg, char* para = "")
{
    ofstream ofs(errorfilename.c_str());
    if (ofs)
    {
        ofs << "f:" << function << endl;
        ofs << "error:" << "msg" << endl;
        ofs << "additon" << para << endl;
    }
    else
    {
        cout << "can not output error log" <<endl;
    }
}
*/


