#pragma once
#include<iostream>
#include<vector>
#include<map>
#include<utility>
#include<string>
#include<bitset>
#include<cstdlib>

//�Ĵ���
class Register
{
public:
    int number;//���
    std::string name;//����
    std::bitset<32> value;//�洢����

    //Ĭ�Ϲ��캯��
    Register() : number{ -1 } {}

    //���ι��캯����������ֵ
    Register(int number, const std::string& name, const std::bitset<32>& value) : number{ number }, name{ name }, value{ value } {}

    //���ι��캯����������ֵ
    Register(int number, std::string&& name, std::bitset<32>&& value) : number{ number }, name{ std::move(name) }, value{ std::move(value) } {}

    //�������üĴ���
    void Reset(int number, const std::string& name)
    {
        this->number = number;
        this->name = name;
    }
};

//�ڴ�
class Memory
{
public:
    //�ڴ���map<��ַ,�洢����>��ӳ����ģ��
    std::map<unsigned long, std::bitset<32>> text_segment;//����Σ�������룬��ַ��0x00400000��ʼ��������
    std::map<unsigned long, std::bitset<32>> data_segment;//���ݶΣ����澲̬���ݣ���ַ��0x10000000��ʼ��������
    std::map<unsigned long, std::bitset<32>> stack_segment;//��ջ�Σ���ַ��0x7ffffffc��ʼ���µݼ�

    //ָ�����εĿ��ڴ����С��ַ
    std::bitset<32> texttop;

    //Ĭ�Ϲ��캯������ʼ���ڴ��ַ
    Memory() :text_segment{ make_pair(0x00400000,std::bitset<32>{}) },
        data_segment{ make_pair(0x10000000,std::bitset<32>{}) },
        stack_segment{ make_pair(0x7ffffffc,std::bitset<32>{}) }, texttop{ 0x00400000 } {};
};

//ϵͳ
class System
{
public:
    Memory mem;//�ڴ�
    std::vector<Register> registers{ 32 };//�Ĵ�����
    Register PC;//���������

    //Ĭ�Ϲ��캯��
    System()
    {
        PC.value = std::bitset <32>{ 0x00400000 };
        InitRegister();
    }

private:
    //��ʼ���Ĵ�����
    void InitRegister();

public:
    //���ݱ���ҵ��Ĵ��������ؼĴ�����
    Register& FindRegister(int number);

    //���������ҵ��Ĵ��������ؼĴ�����
    Register& FindRegister(const std::string& name);

    //�����ڴ棬�����ַ�������ڴ��ַ����
    std::bitset<32>& AccessMemory(const std::bitset<32>& address);

    //���ڴ�������ӻ���ָ��
    void PushCodeToMemory(const std::bitset<32>& code);

    //��ʾ�ڴ�״̬
    void PrintSystem() const;
};

