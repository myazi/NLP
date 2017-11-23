/**

MEMM是ME模型与MM模型（马尔科夫模型，去掉了隐的概念）的结合，采用的思想是最大熵模型的思想，即使得在满足约束条件下熵最大，
从其对偶形式可以看出是，最大化p（y|x）=wf(x,y)/Z(x)，其中f(x,y)是特征函数的定义，w为该特征的参数，
Z(x)是y在x的条件下的所有肯取值概率和，作归一化项，但这里是局部归一化，会导致标记偏置。

特征的形式采用MM形式，即引入状态与观测值，状态既可以是字构词的BMES，也可以是词性标准的形式

如何定义特征是问题的关键，下面程序上采用p(Si|oi,si-1)的形式定义特征，即结合前向状态和当前观察值。
（独立假设p(Si,oi,si-1)=p(Si|oi,si-1)=p(Si|oi)p(Si|si-1)则就简化参数）联合概率写成条件概率形式
归一化项即为在oi和si-1条件下所有si取值的概率和

参数估计方法有梯度法，牛顿法，GIS
当然采用最大熵极大似然估计，由最大熵模型的思想，特殊情况下（特征就是条件概率，或者联合概率的情况下）参数就等于统计出来的条件概率。
本程序就是采用这种联合概率作为特征（大多情况下采用这种特征定义就是）


解码过程同样是采用Viterbi算法，而不是采用归一化后使其概率最大化，归一化是建立在特征的基础上，
如果归一化则是归一化p（y|x）=wf(x,y)/Z(x)，那么求解其最优路径则需要在Viterbi算法基础上该，本程序没有进行归一化
只是与HMM模型中的有些不同


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
    string dic[VEC_LEN];//观测值字典
    double pos[VEC_LEN][STATE];//参数p(si|oi)
    double pss[STATE][STATE];//参数p(si|si-1)
    double pos0[STATE];//这里使用p(s1|o1,s0)=p(s1|o1)*p(s1)=p(s|o)*p(s1)
    int len;
};
DICOS dicos;

/**

根据字典获得测试数据中观测所在的位置

**/

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

/**

    根据状态来产生数组下标

**/

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

    字在词中位置对应的状态映射策略

**/
int wordToState(Data *p,int word,int j,int vl,string &state)
{
    char ch;
    if(p->attr_string[j].length()==2)
    {
        dicos.pos[vl][3]++;
        state.append(1,'3');
        state.append(1,' ');
    }
    if(p->attr_string[j].length()==4)
    {
        if(word==0)
        {
            dicos.pos[vl][0]++;
            state.append(1,'0');
            state.append(1,' ');
        }
        else
        {
            dicos.pos[vl][2]++;
            state.append(1,'2');
            state.append(1,' ');
        }
    }
    if(p->attr_string[j].length()==6)
    {
        dicos.pos[vl][word/2]++;
        ch=(char)(word/2+48);
        state.append(1,ch);
        state.append(1,' ');
    }
    if(p->attr_string[j].length()==8)
    {
        if(word==0)
        {
            dicos.pos[vl][0]++;
            state.append(1,'0');
            state.append(1,' ');
        }
        if(word==6)
        {
            dicos.pos[vl][2]++;
            state.append(1,'2');
            state.append(1,' ');
        }
        else
        {
            dicos.pos[vl][1]++;
            state.append(1,'1');
            state.append(1,' ');
        }
    }
}

/**

    根据训练数据，获取观测值字典，并把观测字典写到文件中，
    同时可以得到HMM模型中的三个重要参数

**/

void createVocabList(dataToMatrix dtm)
{
    ofstream ofile;
    ofstream ofile_arg;
    ofstream ofile_state;
    ofile.open("data\\memm_dic.txt");
    ofile_state.open("data\\memm_state.txt");
    int i,j,k,vl;
    int dic_len=0;
    int word;
    Data *p;
    p=dtm.dataSet->next;
    string state;
    char ch;
    double sum;
    /**
    字典建立
    */
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
                        wordToState(p,word,j,vl,state);//p是句子，j是第几个词，word是词中的第几个字，vl是字在字典中的位置
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
    cout<<"col="<<dtm.col<<endl;
    cout<<"vec_len="<<dic_len<<endl;

    /**
    参数估计
    **/

    ofile_arg.open("data\\memm_arg.txt");
    for(i=0; i<dicos.len; i++)
    {
        ofile_arg<<dicos.dic[i]<<"  ";
        sum=0;
        for(j=0; j<STATE; j++)
        {
            sum+=dicos.pos[i][j];
        }
        for(j=0; j<STATE; j++)
        {
            dicos.pos[i][j]/=sum;
            ofile_arg<<dicos.pos[i][j]<<"  ";
        }
        ofile_arg<<'\n';
    }
    ofile_arg.close();

    dataToMatrix state_dtm;
    state_dtm.loadData(&state_dtm,"data\\memm_state.txt");
    p=state_dtm.dataSet->next;
    for(i=0; i<state_dtm.col&&p!=NULL; i++)
    {
        dicos.pos0[getState(p->attr_string[0])]++;//统计参数pos0
        for(j=1; p->attr_string[j]!="#"&&j<p->len&&p->len>1; j++)
        {
            dicos.pss[getState(p->attr_string[j-1])][getState(p->attr_string[j])]++;//统计参数pss
        }
        p=p->next;
    }
    cout<<"i="<<i<<endl;
    cout<<"pss:--------------"<<endl;
    for(i=0; i<STATE; i++)
    {
        sum=0;
        for(j=0; j<STATE; j++)
        {
            sum+=dicos.pss[i][j];
        }
        for(j=0; j<STATE; j++)
        {
            dicos.pss[i][j]/=sum;
            cout<<dicos.pss[i][j]<<"  ";
        }
        cout<<endl;
    }
    cout<<"PI:--------------"<<endl;
    sum=0;
    for(i=0; i<STATE; i++)
    {
        sum+=dicos.pos0[i];
    }
    for(i=0; i<STATE; i++)
    {
        dicos.pos0[i]/=sum;
        cout<<dicos.pos0[i]<<"  ";
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
        deta[0][i]=dicos.pos0[i]*dicos.pos[pos][i];
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
                    if(deta[t-1][j]*dicos.pss[j][i]*dicos.pos[pos][i]>max_deta)
                    {
                        max_deta=deta[t-1][j]*dicos.pss[j][i]*dicos.pos[pos][i];
                    }
                    if(deta[t-1][j]*dicos.pss[j][i]>max_fai)
                    {
                        max_fai=deta[t-1][j]*dicos.pss[j][i];
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
            max_i=fai[t+1][max_i];
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
    testDtm.loadData(&testDtm,"data\\memm_test.txt");
    hmm(testDtm);//调用HMM算法，其中testDtm为测试状态序列
    return 0;
}
