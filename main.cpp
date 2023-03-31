#include <iostream>
#include <sstream>
#include "map"
#include "string"
#include "vector"
#include "iterator"
#include "bitset"

using namespace std;

// 寄存器
class Register {
public:
    int number;
    string name;
    bitset<32> value;

    Register(int number, const string &name, const bitset<32> &value) : number(number), name(name), value(value) {}

    void Reset(int number, string name) {
        this->number = number;
        this->name = name;
    }
};

class Instruction {
public:
    bitset<32> address;// 指令地址
    vector<string> assemblecode;// 汇编代码（按空格分割成数组并去掉逗号）
    bitset<32> machinecode;//机器码
    Instruction(const bitset<32> &address, const vector<string> &assemblecode, const bitset<32> &machinecode) : address(
            address), assemblecode(assemblecode), machinecode(machinecode) {};
};

// 记录指令数量
unsigned int Count = 0;
const unsigned int StartAddress = 4194304;//0x00400000,指令的初始地址
// 要处理的所有指令
string allinstructions[] = {"add", "addi", "sub",
                            "and", "or", "sll",
                            "j", "jr", "lw",
                            "sw", "beq"};

vector<Register> registers;
vector<Instruction> instructions;
vector<pair<bitset<32>, string>> Lables;//标签库，存储标签的地址及字符串

void InitRegister();//初始化寄存器
// 读取汇编码并按空格分隔成vector<string>并去除逗号，
// 如果第一个子字符串不是要处理的指令码，判断其为标签，去除并加入标签库，处理后的结果插入Instruction
void InputInsToMap();

void Compilation();//汇编的主要程序
void Add(vector<string> code, unsigned int i);

void Addi(vector<string> code, unsigned int i);

void Sub(vector<string> code, unsigned int i);

void Sll(vector<string> code, unsigned int i);

void And(vector<string> code, unsigned int i);

void Or(vector<string> code, unsigned int i);

void J(vector<string> code, unsigned int i);

void Jr(vector<string> code, unsigned int i);

void Lw(vector<string> code, unsigned int i);

void Sw(vector<string> code, unsigned int i);

void Beq(vector<string> code, unsigned int i);

void PrintBinaryAddress();

void PrintHexAddress();

int main() {
    InitRegister();// 初始化寄存器
    InputInsToMap();
    Compilation();
    PrintBinaryAddress();
    //PrintHexAddress();
    return 0;
}

void InitRegister() {
    bitset<32> b(0);
    Register temp(0, "$zero", b);
    registers.push_back(temp);// 插入$zero
    temp.Reset(1, "at");
    registers.push_back(temp);// 插入$at
    temp.Reset(2, "$v0");
    registers.push_back(temp);// 插入$v0
    temp.Reset(3, "$v1");
    registers.push_back(temp);// 插入$v1
    for (int i = 4; i < 8; i++) {
        temp.Reset(i, "$a" + to_string(i - 4));
        registers.push_back(temp);;// 插入$a
    }
    for (int i = 8; i < 16; i++) {
        temp.Reset(i, "$t" + to_string(i - 8));
        registers.push_back(temp);;// 插入$t
    }
    for (int i = 16; i < 24; i++) {
        temp.Reset(i, "$s" + to_string(i - 16));
        registers.push_back(temp);;// 插入$s
    }
    for (int i = 24; i < 26; i++) {
        temp.Reset(i, "$t" + to_string(i - 16));
        registers.push_back(temp);;// 插入$t
    }
    for (int i = 26; i < 28; i++) {
        temp.Reset(i, "$k" + to_string(i - 26));
        registers.push_back(temp);;// 插入$k
    }
    temp.Reset(28, "$gp");
    registers.push_back(temp);;// 插入$gp
    temp.Reset(29, "$sp");
    registers.push_back(temp);;// 插入$sp
    temp.Reset(30, "$fp");
    registers.push_back(temp);;// 插入$fp
    temp.Reset(31, "$ra");
    registers.push_back(temp);;// 插入$ra
}

