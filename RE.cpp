#define _CRT_SECURE_NO_WARNINGS
#include "RE.h"
#include "stdio.h"
using namespace RE;

void Lex::Regist(string e,Lex::action act)
{
    char temp[6];
    actions[re_num]=act;
    if(re_num>0) re+="|";
    sprintf(temp,")#%d#)",re_num);
    e="(("+e+temp;
    re+=e;
    re_num++;
}
DFA tdfa;
void Lex::Init()
{
    re+=")";

    SyntaxTree tree;
    //cout<<re<<'\n';
    tree.BuildTree(re);
    //tree.DEBUG();
    tree.BuildDFA(&tdfa);
    tdfa.minimizeDFA(&dfa);
    inited=true;
}

void Lex::Run(string s)
{
    if(!inited) return;
    cout<<s<<'\n';
    const char *i,*lsti;
    i=&s[0];
    int P=-1;
    int l=s.length();
    while(*i)
    {
        lsti=i;
        P=dfa.Run(i);
        if(*i) i++;
        if(P!=-1)
            actions[P](s.substr(lsti-&s[0],i-lsti));
        //cout<<*i<<" ";
    }
}

void Lex::compile()
{
    static char C[100000];
    dfa.compile2C(C);
    cout<<C;
}
