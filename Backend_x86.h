#ifndef LANGUAGE_BACKEND_X86_H
#define LANGUAGE_BACKEND_X86_H

#include "Language.h"
#include <map>

#define NLD Node->Left->Elem->ElemData
#define NRD Node->Right->Elem->ElemData
#define NLN Node->Left->Elem->Name
#define NRN Node->Right->Elem->Name
#define NN  Node->Elem->Name
#define ND  Node->Elem->ElemData
#define NPE Node->Parent->Elem
#define NT  Node->Elem->Type


class Program{
public:
    std::map <char*, int> Variables;

    int AddMark()
    {
        std::pair <int, int> Temp = {Name_Mark++, Size};
        Mark.insert(Temp);

        return Name_Mark - 1;
    }

    void Insert(int Value)
    {
        assert(Value >= 0);

        if (Size == MaxSize - 10)
            Add_Mem();

        Data[Size] = (char) Value;
        Size++;
    }

    int GetCurPos()
    {
        return Size;
    }

    void Edit_Address(int Value, int Pos)
    {
        assert(Value >= 0);

        Data[Pos] = Value % 256;
        Pos++;
        Data[Pos] = Value / 256;
    }

    void Write_Down()
    {
        FILE *Output = fopen(Out, "w");
        assert(Output);

        Exit();

        Data[97] = Size % 256;
        Data[98] = Size / 256;
        Data[105] = Size % 256;
        Data[106] = Size / 256;

        fwrite(Data, sizeof(char), Size, Output);

        fclose(Output);
    }

    void Exit()
    {
        Insert(184); //mov rax, 60
        Insert(60);
        Insert(0);
        Insert(0);
        Insert(0);

        Insert(72); //xor rdi, rdi
        Insert(49);
        Insert(255);

        Insert(15); //syscall
        Insert(5);
    }

    void Dump()
    {
        printf("\n\n---------------------------------------------------------------------------------------------\n\n");
        printf("Welcome to Dump of Class Program Code\n");
        printf("If you see this, you have fucked up. My congratulations!\n");
        printf("Here is the information about codes\n\n");
        printf("Here is ELF_h and Program_h\n\n\n");
        for (int index = 0; index < 80; index += 10){
            printf("[%03d]: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",index,
                   Data[index + 0], Data[index + 1],Data[index + 2], Data[index + 3], Data[index + 4],
                   Data[index + 5], Data[index + 6], Data[index + 7], Data[index + 8], Data[index + 9],
                   Data[index + 10],Data[index + 11], Data[index + 12], Data[index + 13],
                   Data[index + 14], Data[index + 15]);
        }
        printf("\n\nAnd here are the codes of your stupid program!\n\n\n");
        for (int index = 80; index < Size; index += 10){
            printf("[%03d]: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",index,
                   Data[index + 0], Data[index + 1],Data[index + 2], Data[index + 3], Data[index + 4],
                   Data[index + 5], Data[index + 6], Data[index + 7], Data[index + 8], Data[index + 9],
                   Data[index + 10],Data[index + 11], Data[index + 12], Data[index + 13],
                   Data[index + 14], Data[index + 15]);
        }
        printf("\n\nGood luck my friend!\nI hope that we will never meet again!\n\n");
        printf("\n\n--------------------------------------------------------------------------------------------\n\n");
    }

    Program()
    {
        Data = (char*) calloc(100000, sizeof(char));
        MaxSize = 100000;
        Size = 0;
    }

    ~Program()
    {
        //free(Data);
        Size = -1;
        MaxSize = -1;
    }

private:
    char * Data = nullptr;
    size_t Size = -1;
    size_t MaxSize = -1;
    std::map <int, int> Mark;
    unsigned Name_Mark = 1;

    void Add_Mem()
    {
        Data = (char*) realloc (Data, MaxSize * 2);
        MaxSize *= 2;
    }
};





void Explore_Tree_x86(Branch* Node);
void My_Switch_x86 (Branch *Node);
void Backend_x86(Branch *Root);
void System_OP_Switch_x86(Branch *Node);
void Ret_x86(Branch *Node);
void Math_OP_x86(int ElemData);
void Compare_x86();
void Variables_x86(Branch *Node);
void Assignment_x86(Branch *Node);
void Scan_Variables(Branch *Node, unsigned &Shift, std::map<char*, int> &Variables);
void Mem_For_Var(int NumOfVar);
void Math_Func_x86(Branch *Node);
void Print_x86(Branch *Node);
void Make_ELF();
void add();
void sub();
void mul();
void div();


#endif //LANGUAGE_BACKEND_X86_H
