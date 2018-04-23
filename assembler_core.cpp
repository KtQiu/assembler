#include <assembler_core.h>
int pc;

int reg_ra;

map<string, int> global_loop_address;

void find_loop(istream &inf)
{
    string ass_code;
    int line_num = 0;
    const int n_delimiter = 5;
    char delimiter[10] = "\n ,\t;";
    while (getline(inf, ass_code))
    {
        //	cout << line_num << '\t' ;
        string::size_type i = 0;
        string::size_type j;
        string::size_type temp;
        int k = 0;
        int m = 0;
        int tmp_line = 0;
        string s;
        temp = ass_code.find('#');
        //s是删除了注释的一行代码
        //HACK:split 需要修改
        if (temp != string::npos)
        {
            s = ass_code.substr(0, temp - i);
        }
        else
            s = ass_code;
        string::size_type location_jinghao = s.find(':');
        //HACK:修改万一只有一个“：”出现的时候
        if (location_jinghao != string::npos)
        {
            s.replace(location_jinghao, 1, " ");
            for (m = 0; m < location_jinghao; m++)
            {
                if (isgraph(s[m]))
                    break;
            }
            for (k = location_jinghao - 1; k >= 0; k--)
            {
                if (isgraph(s[k]))
                    break;
            }
            global_loop_address[s.substr(m, k + 1)] = line_num * 4;

            for (tmp_line = location_jinghao; tmp_line < s.length(); tmp_line++)
            {
                if (isgraph(s[tmp_line]))
                {
                    line_num++;
                    break;
                }
            }
        }
        else
        {
            k = 0;
            while (k < s.length())
            {
                if (isgraph(s[k]))
                {
                    line_num++;
                    break;
                }
                k++;
            }
        }
    }
}

//HACK:分割汇编代码中不同的块,但是还缺少了对//的注释的实现，缺少对LOOP的实现
//字符分割函数 遇到#就停止
void split(const string &code, char *delimiter, int n_delimiter, vector<string> &v)
{
    string::size_type i = 0;
    string::size_type j;
    string::size_type temp;
    string s;
    //先除去#， s是除掉了注释的字符串
    temp = code.find('#');
    if (temp != std::string::npos)
        s = code.substr(0, temp);
    else
        s = code;
    temp = s.find(':');
    if (temp != std::string::npos)
        s = s.replace(0, temp + 1, " ");
    //下面除去 delimiter里面的所有出现的字符集合
    j = s.find(*delimiter);
    int cnt = 1;
    while (cnt < n_delimiter)
    {
        temp = s.find(*(delimiter + cnt));
        j = (temp < j ? temp : j);
        cnt++;
    }

    while (j != std::string::npos)
    {
        if (j - i != 0)
            v.push_back(s.substr(i, j - i));
        i = ++j;
        cnt = 0;
        j = s.find(*(delimiter + cnt), i);
        cnt++;
        while (cnt < n_delimiter)
        {
            temp = s.find(*(delimiter + cnt), i);
            j = (temp < j ? temp : j);
            cnt++;
        }
    }
    for (int t = 0; t < n_delimiter; t++)
    {
        if (s[i] != *(delimiter + t))
        {
            v.push_back(s.substr(i, s.length() - i));
            break;
        }
    }
}

int bin2dec(string binary, int length)
{
    int dec_num = 0;
    for (int i = 0; i < length; i++)
    {
        dec_num += pow(binary[i] - '0', i);
    }
    return dec_num;
}

//@descriptions:dec_num: 十进制的数字 bin:输出的二进制的字符串 len_bin:二进制的长度

string dec2bin(int dec_num, int len_bin)
{
    string bin = "";
    for (int i = len_bin - 1; i >= 0; i--, dec_num >>= 1)
        bin += ('0' + (01 & dec_num));
    reverse(bin.begin(), bin.end());
    return bin;
}

//HAVEDONE:R，I，J指令的实现
//TODO:汇编核心代码 基本实现
//使用tolower函数统一小写，然后用map去查找相应的 machine code

