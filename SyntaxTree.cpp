#define _CRT_SECURE_NO_WARNINGS
#include "SyntaxTree.h"
#include <queue>
#include <list>
using namespace RE;
int SyntaxTree::Num=0;

SyntaxTree::symble SyntaxTree::Analyse(char c,bool b)
{
    SyntaxTree::symble result;
    result.value=c;
    if(b)
    {
        result.ID=SyntaxTree::Val;
        result.type=SyntaxTree::V;
        result.priority=-1;
        strcpy(result.IDname,"Val");
        return result;
    }
    switch(c)
    {
        case '|' : result.ID=SyntaxTree::Or;
                result.priority=1; result.type=SyntaxTree::LR;
                strcpy(result.IDname,"Or");break;
        case '@' : result.ID=SyntaxTree::Null;
                result.priority=0; result.type=SyntaxTree::V;
                strcpy(result.IDname,"Null");break;
        case '+' : result.ID=SyntaxTree::Plus;
                result.priority=3; result.type=SyntaxTree::L;
                strcpy(result.IDname,"Plus");break;
        case '?' : result.ID=SyntaxTree::QM;
                result.priority=3; result.type=SyntaxTree::L;
                strcpy(result.IDname,"QM");break;
        case '*' : result.ID=SyntaxTree::Star;
                result.priority=3; result.type=SyntaxTree::L;
                strcpy(result.IDname,"Star");break;
        case '&' : result.ID=SyntaxTree::Cat;
                result.priority=2; result.type=SyntaxTree::LR;
                strcpy(result.IDname,"Cat");break;
        case '(' :
        case '<' : result.ID=SyntaxTree::LB;
                result.priority=0; result.type=SyntaxTree::R;
                strcpy(result.IDname,"LB");break;
        case ')' :
        case '>' : result.ID=SyntaxTree::RB;
                result.priority=10086; result.type=SyntaxTree::L;
                strcpy(result.IDname,"RB");break;
        case '\"':
                result.ID=SyntaxTree::Str;
                result.priority=0; result.type=SyntaxTree::V;
                strcpy(result.IDname,"Str");break;
        case '[':
                result.ID=SyntaxTree::CSet;
                result.priority=0; result.type=SyntaxTree::V;
                strcpy(result.IDname,"CSet");break;
        case '{':
                result.ID=SyntaxTree::Rep;
                result.priority=3; result.type=SyntaxTree::L;
                strcpy(result.IDname,"Rep");break;
        case '#' : result.ID=SyntaxTree::End;
                result.priority=0; result.type=SyntaxTree::V;
                strcpy(result.IDname,"End");break;
        default : result.ID=SyntaxTree::Val;
                result.priority=-1; result.type=SyntaxTree::V;
                strcpy(result.IDname,"Val");break;

    }
    return result;
}

void SyntaxTree::Updata(SyntaxTree::TreeNode* r)
{
    switch(r->value.ID)
    {
        case SyntaxTree::Val:
        case SyntaxTree::End:
            r->firstpos.insert(r);r->lastpos.insert(r);
            r->nullable=false;
            break;
        case SyntaxTree::Null:
            r->nullable=true;
            break;
        case SyntaxTree::Cat:
            r->nullable=r->L->nullable&&r->R->nullable;
            r->firstpos=r->L->firstpos;
            r->lastpos=r->R->lastpos;
            if(r->L->nullable)
                r->firstpos.insert(r->R->firstpos.begin(),r->R->firstpos.end());
            if(r->R->nullable)
                r->lastpos.insert(r->L->lastpos.begin(),r->L->lastpos.end());
            for(auto i=r->L->lastpos.begin();i!=r->L->lastpos.end();i++)
                (*i)->followpos.insert(r->R->firstpos.begin(),r->R->firstpos.end());
            break;
        case SyntaxTree::Or:
            r->nullable=r->L->nullable||r->R->nullable;
            r->firstpos=r->L->firstpos;
            r->lastpos=r->R->lastpos;
            r->firstpos.insert(r->R->firstpos.begin(),r->R->firstpos.end());
            r->lastpos.insert(r->L->lastpos.begin(),r->L->lastpos.end());
            break;
        case SyntaxTree::Star:
            r->nullable=true;
            r->firstpos=r->L->firstpos;
            r->lastpos=r->L->lastpos;
            for(auto i=r->L->lastpos.begin();i!=r->L->lastpos.end();i++)
                (*i)->followpos.insert(r->L->firstpos.begin(),r->L->firstpos.end());
            break;
        case SyntaxTree::Plus:
            r->nullable=false;
            r->firstpos=r->L->firstpos;
            r->lastpos=r->L->lastpos;
            for(auto i=r->L->lastpos.begin();i!=r->L->lastpos.end();i++)
                (*i)->followpos.insert(r->L->firstpos.begin(),r->L->firstpos.end());
            break;
        case SyntaxTree::QM:
            r->nullable=true;
            r->firstpos=r->L->firstpos;
            r->lastpos=r->L->lastpos;
            break;
    }
}




