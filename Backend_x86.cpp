#include "Backend_x86.h"

class Program Code;


void Backend_x86(Branch *Root)
{
    Find_Functions(Root);

    Explore_Tree_x86(Root);

    Code.Write_Down();

    //Code.Dump();
}


void Explore_Tree_x86(Branch* Node)
{
    assert(Node);

    if (NTYPE == MODE_SYSTEM_OP){

        System_OP_Switch_x86(Node);

        if (NTYPE != MODE_CONNECTIONS &&
            NTYPE != MODE_NIL &&
            Node->Elem->ElemData != oper)

            return;
    }

    if (NTYPE == MODE_FUNCTION){
        Functions_x86(Node);
        return;
    }

    if (Node->Left != nullptr)
        Explore_Tree_x86(Node->Left);

    if (NL == nullptr && NR == nullptr){
        My_Switch_x86(Node);
        return;
    }

    if (Node->Right != nullptr)
        Explore_Tree_x86(Node->Right);

    My_Switch_x86(Node);
}


void My_Switch_x86 (Branch *Node)
{
    assert(Node);

    switch (NTYPE){

        case MODE_MATH_OP:     {Math_OP_x86(Node);                                       break;}

        case MODE_SYSTEM_OP:   {System_OP_Switch_x86(Node);                              break;}

        case MODE_CONNECTIONS: {                                                         break;}

        case MODE_NUMBER:      {Number_x86(Node);                                        break;}

        case MODE_VARIABLE:    {Variables_x86(Node);                                     break;}

        case MODE_MATH_FUNC:   {Math_Func_x86();                                         break;}

        case MODE_NIL:         {                                                         break;}

        case MODE_FUNCTION:    {Functions_x86(Node);                                     break;}


        default: {fprintf(stderr, "Problem occurred in My_Switch_x86, please check input.\n"
                                  "Type - %d\nData - %d", NTYPE, ND); abort();}

    }
}

void System_OP_Switch_x86(Branch *Node)
{
    switch (ND) {

        case if_else: {                                     return;}

        case oper:    {                                     return;}

        case '=':     {Assignment_x86(Node);                return;}

        case ret:     {Variables_x86(NL); Ret_x86();        return;}

        case print:   {Variables_x86(NL); Print_x86();      return;}

        case scan:    {Scan_x86(); Assignment_x86(Node);    return;}

        case While:   {
                        long int Pos_1 = Code.GetCurPos();

                        My_Switch_x86(NL->Left);
                        My_Switch_x86(NL->Right);

                        Math_OP_x86(NL);

                        Code.Insert(0x00); //Space for label address
                        Code.Insert(0x00);
                        Code.Insert(0x00);
                        Code.Insert(0x00);

                        long int Pos_2 = Code.GetCurPos();

                        Explore_Tree_x86(NR);

                        Code.Insert(0xE9); //jmp

                        Code.Insert(0x00); //Space for label address
                        Code.Insert(0x00);
                        Code.Insert(0x00);
                        Code.Insert(0x00);

                        Code.Edit_Address(Pos_1 - Code.GetCurPos(), Code.GetCurPos() - 4);

                        Code.Edit_Address(Code.GetCurPos() - Pos_2, Pos_2 - 4);

                        return;
                        }

        case If:      {
                       My_Switch_x86(NL->Left);
                       My_Switch_x86(NL->Right);

                       Math_OP_x86(NL);

                       Code.Insert(0x00); //Space for label address
                       Code.Insert(0x00);
                       Code.Insert(0x00);
                       Code.Insert(0x00);

                       long int Pos_1 = Code.GetCurPos();

                       Explore_Tree_x86(NR->Left);


                       Code.Insert(0xE9); //jmp

                       Code.Insert(0x00); //Space for label address
                       Code.Insert(0x00);
                       Code.Insert(0x00);
                       Code.Insert(0x00);
                       long int Pos_2 = Code.GetCurPos();

                       Code.Edit_Address(Code.GetCurPos() - Pos_1, Pos_1 - 4);

                       Explore_Tree_x86(NR->Right);

                       Code.Edit_Address(Code.GetCurPos() - Pos_2, Pos_2 - 4);

                       return;}



        default: {fprintf(stderr, "Unknown System operator\n%d\n", ND); abort();}
    }
}


