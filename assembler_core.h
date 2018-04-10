#ifndef ASSEMBLER_CORE_H
#define ASSEMBLER_CORE_H


#include <iostream>
#include "vector"
#include "map"
#include "string"
#include "algorithm"
#include "fstream"

using namespace std;

void find_loop(istream & inf);

void split(const string & code, char * delimiter, int n_delimiter, vector<string>& v);

int bin2dec_5bit(string array);

string dec2bin(int dec_num, int len_bin);

string asm2bin(vector<string>& v);

string bin2asm(vector<string> &v);

extern int pc;

extern int reg_ra;

extern map<string, int> global_loop_address;

#endif // ASSEMBLER_CORE_H
