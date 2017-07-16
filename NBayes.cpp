#include <iostream>
using namespace std;
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define Snum 6
#define Wnum 10
#define Wlen 10
#define num 100
struct TrainData
{
    char *postingList[Snum][Wnum];
    bool classVec[Snum];
};
int main()
{
    //设置样本
    TrainData traindata;
    char data[num][Wlen]= {"my", "dog", "has", "flea", "problems", "help", "please",
                           "maybe", "not", "take", "him", "to", "dog", "park", "stupid",
                           "my", "dalmation", "is", "so", "cute", "I", "love", "him",
                           "stop", "posting", "stupid", "worthless", "garbage",
                           "mr", "licks", "ate", "my", "steak", "how", "to", "stop", "him",
                           "quit", "buying", "worthless", "dog", "food", "stupid"
                          };

    int Slen[Snum]= {7,8,8,5,9,6};
    bool classVec[Snum]= {0,1,0,1,0,1};
    int i=0,j=0,k=0,l=0,dis=0;
    for(i=0; i<num; i++)
    {
        puts(data[i]);
    }
//初始化样本数据
    for(i=0; i<Snum; i++)
    {
        traindata.classVec[i]=classVec[i];
        *traindata.postingList[i]=(char *)malloc(Wnum*sizeof(char));
        for(j=0; j<Wnum; j++)
        {
            traindata.postingList[i][j]=(char *)malloc(Wlen*sizeof(char));
            *traindata.postingList[i][j]=NULL;
        }
        for(k=0; k<Slen[i]; k++)
        {
            strcpy(traindata.postingList[i][k],data[dis]);
            puts(traindata.postingList[i][k]);
            dis++;
        }
    }
//统计样本
    char vocabList[num][Wlen];
    int Vnum=0;
    for(i=0; i<Snum; i++)
    {
        for(j=0; *traindata.postingList[i][j]!=NULL; j++)
        {
            for(k=0; k<Vnum; k++)
            {
                if(!strcmp(traindata.postingList[i][j],vocabList[k]))
                    break;
            }
            if(k==Vnum)
            {
                Vnum++;
                strcpy(vocabList[k],traindata.postingList[i][j]);
            }
        }
    }
    for(i=0; i<Vnum; i++)
    {
        puts(vocabList[i]);
    }

//样本生成特征矩阵表示
    int trainMatrix[Vnum][Snum];
    for(i=0; i<Vnum; i++)
    {
        for(j=0; j<Snum; j++)
        {
            trainMatrix[i][j]=0;
            for(k=0; *traindata.postingList[j][k]!=NULL; k++)
            {
                if(!strcmp(traindata.postingList[j][k],vocabList[i]))
                    trainMatrix[i][j]=1;
            }
            cout<<trainMatrix[i][j]<<"  ";
        }
        cout<<endl;
    }
//参数估计
    double pAb=0;
    double p0Vec[Vnum],p1Vec[Vnum];
    double p0sum=0,p1sum=0;
    for(i=0; i<Snum; i++)
    {
        pAb+=classVec[i];
    }
    cout<<"pAb="<<pAb<<endl;
    pAb/=Snum;
    for(i=0; i<Snum; i++)
    {
        for(j=0; j<Vnum; j++)
        {
            if(classVec[i])
                p1sum+=trainMatrix[j][i];
            else
            {
                p0sum+=trainMatrix[j][i];
            }
        }
    }
    for(i=0; i<Vnum; i++)
    {
        p0Vec[i]=1.0/Vnum;//不规范平滑处理
        p1Vec[i]=1.0/Vnum;//平滑处理
        for(j=0; j<Snum; j++)
        {

            if(classVec[j]==0)
            {
                p0Vec[i]+=trainMatrix[i][j];
            }
            else
            {
                p1Vec[i]+=trainMatrix[i][j];
            }
        }
        p0Vec[i]/=p0sum;
        p1Vec[i]/=p1sum;
        cout<<p0Vec[i]<<"  &  "<<p1Vec[i]<<endl;
    }
    cout<<p1sum<<"&"<<p0sum<<endl;

//生成测试样本的特征向量
    char testData[Wnum][Wlen]= {"my","stupid","dog","garbage"};
    int testVec[Vnum];
    for(i=0; i<Vnum; i++)
    {
        testVec[i]=0;
        for(j=0; j<Wnum; j++)
        {
            if(!strcmp(testData[j],vocabList[i]))
                testVec[i]=1;
        }
        cout<<testVec[i]<<"  ";
        cout<<endl;
    }

    double p1=1.0,p0=1.0;
    for(i=0; i<Vnum; i++)
    {
        if(testVec[i]!=0)
        {
            p0*=p0Vec[i];
            p1*=p1Vec[i];
            cout<<p0Vec[i]<<endl;
            cout<<p1Vec[i]<<endl;
        }
        p0*=(1-pAb);
        p1*=pAb;
    }
    cout<<"my stupid dog garbage"<<endl;
    cout<<"p0="<<p0<<"&"<<"p1="<<p1<<endl;
    if(p0<p1)
        cout<<"class="<<1;
    else
        cout<<"class="<<0;
    return 0;
}

