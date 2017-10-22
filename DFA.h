#ifndef DFA_H
#define DFA_H
#include <iostream>
#include <stack>
#include <vector>
#include <set>
#include <memory.h>
#include <string.h>
#include "CharMap.h"

using  namespace std;
namespace RE
{
    struct DFA
    {
        static const int Max_States_Num=20000;
        static const int Char_Set_Num=128;
        static const int Char_Range=128;
        CharMap cmap;
        int State_Num;
        int States[Max_States_Num][Char_Set_Num];
        int Final[Max_States_Num];
        DFA()
        {
            memset(States,-1,sizeof(States));
            memset(Final,-1,sizeof(Final));
            State_Num=0;
        }
        int move(int,char);
        int Run(const char* &);
        void minimizeDFA(DFA*);
        void compile2C(char*);
    };

}
#endif // DFA_H
