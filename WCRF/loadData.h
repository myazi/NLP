#include <iostream>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <fstream>
#include <sstream>
#include <stack>
#include <string>
#define ATTR_NUM 2000
using namespace std;
struct Attr_String
{
    string attr;
    Attr_String *next;
};
struct Data
{
    int id;
    Attr_String *Attr_All;//用于字符型属性
    Data *next;
    int len;
};
class dataToMatrix
{
public:
    Data *dataSet;
    int col;
    int row;
public:
    int loadData(dataToMatrix *dtm,char *file)
    {
        int i=0,j=0,pos=0,k,num;
        char ch=' ';
        ifstream infile;
        string tmpstrline;
        Data *p;
        Attr_String *as;
        dtm->dataSet=new Data;
        dtm->dataSet->next=NULL;
        p=dtm->dataSet;
        Data *datatmp;
        Attr_String *astmp;
        dtm->row=0;
        cout<<file<<endl;
        infile.open(file,ios::in);
        while(!infile.eof())
        {
            getline(infile,tmpstrline,'\n');//读取文件中一行的数据，保存为string类型
            stringstream input(tmpstrline);
            if(tmpstrline!="\0")////由于读取文件结束符同样会继续该操作
            {
                datatmp=new Data;
                datatmp->id=i;
                datatmp->next=NULL;
                datatmp->Attr_All=new Attr_String;
                datatmp->Attr_All->next=NULL;
                j=0;
                as=datatmp->Attr_All;
                while(input>>as->attr)
                {
                    pos=as->attr.find(ch,0);

                    as->attr=as->attr.substr(0,pos);
                    if(as->attr=="．")
                    as->attr=".";
                    num=as->attr.size();
                    astmp=new Attr_String;
                    astmp->next=NULL;
                    as->next=astmp;
                    as=as->next;
                    j++;
                }
                datatmp->len=j;
                p->next=datatmp;
                p=p->next;
                dtm->row++;
            }
            i++;
        }
        infile.close();
        return 0;
    }
    int print(dataToMatrix dtm)
    {
        //检测数据加载是否正确
        int i,j;
        Data *p=dtm.dataSet->next;
        Attr_String *as;
        for(i=0; i<dtm.row&&p!=NULL; i++)
        {
            as=p->Attr_All;
            for(j=0; as!=NULL&&j<p->len; j++)
            {
                cout<<as->attr<<"  ";
                as=as->next;
            }
            p=p->next;
            cout<<endl;
        }
        return i;
    }
};
