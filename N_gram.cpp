#include <iostream>
#include <stdio.h>
#include <string>
#include <math.h>
#include <fstream>
#include <sstream>
#include <stack>
#include "matrix.h"
#define VEC_LEN 100000
#define MAX_FC 1000
#define int_max 999
#include <stack>
using namespace std;

/**字典采用动态分配方式更好*/
struct DICOS
{
    string dic[VEC_LEN];
    double arg[VEC_LEN];
    int len;

};

/**
这里只是简单的测试，所以未采用动态分配的方式来定义分词结构体
*/
struct FenCi
{
    string fenci[MAX_FC];//分词的个数
    double adj[MAX_FC][MAX_FC];//邻接矩阵，0表示不能连接，其概率取对数再取负号为距离权重，方便使用最短路径算法
    int pos[MAX_FC];//记录词在字典中的位置，方便获取词的一元概率
    int len;//分词的实际长度
};
FenCi fc;
bool visited[MAX_FC];//最短路径访问标志

DICOS dicos;

void createVocabList(dataToMatrix dtm)
{
    ofstream ofile_arg;
    ofstream ofile;
    ofile.open("data\\N_gram_dic.txt");
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
                {
                    dicos.arg[vl]++;//对词频进行统计，作为概率参数
                    break;
                }
            }
            if(vl==dic_len)
            {
                dicos.dic[vl]=p->attr_string[j];//对字典进行扩展
                dicos.arg[vl]=1;
                ofile<<p->attr_string[j]<<" ";
                dic_len++;
            }
        }
        p=p->next;
    }
    ofile.close();
    dicos.len=dic_len;
    ofile_arg.open("data\\N_gram_arg.txt");
    for(i=0; i<dicos.len; i++)
        ofile_arg<<dicos.arg[i]<<" ";
    ofile_arg.close();
    cout<<"vec_len="<<dic_len<<endl;
}

int Ngram(dataToMatrix testDtm)
{
    int i,j,k;
    Data *p;
    p=testDtm.dataSet->next;
    double maxp=1;
    for(i=0; testDtm.col&&p!=NULL; i++)
    {
        j=0;
        fc.fenci[j++]="BOS";//引入句子初始节点
        for(k=0; k<dicos.len; k++)
        {
            if(p->attr_string[0].find(dicos.dic[k],0)!=string::npos)
            {
                for(i=0; i<p->attr_string[0].length(); i+=2)
                {
                    if(p->attr_string[0].substr(i,2)==dicos.dic[k].substr(0,2))
                    {//考虑到中文的编码问题，对匹配的字符进行再次确认是否符合分词要求
                        //cout<<dicos.dic[k]<<"  ";
                        fc.fenci[j]=dicos.dic[k];
                        fc.pos[j]=k;
                        //cout<<"k="<<k<<"fc="<<fc.fenci[j]<<"pinfo="<<fc.pinfo[j]<<endl;
                        j++;
                    }
                }

            }
        }
        fc.fenci[j++]="END";//引入句子的终止节点
        fc.len=j;
        p=p->next;
    }
    for(i=0; i<fc.len; i++)
    {
        //cout<<fc.fenci[i]<<"    ";
    }
    //cout<<endl;
    p=testDtm.dataSet->next;
    string sen="";
    sen.append("BOS");
    sen.append(p->attr_string[0]);
    sen.append("END");
    ofstream onfile;
    onfile.open("data\\pp.txt");
    for(i=0; i<fc.len; i++)
    {
        for(j=0; j<fc.len; j++)
        {
            fc.adj[i][j]=-1;
            if(sen.find(fc.fenci[i]+fc.fenci[j],0)!=string::npos)
            {
                fc.adj[i][j]=-log((dicos.arg[int(fc.pos[j])])/dicos.len);
                //将最大化似然估计转化为最短路径的关键变换
            }
            cout<<fc.adj[i][j]<<"  ";
            onfile<<fc.adj[i][j];
        }
        cout<<endl;
    }
    return 0;
}

void showpath(double *path,int v,int v0)
{
    stack<int> s;
    int u=v;
    while(v!=v0)
    {
        s.push(v);
        v=path[v];
    }
    s.push(v);
    while(!s.empty())
    {
        cout<<fc.fenci[s.top()]<<'/';
        s.pop();
    }
}

void shortpath_dij(int v0)
{
    int i,j,k;
    double *path,*dist;
    path=(double  *)malloc(sizeof(double)*MAX_FC);
    dist=(double *)malloc(sizeof(double)*MAX_FC);
    for(i=0; i<fc.len; i++)
    {
        if(fc.adj[v0][i]>0&&i!=v0)//初始化
        {
            dist[i]=fc.adj[v0][i];//记录下从起点（第一个最短路径上的点）
            //到各点的距离
            path[i]=v0;//path记录最短路径上从v0到vi的前一个顶点，方便后面做
            //最短路径遍历，它采用的是一种返回的方式进行遍历
        }
        else
        {
            dist[i]=int_max;
            path[i]=v0;//与v0不相邻的顶点默认他的最短路径上的前一个结点是v0
        }
        visited[i]=false;
    }
    path[v0]=v0;
    dist[v0]=0;
    visited[v0]=true;
    for(i=1; i<fc.len; i++)
    {
        int min=int_max;
        int u=v0;
        for(j=0; j<fc.len; j++)
        {
            if(visited[j]==false&&dist[j]<min)
            {
                min=dist[j];
                u=j;
            }
        }
        visited[u]=true;//其实是用来表示是否已经在最短路径上，如果在的话就至true，
        //后面就不对它进行寻找最短路径
        for(k=0; k<fc.len; k++)
        {
            if(visited[k]==false&&fc.adj[u][k]>0&&min+fc.adj[u][k]<dist[k])
            {
                dist[k]=min+fc.adj[u][k];
                path[k]=u;
            }
        }
    }
    for(i=0;i<fc.len;i++)
    {
        showpath(path,i,v0);
        cout<<":  "<<v0<<"->"<<i<<"="<<dist[i]<<endl;

    }
    int request=fc.len-1;
    showpath(path,request,v0);//遍历查询的最短路径输出
    //cout<<dist[request]<<endl;

}

int main()
{
    int k,i=0,j=0;
    //char file[20]="ddata\\ic.txt";
    ifstream infile;
    infile.open("data\\dicfile.txt",ios::in);
    string tmpstrline;
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
    i=0;
    infile.open("data\\N_gram_arg.txt");
    while(!infile.eof())
    {
        getline(infile,tmpstrline,'\n');//读取文件中一行的数据，保存为string类型
        stringstream input(tmpstrline);
        if(tmpstrline!="\0")////由于读取文件结束符同样会继续该操作
        {
            while(input>>dicos.arg[i])
            {
                i++;
            }
        }
    }
    dicos.len=i;
    double sum=0;
    for(i=0; i<dicos.len; i++)
    {
        sum+=dicos.arg[i];
    }
    cout<<"总单词数："<<sum<<"字典长度："<<dicos.len<<endl;

    dataToMatrix testDtm;
    testDtm.loadData(&testDtm,"data\\Ntest.txt");
    testDtm.print(testDtm);
    Ngram(testDtm);
    shortpath_dij(0);

}