// 读取汇编码并按空格分隔成vector<string>并去除逗号，处理标签
void InputInsToMap() {
    string temp;
    while (true) {// 读取指令并插入，地址统一从0开始
        getline(cin, temp, '\n');
        if (temp.empty())
            break;
        string result;// 临时存放分割后的子字符串
        vector<string> words;
        stringstream iss(temp);
        int pos;//','的位置
        while (iss >> result) {// 按空格分隔后push入vector
            if ((pos = result.find(',')) >= 0)
                result.erase(pos);
            words.push_back(result);
        }
        // 如果第一个字符串不是任何我们要处理的指令字符串，将其作为标签处理
        if (count(begin(allinstructions), end(allinstructions), words.at(0)) <= 0) {
            // 将标签加入标签库并在原指令开头中去除
            words[0].pop_back();//去掉标签末尾的':'
            Lables.emplace_back(StartAddress + Count * 4, words.at(0));
            words.erase(words.begin());
        }
        Instruction tempins(bitset<32>(StartAddress + (Count++) * 4), words, bitset<32>(0));
        instructions.push_back(tempins);
    }

}

// 汇编的主程序
void Compilation() {
    for (int i = 0; i < Count; i++) {
        if (instructions.at(i).assemblecode.at(0) == "add") {
            Add(instructions.at(i).assemblecode, i);
            continue;
        }
        if (instructions.at(i).assemblecode.at(0) == "addi") {
            Addi(instructions.at(i).assemblecode, i);
            continue;
        }
        if (instructions.at(i).assemblecode.at(0) == "sub") {
            Sub(instructions.at(i).assemblecode, i);
            continue;
        }
        if (instructions.at(i).assemblecode.at(0) == "sll") {
            Sll(instructions.at(i).assemblecode, i);
            continue;
        }
        if (instructions.at(i).assemblecode.at(0) == "and") {
            And(instructions.at(i).assemblecode, i);
            continue;
        }
        if (instructions.at(i).assemblecode.at(0) == "or") {
            Or(instructions.at(i).assemblecode, i);
            continue;
        }
        if (instructions.at(i).assemblecode.at(0) == "j") {
            J(instructions.at(i).assemblecode, i);
            continue;
        }
        if (instructions.at(i).assemblecode.at(0) == "jr") {
            Jr(instructions.at(i).assemblecode, i);
            continue;
        }
        if (instructions.at(i).assemblecode.at(0) == "lw") {
            Lw(instructions.at(i).assemblecode, i);
            continue;
        }
        if (instructions.at(i).assemblecode.at(0) == "sw") {
            Sw(instructions.at(i).assemblecode, i);
            continue;
        }
        if (instructions.at(i).assemblecode.at(0) == "beq") {
            Beq(instructions.at(i).assemblecode, i);
            continue;
        }
    }
}

// 根据寄存器名字找到编号
int FindNumByName(const string &name) {
    for (auto &i: registers) {
        if (i.name == name)
            return i.number;
    }
    return -1;// 未找到
}

// 根据寄存器名字找到值
bitset<32> FindValueByName(const string &name) {
    for (auto &i: registers) {
        if (i.name == name)
            return i.value;
    }
    return NULL;// 未找到
}

// 根据标签名找到对应的地址
bitset<32> FindAddressByName(const string &name) {
    for (auto &i: Lables) {
        if (i.second == name)
            return i.first;
    }
    return NULL;// 未找到
}

//add rd, rs, rt
void Add(vector<string> code, unsigned int i) {
    int rd = FindNumByName(code.at(1));
    int rs = FindNumByName(code.at(2));
    int rt = FindNumByName(code.at(3));
    unsigned int decimal = 32 + rd * 2048 + rt * 65536 + rs * 2097152;
    bitset<32> result(decimal);// 二进制的机器码
    instructions.at(i).machinecode = result;
}

void Addi(vector<string> code, unsigned int i) {
    int rt = FindNumByName(code.at(1));
    int rs = FindNumByName(code.at(2));
    int imm = stoi(code.at(3));
    unsigned int decimal;
    if (imm >= 0) {
        decimal = imm + rt * 65536 + rs * 2097152 + 536870912;
    } else {
        decimal = 65536 + imm + rt * 65536 + rs * 2097152 + 536870912;
    }

    bitset<32> result(decimal);// 二进制的机器码
    instructions.at(i).machinecode = result;
}

void Sub(vector<string> code, unsigned int i) {
    int rd = FindNumByName(code.at(1));
    int rs = FindNumByName(code.at(2));
    int rt = FindNumByName(code.at(3));
    unsigned int decimal = 34 + rd * 2048 + rt * 65536 + rs * 2097152;
    bitset<32> result(decimal);// 二进制的机器码
    instructions.at(i).machinecode = result;
}

void Sll(vector<string> code, unsigned int i) {
    int rd = FindNumByName(code.at(1));
    int rt = FindNumByName(code.at(2));
    int shame = stoi(code.at(3));
    unsigned int decimal = shame * 64 + rd * 2048 + rt * 65536;
    bitset<32> result(decimal);// 二进制的机器码
    instructions.at(i).machinecode = result;
}

