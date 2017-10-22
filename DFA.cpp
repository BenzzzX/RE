
#include "SyntaxTree.h"
#include <algorithm>
#include <iterator>
#include <list>


using namespace RE;

int DFA::move(int s,char c)
{
    return States[s][cmap[c]];
}

int DFA::Run(const char* &i)
{
    int state=0,last_final=-1;
    int u=0,finalI=0;
    for(int next_state=move(state,*(i+u));*(i+u)&&next_state!=-1;u++,next_state=move(state,*(i+u)))
    {

        if(Final[next_state]==-1)
        {
            //cout<<"state["<<state<<"] catch \'"<<*(i+u)<<"\' and tran. to state["<<next_state<<"]\n";
            state=next_state;
        }
        else
        {
            //cout<<"state["<<state<<"] catch \'"<<*(i+u)<<"\' and comp. a Pattern["<<Final[next_state]<<"]\n";
            state=next_state;
            last_final=Final[state];
            finalI=u;
        }
    }
    if(state==-1) u--;
    //cout<<"miss match at:\'"<<*i<<"\'\n";
    //if(last_final==-1) cout<<"miss match at end\n";
    //else cout<<"arrive Pattern["<<last_final<<"]\n";
    if(last_final==-1) return -1;
    i=i+finalI;
    return last_final;
}


void DFA::minimizeDFA(DFA* aim)
{
    static int Num=0;
    struct StateSet
    {
        set<int> sets;
        int Index;
        StateSet():Index(Num++){}
    };
    static StateSet *Innew[Max_States_Num],*Inold[Max_States_Num];
    Num=0;
    StateSet *s1=new StateSet(),*s2=new StateSet();
    list<StateSet*> oldq,newq;
    for(int i=0;i<State_Num;i++)
        if(Final[i]==-1) {s1->sets.insert(i);Innew[i]=s1;}
        else {s2->sets.insert(i);Innew[i]=s2;}
    newq.push_back(s1);newq.push_back(s2);
    while(oldq.size()!=newq.size())
    {
        oldq=newq;
        memcpy(Inold,Innew,sizeof(Innew));
        for(list<StateSet*>::iterator j=oldq.begin();j!=oldq.end();j++)
        {
            int st=*((*j)->sets.begin());
            if((*j)->sets.size()==1) continue;
            set<int> temp;
            for(set<int>::iterator z=(*j)->sets.begin();z!=(*j)->sets.end();z++)
            {
                if(Final[st]!=Final[*z])
                {
                    temp.insert(*z);
                    break;
                }
                for(int i=0;i<cmap.size();i++)
                {
                    char c=cmap.ReverseMap(i);
                    int state=move(st,c);
                    if((state==-1&&move(*z,c)!=-1)||(Inold[state]!=Inold[move(*z,c)]))
                    {
                        temp.insert(*z);
                        break;
                    }
                }
            }
            if(temp.empty()) continue;
            StateSet *n=new StateSet();
            n->sets=temp;
            for(set<int>::iterator z=n->sets.begin();z!=n->sets.end();z++)
            {
                (*j)->sets.erase(*z);
                Innew[*z]=n;
            }
            newq.push_back(n);
        }
    }
    //cout<<"Minimize DFA:\n";
    for(list<StateSet*>::iterator j=oldq.begin();j!=oldq.end();j++)
    {
        int st=*((*j)->sets.begin());
        aim->Final[Inold[st]->Index]=Final[st];
        /***************************************************************************
        cout<<"New ";
        if(Final[st]!=-1) cout<<"Final";
        cout<<"Tag["<<Inold[st]->Index<<"] equal to old Tags[";
        for(set<int>::iterator z=(*j)->sets.begin();z!=(*j)->sets.end();z++) cout<<" "<<*z;
        cout<<" ]\n";
        //*****************************************************************************/
        for(int i=0;i<cmap.size();i++)
        {

            char c=cmap.ReverseMap(i);
            int state=move(st,c);
            if(state==-1) continue;
            /***************************************************************************
            cout<<"New Tran["<<Inold[st]->Index<<"]["<<c<<"]to: "<<Inold[state]->Index<<"\n";
            //***************************************************************************/
            aim->States[Inold[st]->Index][aim->cmap[c]]=Inold[state]->Index;
        }
    }
    aim->State_Num=Num;
    for(list<StateSet*>::iterator j=oldq.begin();j!=oldq.end();j++)
        delete *j;
}

#include <stdio.h>
inline char *Apply2Dst(char *Dst,const char *a)
{
    for(;*a;a++,Dst++) *Dst=*a;
    return Dst;
}

void DFA::compile2C(char *Dst)
{
    char temp[50];
    Dst=Apply2Dst(Dst,"#include <stdio.h>\n");
    for(int i=0;i<State_Num;i++)
    {
        sprintf(temp,"int state%d(char *Dst);\n",i);Dst=Apply2Dst(Dst,temp);
    }
    for(int i=0;i<State_Num;i++)
    {
        sprintf(temp,"int state%d(char *Dst)\n",i);Dst=Apply2Dst(Dst,temp);
        Dst=Apply2Dst(Dst,"{\n");
        if(Final[i]!=-1)
        {
            Dst=Apply2Dst(Dst,"\tprintf(\"matched\");\n"
                               "\treturn 1;\n");
        }
        Dst=Apply2Dst(Dst,"\tif(!*Dst) return 0;\n"
                      "\tswitch(*Dst)\n"
                      "\t{\n");
        for(int c=0;c<Char_Set_Num;c++)
        {
            if(States[i][c]!=-1)
            {
                sprintf(temp,"\tcase '%c':\n",cmap.ReverseMap(c));Dst=Apply2Dst(Dst,temp);
                sprintf(temp,"\t\treturn state%d(Dst+1);\n",States[i][c]);Dst=Apply2Dst(Dst,temp);
            }
        }
        Dst=Apply2Dst(Dst,"\tdefault:\n"
                  "\t\treturn 0;\n"
                  "\t}\n"
                  "\treturn 0;\n"
                  "}\n");
    }
    Dst=Apply2Dst(Dst,"\nint main()\n"
                  "{\n"
                  "\tchar Dst[1000];\n"
                  "\tscanf(\"%s\",Dst);\n"
                  "\tstate0(Dst);\n"
                  "\treturn 0;\n"
                  "}\n");
}