void Assignment_x86(Branch *Node)
{
    int Shift = Code.Function[Code.Cur_Func].Find_Variable(NLN);
    Explore_Tree_x86(NR);
    if (NR->Elem->Type != MODE_FUNCTION)
        Code.Insert(0x58); // pop rax

    Code.Insert(0x488985, TBYTE);   // mov [rbp - Shift], rax
    Code.Insert(256 - Shift);
    Code.Insert(0xFF);
    Code.Insert(0xFF);
    Code.Insert(0xFF);
}

void Ret_x86()
{
    Code.Insert(0x58); //pop rax
}


void Find_Functions(Branch *Node) {
    for (Branch *Another_Node = NR; Another_Node->Left != nullptr; Another_Node = Another_Node->Right) {
        if (Another_Node->Left->Elem->Type == MODE_FUNCTION) {
            Code.Function[Code.Num_Func].Name = Another_Node->Left->Elem->Name;
            int Shift = 8;
            Scan_Function(Another_Node->Left, &Shift);
            Code.Num_Func++;
        }
    }
}

void Scan_Function(Branch *Node, int *Shift)
{
    if (NL != nullptr)
        Scan_Function(NL, Shift);
    if (NR != nullptr)
        Scan_Function(NR, Shift);

    if (NR == nullptr && NL == nullptr){
        if (NT == MODE_VARIABLE &&
        Code.Function[Code.Num_Func].Find_Variable(NN) == NoSuchVars){
            Code.Function[Code.Num_Func].Variables[Code.Function[Code.Num_Func].Amount_Variables].Shift = *Shift;
            Code.Function[Code.Num_Func].Variables[Code.Function[Code.Num_Func].Amount_Variables].Name = NN;
            Code.Function[Code.Num_Func].Amount_Variables++;
            *Shift += 8;
        }
    }
}

void Functions_x86(Branch *Node)
{

    if (Node->Parent->Parent->Parent == nullptr){ // main

        Code.Cur_Func = Code.Which_Func(Node);

        int Shift = Code.Function[Code.Cur_Func].Amount_Variables;

        Code.Insert(0x55); //push rbp

        Code.Insert(0x4889E5, TBYTE); //mov rbp, rsp

        Code.Insert(0x4883EC, TBYTE); // sub rsp, Shift
        Code.Insert(8 * Shift);

        Explore_Tree_x86(NR);

        Code.Insert(0x4889EC, TBYTE); //mov rsp, rbp

        Code.Insert(0x5D); //pop rbp

        Code.Exit();

        return;
    }

    if (Node->Parent->Elem->Type == MODE_CONNECTIONS &&
        Node->Parent->Elem->ElemData == ';') {

        Code.Cur_Func = Code.Which_Func(Node);

        Code.Function[Code.Cur_Func].Address = Code.GetCurPos();

        Code.Insert(0x415E, Word); //pop r14

        Code.Insert(0x4883EC, TBYTE); //sub rsp, 8 * amount_loc
        Code.Insert(8 * (Code.Function[Code.Cur_Func].Amount_Variables - GetAmountOfExternVars(NL)));

        Code.Insert(0x4156, Word); //push r14

        Code.Insert(0x55); //push rbp

        Code.Insert(0x4889E5, TBYTE); //mov rbp, rsp

        Code.Insert(0x4883C5, TBYTE); //add rbp, 16 + 8 * (amount_loc + amount_extern)
        Code.Insert(16 + 8 * (Code.Function[Code.Cur_Func].Amount_Variables));

        Explore_Tree_x86(NR);

        Code.Insert(0x5D); //pop rbp

        Code.Insert(0x415E, Word); //pop r14

        Code.Insert(0x4883C4, TBYTE);  // add rsp, 8 * (amount_loc + amount_extern)
        Code.Insert(8 * (Code.Function[Code.Cur_Func].Amount_Variables));

        Code.Insert(0x4156, Word); //push r14

        Code.Insert(0xC3); // ret

        return;
    }

    Explore_Tree_x86(NL);

    Code.Insert(0xE8); //call

    Code.Insert(0x00); //call address
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);

    Code.Function[Code.Which_Func(Node)].Calls[Code.Function[Code.Which_Func(Node)].Call_Amount++] = Code.GetCurPos();
}

