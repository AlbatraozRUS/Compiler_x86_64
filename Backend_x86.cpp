#include "Backend_x86.h"
class Program Code;


void Backend_x86(Branch *Root)
{
    unsigned Shift = 4;
    std::map <char*, int> Variables;
    Scan_Variables(Root, Shift, Variables);

    Code.Insert(85); // push rbp

    Code.Insert(72); //mov rbp, rsp
    Code.Insert(137);
    Code.Insert(229);

    Code.Insert(72); //sub rsp, 4 * num of variables
    Code.Insert(131);
    Code.Insert(4 * Variables.size());
    Mem_For_Var(Variables.size());

    Explore_Tree_x86(Root);

    Code.Insert(72); //mov rsp, rbp
    Code.Insert(137);
    Code.Insert(236);

    Code.Insert(93); // pop rbp

    Code.~Program();
}


void Explore_Tree_x86(Branch* Node)
{
    assert(Node);

    if (NTYPE == MODE_SYSTEM_OP){

        //ASM_SYSTEM_OP(Node, ASM_Out);

        if (NTYPE != MODE_CONNECTIONS &&
            NTYPE != MODE_NIL &&
            Node->Elem->ElemData != oper)

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

        //case MODE_SYSTEM_OP:   {ASM_SYSTEM_OP(Node, ASM_Out);                            break;}

        case MODE_CONNECTIONS: {                                                         break;}

        case MODE_NUMBER:      {Code.Insert(106); Code.Insert(ND);                 break;}

        //case MODE_VARIABLE:    {ASM_VARIABLES(Node, ASM_Out);                            break;}

        //case MODE_MATH_FUNC:   {ASM_MATH_FUNC(Node, ASM_Out);                            break;}

        case MODE_NIL:         {                                                         break;}

        //case MODE_FUNCTION:    {ASM_FUNCTION(Node, ASM_Out);                             break;}


        default: {fprintf(stderr, "Problem occurred in My_Switch, please check input.\n"
                                  "Type - %d\nData - %d", NTYPE, ND); abort();}

    }
}


void Scan_Variables(Branch *Node, unsigned &Shift, std::map<char*, int> &Variables)
    {
        if (NL != nullptr)
            Scan_Variables(NL, Shift, Variables);

        if (NR != nullptr)
            Scan_Variables(NR, Shift, Variables);

        if (NLD == nullptr && NRD == nullptr){
            if (Variables.find(NN) == Variables.end()){
                std::pair <char*, int> temp {NN, Shift};
                Shift += 4;
                Variables.insert(temp);
            }
        }
    }


void Mem_For_Var(int NumOfVar)
{
    for (int Shift = -4, index = 0; index < NumOfVar; index++, Shift -= 4) {
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

        //case '>': {fprintf(ASM_Out, "Ja ");   break;}

        //case '=': {fprintf(ASM_Out, "Neq ");   break;}

        //case '<': {fprintf(ASM_Out, "Jb ");   break;}


        default: {fprintf(stderr, "Unknown Math operator\n[%d] %c\n", ElemData, ElemData); abort();}

    }
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
