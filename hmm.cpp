/**
HMM模型中三个基本问题，概率计算问题，学习参数问题，预测问题。
针对这三个问题有HMM模型中的

1 前向后向算法

2 Baum-Welch算法

3 Veiterbi算法

下面的程序是上面三个算法的实现，考虑到用于分词，所以设计成读取文件的形式
但实际上中文分词算法中的学习参数问题并不采用Baum-Welch算法，而是采用监督学习算法中的极大似然估计，
所以Baum-Welch算法中读取的文件只是一个极少词的文件，用EM算法求解学习参数。
所以在学习参数问题上写了两种学习算法，一Baum-Welch算法，二 极大似然估计
并采用统计学习方法一书中的例子进行验证Veiterbi算法的正确性。
最后在另外一个文件中实现极大似然的HMM模型的中文分词


**/


#include <iostream>
#include <stdio.h>
#include <string>
#include <math.h>
#include <fstream>
#include <sstream>
#include <stack>
#include <map>
#include "matrix.h"
#define VEC_LEN 10000
#define STATE 3
#define MAX_FC 1000
#define int_max 999
#define double_min -1
#include <stack>
using namespace std;

struct DICOS
{
    string dic[VEC_LEN];
    double arg[VEC_LEN];//词频
    double a[STATE][STATE];//参数A
    double b[STATE][VEC_LEN];//参数B
    double pi[STATE];//参数pi
    int len;
    int length;
};
double alpha[MAX_FC][STATE];
double bata[MAX_FC][STATE];
double epsion[MAX_FC][STATE][STATE];
double Epsion[STATE][STATE];
double Gam[STATE];
double Gamvk[STATE][MAX_FC];
double sum[STATE]= {0};
double sumall=0;
DICOS dicos;

void createVocabList(dataToMatrix dtm)
{
    ofstream ofile_arg;
    ofstream ofile;
    ofile.open("data\\hmm_dic.txt");
    int i,j,k,vl;
    int dic_len=0;
    int length=0;
    Data *p;
    p=dtm.dataSet->next;
    for(i=0; i<dtm.col; i++)
    {
        for(j=0; p->attr_string[j]!="#"&&j<p->len; j++)
        {
            length++;
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
                ofile<<p->attr_string[j]<<"  ";
                dic_len++;
            }
        }
        p=p->next;
    }
    ofile.close();
    dicos.len=dic_len;
    dicos.length=length;
    ofile_arg.open("data\\hmm_arg.txt");
    for(i=0; i<dicos.len; i++)
        ofile_arg<<dicos.arg[i]<<"  ";
    ofile_arg.close();
    cout<<"vec_len="<<dic_len<<endl;
}
int getPos(string str)
{
    int i=0,j;
    for(i=0; i<dicos.len; i++)
    {
        if(!str.compare(dicos.dic[i]))
            return i;
    }
    cout<<"状态字典中不存在该状态"<<endl;
    return -1;
}

/**
统计学习方法p186例题测试VTB算法正确性
把Ntest文件中的字典长度看作观察序列可能值，上下文看作观察序列
隐藏状态在参数STATE设置，在字构词分词中STATE=4，测试本例时修改为STATE=3
**/

int init_test()
{
    double A[STATE][STATE]= {0.5,0.2,0.3,
                     0.3,0.5,0.2,
                     0.2,0.3,0.5
                    };
    double B[STATE][2]= {0.5,0.5,
                     0.4,0.6,
                     0.7,0.3
                    };
    double pi[STATE]= {0.2,0.4,0.4};
    int i,j;

    for(i=0; i<STATE; i++)
    {
        for(j=0; j<STATE; j++)
        {
            dicos.a[i][j]=A[i][j];
        }
    }
    for(i=0; i<STATE; i++)
    {
        for(j=0; j<dicos.len; j++)
        {
            dicos.b[i][j]=B[i][j];
        }
    }
    for(i=0; i<STATE; i++)
    {
        dicos.pi[i]=pi[i];
    }
}

int forwardBack(dataToMatrix dtm)
{
    //前向算法
    int s,t,i,j,k;
    int pos;
    Data *p;
    p=dtm.dataSet->next;
    sumall=0;
    for(i=0; i<STATE; i++)
    {
        pos=getPos(p->attr_string[0]);
        alpha[0][i]=dicos.pi[i]*dicos.b[i][pos];
    }
    for(t=1; t<p->len; t++)
    {
        for(i=0; i<STATE; i++)
        {
            sum[i]=0;
            for(j=0; j<STATE; j++)
            {
                sum[i]+=alpha[t-1][j]*dicos.a[j][i];
            }
            pos=getPos(p->attr_string[t]);
            alpha[t][i]=sum[i]*dicos.b[i][pos];
        }
    }
    for(i=0; i<STATE; i++)
    {
        sumall+=alpha[p->len-1][i];
    }
    cout<<"FORWARD："<<"sum="<<sumall<<endl;

    //后向算法
    for(i=0; i<STATE; i++)
    {
        bata[p->len-1][i]=1;
    }
    for(t=p->len-2; t>=0; t--)
    {
        for(i=0; i<STATE; i++)
        {
            sum[i]=0;
            pos=getPos(p->attr_string[t+1]);
            for(j=0; j<STATE; j++)
            {
                sum[i]+=dicos.a[i][j]*dicos.b[j][pos]*bata[t+1][j];
            }
            bata[t][i]=sum[i];
        }
    }
    sumall=0;
    pos=getPos(p->attr_string[0]);
    for(i=0; i<STATE; i++)
    {
        sumall+=bata[0][i]*dicos.pi[i]*dicos.b[i][pos];
    }
    cout<<"BACK："<<"sum="<<sumall<<endl;
    return 0;
}