void Variables_x86(Branch *Node)
{
    if (NPE->Type == MODE_SYSTEM_OP && (NPE->ElemData == scan || NPE->ElemData == '='))
        return;

    int Shift = Code.Function[Code.Cur_Func].Find_Variable(NN);

    if (Shift == NoSuchVars) {
        printf("Variable \"%s\" was not found!\n", NN);
        abort();
    }

    Code.Insert(0xFF75, Word); //push [rbp - Shift]
    Code.Insert(256 - Shift);
}

void Number_x86(Branch *Node)
{
    Code.Insert(0x68);
    Code.Insert(ND * 100);
    Code.Insert((100 * ND) >> 8);
    Code.Insert((100 * ND) >> 16);
    Code.Insert((100 * ND) >> 24);
}

int GetAmountOfExternVars(Branch *Node)
{
    int Sum = 0;

    for (Branch* AnotherNode = Node; AnotherNode != nullptr; AnotherNode = AnotherNode->Left)
        if (AnotherNode->Right != nullptr) {
            if (AnotherNode->Right->Elem->Type == MODE_VARIABLE)
                Sum++;
        }
    return Sum;
}

void Math_OP_x86(Branch *Node)
{
    switch (ND){

        case '+': {add();          break;}

        case '-': {sub();          break;}

        case '/': {div();          break;}

        case '*': {mul();          break;}

        case '<': {
                    Compare_x86();

                    Code.Insert(0x0F8F, Word); //jg
                    break;
        }

        case '=': {
                    Compare_x86();

                    Code.Insert(0x0F85, Word); //jne
                    break;}

        case '>': {
                    Compare_x86();

                    Code.Insert(0x0F8C, Word); //jl
                    break;}


        default: {fprintf(stderr, "Unknown Math operator\n[%d] %c\n", ND, ND); abort();}

    }
}

void Compare_x86()
{
   Code.Insert(0x5B); //pop rbx
   Code.Insert(0x58); //pop rax

   Code.Insert(0x4839D8, TBYTE); //cmp rax, rbx
}

void Math_Func_x86()
{
    Code.Insert(0x6A0A, Word); //push 10

    Code.Insert(0x9B); //fwait

    Code.Insert(0xDBE3, Word); //finit

    Code.Insert(0xDB442408, DWORD);  //fild DWORD [rsp]

    Code.Insert(0xD9FA, Word); //fsqrt

    Code.Insert(0x9B);  //fwait

    Code.Insert(0xDA0C24, TBYTE); //fimul DWORD [rsp]

    Code.Insert(0xD9FC, Word); //frndinit

    Code.Insert(0x4883C408, DWORD); //add rsp, 8


    Code.Insert(0x48C70424, DWORD); //mov qword [rsp], 0
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);

    Code.Insert(0xDB1424, TBYTE); //fist DWORD [rsp]
}


void add()
{
    Code.Insert(0x58);                      // pop rax
    Code.Insert(0x5B);                      // pop rbx

    Code.Insert(0x4801D8, TBYTE);           // add rax, rbx


    Code.Insert(0x50);                      //push rax
}

void sub()
{
    Code.Insert(0x5B);                      // pop rbx
    Code.Insert(0x58);                      // pop rax

    Code.Insert(0x4829D8, TBYTE);           // sub rax, rbx

    Code.Insert(0x50);                      // push rax
}

void mul()
{
    Code.Insert(0x6A64, Word);              // push 100

    Code.Insert(0x9B);                      // wait

    Code.Insert(0xDBE3, Word);              // fninit

    Code.Insert(0xDB442410, DWORD);         // fild DWORD [rsp + 16]

    Code.Insert(0xDA4C2408, DWORD);         // fimul DWORD [rsp + 8]

    Code.Insert(0xDA3424, TBYTE);           // fidiv DWORD [rsp]

    Code.Insert(0x4883C410, DWORD);         // add rsp, 16

    Code.Insert(0x48C70424, DWORD); //mov qword [rsp], 0
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);

    Code.Insert(0xDB1424, TBYTE); //fist DWORD [rsp]

    Code.Insert(0x4C631C24, DWORD); //movsx r11, DWORD [rsp]

    Code.Insert(0x4C891C24, DWORD); //mov qword [rsp], r11
}

