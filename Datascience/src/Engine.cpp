#include "Engine.h"

const string prefix = "./model/";
const string part_stem_filename = "_STEM.txt";

const string part_dict_filename = "_DICT.txt";

const string part_encode_dup_filename = "_ENCODE_DUP.txt";

const string part_encode_filename = "_ENCODE_NDUP.bin";
const string part_count_filename = "_COUNT_NDUP.bin";
const string part_pointer_filename = "_POINTER_NDUP.bin";

const string para_filename = "_n_para.txt";
//const string s_di = "_TXT_ENCODE_DUP.bin"


int binary_search(const vector<string>& sorted_vec, string key) {
   int mid, left = 0 ;
   int right = sorted_vec.size(); // one position passed the right end
   while (left < right) {
      mid = left + (right - left)/2;
      if (key > sorted_vec[mid]){
          left = mid+1;
      }
      else if (key < sorted_vec[mid]){
        right = mid;
      }
      else {
        return mid;
     }
   }

   return -1;
}

Engine::Engine(string image_txt_type)
{


    this->image_txt_type = image_txt_type;

    //this->n_candidate = text_candidate;
    //this->n_NDUP = text_NDUP;

    read_para();
    this->pointer = new int[n_candidate+1];
    this->ENCODE_NDUP = new int[n_NDUP+1];
    this->COUNT_NDUP = new int[n_NDUP+1];

    read_bin();
}

Engine::Engine(Engine& text, Engine& image)
{
    this->image_txt_type = COMBINE;

    this->n_candidate = text.get_n_candidate();
    this->n_NDUP = text.get_n_NDUP() + image.get_n_NDUP();

    this->pointer = new int[n_candidate+1];
    this->ENCODE_NDUP = new int[n_NDUP +1];
    this->COUNT_NDUP = new int[n_NDUP+1];


    //this->g_words =
    //this->g_freqs =

    this->combine(text, image);
}


Engine::~Engine()
{

    delete[] pointer;
    delete[] ENCODE_NDUP;
    delete[] COUNT_NDUP;

    pointer = NULL;
    ENCODE_NDUP = NULL;
    COUNT_NDUP = NULL;
}

void Engine::build_dict()
{
    string filename = prefix + image_txt_type + part_stem_filename;
    std::map<string, double> dict;
    vector< vector<string> > temp;


    ifstream fin(filename.c_str());

    if (!fin)
    {
        cout << "error" << filename << endl;
    }

    string s = "";
    int line = 0;
	while(getline(fin, s))
	{
	    //last line
        if (s == "")
        {
            continue;
        }
        else{}

        //pointer[i] = count;
        s = s + " ";

		string split =  " ";
		string token = "";

		size_t head = 0;
		size_t end = s.find(split, head);
		cout << s << endl;

        int count = 0;
        temp.push_back(*(new vector<string>()));

        while (end != string::npos) {
			token = s.substr(head, end-head);


            temp[line].push_back(token);

            count++;

            if (token == "")
            {

                head = end + 1;
                end = s.find(split, head);
                continue;
            }


            map<string, double>::iterator it = dict.find(token);
            if (it != dict.end())
            {
                it->second += 1.0f;
            }

            else
            {
                dict.insert(pair<string, double>(token, 1.0f));
            }

            head = end + 1;
			end = s.find(split, head);
		}

		line++;
    }
    fin.close();




    //write dict
    string fout_filename = prefix + image_txt_type + part_dict_filename;

    int count_dict = dict.size();
    g_words.reserve(count_dict);
    g_freqs.reserve(count_dict);

    ofstream fout(fout_filename.c_str());

    for (map<string, double>::iterator iter = dict.begin(); iter != dict.end(); iter++ )
    {
        g_words.push_back(iter->first);
        g_freqs.push_back(iter->second);

        fout << iter->first << " " << iter->second << endl;
    }
    fout.close();



    //store dup encoding
    vector< vector<int> > encode_dup;
cout << temp.size();
    //string filename = prefix + image_txt_type + part_stem_filename;


    fout_filename = prefix + image_txt_type + part_encode_dup_filename;
    //fout(fout_filename.c_str(), "w");
    fout.open(fout_filename.c_str());

    for (int j = 0; j<= temp.size()-1; j++)
    {
    cout << "j:" << j << endl;
encode_dup.push_back(*(new vector<int>()));
string wstr = "";

        for (int k = 0; k <= temp[j].size()-1; k++)
        {
            if (j == 1470)
            {
                cout << temp[j].size();
            }

            cout << "k" << k << endl;
            string cur = temp[j][k];

            int pos = binary_search(g_words, cur);

            if (pos)
            {
                int index = pos;

                encode_dup[j].push_back(index);
/*
                char buffer[64];
                itoa(index, buffer, 10);
                string intstr(buffer);
*/
                wstr += to_string(index) + " ";
            }
            else
            {
                cout << "error:dict search error" << endl;
            }
        }

        fout << wstr << endl;
    }

    fout.close();

    //


}



