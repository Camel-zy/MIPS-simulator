#include"systemsimulation.h"

using namespace std;

//��ʼ���Ĵ�����
void System::InitRegister()
{
    bitset<32> b(0);
    registers.at(0) = Register{ 0, "$zero", b };  // ����$zero
    registers.at(1) = Register{ 1, "at", b };  // ����$at
    for (int i = 2; i < 4; i++) {
        registers.at(i) = Register{ i, "$v" + to_string(i - 2), b };
        // ����$v0-%v1
    }
    for (int i = 4; i < 8; i++) {
        registers.at(i) = Register{ i, "$a" + to_string(i - 4), b };
        // ����$a0-%a3
    }
    for (int i = 8; i < 16; i++) {
        registers.at(i) = Register{ i, "$t" + to_string(i - 8), b };
        // ����$t1-$t7
    }
    for (int i = 16; i < 24; i++) {
        registers.at(i) = Register{ i, "$s" + to_string(i - 16), b };
        // ����$s1-$s7
    }
    for (int i = 24; i < 26; i++) {
        registers.at(i) = Register{ i, "$t" + to_string(i - 16), b };
        // ����$t8-$t9
    }
    for (int i = 26; i < 28; i++) {
        registers.at(i) = Register{ i, "$k" + to_string(i - 26), b };
        // ����$k0-%k1
    }
    registers.at(28) = Register{ 28, "$gp", (bitset <32>)0x10008000 };  // ����$gp
    registers.at(29) = Register{ 29, "$sp", (bitset <32>)0x7ffffffc };  // ����$sp
    registers.at(30) = Register{ 30, "$fp", (bitset <32>)0x7ffffffc };  // ����$fp
    registers.at(31) = Register{ 31, "$ra", b };  // ����$ra
}

//���ݱ���ҵ��Ĵ��������ؼĴ�����
Register& System::FindRegister(int number)
{
    //�����Ĵ�����
    for (auto& i : registers)
        if (i.number == number)
            return i;
    //���Ҳ����򱨴�
    cerr << "�Ĵ���������" << endl;
    exit(-1);
}

//���������ҵ��Ĵ��������ؼĴ�����
Register& System::FindRegister(const string& name)
{
    //�����Ĵ�����
    for (auto& i : registers)
        if (i.name == name)
            return i;
    //���Ҳ����򱨴�
    cerr << "�Ĵ���������" << endl;
    exit(-1);
}

//�����ڴ棬�����ַ�������ڴ��ַ����
bitset<32>& System::AccessMemory(const bitset<32>& address)
{
    //����������ֹ����
    if (address.to_ulong() < 0x00400000)
    {
        cerr << "�ڴ汣�����򣬲��������" << endl;
        exit(-1);
    }
    //����Σ����ػ���ָ��
    else if (address.to_ulong() < 0x10000000)
    {
        if (address.to_ulong() < mem.texttop.to_ulong())
            return mem.text_segment.at(address.to_ulong());
        else
        {
            cerr << "�õ�ַû�д���洢�����������" << endl;
            exit(-1);
        }
    }
    //���ݶΣ����ش洢����
    else if (address.to_ulong() < registers.at(29).value.to_ulong())
    {
        return mem.data_segment.at(address.to_ulong());
    }
    //��ջ�Σ����ض�ջ����
    else if (address.to_ulong() < 0x7fffffff)
    {
        return mem.stack_segment[address.to_ulong()];
    }
    //��ַԽ�籨��
    else
    {
        cerr << "������ڴ��ַ�������ڴ��С" << endl;
        exit(-1);
    }
}

//���ڴ�������ӻ���ָ��
void System::PushCodeToMemory(const bitset<32>& code)
{
    mem.text_segment[mem.texttop.to_ulong()] = code;
    mem.texttop = bitset<32>{ mem.texttop.to_ulong() + 0x04 };
    //�����볬���ڴ����α���
    if (mem.texttop.to_ulong() > 0x10000100)
    {
        cerr << "�������������ڴ����δ�С" << endl;
        exit(-1);
    }
}

void System::PrintSystem() const
{
    //������Ҫ��Ҫ��ͼ�ν���
}