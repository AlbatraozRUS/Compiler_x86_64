#ifndef LANGUAGE_BACKEND_X86_H
#define LANGUAGE_BACKEND_X86_H

#include "Language.h"
#include <string>

#define NLD Node->Left->Elem->ElemData
#define NRD Node->Right->Elem->ElemData
#define NLN Node->Left->Elem->Name
#define NRN Node->Right->Elem->Name
#define NN  Node->Elem->Name
#define ND  Node->Elem->ElemData
#define NPE Node->Parent->Elem
#define NT  Node->Elem->Type

const int MaxFuncs = 10;
const int NoSuchVars = -1;

struct Vars{
    char *Name = nullptr;
    int  Shift = -1;
};


struct Functions{
public:
    char *Name = nullptr;
    int Address = -1;

    int* Calls = nullptr;
    int Call_Amount = 0;

    struct Vars* Variables = nullptr;
    int Amount_Variables   = 0;

    int Find_Variable(char *Value)
    {
        for (int index = 0; index < Amount_Variables; index++) {
            if (strcmp(Variables[index].Name, Value) == 0) {
                return Variables[index].Shift;
            }
        }
        return NoSuchVars;
    }

};



class Program{
public:
    struct Functions* Function = nullptr;
    int Num_Func = 0;
    int Cur_Func = -1;

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

    void Edit_Address(long int Value, int Pos)
    {
        Data[Pos] = (char) (Value);
        Data[Pos + 1] = (char) (Value >> 8);
        Data[Pos + 2] = (char) (Value >> 16);
        Data[Pos + 3] = (char) (Value >> 24);
    }

    int Which_Func (Branch *Node)
    {
        for (int Func_index = 0; Func_index < Num_Func; Func_index++) {
            if (strcmp(NN, Function[Func_index].Name) == 0)
                return Func_index;
        }
        printf("Function %s was not Found!\n", NN);
        abort();
    }

    void Write_Down()
    {
        FILE *Output = fopen(Out, "w");
        assert(Output);

        Fill_Calls();

        Data[97] = Size % 256;
        Data[98] = Size / 256;
        Data[105] = Size % 256;
        Data[106] = Size / 256;

        fwrite(Data, sizeof(char), Size, Output);

        fclose(Output);
    }

    void Fill_Calls()
    {
        for (int NumFunc = 0; NumFunc < Num_Func; NumFunc++) {
            for (int Num_Call = 0; Num_Call < Function[NumFunc].Call_Amount; Num_Call++) {
                Edit_Address(Function[NumFunc].Address - Function[NumFunc].Calls[Num_Call],
                                Function[NumFunc].Calls[Num_Call] - 4);
            }
        }
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
        Function = (Functions*) calloc(MaxFuncs, sizeof(Functions));
        for (int index = 0; index < MaxFuncs; index++){
            Function[index].Variables = (Vars*) calloc(40, sizeof(Vars));
            Function[index].Calls     = (int *) calloc(100, sizeof(int));
        }
        Data = (char*) calloc(5000, sizeof(char));
        MaxSize = 5000;
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

    void Add_Mem()
    {
        Data = (char*) realloc (Data, MaxSize * 2);
        MaxSize *= 2;
    }
};




void Scan_Function(Branch *Node, int *Shift);
void Find_Functions(Branch *Node);
void Explore_Tree_x86(Branch* Node);
void My_Switch_x86 (Branch *Node);
void Backend_x86(Branch *Root);
void System_OP_Switch_x86(Branch *Node);
void Functions_x86(Branch *Node);
void Ret_x86(Branch *Node);
void Number_x86(Branch *Node);
void Math_OP_x86(Branch *Node);
void Compare_x86();
void Variables_x86(Branch *Node);
void Assignment_x86(Branch *Node);
void Math_Func_x86(Branch *Node);
void Print_x86();
void Scan_x86();
int GetAmountOfExternVars(Branch *Node);
void Make_ELF();
void add();
void sub();
void mul();
void div();


#endif //LANGUAGE_BACKEND_X86_H