int Baum_Weach(dataToMatrix dtm)
{
    //Baum-Welch算法，迭代收敛
    //一些概率与期望值的计算
    int s,t,i,j,k,iter;
    int pos;
    Data *p;
    p=dtm.dataSet->next;
    for(iter=0; iter<100; iter++)
    {
        forwardBack(dtm);
        /**
        参数A
        **/
        cout<<"A-----------------"<<endl;
        for(i=0; i<STATE; i++)
        {
            for(j=0; j<STATE; j++)
            {
                Epsion[i][j]=0;
            }
        }

        for(t=0; t<p->len-1; t++)
        {
            pos=getPos(p->attr_string[t+1]);
            for(i=0; i<STATE; i++)
            {
                for(j=0; j<STATE; j++)
                {
                    epsion[t][i][j]=alpha[t][i]*dicos.a[i][j]*dicos.b[j][pos]*bata[t+1][j];
                    epsion[t][i][j]/=sumall;
                    Epsion[i][j]+=epsion[t][i][j];
                }
            }
        }
        for(i=0; i<STATE; i++)
        {
            Gam[i]=0;
        }
        for(t=0; t<p->len-1; t++)
        {
            for(i=0; i<STATE; i++)
            {
                Gam[i]+=(alpha[t][i]*bata[t][i])/sumall;
            }
        }

        for(i=0; i<STATE; i++)
        {
            for(j=0; j<STATE; j++)
            {
                dicos.a[i][j]=Epsion[i][j]/Gam[i];
                cout<<dicos.a[i][j]<<"  ";
            }
            cout<<endl;
        }
        /**
        参数B
        **/
        cout<<"B----------------"<<endl;
        for(i=0; i<STATE; i++)
        {
            for(t=0; t<p->len; t++)
            {
                Gamvk[i][t]=0;
            }
        }
        for(k=0; k<dicos.len; k++)
        {
            for(t=0; t<p->len; t++)
            {
                pos=getPos(p->attr_string[t]);
                if(pos==k)
                {
                    for(i=0; i<STATE; i++)
                    {
                        Gamvk[i][k]+=(alpha[t][i]*bata[t][i])/sumall;//这里的t+1暂时表示vk的频率
                    }
                }
            }
        }

        for(i=0; i<STATE; i++)
        {
            Gam[i]=0;
        }
        for(t=0; t<p->len; t++)
        {
            for(i=0; i<STATE; i++)
            {
                Gam[i]+=(alpha[t][i]*bata[t][i])/sumall;
            }
        }

        for(i=0; i<STATE; i++)
        {
            for(k=0; k<dicos.len; k++)
            {
                dicos.b[i][k]=Gamvk[i][k]/Gam[i];
                cout<<dicos.b[i][k]<<"  ";
            }
            cout<<endl;
        }

        /**
        参数pi
        **/
        cout<<"pi----------------"<<endl;
        for(i=0; i<STATE; i++)
        {
            dicos.pi[i]=(alpha[0][i]*bata[0][i])/sumall;
            cout<<dicos.pi[i]<<"  ";
        }
        cout<<endl;
    }
    return 0;
}

int Viterbi(dataToMatrix testDtm)
{
    int t,i,j,k;
    int pos;
    Data *p;
    p=testDtm.dataSet->next;
    double deta[VEC_LEN][STATE];
    int fai[VEC_LEN][STATE];
    double max_deta;
    double max_fai;
    int max_i;
    for(i=0; i<STATE; i++)
    {
        pos=getPos(p->attr_string[0]);
        deta[0][i]=dicos.pi[i]*dicos.b[i][pos];
        fai[0][i]=0;
    }
    for(t=1; t<p->len; t++)
    {
        for(i=0; i<STATE; i++)
        {
            max_deta=double_min;
            max_fai=double_min;
            for(j=0; j<STATE; j++)
            {
                pos=getPos(p->attr_string[t]);
                if(deta[t-1][j]*dicos.a[j][i]*dicos.b[i][pos]>max_deta)
                {
                    max_deta=deta[t-1][j]*dicos.a[j][i]*dicos.b[i][pos];
                }
                if(deta[t-1][j]*dicos.a[j][i]>max_fai)
                {
                    max_fai=deta[t-1][j]*dicos.a[j][i];
                    max_i=j;
                }
            }
            deta[t][i]=max_deta;
            fai[t][i]=max_i;
        }
    }
    max_deta=double_min;
    for(i=0; i<STATE; i++)
    {
        if(deta[p->len-1][i]>max_deta)
        {
            max_deta=deta[p->len-1][i];
            max_i=i;
        }
        cout<<"P*="<<deta[p->len-1][i]<<endl;
    }
    cout<<max_i;
    for(t=p->len-2; t>=0; t--)
    {
        cout<<fai[t+1][max_i];
        max_deta=double_min;
        for(i=0; i<STATE; i++)
        {
            if(deta[t][i]>max_deta)
            {
                max_deta=deta[p->len-1][i];
                max_i=i;
            }
        }

    }
}

int hmm(dataToMatrix testDtm)
{
    init_test();
    forwardBack(testDtm);
    Baum_Weach(testDtm);
    init_test();
    Viterbi(testDtm);
    return 0;
}

int main()
{
    dataToMatrix dtm;
    dtm.loadData(&dtm,"data\\Ntest.txt",'1');//加载训练集

    createVocabList(dtm);//创建状态字典，用于存放参数值

    dataToMatrix testDtm;//加载测试数据
    testDtm.loadData(&testDtm,"data\\Ntest.txt",'1');

    hmm(testDtm);//调用HMM算法，其中testDtm为测试状态序列

    return 0;
}
