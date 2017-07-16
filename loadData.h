#include <iostream>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <fstream>
#include <sstream>
#include <stack>
#include <string>
using namespace std;
#define ATTR_NUM 1500

struct Data
{
    //可以用共用体类型
    int id;
    string attr_string[ATTR_NUM];//用于字符型属性
    double weight;
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
        int i=0,j=0,pos=0,k;
        char ch='/';
        ifstream infile;
        string tmpstrline;
        Data *p;
        dtm->dataSet=new Data;
        dtm->dataSet->next=NULL;
        p=dtm->dataSet;
        Data *datatmp;
        dtm->col=0;
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
                j=0;
                while(input>>datatmp->attr_string[j])
                {
                    pos=datatmp->attr_string[j].find(ch,0);
                    datatmp->attr_string[j]=datatmp->attr_string[j].substr(0,pos);
                    j++;
                }
                datatmp->attr_string[j]="#";
                datatmp->len=j;
                p->next=datatmp;
                p=p->next;
                dtm->col++;
            }
            i++;
        }
        infile.close();
        return 0;
    }

    int loadData(dataToMatrix *dtm,char *file,int type)
    {
        int i=0,j=0;
        ifstream infile;
        string tmpstrline;
        Data *p;
        dtm->dataSet=new Data;
        dtm->dataSet->next=NULL;
        p=dtm->dataSet;
        Data *datatmp;
        dtm->col=1;
        cout<<file<<endl;
        infile.open(file,ios::in);
        datatmp=new Data;
        datatmp->next=NULL;
        while(!infile.eof())
        {
            getline(infile,tmpstrline,'\n');//读取文件中一行的数据，保存为string类型
            stringstream input(tmpstrline);
            if(tmpstrline!="\0")////由于读取文件结束符同样会继续该操作
            {
                while(input>>datatmp->attr_string[j])
                {
                    j++;
                }
            }
        }
        p->next=datatmp;
        datatmp->len=j;
        dtm->row=j;
        infile.close();
        return 0;
    }
    int loadData(dataToMatrix *dtm,char *file,char type)
    {
        int i=0,j=0,pos=0,k;
        char ch=' ';
        ifstream infile;
        string tmpstrline;
        Data *p;
        dtm->dataSet=new Data;
        dtm->dataSet->next=NULL;
        p=dtm->dataSet;
        Data *datatmp;
        dtm->col=0;
        dtm->row=ATTR_NUM;
        cout<<file<<endl;
        infile.open(file,ios::in);
        while(!infile.eof()&&i<20)
        {
            getline(infile,tmpstrline,'\n');//读取文件中一行的数据，保存为string类型
            stringstream input(tmpstrline);
            if(tmpstrline!="\0")////由于读取文件结束符同样会继续该操作
            {
                datatmp=new Data;
                datatmp->id=i;
                datatmp->next=NULL;
                k=0,j=0;
                int len=tmpstrline.length();
                while(j*2<len)
                {
                    if(tmpstrline.substr(j*2,1)!=" ")
                    {
                        datatmp->attr_string[k]=tmpstrline.substr(j*2,2);
                        //cout<<datatmp->attr_string[k]<<"&";
                        k++;
                    }
                    j++;
                }
                //datatmp->attr_string[k]="#";
                datatmp->len=k;
                p->next=datatmp;
                p=p->next;
                dtm->col++;
            }
            i++;
        }
        //cout<<"k="<<k<<endl;
        infile.close();
        return 0;
    }


    int print(dataToMatrix dtm)
    {
        //检测数据加载是否正确
        int i,j;
        Data *p=dtm.dataSet->next;
        for(i=0; i<dtm.col&&p!=NULL; i++)
        {
            for(j=0; p->attr_string[j]!="#"&&j<dtm.row; j++)
            {
                cout<<p->attr_string[j]<<"  ";
            }
            p=p->next;
            cout<<endl;
        }
        return i;
    }
};
