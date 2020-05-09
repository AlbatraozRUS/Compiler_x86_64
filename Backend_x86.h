#ifndef LANGUAGE_BACKEND_X86_H
#define LANGUAGE_BACKEND_X86_H

#include "Language.h"
#include <map>

#define NLD Node->Left->Elem->Name
#define NRD Node->Right->Elem->Name
#define NN  Node->Elem->Name
#define ND  Node->Elem->ElemData

class Program{
public:
    void Insert(int Value)
    {
        assert(Value >= 0);

        if (Size == MaxSize - 10)
            Add_Mem();

        Data[Size] = (char) Value;
        Size++;
    }

    void Dump()
    {
        fprintf(stderr,
        "\n\n----------------------------------------------------------------------------------------------\n\n");
        printf("Welcome to Dump of Class Program Code\n");
        printf("If you this, you have fucked up. My congratulations!\n");
        printf("Here is the information about codes\n\n\n");
        for (int index = 0; index <= Size; index += 10){
            printf("[%03d]: %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d\n",index,
                   Data[index + 0], Data[index + 1],Data[index + 2], Data[index + 3], Data[index + 4],
                   Data[index + 5], Data[index + 6], Data[index + 7], Data[index + 8], Data[index + 9]);
        }
        printf("\n\nGood luck my friend!\nI hope that we will never meet again!\n\n");
        fprintf(stderr,
        "\n\n----------------------------------------------------------------------------------------------\n\n");
    }

    Program()
    {
        Data = (char*) calloc(100000, sizeof(char));
        MaxSize = 100000;
        Size = 0;
    }

    ~Program()
    {
        free(Data);
        Size = -1;
        MaxSize = -1;
    }

private:
    char * Data = nullptr;
    size_t Size = -1;
    size_t MaxSize = -1;

    void Add_Mem()
    {
        Data = (char*) realloc (Data, MaxSize * 2);
        MaxSize *= 2;
    }
};





void Explore_Tree_x86(Branch* Node);
void My_Switch_x86 (Branch *Node);
void Backend_x86(Branch *Root);
void Math_OP_x86(int ElemData);
void Scan_Variables(Branch *Node, unsigned &Shift, std::map<char*, int> &Variables);
void Mem_For_Var(int NumOfVar);
void add();
void sub();
void mul();
void div();


#endif //LANGUAGE_BACKEND_X86_H