string asm2bin(vector<string> &v)
{
    map<string, pair<string, string>> asm2bin_map;
    map<string, string> reg_map;

    asm2bin_map = {
        {"add", {"R", "000000ssssstttttddddd00000100000"}},  //add $d, $s, $t
        {"addu", {"R", "000000ssssstttttddddd00000100001"}}, //addu $d, $s, $t
        {"and", {"R", "000000ssssstttttddddd00000100100"}},  //and $d, $s, $t
        {"sub", {"R", "000000ssssstttttddddd00000100010"}},  //sub $d, $s, $t
        {"subu", {"R", "000000ssssstttttddddd00000100011"}}, //subu $d, $s, $t
        {"and", {"R", "000000ssssstttttddddd00000100100"}},  //and $d, $s, $t
        {"or", {"R", "000000ssssstttttddddd00000100101"}},   //or  $d, $s, $t
        {"xor", {"R", "000000ssssstttttddddd00000100110"}},  // xor $d, $s, $t
        {"slt", {"R", "000000ssssstttttddddd00000101010"}},  // slt $d, $s, $t
        {"sltu", {"R", "000000ssssstttttddddd00000101011"}},
        {"sllv", {"R", "000000ssssstttttddddd00000000100"}},
        {"srlv", {"R", "000000ssssstttttddddd00000000110"}},

        {"addi", {"I", "001000ssssstttttiiiiiiiiiiiiiiii"}},  //addi $t, $s, imm
        {"addiu", {"I", "001001ssssstttttiiiiiiiiiiiiiiii"}}, //addiu $t, $s, imm
        {"andi", {"I", "001100ssssstttttiiiiiiiiiiiiiiii"}},  //andi $t, $s, imm
        {"beq", {"I", "000100ssssstttttiiiiiiiiiiiiiiii"}},   //beq $s, $t, offset
        {"lw", {"I", "100011ssssstttttiiiiiiiiiiiiiiii"}},    //lw $t, offset($s)
        {"lb", {"I", "100000ssssstttttiiiiiiiiiiiiiiii"}},    //lb $t, offset($s)
        {"ori", {"I", "001101ssssstttttiiiiiiiiiiiiiiii"}},
        {"sw", {"I", "101011ssssstttttiiiiiiiiiiiiiiii"}},
        {"bne", {"I", "000101ssssstttttiiiiiiiiiiiiiiii"}},
        {"slti", {"I", "001010ssssstttttiiiiiiiiiiiiiiii"}},
        {"sltiu", {"I", "001011ssssstttttiiiiiiiiiiiiiiii"}},

        {"j", {"J", "000010iiiiiiiiiiiiiiiiiiiiiiiiii"}},   //j target
        {"jal", {"J", "000011iiiiiiiiiiiiiiiiiiiiiiiiii"}}, //jal target
        {"jr", {"J", "000000sssss000000000000000001000"}},  //jr $s

        {"sll", {"S", "00000000000tttttdddddhhhhh000000"}}, //shift operation
        {"srl", {"S", "00000000000tttttdddddhhhhh000010"}}, //
        {"sra", {"S", "00000000000tttttdddddhhhhh000011"}},

    };

    reg_map = {
        {"$zero", "00000"},
        {"$at", "00001"},
        {"$v0", "00010"},
        {"$v1", "00011"},
        {"$a0", "00100"},
        {"$a1", "00101"},
        {"$a2", "00110"},
        {"$a3", "00111"},
        {"$t0", "01000"},
        {"$t1", "01001"},
        {"$t2", "01010"},
        {"$t3", "01011"},
        {"$t4", "01100"},
        {"$t5", "01101"},
        {"$t6", "01110"},
        {"$t7", "01111"},
        {"$s0", "10000"},
        {"$s1", "10001"},
        {"$s2", "10010"},
        {"$s3", "10011"},
        {"$s4", "10100"},
        {"$s5", "10101"},
        {"$s6", "10110"},
        {"$s7", "10111"},
        {"$t8", "11000"},
        {"$t9", "11001"},
        {"$k0", "11010"},
        {"$k1", "11011"},
        {"$gp", "11100"},
        {"$sp", "11101"},
        {"$fp", "11110"},
        {"$ra", "11111"},
    };

    //string bin_code_1_6;
    //变成小写的字母
    transform(v[0].begin(), v[0].end(), v[0].begin(), ::tolower);
    string bin_code = asm2bin_map[v[0]].second;
    string bin_type = asm2bin_map[v[0]].first;
    string reg_s;
    string reg_t;
    string reg_d;

    //cout << bin_code << endl << endl;
    if (bin_type == "R")
    {
        //R型：add $d, $s, $t   000000ssssstttttddddd00000100000
        transform(v[1].begin(), v[1].end(), v[1].begin(), ::tolower);
        transform(v[2].begin(), v[2].end(), v[2].begin(), ::tolower);
        transform(v[3].begin(), v[3].end(), v[3].begin(), ::tolower);
        reg_d = reg_map[v[1]];
        reg_s = reg_map[v[2]];
        reg_t = reg_map[v[3]];
        bin_code.replace(6, 5, reg_s);
        bin_code.replace(11, 5, reg_t);
        bin_code.replace(16, 5, reg_d);
        pc += 4;
    }
    else if (bin_type == "I")
    {
        if (v.size() == 4)
        {
            //I 型的指令	addi $t, $s, imm; # $t = $s + imm 001000sssssttttt iiii iiii iiii iiii 还有bne的类型
            transform(v[1].begin(), v[1].end(), v[1].begin(), ::tolower);
            transform(v[2].begin(), v[2].end(), v[2].begin(), ::tolower);
            reg_t = reg_map[v[1]];
            reg_s = reg_map[v[2]];
            string imm;
            if (v[0] == "bne" || v[0] == "beq")
            {
                int imm_int = pc - global_loop_address[v[3]];
                imm = dec2bin(imm_int, 16);
                //bne beq 的时候PC的存在bug
            }
            else
                imm = dec2bin(stoi(v[3]), 16);
            bin_code.replace(6, 5, reg_s);
            bin_code.replace(11, 5, reg_t);
            bin_code.replace(16, 16, imm);
            pc += 4;
        }
        else if (v.size() == 3)
        {
            //			{ "lw", { "I","100011ssssstttttiiiiiiiiiiiiiiii" } },//lw $t, offset($s)
            transform(v[1].begin(), v[1].end(), v[1].begin(), ::tolower);
            reg_t = reg_map[v[1]];

            int l_bracket = 0;
            int r_bracket = 0;
            l_bracket = v[2].find("(");
            r_bracket = v[2].find(")");
            string reg_temp = v[2].substr(l_bracket, r_bracket + 1);
            string imm_temp = v[2].substr(0, l_bracket + 1);
            reg_s = reg_map[reg_temp];
            string imm = dec2bin(stoi(imm_temp), 16);
            bin_code.replace(6, 5, reg_s);
            bin_code.replace(11, 5, reg_t);
            bin_code.replace(16, 16, imm);
            pc += 4;
        }
    }
    else if (bin_type == "J")
    {
        if ((v[0] == "j") || (v[0] == "jal"))
        {
            // { "j",       {"J","000010iiiiiiiiiiiiiiiiiiiiiiiiii"} },//j target
            int imm_int = pc - global_loop_address[v[1]];
            string imm = dec2bin(imm_int, 26);
            imm = dec2bin(imm_int, 26);
            bin_code.replace(6, 26, imm);
            if (v[0] == "jal")
            {
                reg_ra = pc + 4;
            }
            pc = global_loop_address[v[1]];
        }
        else if (v[0] == "jr")
        {
            //{ "jr", {"J","0000 00ss sss0 0000 0000000000001000"}},//jr $s
            reg_s = reg_map[v[1]];
            bin_code.replace(6, 5, reg_s);
            pc = reg_ra;
        }
    }
    else if (bin_type == "S")
    {
        //{"sll",{"S","00000000000tttttdddddhhhhh000000"}},//shift operation
        reg_t = reg_map[v[1]];
        reg_d = reg_map[v[2]];
        string imm = dec2bin(stoi(v[3]), 5);
        bin_code.replace(11, 5, reg_t);
        bin_code.replace(16, 5, reg_d);
        bin_code.replace(21, 5, imm);
        pc += 4;
    }
    return bin_code;
}

