#include "Backend_x86.h"
class Program Code;


//TODO sqrt
//TODO Проверить работу адресами для call
//TODO Вызов функций проверить

void Backend_x86(Branch *Root)
{
    Make_ELF();

   // unsigned Shift = 4;
    //Scan_Variables(Root, Shift, Code.Variables);

    Find_Functions(Root);

   // Code.Insert(85); // push rbp

   // Code.Insert(72); //mov rbp, rsp
    //Code.Insert(137);
    //Code.Insert(229);
                        //Проблема с глобальными и локальными переменными
    //Code.Insert(72); //sub rsp, 4 * num of variables
    //Code.Insert(131);
    //Code.Insert(236);
    //Code.Insert(4 * Code.Variables.size());
    //Mem_For_Var(Code.Variables.size());

    Explore_Tree_x86(Root);

//    Code.Insert(72); //mov rsp, rbp
//    Code.Insert(137);
//    Code.Insert(236);

    //Code.Insert(93); // pop rbp

    Code.Write_Down();

    //Code.Dump();

    Code.~Program();


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

//    if (Node->Elem->Name != nullptr){
//        if (strcmp(Node->Elem->Name, "корешок") == 0){
//            Explore_Tree(Node->Left);
//            ASM_MATH_FUNC(Node);
//            return;
//        }
//    }

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

        case MODE_MATH_OP:     {Math_OP_x86(ND);                                         break;}

        case MODE_SYSTEM_OP:   {System_OP_Switch_x86(Node);                              break;}

        case MODE_CONNECTIONS: {                                                         break;}

        case MODE_NUMBER:      {Code.Insert(0x6A); Code.Insert(ND);                       break;}

        case MODE_VARIABLE:    {Variables_x86(Node);                                     break;}

       // case MODE_MATH_FUNC:   {Math_Func_x86(Node);                                     break;}

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

        case ret:     {Variables_x86(NL); Ret_x86(Node);    return;}

        case print:   {Variables_x86(NL); Print_x86();      return;}

        case scan:    {Scan_x86(); Assignment_x86(Node);    return;}

        case If:      {
                       My_Switch_x86(NL->Left);
                       My_Switch_x86(NL->Right);

                       Math_OP_x86(NLD);

                       Code.Insert(0); //Space for Mark`s address
                       Code.Insert(0);
                       Code.Insert(0);
                       Code.Insert(0);

                       long int Pos_1 = Code.GetCurPos();

                       Explore_Tree_x86(NR->Left);


                       Code.Insert(233); //jmp

                       Code.Insert(0); //Space for Mark`s address
                       Code.Insert(0);
                       Code.Insert(0);
                       Code.Insert(0);
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
    //Нельзя присвоить значение одной переменной другой
    int Shift = Code.Function[Code.Cur_Func].Find_Variable(NLN);
    My_Switch_x86(NR);

    Code.Insert(88); // pop rax

    Code.Insert(0x48); // mov [rbp - Shift], rax
    Code.Insert(0x89);
    Code.Insert(0x85);
    Code.Insert(256 - Shift);
    Code.Insert(0xFF);
    Code.Insert(0xFF);
    Code.Insert(0xFF);
}

void Ret_x86(Branch *Node)
{
    Code.Insert(0x58); //pop rax

    Code.Insert(195); //ret
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
    Code.Cur_Func = Code.Which_Func(Node);

    if (Node->Parent->Parent->Parent == nullptr){ // main

        int Shift = Code.Function[Code.Cur_Func].Amount_Variables;

        Code.Insert(0x55); //push rbp

        Code.Insert(0x48); //mov rbp, rsp
        Code.Insert(0x89);
        Code.Insert(0xE5);

        Code.Insert(0x48); // sub rsp, Shift
        Code.Insert(0x83);
        Code.Insert(0xEC);
        Code.Insert(8 * Shift);

        Explore_Tree_x86(NR);

        Code.Insert(0x48); //mov rsp, rbp
        Code.Insert(0x89);
        Code.Insert(0xEC);

        Code.Insert(0x5D); //pop rbp

        return;
    }

    if (Node->Parent->Elem->Type == MODE_CONNECTIONS &&
        Node->Parent->Elem->ElemData == ';') {

        Code.Function[Code.Cur_Func].Address = Code.GetCurPos();

        Code.Insert(0x41); //pop r14
        Code.Insert(0x5E);

        Code.Insert(0x48); //sub rsp, 4 * amount_loc
        Code.Insert(0x83);
        Code.Insert(0xEC);
        Code.Insert(8 * (Code.Function[Code.Cur_Func].Amount_Variables - GetAmountOfExternVars(NL)));

        Code.Insert(0x41); //push r14
        Code.Insert(0x56);

        Code.Insert(0x55); //push rbp

        Code.Insert(0x48); //mov rbp, rsp
        Code.Insert(0x89);
        Code.Insert(0xE5);

        Code.Insert(0x48); //add rbp, 16 + 4 * (amount_loc + amount_extern)
        Code.Insert(0x83);
        Code.Insert(0xC5);
        Code.Insert(16 + 8 * (Code.Function[Code.Cur_Func].Amount_Variables));

        Explore_Tree_x86(NR);

        Code.Insert(0x5D); //pop rbp

        Code.Insert(0x41); // pop r14
        Code.Insert(0x5E);

        Code.Insert(0x48); // add rsp, 4 * (amount_loc + amount_extern)
        Code.Insert(0x83);
        Code.Insert(0xC4);
        Code.Insert(8 * (Code.Function[Code.Cur_Func].Amount_Variables));

        Code.Insert(0x41); // push r14
        Code.Insert(0x56);

        Code.Insert(0xC3); // ret

        return;
    }

    Explore_Tree_x86(NL);
    Code.Insert(232);

    Code.Function[Code.Which_Func(Node)].Calls[Code.Function[Code.Which_Func(Node)].Call_Amount++] = Code.GetCurPos();
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
}

void Variables_x86(Branch *Node)
{
    if (NPE->Type == MODE_SYSTEM_OP && (NPE->ElemData == scan || NPE->ElemData == '='))
        return;

    int Shift = Code.Function[Code.Cur_Func].Find_Variable(NN);

    Code.Insert(255); //push [rbp - Shift]
    Code.Insert(117);
    Code.Insert(256 - Shift);
}

int GetAmountOfExternVars(Branch *Node)
{
    int Sum = 0;

    for (Branch* AnotherNode = Node; AnotherNode != nullptr; AnotherNode = AnotherNode->Left)
        if (AnotherNode->Elem->Type == MODE_VARIABLE)
            Sum++;

    return Sum;
}

void Scan_Variables(Branch *Node, unsigned &Shift, std::map<char*, int> &Variables)
    {
        assert(Shift <= 128);

        if (NL != nullptr)
            Scan_Variables(NL, Shift, Variables);

        if (NR != nullptr)
            Scan_Variables(NR, Shift, Variables);

        if (NL == nullptr && NR == nullptr){
            if (NT == MODE_VARIABLE) {
                if (Variables.find(NN) == Variables.end()) {
                    std::pair<char *, int> temp{NN, Shift};
                    Shift += 8;
                    Variables.insert(temp);
                }
            }
        }
    }


void Mem_For_Var(int NumOfVar)
{
    for (int Shift = -8, index = 0; index < NumOfVar; index++, Shift -= 8) {
        Code.Insert(199); //mov dword ptr [rbp - num], 0
        Code.Insert(69);
        Code.Insert(256 + Shift);
        Code.Insert(0);
        Code.Insert(0);
        Code.Insert(0);
        Code.Insert(0);
    }
}


void Math_OP_x86(int ElemData)
{
    switch (ElemData){

        case '+': {add(); break;}

        case '-': {sub(); break;}

        case '/': {div(); break;}

        case '*': {mul(); break;}

        case '>': {
                   Compare_x86();
                   Code.Insert(15);
                   Code.Insert(135);
                   break;}

        case '=': {
                   Compare_x86();
                   Code.Insert(15);
                   Code.Insert(133);
                   break;}

        case '<': {
                   Compare_x86();
                   Code.Insert(15);
                   Code.Insert(130);
                   break;}


        default: {fprintf(stderr, "Unknown Math operator\n[%d] %c\n", ElemData, ElemData); abort();}

    }
}

void Compare_x86()
{
    Code.Insert(91); //pop rbx
    Code.Insert(88); //pop rax

    Code.Insert(72); //cmp rax, rbx
    Code.Insert(57);
    Code.Insert(216);
}

void Math_Func_x86(Branch *Node)
{
    //Вызов функции квадратного корня и прочего
}


void Scan_x86()
{
    Code.Insert(0x56);
    Code.Insert(0x57);
    Code.Insert(0x41);
    Code.Insert(0x51);
    Code.Insert(0x52);
    Code.Insert(0x55);
    Code.Insert(0x48);
    Code.Insert(0x89);
    Code.Insert(0xE5);
    Code.Insert(0x48);
    Code.Insert(0x83);
    Code.Insert(0xEC);
    Code.Insert(0x08);
    Code.Insert(0xBF);
    Code.Insert(0x02);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x48);
    Code.Insert(0x89);
    Code.Insert(0xE6);
    Code.Insert(0xBA);
    Code.Insert(0x08);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x48);
    Code.Insert(0x31);
    Code.Insert(0xC0);
    Code.Insert(0x0F);
    Code.Insert(0x05);
    Code.Insert(0x48);
    Code.Insert(0xFF);
    Code.Insert(0xCE);
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
    Code.Insert(0x2C);
    Code.Insert(0x30);
    Code.Insert(0x48);
    Code.Insert(0xF7);
    Code.Insert(0xE7);
    Code.Insert(0x49);
    Code.Insert(0x01);
    Code.Insert(0xC1);
    Code.Insert(0x48);
    Code.Insert(0x39);
    Code.Insert(0xE6);
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
    Code.Insert(0xDB);
    Code.Insert(0x4C);
    Code.Insert(0x89);
    Code.Insert(0xC8);
    Code.Insert(0x48);
    Code.Insert(0x83);
    Code.Insert(0xC4);
    Code.Insert(0x08);
    Code.Insert(0x48);
    Code.Insert(0x89);
    Code.Insert(0xEC);
    Code.Insert(0x5D);
    Code.Insert(0x5A);
    Code.Insert(0x41);
    Code.Insert(0x59);
    Code.Insert(0x5F);
    Code.Insert(0x5E);
    Code.Insert(0x50);
}

