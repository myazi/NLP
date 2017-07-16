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
#define VEC_LEN 5000
#define STATE 4
#define MAX_FC 20000
#define int_max 999
#define double_min -1
#include <stack>
using namespace std;

struct DICOS
{
    string dic[VEC_LEN];
    double a[STATE][STATE];//参数A
    double b[STATE][VEC_LEN];//参数B
    double pi[STATE];//参数pi
    int len;
    int length;
};
DICOS dicos;

int getPos(string str)
{
    int i=0;
    for(i=0; i<dicos.len; i++)
    {
        if(!str.compare(dicos.dic[i]))
            return i;
    }
    cout<<"状态字典中不存在该状态"<<endl;
    return -1;
}

int getState(string str)
{
    if(str=="0")
        return 0;
    if(str=="1")
        return 1;
    if(str=="2")
        return 2;
    if(str=="3")
        return 3;
    return 0;
}
/**
映射策略对分词的质量影响非常之大
**/
int wordToState(Data *p,int word,int j,int vl,string &state)
{
    char ch;
    if(p->attr_string[j].length()==2)
    {
        dicos.b[3][vl]++;
        state.append(1,'3');
        state.append(1,' ');
    }
    if(p->attr_string[j].length()==4)
    {
        if(word==0)
        {
            dicos.b[0][vl]++;
            state.append(1,'0');
            state.append(1,' ');
        }
        else
        {
            dicos.b[2][vl]++;
            state.append(1,'2');
            state.append(1,' ');
        }
    }
    if(p->attr_string[j].length()==6)
    {
        dicos.b[word/2][vl]++;
        ch=(char)(word/2+48);
        state.append(1,ch);
        state.append(1,' ');
    }
    if(p->attr_string[j].length()==8)
    {
        if(word==0)
        {
            dicos.b[0][vl]++;
            state.append(1,'0');
            state.append(1,' ');
        }
        if(word==6)
        {
            dicos.b[2][vl]++;
            state.append(1,'2');
            state.append(1,' ');
        }
        else
        {
            dicos.b[1][vl]++;
            state.append(1,'1');
            state.append(1,' ');
        }
    }
}
void createVocabList(dataToMatrix dtm)
{
    ofstream ofile;
    ofstream ofile_arg;
    ofstream ofile_state;
    ofile.open("data\\hmm_dic.txt");
    ofile_state.open("data\\hmm_state.txt");
    int i,j,k,vl;
    int dic_len=0;
    int length=0;
    int word;
    Data *p;
    p=dtm.dataSet->next;
    string state;
    char ch;
    double sum;
    for(i=0; i<dtm.col&&p!=NULL; i++)
    {
        state="";
        for(j=0; p->attr_string[j]!="#"&&j<p->len; j++)
        {
            if(p->attr_string[j].length()%2!=0||p->attr_string[j].length()>9)
                continue;
            for(word=0; word<p->attr_string[j].length(); word+=2)
            {
                for(vl=0; vl<dic_len; vl++)
                {
                    if(!p->attr_string[j].substr(word,2).compare(dicos.dic[vl]))
                    {
                        wordToState(p,word,j,vl,state);
                        break;
                    }
                }
                if(vl==dic_len)
                {
                    dicos.dic[vl]=p->attr_string[j].substr(word,2);//对字典进行扩展
                    wordToState(p,word,j,vl,state);
                    ofile<<p->attr_string[j].substr(word,2)<<"  ";
                    dic_len++;
                }
            }
        }
        state.append(1,'\n');
        p=p->next;
        ofile_state<<state;
    }
    ofile.close();
    ofile_state.close();
    dicos.len=dic_len;
    dicos.length=length;
    ofile_arg.open("data\\hmm_arg.txt");
    for(j=0; j<dicos.len; j++)
    {
        ofile_arg<<dicos.dic[j]<<"  ";
        sum=0;
        for(i=0; i<STATE; i++)
        {
            sum+=dicos.b[i][j];
        }
        for(i=0; i<STATE; i++)
        {
            dicos.b[i][j]/=sum;
            ofile_arg<<dicos.b[i][j]<<"  ";
        }
        ofile_arg<<'\n';
    }
    ofile_arg.close();
    cout<<"col="<<dtm.col<<endl;
    cout<<"vec_len="<<dic_len<<endl;

    dataToMatrix state_dtm;
    state_dtm.loadData(&state_dtm,"data\\hmm_state.txt");
    p=state_dtm.dataSet->next;
    for(i=0; i<state_dtm.col&&p!=NULL; i++)
    {
        dicos.pi[getState(p->attr_string[0])]++;//统计参数pi
        for(j=1; p->attr_string[j]!="#"&&j<p->len&&p->len>1; j++)
        {
            dicos.a[getState(p->attr_string[j-1])][getState(p->attr_string[j])]++;//统计参数a
        }
        p=p->next;
    }
    cout<<"i="<<i<<endl;
    cout<<"A:--------------"<<endl;
    for(i=0; i<STATE; i++)
    {
        sum=0;
        for(j=0; j<STATE; j++)
        {
            sum+=dicos.a[i][j];
        }
        for(j=0; j<STATE; j++)
        {
            dicos.a[i][j]/=sum;
            cout<<dicos.a[i][j]<<"  ";
        }
        cout<<endl;
    }
    cout<<"PI:--------------"<<endl;
    sum=0;
    for(i=0; i<STATE; i++)
    {
        sum+=dicos.pi[i];
    }
    for(i=0; i<STATE; i++)
    {
        dicos.pi[i]/=sum;
        cout<<dicos.pi[i]<<"  ";
    }
    cout<<endl;
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
    for(i=0; i<testDtm.col&&p!=NULL; i++)
    {


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
            max_deta=double_min;
            cout<<fai[t+1][max_i];
            for(i=0; i<STATE; i++)
            {
                if(deta[t][i]>max_deta)
                {
                    max_deta=deta[t][i];
                    max_i=i;
                }
            }
        }
        p=p->next;
    }
}


int hmm(dataToMatrix testDtm)
{
    Viterbi(testDtm);
    return 0;
}

int main()
{
    dataToMatrix dtm;
    dtm.loadData(&dtm,"data\\pku_training.txt");//加载训练集

    createVocabList(dtm);//创建状态字典，用于存放参数值

    dataToMatrix testDtm;//加载测试数据
    testDtm.loadData(&testDtm,"data\\hmm_test.txt");

    hmm(testDtm);//调用HMM算法，其中testDtm为测试状态序列

    return 0;
}