void div()
{
    Code.Insert(0x6A64, Word); //push 100

    Code.Insert(0x9B); //wait

    Code.Insert(0xDBE3, Word); //fninit

    Code.Insert(0xDB442410, DWORD); //fild DWORD [rsp + 16]

    Code.Insert(0xDA742408, DWORD); //fild DWORD [rsp + 8]

    Code.Insert(0xDA0C24, TBYTE); // fimul DWORD [rsp]

    Code.Insert(0xD9FC, Word); //frndint

    Code.Insert(0x4883C410, DWORD); //add rsp, 16

    Code.Insert(0x48C70424, DWORD); //mov qword [rsp], 0
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);

    Code.Insert(0xDB1424, TBYTE); //fist DWORD [rsp]

    Code.Insert(0x4C631C24, DWORD); //movsx r11, DWORD [rsp]

    Code.Insert(0x4C891C24, DWORD); //mov qword [rsp], r11
}

void Optimize()
{
    char *Data = Code.GetBuf();
    int Size   = Code.GetCurPos();

    for (int Iteration = 0; Iteration < 3; Iteration++){
        for (int Pos = 0; Pos < Size; Pos++){

            if (*(Data + Pos) == 0x50 && *(Data + Pos + 1) == 0x58)             // push rax, pop rax ->
                Code.Edit_Code(Pos, 0x9090, Word);                              // nop, nop

            if (*(Data + Pos + 1) == 0x75 && *(Data + Pos + 3) == 0x58) {       // pop rax
                int Temp = *(char*)(Data + Pos + 2);                            //    |
                Code.Edit_Code(Pos, 0x488B45, TBYTE);                           //    V
                Code.Edit_Code(Pos + 3, Temp, 1);                               // mov rax, QWORD [rbp - Shift]
            }
        }
    }
}



