void Print_x86()// Изменить получение переменной
{
    Code.Insert(0x58);
    Code.Insert(0x55);
    Code.Insert(0x48);
    Code.Insert(0x89);
    Code.Insert(0xE5);
    Code.Insert(0x48);
    Code.Insert(0x83);
    Code.Insert(0xEC);
    Code.Insert(0x08);
    Code.Insert(0x48);
    Code.Insert(0x31);
    Code.Insert(0xC9);
    Code.Insert(0xBB);
    Code.Insert(0x0A);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x48);
    Code.Insert(0x83);
    Code.Insert(0xF8);
    Code.Insert(0x00);
    Code.Insert(0x74);
    Code.Insert(0x1D);
    Code.Insert(0x48);
    Code.Insert(0x31);
    Code.Insert(0xD2);
    Code.Insert(0x48);
    Code.Insert(0xF7);
    Code.Insert(0xF3);
    Code.Insert(0x48);
    Code.Insert(0x39);
    Code.Insert(0xD0);
    Code.Insert(0x74);
    Code.Insert(0x17);
    Code.Insert(0x48);
    Code.Insert(0x83);
    Code.Insert(0xC2);
    Code.Insert(0x30);
    Code.Insert(0x49);
    Code.Insert(0x89);
    Code.Insert(0xE1);
    Code.Insert(0x49);
    Code.Insert(0x01);
    Code.Insert(0xC9);
    Code.Insert(0x49);
    Code.Insert(0x89);
    Code.Insert(0x11);
    Code.Insert(0x48);
    Code.Insert(0xFF);
    Code.Insert(0xC1);
    Code.Insert(0xEB);
    Code.Insert(0xE3);
    Code.Insert(0x48);
    Code.Insert(0x89);
    Code.Insert(0xC2);
    Code.Insert(0xEB);
    Code.Insert(0xE9);
    Code.Insert(0x48);
    Code.Insert(0x83);
    Code.Insert(0xF8);
    Code.Insert(0x00);
    Code.Insert(0x75);
    Code.Insert(0xE3);
    Code.Insert(0xB8);
    Code.Insert(0x01);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x48);
    Code.Insert(0x89);
    Code.Insert(0xE6);
    Code.Insert(0x48);
    Code.Insert(0x01);
    Code.Insert(0xCE);
    Code.Insert(0x48);
    Code.Insert(0xFF);
    Code.Insert(0xCE);
    Code.Insert(0xBF);
    Code.Insert(0x01);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0xBA);
    Code.Insert(0x01);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x00);
    Code.Insert(0x51);
    Code.Insert(0x0F);
    Code.Insert(0x05);
    Code.Insert(0x59);
    Code.Insert(0x48);
    Code.Insert(0xFF);
    Code.Insert(0xC9);
    Code.Insert(0x48);
    Code.Insert(0x83);
    Code.Insert(0xF9);
    Code.Insert(0x00);
    Code.Insert(0x75);
    Code.Insert(0xDB);
    Code.Insert(0x48);
    Code.Insert(0x83);
    Code.Insert(0xC4);
    Code.Insert(0x08);
    Code.Insert(0x48);
    Code.Insert(0x89);
    Code.Insert(0xEC);
    Code.Insert(0x5D);
}