SyntaxTree::TreeNode *SyntaxTree::copy(SyntaxTree::TreeNode* r)
{
    SyntaxTree::TreeNode *n=new SyntaxTree::TreeNode(r);
    if(r->L) n->L=copy(r->L);
    if(r->R) n->R=copy(r->R);
    Updata(n);
    return n;
}


void SyntaxTree::BuildTree(string Reg)
{
    SyntaxTree::Num=0;
    stack<symble> ss;
    stack<TreeNode*> cs;
    symble sym;
    SyntaxTree::symbleType last_symt=SyntaxTree::R;
    stack<int> C_O;
    #define  ExcuteSymble \
    { \
        SyntaxTree::symble sym=ss.top();ss.pop(); \
        if(sym.ID==SyntaxTree::Rep) \
        { \
            char *jj=sym.IDname+4; \
            int mini=0,maxi=0; \
            while((*jj)!=','&&(*jj)) mini=mini*10+(*jj++)-'0'; \
            if(!*jj) maxi=mini; \
            else if(*jj==',')  \
            { \
                jj++; \
                if(!(*jj)) maxi=-1; \
                else while(isdigit(*jj)) maxi=maxi*10+(*jj++)-'0'; \
            } \
            SyntaxTree::TreeNode *r=cs.top(),*root,*temp;cs.pop(); \
            root=r; \
            symble p=Analyse('&',false); \
            if(mini==0) \
            { \
                temp=root; \
                root=new SyntaxTree::TreeNode(Analyse('?',false)); \
                root->L=temp; \
                Updata(root); \
            } \
            for(int j=1;j<mini;j++) \
            { \
                temp=root; \
                root=new SyntaxTree::TreeNode(p); \
                root->L=temp; \
                root->R=copy(r); \
                Updata(root); \
            } \
            if(maxi==-1)  \
            { \
                temp=root; \
                root=new SyntaxTree::TreeNode(p); \
                root->L=temp; \
                temp=new SyntaxTree::TreeNode(Analyse('*',false)); \
                temp->L=copy(r); \
                Updata(temp); \
                root->R=temp; \
                Updata(root); \
            } \
            else \
            { \
                symble pp=Analyse('?',false); \
                for(int j=mini;j<maxi;j++) \
                { \
                    temp=root; \
                    root=new SyntaxTree::TreeNode(p); \
                    root->L=temp; \
                    temp=new SyntaxTree::TreeNode(pp); \
                    temp->L=copy(r); \
                    Updata(temp); \
                    root->R=temp; \
                    Updata(root); \
                } \
            } \
            cs.push(root); \
        } \
        else \
        { \
            SyntaxTree::TreeNode *r=new SyntaxTree::TreeNode(sym);\
            switch(sym.type) \
            { \
                case SyntaxTree::LR: \
                    r->R=cs.top();cs.pop(); \
                    r->L=cs.top();cs.pop(); \
                    break; \
                case SyntaxTree::L: \
                    r->L=cs.top();cs.pop(); \
                    r->R=nullptr; \
                    break; \
            } \
            Updata(r); \
            cs.push(r); \
        } \
    }

    #define pop_stack_when(a) while(!ss.empty()&&a) ExcuteSymble

    for(string::iterator i=Reg.begin();i!=Reg.end();i++,last_symt=sym.type)
    {
        if(*i=='\\')
            sym=Analyse(*(++i),true);
        else sym=Analyse(*i,false);
        if(last_symt!=SyntaxTree::LR&&last_symt!=SyntaxTree::R&&sym.type!=SyntaxTree::L&&sym.type!=SyntaxTree::LR)
        {
            symble p;
            if(C_O.top()==1) p=Analyse('&',false);
            else p=Analyse('|',false);
            pop_stack_when(p.priority<ss.top().priority);
            ss.push(p);
        }
        if(sym.ID==SyntaxTree::Str)
        {

            if(*(++i)!='\"')
            {
                symble p;
                p=Analyse('&',false);
                SyntaxTree::TreeNode *root=new SyntaxTree::TreeNode(Analyse(*(i++),true)),*temp;
                Updata(root);
                while(*i!='\"')
                {
                    temp=root;
                    root=new SyntaxTree::TreeNode(p);
                    root->L=temp;
                    root->R=new SyntaxTree::TreeNode(Analyse(*(i++),true));
                    Updata(root->R);
                    Updata(root);

                }
                cs.push(root);
            }
            else
                cs.push(new SyntaxTree::TreeNode(Analyse('@',false)));
        }
        else if(sym.ID==SyntaxTree::CSet)
        {
            static bool charset[DFA::Char_Range];
            if(*(++i)!=']')
            {

                symble p;
                p=Analyse('|',false);
                if(*i=='^')
                {
                    memset(charset,0,sizeof(charset));
                    i++;char lastc=*i;
                    while(*i!=']')
                    {
                        if(*i=='\\') i++;
                        else if(*i=='-')
                        {
                            i++;
                            //cout<<"Not "<<lastc<<"-"<<*i<<"\n";
                            for(int j=lastc+1;j<*i;j++)
                                charset[lastc=j]=true;
                        }
                        charset[lastc=*(i++)]=true;
                    }
                    int j=0;
                    for(;charset[j]&&j<DFA::Char_Range;j++);
                    SyntaxTree::TreeNode *root=new SyntaxTree::TreeNode(Analyse(j++,true)),*temp;
                    Updata(root);
                    for(;j<DFA::Char_Range;j++)
                        if(!charset[j])
                        {
                            temp=root;
                            root=new SyntaxTree::TreeNode(p);
                            root->L=temp;
                            root->R=new SyntaxTree::TreeNode(Analyse(j,true));
                            Updata(root->R);
                            Updata(root);
                        }
                    cs.push(root);
                }
                else
                {
                    char lastc=*i;
                    SyntaxTree::TreeNode *root=new SyntaxTree::TreeNode(Analyse(*(i++),true)),*temp;
                    Updata(root);
                    while(*i!=']')
                    {
                        if(*i=='\\') i++;
                        else if(*i=='-')
                        {
                            i++;
                            for(int j=lastc+1;j<*i;j++)
                            {
                                temp=root;
                                root=new SyntaxTree::TreeNode(p);
                                root->L=temp;
                                root->R=new SyntaxTree::TreeNode(Analyse(lastc=j,true));
                                Updata(root->R);
                                Updata(root);
                            }
                        }
                        temp=root;
                        root=new SyntaxTree::TreeNode(p);
                        root->L=temp;
                        root->R=new SyntaxTree::TreeNode(Analyse(lastc=*(i++),true));
                        Updata(root->R);
                        Updata(root);

                    }
                    cs.push(root);
                }
            }
            else
                cs.push(new SyntaxTree::TreeNode(Analyse('@',false)));

        }
        else if(sym.ID==SyntaxTree::LB)
        {
            if(sym.value=='(') C_O.push(1);
            else C_O.push(0);
            ss.push(sym);
        }
        else if(sym.ID==SyntaxTree::RB)
        {
            pop_stack_when(ss.top().ID!=LB);
            ss.pop();
            C_O.pop();
        }
        else if(sym.type==V)
        {
            if(sym.ID==SyntaxTree::End)
            {
                i++;
                int len=strlen(sym.IDname);
                sym.IDname[len++]=' ';
                while(*i!='#') sym.IDname[len++]=*(i++);
                sym.IDname[len]='\0';
            }
            SyntaxTree::TreeNode *t=(new SyntaxTree::TreeNode(sym));
            Updata(t);
            cs.push(t);
        }
        else
        {
            if(sym.ID==SyntaxTree::Rep)
            {
                i++;

                int len=strlen(sym.IDname);
                sym.IDname[len++]=' ';
                while(*i!='}') sym.IDname[len++]=*(i++);
                sym.IDname[len]='\0';
            }
            pop_stack_when(sym.priority<=ss.top().priority);
            ss.push(sym);
        }


    }
    root=cs.top();
    #undef ExcuteSymble
    #undef pop_stack_when
}