void Print_x86()
{
    Code.Insert(0x58);
    Code.Insert(0x55);
    Code.Insert(0x48);
    Code.Insert(0x89);
    Code.Insert(0xe5);
    Code.Insert(0x48);
    Code.Insert(0x83);
    Code.Insert(0xec);
    Code.Insert(0x08);
    Code.Insert(0x4d);
    Code.Insert(0x31);
    Code.Insert(0xff);
    Code.Insert(0xbb);
    Code.Insert(0x0a);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x48);
    Code.Insert(0x83);
    Code.Insert(0xf8);
    Code.Insert(0x00);
    Code.Insert(0x74);
    Code.Insert(0x25);
    Code.Insert(0x48);
    Code.Insert(0x3d);
    Code.Insert(0xfe);
    Code.Insert(0xff);
    Code.Insert(0xff);
    Code.Insert(0x7f);
    Code.Insert(0x77);
    Code.Insert(0x22);
    Code.Insert(0x48);
    Code.Insert(0x31);
    Code.Insert(0xd2);
    Code.Insert(0x48);
    Code.Insert(0xf7);
    Code.Insert(0xf3);
    Code.Insert(0x48);
    Code.Insert(0x39);
    Code.Insert(0xd0);
    Code.Insert(0x74);
    Code.Insert(0x65);
    Code.Insert(0x48);
    Code.Insert(0x83);
    Code.Insert(0xc2);
    Code.Insert(0x30);
    Code.Insert(0x49);
    Code.Insert(0x89);
    Code.Insert(0xe1);
    Code.Insert(0x4d);
    Code.Insert(0x01);
    Code.Insert(0xf9);
    Code.Insert(0x41);
    Code.Insert(0x88);
    Code.Insert(0x11);
    Code.Insert(0x49);
    Code.Insert(0xff);
    Code.Insert(0xc7);
    Code.Insert(0xeb);
    Code.Insert(0xe3);
    Code.Insert(0x48);
    Code.Insert(0x89);
    Code.Insert(0xc2);
    Code.Insert(0xeb);
    Code.Insert(0xe9);
    Code.Insert(0x48);
    Code.Insert(0x89);
    Code.Insert(0xc2);
    Code.Insert(0x48);
    Code.Insert(0x31);
    Code.Insert(0xc0);
    Code.Insert(0x48);
    Code.Insert(0x29);
    Code.Insert(0xd0);
    Code.Insert(0x48);
    Code.Insert(0x89);
    Code.Insert(0xe6);
    Code.Insert(0xc7);
    Code.Insert(0x04);
    Code.Insert(0x24);
    Code.Insert(0x2d);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x50);
    Code.Insert(0xb8);
    Code.Insert(0x01);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0xbf);
    Code.Insert(0x01);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0xba);
    Code.Insert(0x01);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x0f);
    Code.Insert(0x05);
    Code.Insert(0x58);
    Code.Insert(0x4d);
    Code.Insert(0x31);
    Code.Insert(0xff);
    Code.Insert(0xeb);
    Code.Insert(0xb3);
    Code.Insert(0xb8);
    Code.Insert(0x01);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0xba);
    Code.Insert(0x01);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0xbf);
    Code.Insert(0x01);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x49);
    Code.Insert(0x89);
    Code.Insert(0xe2);
    Code.Insert(0x4d);
    Code.Insert(0x01);
    Code.Insert(0xea);
    Code.Insert(0x49);
    Code.Insert(0xff);
    Code.Insert(0xc2);
    Code.Insert(0x41);
    Code.Insert(0xc6);
    Code.Insert(0x02);
    Code.Insert(0x2e);
    Code.Insert(0x4c);
    Code.Insert(0x89);
    Code.Insert(0xd6);
    Code.Insert(0x0f);
    Code.Insert(0x05);
    Code.Insert(0xeb);
    Code.Insert(0x0f);
    Code.Insert(0x48);
    Code.Insert(0x83);
    Code.Insert(0xf8);
    Code.Insert(0x00);
    Code.Insert(0x75);
    Code.Insert(0x95);
    Code.Insert(0x4d);
    Code.Insert(0x89);
    Code.Insert(0xfd);
    Code.Insert(0x49);
    Code.Insert(0x83);
    Code.Insert(0xff);
    Code.Insert(0x02);
    Code.Insert(0x74);
    Code.Insert(0xce);
    Code.Insert(0xb8);
    Code.Insert(0x01);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x48);
    Code.Insert(0x89);
    Code.Insert(0xe6);
    Code.Insert(0x4c);
    Code.Insert(0x01);
    Code.Insert(0xfe);
    Code.Insert(0x48);
    Code.Insert(0xff);
    Code.Insert(0xce);
    Code.Insert(0xbf);
    Code.Insert(0x01);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0xba);
    Code.Insert(0x01);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x0f);
    Code.Insert(0x05);
    Code.Insert(0x49);
    Code.Insert(0xff);
    Code.Insert(0xcf);
    Code.Insert(0x49);
    Code.Insert(0x83);
    Code.Insert(0xff);
    Code.Insert(0x00);
    Code.Insert(0x75);
    Code.Insert(0xd7);
    Code.Insert(0x48);
    Code.Insert(0x89);
    Code.Insert(0xe6);
    Code.Insert(0xc7);
    Code.Insert(0x04);
    Code.Insert(0x24);
    Code.Insert(0x0d);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0xb8);
    Code.Insert(0x01);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0xbf);
    Code.Insert(0x01);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0xba);
    Code.Insert(0x01);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x0f);
    Code.Insert(0x05);
    Code.Insert(0xc7);
    Code.Insert(0x04);
    Code.Insert(0x24);
    Code.Insert(0x0a);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x0f);
    Code.Insert(0x05);
    Code.Insert(0x48);
    Code.Insert(0x83);
    Code.Insert(0xc4);
    Code.Insert(0x10);
    Code.Insert(0x48);
    Code.Insert(0x89);
    Code.Insert(0xec);
    Code.Insert(0x5d);
}