void add()
{
    Code.Insert(88); // pop rax
    Code.Insert(91); // pop rbx

    Code.Insert(72); // add rax, rbx
    Code.Insert(1);
    Code.Insert(216);

    Code.Insert(80); // push rax
}

void sub()
{
    Code.Insert(88); //pop rax
    Code.Insert(91); //pop rbx

    Code.Insert(72); // sub rbx, rax
    Code.Insert(41);
    Code.Insert(195);

    Code.Insert(83); // push rbx
}

void mul()
{
    Code.Insert(88); // pop rax
    Code.Insert(91); // pop rbx

    Code.Insert(72); // mul rbx
    Code.Insert(247);
    Code.Insert(227);

    Code.Insert(83); // push rbx
}

void div()
{
    Code.Insert(91); // pop rbx
    Code.Insert(88); // pop rax

    Code.Insert(82); // push rdx

    Code.Insert(72);// xor rdx, rdx
    Code.Insert(49);
    Code.Insert(210);

    Code.Insert(72); //div rbx
    Code.Insert(247);
    Code.Insert(243);

    Code.Insert(90); //pop rdx
    Code.Insert(83); // push rbx
}

void Make_ELF()
{
    Code.Insert(127);
    Code.Insert(69);
    Code.Insert(76);
    Code.Insert(70);
    Code.Insert(2);
    Code.Insert(1);
    Code.Insert(1);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(2);
    Code.Insert(0);
    Code.Insert(62);
    Code.Insert(0);
    Code.Insert(1);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(128);
    Code.Insert(0);
    Code.Insert(64);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(64);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(64);
    Code.Insert(0);
    Code.Insert(56);
    Code.Insert(0);
    Code.Insert(1);
    Code.Insert(0);
    Code.Insert(64);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(1);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(5);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(64);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(64);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0); //Size
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);  //Size;
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(32);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);
    Code.Insert(0);

}