void And(vector<string> code, unsigned int i) {
    int rd = FindNumByName(code.at(1));
    int rs = FindNumByName(code.at(2));
    int rt = FindNumByName(code.at(3));
    unsigned int decimal = 36 + rd * 2048 + rt * 65536 + rs * 2097152;
    bitset<32> result(decimal);// 二进制的机器码
    instructions.at(i).machinecode = result;
}

void Or(vector<string> code, unsigned int i) {
    int rd = FindNumByName(code.at(1));
    int rs = FindNumByName(code.at(2));
    int rt = FindNumByName(code.at(3));
    unsigned int decimal = 37 + rd * 2048 + rt * 65536 + rs * 2097152;
    bitset<32> result(decimal);// 二进制的机器码
    instructions.at(i).machinecode = result;
}

void J(vector<string> code, unsigned int i) {
    bitset<32> address = FindAddressByName(code.at(1));// 二进制的标签地址
    // 如果指令后的地址是用标签表示的,address不会为null
    if (address == NULL) {// 十进制地址值跳转
        address = bitset<32>(stoi(code.at(1)));
    }
    address = address >> 2;// 右移两位
    int result = address.to_ulong();
    result = result + 134217728;
    instructions.at(i).machinecode = bitset<32>(result);
}

void Jr(vector<string> code, unsigned int i) {
    int rs = FindNumByName(code.at(1));
    unsigned int decimal = 8 + rs * 2097152;
    bitset<32> result(decimal);// 二进制的机器码
    instructions.at(i).machinecode = bitset<32>(result);
}

//lw $t0, 0($t1)
void Lw(vector<string> code, unsigned int i) {
    int rt = FindNumByName(code.at(1));
    int startpos = code.at(2).find_first_of('(');// 找出第一个左括号的位置，进而分理出rs
    int endpos = code.at(2).find_last_of(')');// 找出第一个左括号的位置，进而分理出rs
    string rs_s = code.at(2).substr(startpos + 1, endpos - startpos - 1);
    int rs = FindNumByName(rs_s);
    int offset = stoi(code.at(2).substr(0, startpos));
    unsigned int decimal;
    if (offset >= 0) {
        decimal = offset + rt * 65536 + rs * 2097152 + 2348810240;
    } else {
        decimal = 65536 + offset + rt * 65536 + rs * 2097152 + 2348810240;
    }
    bitset<32> result(decimal);// 二进制的机器码
    instructions.at(i).machinecode = bitset<32>(result);
}

void Sw(vector<string> code, unsigned int i) {
    int rt = FindNumByName(code.at(1));
    int startpos = code.at(2).find_first_of('(');// 找出第一个左括号的位置，进而分理出rs
    int endpos = code.at(2).find_last_of(')');// 找出第一个左括号的位置，进而分理出rs
    string rs_s = code.at(2).substr(startpos + 1, endpos - startpos - 1);
    int rs = FindNumByName(rs_s);
    int offset = stoi(code.at(2).substr(0, startpos));
    unsigned int decimal;
    if (offset >= 0) {
        decimal = offset + rt * 65536 + rs * 2097152 + 2885681152;
    } else {
        decimal = 65536 + offset + rt * 65536 + rs * 2097152 + 2885681152;
    }
    bitset<32> result(decimal);// 二进制的机器码
    instructions.at(i).machinecode = bitset<32>(result);
}

void Beq(vector<string> code, unsigned int i) {
    int rs = FindNumByName(code.at(1));
    int rt = FindNumByName(code.at(2));
    bitset<32> label = FindAddressByName(code.at(3));// 标签对应的二进制地址值
    if (label != NULL) {
        long dec_label = label.to_ulong();
        long pc = instructions.at(i).address.to_ulong();
        int offset = (dec_label - pc - 4) / 4;
        unsigned int decimal;
        if (offset >= 0) {
            decimal = offset + rt * 65536 + rs * 2097152 + 268435456;
        } else {
            decimal = 65536 + offset + rt * 65536 + rs * 2097152 + 268435456;
        }

        bitset<32> result(decimal);// 二进制的机器码
        instructions.at(i).machinecode = bitset<32>(result);
    }
}

void PrintBinaryAddress() {
    for (auto &instruction: instructions) {
        cout << instruction.machinecode << endl;
    }
}

void PrintHexAddress() {
    for (auto &instruction: instructions) {
        cout << hex << instruction.machinecode.to_ulong() << endl;
    }
}





