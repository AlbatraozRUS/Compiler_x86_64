#ifndef LANGUAGE_BACKEND_X86_H
#define LANGUAGE_BACKEND_X86_H

const int MaxFuncs = 10;
const int NoSuchVars = -1;

#include "Language.h"
#include <string>

#define NLN Node->Left->Elem->Name
#define NN  Node->Elem->Name
#define ND  Node->Elem->ElemData
#define NPE Node->Parent->Elem
#define NT  Node->Elem->Type
#define Word 2
#define TBYTE 3
#define DWORD 4

void Optimize();

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

struct ELF_header
{
    const int EI_MAG         = 0x464C457F;
    const char I_CLASS       = 0x02;
    const char EI_DATA       = 0x01;
    const char EI_VERSION    = 0x01;
    const char EI_OSABI      = 0x00;
    const size_t EI_OSABIVER = 0x0000000000000000;
    const short E_TYPE       = 0x0002;
    const short E_MACHINE    = 0x003E;
    const int E_VERSION      = 0x00000001;
    const size_t E_ENTRY     = 0x0000000000400080;
    const size_t E_PHOFF     = 0x0000000000000040;
    const size_t E_SHOFF     = 0x0000000000000000;
    const int E_FLAGS        = 0x00000000;
    const short E_EHSIZE     = 0x0040;
    const short E_PHENTSIZE  = 0x0038;
    const short E_PHNUM      = 0x0001;
    const short E_SHENTSIZE  = 0x0040;
    const short E_SHNUM      = 0x0005;
    const short E_SHSTRNDX   = 0x0004;
};

struct Program_header
{
    const int P_TYPE         = 0x00000001;
    const int P_FLAGS        = 0x00000005;
    const size_t P_OFFSET    = 0x0000000000000000;
    const size_t P_VADDR     =  0x0000000000400000;
    const size_t P_PADDR     = 0x0000000000400000;
    size_t P_FILES;
    size_t P_MEMSZ;
    const size_t P_ALIGN     = 0x0000000000200000;
    const size_t P_SPAC      = 0x0000000000000000;
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

    void Insert(int Value, int Amount_Bytes)
    {
        if (Size == MaxSize - 10)
            Add_Mem();

        for (int Byte = Amount_Bytes - 1; Byte >= 0; Byte--){
            Data[Size++] = (char) (Value >> 8 * Byte);
        }
    }

    int GetCurPos()
    {
        return Size;
    }
    char* GetBuf()
    {
        return Data;
    }

    void Edit_Code(int Pos, int Value, int Amount_Bytes){
        for (int Byte = Amount_Bytes - 1; Byte >= 0; Byte--){
            Data[Pos++] = (char) (Value >> 8 * Byte);
        }
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

    void Fill_Calls()
    {
        for (int NumFunc = 0; NumFunc < Num_Func; NumFunc++) {
            for (int Num_Call = 0; Num_Call < Function[NumFunc].Call_Amount; Num_Call++) {
                Edit_Address(Function[NumFunc].Address - Function[NumFunc].Calls[Num_Call],
                                Function[NumFunc].Calls[Num_Call] - 4);
            }
        }
    }

    void Edit_Address(long int Value, int Pos)
    {
        Data[Pos] = (char) (Value);
        Data[Pos + 1] = (char) (Value >> 8);
        Data[Pos + 2] = (char) (Value >> 16);
        Data[Pos + 3] = (char) (Value >> 24);
    }

    void Exit()
    {
        Insert(0xB83C, Word);               //mov rax, 60
        Insert(0x00);
        Insert(0x00);
        Insert(0x00);

        Insert(0x4831FF, TBYTE);            //xor rdi, rdi

        Insert(0x0F05, Word);               //syscall
    }

    void Make_ELF(FILE *Output)
    {
        struct ELF_header ELF_h;
        struct Program_header Program_h;

        Program_h.P_FILES = Size;
        Program_h.P_MEMSZ = Size;

        fwrite(&ELF_h,     sizeof(ELF_header),     1, Output);
        fwrite(&Program_h, sizeof(Program_header), 1, Output);
    }

    void Write_Down()
    {
        FILE *Output = fopen(Out, "w");
        assert(Output);

        Fill_Calls();

        Make_ELF(Output);

        Optimize();

        fwrite(Data, sizeof(char), Size, Output);

        fclose(Output);
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
void Ret_x86();
void Number_x86(Branch *Node);
void Math_OP_x86(Branch *Node);
void Compare_x86();
void Variables_x86(Branch *Node);
void Assignment_x86(Branch *Node);
void Math_Func_x86();
void Print_x86();
void Scan_x86();
int GetAmountOfExternVars(Branch *Node);
void add();
void sub();
void mul();
void div();


#endif //LANGUAGE_BACKEND_X86_H