//@description:binary code to assemble
string bin2asm(string bin_code)
{
    string asm_code;
// trim space
    auto itor = remove_if(bin_code.begin(), bin_code.end(), ::isspace);
    bin_code.erase(itor, bin_code.end());

    map<string, string> bin2asm_map;
    map<string, string> reg_map;
    string tag_bin;
    string key_bin = bin_code.substr(0);
    bin2asm_map = {
        {"000000ssssstttttddddd00000100000", "add"},  //add $d, $s, $t
        {"000000ssssstttttddddd00000100001", "addu"}, //addu $d, $s, $t
        {"000000ssssstttttddddd00000100100", "and"},  //and $d, $s, $t
        {"000000ssssstttttddddd00000100010", "sub"},  //sub $d, $s, $t
        {"000000ssssstttttddddd00000100011", "subu"}, //subu $d, $s, $t
        {"000000ssssstttttddddd00000100100", "and"},  //and $d, $s, $t
        {"000000ssssstttttddddd00000100101", "or"},   //or  $d, $s, $t
        {"000000ssssstttttddddd00000100110", "xor"},  // xor $d, $s, $t
        {"000000ssssstttttddddd00000101010", "slt"},  // slt $d, $s, $t
        {"000000ssssstttttddddd00000101011", "sltu"},
        {"000000ssssstttttddddd00000000100", "sllv"},
        {"000000ssssstttttddddd00000000110", "srlv"},

        {"001000ssssstttttiiiiiiiiiiiiiiii", "addi"},  //addi $t, $s, imm
        {"001001ssssstttttiiiiiiiiiiiiiiii", "addiu"}, //addiu $t, $s, imm
        {"001100ssssstttttiiiiiiiiiiiiiiii", "andi"},  //andi $t, $s, imm
        {"000100ssssstttttiiiiiiiiiiiiiiii", "beq"},   //beq $s, $t, offset
        {"100011ssssstttttiiiiiiiiiiiiiiii", "lw"},    //lw $t, offset($s)
        {"100000ssssstttttiiiiiiiiiiiiiiii", "lb"},    //lb $t, offset($s)
        {"001101ssssstttttiiiiiiiiiiiiiiii", "ori"},
        {"101011ssssstttttiiiiiiiiiiiiiiii", "sw"},
        {"000101ssssstttttiiiiiiiiiiiiiiii", "bne"},
        {"001010ssssstttttiiiiiiiiiiiiiiii", "slti"},
        {"001011ssssstttttiiiiiiiiiiiiiiii", "sltiu"},

        {"000010iiiiiiiiiiiiiiiiiiiiiiiiii", "j"},   //j target
        {"000011iiiiiiiiiiiiiiiiiiiiiiiiii", "jal"}, //jal target
        {"000000sssss000000000000000001000", "jr"},  //jr $s

        {"00000000000tttttdddddhhhhh000000", "sll"}, //shift operation sll $d, $t, $d =$t « h
        {"00000000000tttttdddddhhhhh000010", "srl"}, //
        {"00000000000tttttdddddhhhhh000011", "sra"},
    };

    reg_map = {
        {"00000", "$zero"},
        {"00001", "$at"},
        {"00010", "$v0"},
        {"00011", "$v1"},
        {"00100", "$a0"},
        {"00101", "$a1"},
        {"00110", "$a2"},
        {"00111", "$a3"},
        {"01000", "$t0"},
        {"01001", "$t1"},
        {"01010", "$t2"},
        {"01011", "$t3"},
        {"01100", "$t4"},
        {"01101", "$t5"},
        {"01110", "$t6"},
        {"01111", "$t7"},
        {"10000", "$s0"},
        {"10001", "$s1"},
        {"10010", "$s2"},
        {"10011", "$s3"},
        {"10100", "$s4"},
        {"10101", "$s5"},
        {"10110", "$s6"},
        {"10111", "$s7"},
        {"11000", "$t8"},
        {"11001", "$t9"},
        {"11010", "$k0"},
        {"11011", "$k1"},
        {"11100", "$gp"},
        {"11101", "$sp"},
        {"11110", "$fp"},
        {"11111", "$ra"},
    };

    tag_bin = bin_code.substr(0, 6);
    if (tag_bin == "000000" && bin_code.substr(6) == "000000000000000001000")
    {
        string reg_s = reg_map[bin_code.substr(6, 5)];
        string opration = "jr";
        asm_code = opration + " " + reg_s;
    }
    else if (tag_bin == "000000" && bin_code.substr(21, 5) == "00000")
    {
        key_bin.replace(6, 15, "ssssstttttddddd");
        string opration = bin2asm_map[key_bin];
        string reg_s = reg_map[bin_code.substr(6, 5)];
        string reg_t = reg_map[bin_code.substr(11, 5)];
        string reg_d = reg_map[bin_code.substr(16, 5)];
        asm_code = opration + " " + reg_d + " " + reg_s + " " + reg_t;
    }
    else if (tag_bin == "000000")
    {
        key_bin.replace(11, 15, "tttttdddddhhhhh");
        string opration = bin2asm_map[key_bin];
        string reg_t = reg_map[bin_code.substr(11, 5)];
        string reg_d = reg_map[bin_code.substr(16, 5)];
        string imm = to_string(bin2dec(bin_code.substr(21, 5), 5));
        asm_code = opration + " " + reg_d + " " + reg_t + " " + imm;
    }
    // J JAL    {"000010iiiiiiiiiiiiiiiiiiiiiiiiii", "j"},   j target
    // TODO: 难点Ｊ类的指令跳转的位置，用location + num　统一 ,目前用他的jump的立即数表示
    else if (tag_bin == "000010" || tag_bin == "000011")
    {
        key_bin.replace(6, 26, "iiiiiiiiiiiiiiiiiiiiiiiiii");
        string opration = bin2asm_map[key_bin];
        string imm = to_string(bin2dec(bin_code.substr(6), 26));
        asm_code = opration + " " + imm;
    }else
    {
        key_bin.replace(6, 26, "ssssstttttiiiiiiiiiiiiiiii");
        string opration = bin2asm_map[key_bin];
        string imm = to_string(bin2dec(bin_code.substr(16), 16));
        string reg_s = reg_map[bin_code.substr(6, 5)];
        string reg_t = reg_map[bin_code.substr(11, 5)];
        asm_code = opration + " " + reg_t + " " + reg_s + " " + imm;
    }
    return asm_code;
}

// TODO: PC的地址的计算

// TODO:simulation

// TODO:文件的读取和保存的图形操作 crtl+o ...

// TODO:代码的高亮实现

// TODO: 错误代码的提醒，简单的debug功能
