#ifndef SYNTAXTREE_H
#define SYNTAXTREE_H
#include <iostream>
#include <stack>
#include <vector>
#include <set>
#include <memory.h>
#include <string.h>
#include "DFA.h"

using  namespace std;
namespace RE
{
    class SyntaxTree
    {
        enum symbleID{Alt,Or,Cat,Str,CSet,Star,LB,RB,Val,End,Plus,QM,Null,Rep};
        enum symbleType{LR,L,R,N,V};
        struct symble
        {
            char value;
            int priority;
            symbleType type;
            symbleID ID;
            char IDname[20];
        };
        static int Num;
        struct TreeNode
        {
            TreeNode *L,*R;
            symble value;
            bool nullable;
            int Index;
            set<TreeNode*> firstpos;
            set<TreeNode*> lastpos;
            set<TreeNode*> followpos;
            TreeNode(symble a):value(a),L(nullptr),R(nullptr),Index(Num++){}
            TreeNode(TreeNode* a):value(a->value),Index(Num++),L(nullptr),R(nullptr){}
        }*root;
        symble Analyse(char,bool);
        void Updata(TreeNode*);
        TreeNode* copy(TreeNode*);
        public:
        ~SyntaxTree();
        SyntaxTree():root(nullptr){}
        void BuildTree(string);
        void BuildDFA(DFA*);
        void DEBUG();

    };
}
#endif // SYNTAXTREE_H
