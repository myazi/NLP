#include <iostream>
#include <stdio.h>
#include <string>
#include <math.h>
#include <fstream>
#include <sstream>
#include "matrix.h"
#define VEC_LEN 100000
using namespace std;

struct DICOS
{
    string dic[VEC_LEN];
    int len;
};
DICOS dicos;

void createVocabList(dataToMatrix dtm)
{
    int i,j,k,vl;
    int dic_len=0;
    Data *p;
    p=dtm.dataSet->next;
    for(i=0; i<dtm.col; i++)
    {
        for(j=0; p->attr_string[j]!="#"; j++)
        {
            for(vl=0; vl<dic_len; vl++)
            {
                if(!p->attr_string[j].compare(dicos.dic[vl]))
                    break;
            }
            if(vl==dic_len)
            {
                dicos.dic[vl]=p->attr_string[j];
                dic_len++;
            }
        }
        p=p->next;
    }
    dicos.len=dic_len;
    cout<<"vec_len="<<dic_len<<endl;
}

int Bmm(dataToMatrix testDtm)
{
    int i,j,k;
    Data *p;
    int st;
    int len;
    int num;
    bool flag;
    string tmp;
    p=testDtm.dataSet->next;
    for(i=0; testDtm.col&&p!=NULL; i++)
    {
        len=p->attr_string[0].length();
        st=len;
        flag=false;
        while(st!=0&&!flag)
        {
            flag=true;
            for(num=st>10?10:st; num>0&&flag; num-=1)
            {
                for(k=0; k<dicos.len&&flag; k++)
                {
                    if(!p->attr_string[0].substr(st-num,num).compare(dicos.dic[k]))
                    {
                        tmp.append(p->attr_string[0].substr(st-num,num));
                        //cout<<p->attr_string[0].substr(st-num,num)<<'/';
                        tmp.append(1,'/');
                        st=st-num;
                        flag=false;
                    }
                }
            }
            if(st>10&&flag)
            {
                tmp.append(p->attr_string[0].substr(st-2,2));
                tmp.append(1,'/');
                //cout<<cout<<p->attr_string[0].substr(st-1,2)<<'/';
                st-=2;
                flag=false;
            }
        }
        j=tmp.size();
        for(i=tmp.size()-1; i>0; i--)
        {
            if(tmp[i]=='/')
            {
                cout<<tmp.substr(i+1,j-i);
                j=i;
            }
        }
        cout<<tmp.substr(0,j)<<endl;
        tmp="";
        p=p->next;
    }
}


int Fmm(dataToMatrix testDtm)
{
    int i,j,k;
    Data *p;
    int st;
    int len;
    int num;
    bool flag;
    string tmp;
    p=testDtm.dataSet->next;
    for(i=0; testDtm.col&&p!=NULL; i++)
    {
        st=0;
        len=p->attr_string[0].length();
        flag=false;
        while(st!=len&&!flag)
        {
            flag=true;
            for(num=len-st>10?10:len-st; num>0&&flag; num-=2)
            {
                for(k=0; k<dicos.len&&flag; k++)
                {
                    if(!p->attr_string[0].substr(st,num).compare(dicos.dic[k]))
                    {
                        tmp.append(p->attr_string[0].substr(st,num));
                        tmp.append("/");
                        cout<<p->attr_string[0].substr(st,num)<<'/';
                        st=st+num;
                        flag=false;
                    }
                }
            }
            if(len-st>10&&flag)
            //if()出现为登入词可以采用字构词，即先把其按字划分，后采用字构词
            {
                tmp.append(p->attr_string[0].substr(st,2));
                tmp.append(1,'/');
                cout<<p->attr_string[0].substr(st,2)<<'/';
                st+=2;
                flag=false;
            }
        }
        //cout<<endl;
        //cout<<tmp<<endl;
        tmp="";
        p=p->next;
        cout<<endl;
    }
}
int main()
{

    int k,i,j;
    cout<<"loadData"<<endl;
    char file[20]="data\\dicfile.txt";
    ifstream infile;
    infile.open(file,ios::in);
    string tmpstrline;
    j=0;
    //cout<<"--------------------"<<endl;
    while(!infile.eof())
    {
        getline(infile,tmpstrline,'\n');//读取文件中一行的数据，保存为string类型
        stringstream input(tmpstrline);
        if(tmpstrline!="\0")////由于读取文件结束符同样会继续该操作
        {
            while(input>>dicos.dic[j])
            {
                j++;
            }
        }
    }
    dicos.len=j;
    infile.close();
    //cout<<"load dic ok"<<endl;

    dataToMatrix testDtm;
    //cout<<"loadData"<<endl;
    //cout<<"----------------------"<<endl;
    testDtm.loadData(&testDtm,"data\\stest.txt");
    //cout<<"load test ok"<<endl;
    testDtm.print(testDtm);
    Fmm(testDtm);
    Bmm(testDtm);

}

