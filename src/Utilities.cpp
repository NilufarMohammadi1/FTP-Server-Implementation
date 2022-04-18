#include <string>
#include <fstream>
#include <streambuf>
#include <vector>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <chrono>
#include <ctime> 


using namespace std;

void erase_sub_str(string & main_str, const string & to_erase);
string read_file_to_string(string file_path);
double read_file_to_double(string file_path);
vector<string> parse_command(char* input);


void erase_sub_str(string & main_str, const string & to_erase)
{
    size_t pos = main_str.find(to_erase);
    if (pos != string::npos) {
        main_str.erase(pos, to_erase.length());
    }
}

string read_file_to_string(string file_path) {
    ifstream t(file_path);
    string str;

    t.seekg(0, ios::end);   
    str.reserve(t.tellg());
    t.seekg(0, ios::beg);

    str.assign((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());

    return str;
}

double read_file_to_double(string file_path) {
    ifstream ifile(file_path, ios::in);
    double data;
    ifile >> data;
    return data;
}

vector<string> extract_args(char* input) {
    vector<string> info;
    char *token = strtok(input, " "); 
   
    while (token != NULL) {
        info.push_back(token);
        token = strtok(NULL, " ");
    }
    return info;
}

