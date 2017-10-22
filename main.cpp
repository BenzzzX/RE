#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <functional>
#include <tuple>
#include <ctime>
#include <string>
#include "RE.h"
using namespace std;
double DoubleDigit(string s)
{
    double numl=0,f=10;
    int i=0,l=s.length();
    while(isdigit(s[i])&&i<l) numl=numl*10+s[i++]-'0';
    if(s[i]=='.')
    {
        i++;
        while(isdigit(s[i])&&i<l)
        {
            numl+=((double)(s[i++]-'0')/f);
            f*=10;
        }
    }
    return(numl*2);
}
RE::Lex L;
int main()
{

    /*clock_t startT,endT;
    L.Regist("[0-9]+(.[0-9]+)?",[](string s){cout<<"<";cout<<DoubleDigit(s);cout<<">";});
    L.Regist("www.[0-9]{5,11}\\@qq.com",[](string s){cout<<"["+s+"]";});
    //L.Regist("[_a-zA-Z][_a-zA-Z0-9]{0,}",[](string s){cout<<"("+s+")";});
    //L.Regist("[a-z]+",[](string s){cout<<"("+s+")";});
    L.Regist("[^]",[](string s){cout<<s;});
    startT=clock();
    //L.Regist("(1|2)*1(1|2){10}",[](string s){cout<<s;});
    L.Init();
    endT=clock();
    cout<<"Init time:"<<(double)(endT-startT)/CLOCKS_PER_SEC<<"\n";
	startT = clock();
    L.Run("sdavas12.36dwql;jdpqiwww.672619410@qq.como21jm21.966vaab651sdsa12feasdwwww.123456789@qq.com12354dd12");
	endT = clock();
	cout << "\nRun time:" << (double)(endT - startT) / CLOCKS_PER_SEC << "\n";*/
	L.Regist("[0-9]+(.[0-9]+)?",[](string s){});
	L.Init();
	L.compile();
	//getchar();
    return 0;
}
