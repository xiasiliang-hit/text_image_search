#include "text.h"
#include "common.h"

using namespace std;

const string prefix = "./model/";
const string indexfilename = prefix + "index.txt";
const string freqfilename = prefix + "freq.txt";
const string wordfilename = prefix + "mapper.txt";
const string titlefilename = prefix + "TITLE";


const string indexfilename_bin = prefix +  "index.bin";
const string freqfilename_bin = prefix + "freq.bin";
const string wordfilename_bin = prefix + "mapper.bin";

const string pointerfilemane_bin = prefix + "pointer.bin";


bool comp_int_double(const pair<int, double>& l, const pair<int, double>& r);


TEXT::TEXT(){
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
        words.push_back(word);
        //total_word[i] = (word);
         i++;
    }
    fin.close();
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
        freqs.push_back(freq);
        //total_freq[i] = (freq);
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

		if (i <=2)
			cout << s << endl;

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
			//			cout << token << endl;

			int num = atoi(token.c_str());

			//index.push_back(num);
//total_index[count] = num;
current_word.push_back(num);


			head = end + 1;
			end = s.find(split, head);
			//j++;
            count++;
			//	cout <<"j:"<< j << endl;
		}
inverted_index.push_back(current_word);
 //cout << "here";

		i++;
		//cout << "i:" << i << endl;
	}
/*
	for (int k =0; k<= index.size()-1;k++)
		{
			for (int j =0; j<= index.size() -1; j++)
				{

					cout << index[i][j];
				}
}
*/

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

        titles.push_back(title);
        //total_word[i] = (word);
         i++;
    }
    fin.close();
}

void TEXT::write_index_bin()
{
	FILE* fout ;

	if (fout = fopen(indexfilename_bin.c_str(), "wb"))
	{
	    fwrite(total_index, sizeof(int), sz_total_index + 1, fout);
	    fclose(fout);
    }
    else
    {
        cout << "error: " + indexfilename_bin << endl;
    }

    if (fout = fopen(pointerfilemane_bin.c_str(), "wb"))
    {
        fwrite(pointer, sizeof(int), sz_dict + 1, fout);
        fclose(fout);
    }

    else
    {
        cout << "error: " + pointerfilemane_bin << endl;
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
}

void TEXT::read_index_bin()
{
    FILE* fin;

    if (fin = fopen(indexfilename_bin.c_str(), "r"))
	{
	    fread(total_index, sizeof(int), sz_total_index + 1, fin);
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


void TEXT::write_word_bin()
{
    FILE* fin;

    if (fin = fopen(wordfilename.c_str(), "wb"))
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