void SyntaxTree::DEBUG()
{
    stack<SyntaxTree::TreeNode*> s;
    s.push(root);
    while(!s.empty())
    {
        SyntaxTree::TreeNode *r=s.top();s.pop();
        cout<<"TreeNode["<<r->Index<<"]("<<r->value.IDname<<")value:"<<r->value.value;
        if(r->value.ID!=Val&&r->value.ID!=End)
        {
            cout<<" L:"<<r->L->Index<<" R:";
            if(r->R) cout<<r->R->Index;
            else cout<<"Null";
        }
        else cout<<" Leaf";
        cout<<" lastpos:";
        for(auto i=r->lastpos.begin();i!=r->lastpos.end();i++)
            cout<<(*i)->Index<<" ";
        cout<<"firstpos:";
        for(auto i=r->firstpos.begin();i!=r->firstpos.end();i++)
            cout<<(*i)->Index<<" ";
        cout<<"followpos:";
        for(auto i=r->followpos.begin();i!=r->followpos.end();i++)
            cout<<(*i)->Index<<" ";
        cout<<"\n";
        if(r->L) s.push(r->L);
        if(r->R) s.push(r->R);
        /*
        SyntaxTree::TreeNode *r=s.top();s.pop();
        if(!r->followpos.empty())
        {
            cout<<"followpos["<<r->Index<<"]("<<r->value.value<<")={";
            set<SyntaxTree::TreeNode*>::iterator i=r->followpos.begin();
            cout<<(*i)->Index<<"("<<(*i)->value.value<<")";i++;
            for(;i!=r->followpos.end();i++)
                cout <<","<<(*i)->Index<<"("<<(*i)->value.value<<")";
            cout <<"}\n";
            if(r->L) s.push(r->L);
            if(r->R) s.push(r->R);
        }
        */
    }
}

