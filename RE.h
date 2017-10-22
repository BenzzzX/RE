#ifndef RE_H
#define RE_H
#include <string>
#include "SyntaxTree.h"
#include "DFA.h"
namespace RE
{
    class Lex
    {
        DFA dfa;
        const static int max_re_num=100;
        int re_num;
        string re;
        string names[max_re_num];
        typedef void (*action)(string);
        action actions[max_re_num];
        bool inited;
    public:
        Lex():re_num(0),inited(false){re+="(";}
        void Regist(string,action);
        void Init();
        void Run(string);
        void compile();
    };
}
#endif // RE_H