void Scan_x86()
{
    Code.Insert(0x56);
    Code.Insert(0x57);
    Code.Insert(0x41);
    Code.Insert(0x51);
    Code.Insert(0x52);
    Code.Insert(0x51);
    Code.Insert(0x55);
    Code.Insert(0x48);
    Code.Insert(0x89);
    Code.Insert(0xE5);
    Code.Insert(0x48);
    Code.Insert(0x83);
    Code.Insert(0xEC);
    Code.Insert(0x10);
    Code.Insert(0xBF);
    Code.Insert(0x02);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x48);
    Code.Insert(0x89);
    Code.Insert(0xE6);
    Code.Insert(0xBA);
    Code.Insert(0x10);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x48);
    Code.Insert(0x31);
    Code.Insert(0xC0);
    Code.Insert(0x0F);
    Code.Insert(0x05);
    Code.Insert(0x48);
    Code.Insert(0x31);
    Code.Insert(0xC9);
    Code.Insert(0x8A);
    Code.Insert(0x16);
    Code.Insert(0x80);
    Code.Insert(0xFA);
    Code.Insert(0x2D);
    Code.Insert(0x75);
    Code.Insert(0x05);
    Code.Insert(0xB9);
    Code.Insert(0x01);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x48);
    Code.Insert(0xFF);
    Code.Insert(0xC6);
    Code.Insert(0x8A);
    Code.Insert(0x16);
    Code.Insert(0x80);
    Code.Insert(0xFA);
    Code.Insert(0x0A);
    Code.Insert(0x75);
    Code.Insert(0xF6);
    Code.Insert(0xBF);
    Code.Insert(0x01);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x4D);
    Code.Insert(0x31);
    Code.Insert(0xC9);
    Code.Insert(0x48);
    Code.Insert(0xFF);
    Code.Insert(0xCE);
    Code.Insert(0x48);
    Code.Insert(0x31);
    Code.Insert(0xC0);
    Code.Insert(0x8A);
    Code.Insert(0x06);
    Code.Insert(0x3C);
    Code.Insert(0x2E);
    Code.Insert(0x74);
    Code.Insert(0xF4);
    Code.Insert(0x2C);
    Code.Insert(0x30);
    Code.Insert(0x48);
    Code.Insert(0xF7);
    Code.Insert(0xE7);
    Code.Insert(0x49);
    Code.Insert(0x01);
    Code.Insert(0xC1);
    Code.Insert(0x48);
    Code.Insert(0x89);
    Code.Insert(0xC8);
    Code.Insert(0x48);
    Code.Insert(0x01);
    Code.Insert(0xE0);
    Code.Insert(0x48);
    Code.Insert(0x39);
    Code.Insert(0xC6);
    Code.Insert(0x74);
    Code.Insert(0x10);
    Code.Insert(0x48);
    Code.Insert(0x89);
    Code.Insert(0xF8);
    Code.Insert(0xBF);
    Code.Insert(0x0A);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x48);
    Code.Insert(0xF7);
    Code.Insert(0xE7);
    Code.Insert(0x48);
    Code.Insert(0x89);
    Code.Insert(0xC7);
    Code.Insert(0xEB);
    Code.Insert(0xD1);
    Code.Insert(0x4C);
    Code.Insert(0x89);
    Code.Insert(0xC8);
    Code.Insert(0x48);
    Code.Insert(0x83);
    Code.Insert(0xF9);
    Code.Insert(0x01);
    Code.Insert(0x75);
    Code.Insert(0x09);
    Code.Insert(0x4D);
    Code.Insert(0x31);
    Code.Insert(0xC9);
    Code.Insert(0x49);
    Code.Insert(0x29);
    Code.Insert(0xC1);
    Code.Insert(0x4C);
    Code.Insert(0x89);
    Code.Insert(0xC8);
    Code.Insert(0x48);
    Code.Insert(0x83);
    Code.Insert(0xC4);
    Code.Insert(0x10);
    Code.Insert(0x48);
    Code.Insert(0x89);
    Code.Insert(0xEC);
    Code.Insert(0x5D);
    Code.Insert(0x59);
    Code.Insert(0x5A);
    Code.Insert(0x41);
    Code.Insert(0x59);
    Code.Insert(0x5F);
    Code.Insert(0x5E);
    Code.Insert(0x50);
}