SyntaxTree::~SyntaxTree()
{
    stack<SyntaxTree::TreeNode*> s;
    s.push(root);
    while(!s.empty())
    {
        SyntaxTree::TreeNode *r=s.top();s.pop();
        if(r->L) s.push(r->L);
        if(r->R) s.push(r->R);
        delete r;r=0;
    }
}

template<class a>
bool same_set(set<a> &l,set<a> &r)
{
    if(l.size()!=r.size()) return false;
    typedef typename set<a>::iterator iter;
    for(iter i=l.begin(),j=r.begin();i!=l.end();i++,j++)
        if((*i)!=(*j)) return false;
    return true;
}
void SyntaxTree::BuildDFA(DFA* aim)
{
    static int Num=0;
    struct StateSet
    {
        set<SyntaxTree::TreeNode*> sets;
        int Index;
        StateSet(set<SyntaxTree::TreeNode*>& a):sets(a),Index(Num++){}
    };
    Num=0;
    //cout<<"Build DFA:\n";
    list<StateSet*> qu;
    qu.push_back(new StateSet(root->firstpos));
    for(auto z=qu.begin();z!=qu.end();z++)
    {
        StateSet *t=*z;
        for(auto i=t->sets.begin();i!=t->sets.end();i++)
            if((*i)->value.ID==SyntaxTree::End&&aim->Final[t->Index]==-1)
            {
                char *p=(*i)->value.IDname+4;
                int nu=0;
                while(isdigit(*p)) nu=nu*10+*(p++)-'0';
                aim->Final[t->Index]=nu;break;
            }
        /***********************************************************************
        cout <<"New ";
        if(aim->Final[t->Index]!=-1) cout <<"Final";
        cout <<"Tag["<<t->Index<<"] contains TreeNodes:[";
        for(auto::iterator i=t->sets.begin();i!=t->sets.end();i++)
        cout<<" "<<(*i)->Index;
        cout <<" ]\n";
        //***********************************************************************/
        for(int j=0;j<DFA::Char_Range;j++)
        {
            set<SyntaxTree::TreeNode*> s;
            for(auto i=t->sets.begin();i!=t->sets.end();i++)
                if((*i)->value.ID==SyntaxTree::Val&&(*i)->value.value==j)
                    s.insert((*i)->followpos.begin(),(*i)->followpos.end());
            if(s.empty()) continue;
            StateSet *n=nullptr;
            for(auto h=qu.begin();h!=qu.end();h++)
                if(same_set(s,(*h)->sets))
                    {n=(*h); break;}
            if(!n)
            {
                n=new StateSet(s);
                qu.push_back(n);
            }
            aim->States[t->Index][aim->cmap[j]]=n->Index;
            /***********************************************************************
            cout<<"New Tran:[";
            cout<<t->Index<<"]["<<(char)j<<"]to:"<<n->Index<<"\n";
            //***********************************************************************/
        }
    }
    for(auto z=qu.begin();z!=qu.end();z++)
        delete (*z);
    aim->State_Num=Num;
}