void Engine::encode_ndup()
{
    string filename = prefix + image_txt_type + part_encode_dup_filename;
    ifstream fin(filename.c_str());
    vector< vector<int> > temp_encode_ndup;

    if (!fin)
    {
        cout << "error" << filename << endl;
    }

    string s = "";
    int line = 0;
	while(getline(fin, s))
	{
        if (s == "")
        {
            continue;
        }
        else{}

        //pointer[i] = count;
        //s = s + " ";

		string split =  " ";
		string token = "";

		size_t head = 0;
		size_t end = s.find(split, head);
		cout << s << endl;

        int count = 0;
        temp_encode_ndup.push_back(*(new vector<int>()));

        while (end != string::npos)
        {
			token = s.substr(head, end-head);
			//token += "\n";
			int intcode = atoi(token.c_str());
			temp_encode_ndup[line].push_back(intcode);
			count++;

			head = end+1;
			end = s.find(split, head);
        }
		line++;
	}

    int offset = 0;
    int j = 0;
    for (j = 0; j<= temp_encode_ndup.size()-1; j++)
    {
        map<int, int> dup;  //keyword encoding, duplicated times

        for (int k = 0; k <= temp_encode_ndup[j].size()-1; k++)
        {
            int intcode = temp_encode_ndup[j][k];


                //int index = pos;
                map<int, int>::iterator it = dup.find(intcode);
                if (it != dup.end())
                {
                    it->second += 1;
                }
                else
                {
                    dup.insert(pair<int, int>(intcode, 1));
                }
        }

        *(pointer+j) = offset;
        for (map<int, int>::iterator it = dup.begin(); it != dup.end(); it++)
        {
            cout << offset << endl;

            int code = it->first;
            int ndup_count = it->second;

            *(ENCODE_NDUP+offset) = code;
            *(COUNT_NDUP+offset) = ndup_count;

            offset++;
        }
    }
    *(pointer+j) = offset;


    cout << "n_candidate should be:" << n_candidate << endl;
    cout << "n_NDUP should be:" << offset << endl;

    write_para();

    write_bin();
}

void Engine::write_bin()
{
    string fname = prefix + image_txt_type + part_pointer_filename;
	FILE* f ;

	if (f = fopen(fname.c_str(), "wb"))
	{
		fwrite(pointer, sizeof(int), n_candidate + 1, f);
	}
	else
	{
		cout << "error:" << fname << endl;
	}

	fname = prefix + image_txt_type + part_encode_filename;
	if (f = fopen(fname.c_str(), "wb"))
	{
		fwrite(ENCODE_NDUP, sizeof(int), n_NDUP + 1, f);
	}
	else
	{
		cout << "error:" << fname << endl;
	}

	fname = prefix + image_txt_type + part_count_filename;
	if (f = fopen(fname.c_str(), "wb"))
	{
		fwrite(COUNT_NDUP, sizeof(int), n_NDUP + 1, f);
	}
	else
	{
		cout << "error:" << fname << endl;
	}


}

void Engine::read_bin()
{
	string filename = prefix + image_txt_type + part_pointer_filename;
	FILE* f;
	if (f = fopen(filename.c_str(), "rb"))
	{
		fread(pointer, sizeof(int), n_candidate + 1, f);
	}
	else
	{
		cout << "error:" << filename << endl;
	}

	filename = prefix + image_txt_type + part_encode_filename;
	if (f = fopen(filename.c_str(), "rb"))
	{
		fread(ENCODE_NDUP, sizeof(int), n_NDUP + 1, f);
	}
	else
	{
		cout << "error:" << filename << endl;
	}

	filename = prefix + image_txt_type + part_count_filename;
	if (f = fopen(filename.c_str(), "rb"))
	{
		fread(COUNT_NDUP, sizeof(int), n_NDUP + 1, f);
	}
	else
	{
		cout << "error:" << filename << endl;
	}
}


void Engine::combine(Engine& text, Engine& image)
{
    int offset = 0;
    int i;
    for (i =0; i<= n_candidate-1; i++)
    {
        *(pointer + i) = offset;

        int texthead = *(text.pointer + i);
        int textend = *(text.pointer + i + 1);  //???

        int imghead  = *(image.pointer + i);
        int imgend = *(image.pointer + i + 1);

        int offset = 0;
        int j = 0;
        for (j = texthead; j <= textend -1; j++)
        {
            *(ENCODE_NDUP + j) = *(text.ENCODE_NDUP + j);
			*(COUNT_NDUP + j) = *(text.COUNT_NDUP + j);
        }
        int k = 0;
        for (k = imghead; k<=imgend-1; k++)
        {
            *(ENCODE_NDUP + j + k) = *(image.ENCODE_NDUP + k);
			*(COUNT_NDUP + j + k) = *(image.COUNT_NDUP + k);
        }
        offset += (j+k);
    }
    *(pointer + i) = offset;

}

int Engine::get_row(const int& row, int*& code, int*& count )
{
    //vector<int> result;
    int head = *(pointer+row);
    int end = *(pointer+row+1);

    code = (ENCODE_NDUP + head);  //pointer
    count = (COUNT_NDUP + head);
    return end - head;
}


void Engine::write_para()
{
    string filename = prefix + image_txt_type + para_filename;
    ofstream fout(filename.c_str());
    if (fout)
    {
    fout << n_candidate <<" " << n_NDUP << endl;
    fout.close();
    }
    else
    {
        cout << "error:" << filename << endl;
    }
}

void Engine::read_para()
{
    string filename = prefix + image_txt_type + para_filename;
    ifstream fin(filename.c_str());

    if (fin)
    {

    fin >> n_candidate >> n_NDUP;
    fin.close();
    }
    else
    {
        cout << "error:" << filename << endl;
    }
}


double* Engine::get_freqs()
{
    return &g_freqs[0];
}

/*
void Engine::write_bin()
{

}
*/
