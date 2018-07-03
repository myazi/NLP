/**

局部归一化

未登入词的状态


**/
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <map>
#include <math.h>
#include <fstream>
#include <sstream>
#include "matrix.h"
#include "score.h"
#include <time.h>
#include <vector>
#include <iterator>
#include <map>
#include <algorithm>

#define VEC_LEN 8000 //最大字典数
#define CI_LEN 1000000
#define INDEX 8
#define STATE 4
#define QJBJ 30
#define ZM 54
#define BD 30
#define double_max 10000
#define ADD_ONE 0.03
#define MUL 2
//using namespace std;

typedef struct Word3
{
    short int word;
    Word3 *next;
    int arg[INDEX*2];
    bool flag;
}*three;

/**
连续两个字中的第二个字
*/
typedef struct Word2
{
    short int word;
    three word3;
    Word2 *next;
    int arg[INDEX];
    bool flag;
}*two;
/**
连续两个字中的第一个字
**/
typedef struct DICOS
{
    short int word;
    two word2;
    DICOS *next;
    int arg[INDEX*2];
    bool flag;
}*one;

string dic_ci[CI_LEN];
int dic_ci_num[CI_LEN];
int dic_ci_len=0;
string dic[VEC_LEN];///utf8字典
string dic_ansi[VEC_LEN];///ansi字典
int dic_len=2;
DICOS *dicos;
int NONO=0;
int word_no=0;
int word12_no=0;
int word12_yes=0;
int word123_yes=0;
int stateTostate[INDEX];
double stateTostate_two[INDEX][INDEX];
int no_state[INDEX]= {0,0,0,0,0,0,0,0};
//double ADD_ONE=0.01;
//double MUL=2;
ofstream f1file;
bool flag=0;
string bd[BD]= {"，","。","“","”","：","、","《","》","（","）","？","『","』","℃","●","！","·",":","‘","’","—",",","×","/","(",")","[","]"};
string zm[ZM]= {"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z",
                "A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z"
               };

int getPos(string str,int ii)
{
    int i=0;
    for(i=0; i<dic_len; i++)
    {
        if(!str.compare(dic[i]))
            return i;
    }
    //cout<<"NNNNNNNNNNNNNNNN"<<"ii="<<ii<<endl;
    NONO++;
    return -1;
}

string getWord(int pos)
{
    if(pos>=0)
        return dic[pos];
    else
    {
        //cout<<"pos="<<pos<<endl;
        cin>>pos;
    }
    return "#";
}

int getState(char str)
{
    if(str=='0')
        return 0;
    if(str=='1')
        return 1;
    if(str=='2')
        return 2;
    if(str=='3')
        return 3;
    return 0;
}

int senToword(string sen,string *word)
{
    int i=0,j=0,k=0;
    int num = sen.size();
    string qj[QJBJ]= {"０","１","２","３","４","５","６","７","８","９","．"};
    string bj[QJBJ]= {"0","1","2","3","4","5","6","7","8","9","."};
    while(i < num)
    {
        int size = 1;
        if(sen[i] & 0x80)
        {
            char temp = sen[i];
            temp <<= 1;
            do
            {
                temp <<= 1;
                ++size;
            }
            while(temp & 0x80);
        }
        word[j] = sen.substr(i, size);
        /*for(k=0; k<QJBJ&&qj[k]!=""; k++)
        {
            if(word[j]==qj[k])
            {
                word[j]=bj[k];
            }
        }*/
        i += size;
        j++;
    }
    return j;
}
int stateToindex(int state1,int state2)
{
    if(state1==0&&state2==1)
        return 0;
    if(state1==0&&state2==2)
        return 1;
    if(state1==1&&state2==1)
        return 2;
    if(state1==1&&state2==2)
        return 3;
    if(state1==2&&state2==0)
        return 4;
    if(state1==2&&state2==3)
        return 5;
    if(state1==3&&state2==0)
        return 6;
    if(state1==3&&state2==3)
        return 7;
    cout<<state1<<"&"<<state2<<endl;
    return -1;
}
int stateToindex3(int state1,int state2,int state3)
{
    if(state1==0&&state2==1&&state3==1)
        return 0;
    if(state1==0&&state2==1&&state3==2)
        return 1;
    if(state1==0&&state2==2&&state3==0)
        return 2;
    if(state1==0&&state2==2&&state3==3)
        return 3;
    if(state1==1&&state2==1&&state3==1)
        return 4;
    if(state1==1&&state2==1&&state3==2)
        return 5;
    if(state1==1&&state2==2&&state3==0)
        return 6;
    if(state1==1&&state2==2&&state3==3)
        return 7;
    if(state1==2&&state2==0&&state3==1)
        return 8;
    if(state1==2&&state2==0&&state3==2)
        return 9;
    if(state1==2&&state2==3&&state3==0)
        return 10;
    if(state1==2&&state2==3&&state3==3)
        return 11;
    if(state1==3&&state2==0&&state3==1)
        return 12;
    if(state1==3&&state2==0&&state3==2)
        return 13;
    if(state1==3&&state2==3&&state3==0)
        return 14;
    if(state1==3&&state2==3&&state3==3)
        return 15;
    cout<<state1<<"&"<<state2<<"&"<<state3<<endl;
    return -1;
}

void createVocabList(dataToMatrix dtm)
{
    ofstream ofile;
    ofstream ofile_ci;
    ofstream ofile_ci_dic;
    ofstream ofile_arg;
    ofstream ofile_state;
    ofile.open("data\\dic.utf8");
    ofile_ci.open("data\\dic_ci.utf8");
    ofile_ci_dic.open("data\\ci_dic.utf8");
    ofile_state.open("data\\state.utf8");
    ofile_arg.open("data\\arg.utf8");
    int i,j,k,vl,n,m;
    Data *p;
    Attr_String *as;
    string words[2000];
    int wordLen=0;
    double sum_ci=0;
    dicos=new DICOS;
    dicos->next=NULL;
    dicos->word=-1;
    dicos->word2=new Word2;
    dicos->word2->next=NULL;
    dicos->word2->word=-1;
    dicos->word2->word3=new Word3;
    dicos->word2->word3->word=-1;
    dicos->word2->word3->next=NULL;
    string senall;
    string stateall;
    DICOS *word1;
    Word2 *word2;
    Word3 *word3;
    DICOS *tmp1,*pre1;
    Word2 *tmp2,*pre2;
    Word3 *tmp3,*pre3;
    int pos1=0,pos2=0,pos3=0;
    int state0=0,state1=0,state2=0,state3=0;
    int pre_index=3,index=3;
    dic[0]="#";
    dic[1]="$";
    ofile<<dic[0]<<"  ";
    ofile<<dic[1]<<"  ";
    p=dtm.dataSet->next;
    for(i=0; i<INDEX; i++)
    {
        stateTostate[i]=0;
        for(n=0; n<INDEX; n++)
        {
            stateTostate_two[i][n]=0;
        }
    }

    for(i=0; i<dtm.row&&p!=NULL; i++)
    {
        stateall="";
        stateall.append("3");
        senall="";
        senall.append("#");
        as=p->Attr_All;
        for(j=0; as!=NULL&&j<p->len; j++)
        {
            sum_ci++;
            for(vl=0; vl<dic_ci_len; vl++)
            {
                if(!as->attr.compare(dic_ci[vl]))
                {
                    dic_ci_num[vl]++;
                    break;
                }
            }
            if(vl==dic_ci_len)
            {
                dic_ci[vl]=as->attr;
                dic_ci_num[vl]=1;
                dic_ci_len++;
            }
            senall.append(as->attr);
            wordLen=senToword(as->attr,words);
            for(k=0; k<wordLen; k++)
            {
                for(vl=0; vl<dic_len; vl++)
                {
                    if(!words[k].compare(dic[vl]))
                    {
                        if(wordLen==1)
                            stateall.append("3");
                        if(k==0&&wordLen!=1)
                            stateall.append("0");
                        if(k==wordLen-1&&k!=0)
                            stateall.append("2");
                        if(wordLen-k>1&&k!=0)
                            stateall.append("1");
                        break;
                    }
                }
                if(vl==dic_len)
                {
                    dic[vl]=words[k];//对字典进行扩展
                    if(wordLen==1)
                        stateall.append("3");
                    if(k==0&&wordLen!=1)
                        stateall.append("0");
                    if(k==wordLen-1&&k!=0)
                        stateall.append("2");
                    if(wordLen-k>1&&k!=0)
                        stateall.append("1");
                    ofile<<words[k]<<"  ";
                    dic_len++;
                }
            }
            as=as->next;
        }
        stateall.append("3");
        senall.append("$");
        stateall.append(1,'\n');
        ofile_state<<stateall;
        wordLen=senToword(senall,words);
        pos1=getPos(words[0],i);
        pos2=getPos(words[1],i);
        state0=-1;
        state1=getState(stateall[0]);
        state2=getState(stateall[1]);
        for(j=0; j<wordLen-2; j++)
        {
            //cout<<"j="<<j<<endl;
            word1=dicos;
            pos3=getPos(words[j+2],i);
            state3=getState(stateall[j+2]);
            for(vl=0; word1!=NULL; vl++)
            {
                if(word1->word==pos1)
                {
                    word2=word1->word2;
                    for(n=0; word2!=NULL; n++)
                    {
                        if(word2->word==pos2)
                        {
                            word3=word2->word3;
                            for(m=0; word3!=NULL; m++)
                            {
                                if(word3->word==pos3)
                                {
                                    index=stateToindex3(state1,state2,state3);
                                    word3->arg[index]++;
                                    index=stateToindex(state1,state2);
                                    word2->arg[index]++;
                                    word1->arg[index]++;
                                    if(state0!=-1)
                                    {
                                        index=stateToindex(state0,state1);
                                        word1->arg[index+INDEX]++;
                                    }
                                    break;
                                }
                                pre3=word3;
                                word3=word3->next;
                            }
                            if(word3==NULL)
                            {
                                tmp3=new Word3;
                                tmp3->next=NULL;
                                tmp3->word=pos3;
                                for(m=0; m<INDEX*2; m++)
                                {
                                    tmp3->arg[m]=0;
                                }
                                index=stateToindex3(state1,state2,state3);
                                tmp3->arg[index]=1;
                                index=stateToindex(state1,state2);
                                //stateTostate[index]++;
                                //stateTostate_two[pre_index][index]++;
                                //pre_index=index;
                                word1->arg[index]++;
                                word2->arg[index]++;
                                if(state0!=-1)
                                {
                                    index=stateToindex(state0,state1);
                                    word1->arg[index+INDEX]++;
                                }
                                pre3->next=tmp3;
                            }
                            break;
                        }
                        pre2=word2;
                        word2=word2->next;
                    }
                    if(word2==NULL)
                    {
                        tmp2=new Word2;
                        tmp2->next=NULL;
                        tmp2->word=pos2;
                        for(n=0; n<INDEX; n++)
                        {
                            tmp2->arg[n]=0;
                        }
                        tmp2->word3=new Word3;
                        tmp2->word3->next=NULL;
                        tmp2->word3->word=pos3;
                        for(n=0; n<INDEX*2; n++)
                        {
                            tmp2->word3->arg[n]=0;
                        }
                        index=stateToindex(state1,state2);
                        word1->arg[index]++;
                        //stateTostate[index]++;
                        //stateTostate_two[pre_index][index]++;
                        //pre_index=index;
                        tmp2->arg[index]=1;
                        index=stateToindex3(state1,state2,state3);
                        tmp2->word3->arg[index]=1;
                        if(state0!=-1)
                        {
                            index=stateToindex(state0,state1);
                            word1->arg[index+INDEX]++;
                        }
                        pre2->next=tmp2;
                    }
                    break;
                }
                pre1=word1;
                word1=word1->next;
            }
            if(word1==NULL)
            {
                tmp1=new DICOS;
                tmp1->word=pos1;
                tmp1->next=NULL;
                for(n=0; n<INDEX*2; n++)
                {
                    tmp1->arg[n]=0;
                }
                tmp1->word2=new Word2;
                tmp1->word2->word=pos2;
                tmp1->word2->next=NULL;
                for(n=0; n<INDEX; n++)
                {
                    tmp1->word2->arg[n]=0;
                }
                tmp1->word2->word3=new Word3;
                tmp1->word2->word3->word=pos3;
                tmp1->word2->word3->next=NULL;
                for(n=0; n<INDEX*2; n++)
                {
                    tmp1->word2->word3->arg[n]=0;
                }
                index=stateToindex3(state1,state2,state3);
                tmp1->word2->word3->arg[index]=1;

                index=stateToindex(state1,state2);
                tmp1->arg[index]=1;
                tmp1->word2->arg[index]=1;
                //stateTostate[index]++;
                //stateTostate_two[pre_index][index]++;
                //pre_index=index;
                if(state0!=-1)
                {
                    index=stateToindex(state0,state1);
                    tmp1->arg[index+INDEX]=1;
                }
                pre1->next=tmp1;
            }
            pos1=pos2;
            pos2=pos3;
            state0=state1;
            state1=state2;
            state2=state3;
        }
        p=p->next;
    }

    ofile.close();
    ofile_state.close();

    for(vl=0; vl<dic_ci_len; vl++)
    {
        ofile_ci<<dic_ci[vl]<<" ";
        ofile_ci<<dic_ci_num[vl];
        ofile_ci<<'\n';
    }

    ofile_ci.close();

    cout<<"writing arg"<<endl;
    string argall;
    char arg[20];
    string word;
    int numi=0;
    int numj=0;
    word1=dicos->next;
    double tmp_sum;
    for(i=0; word1!=NULL; i++)
    {
        argall="1111111111111111111111111111111111111111111";
        argall.append(1,'\n');
        word=getWord(word1->word);
        argall.append(word);
        argall.append("  ");
        tmp_sum=0;
        for(n=0; n<INDEX*2; n++)
        {
            sprintf(arg,"%d",word1->arg[n]);
            tmp_sum+=word1->arg[n];
            argall.append(arg);
            argall.append("  ");
        }
        for(n=0; n<INDEX-1; n++)
        {
            if((1.0*(word1->arg[n]+word1->arg[n+1])*1.0/tmp_sum)>0.49&&tmp_sum>100)
            {
                word1->flag=1;
                argall.append("  flag");
                //argall.append(word1->flag);
                ofile_ci_dic<<getWord(word1->word);
                for(m=0; m<INDEX*2; m++)
                {
                    ofile_ci_dic<<word1->arg[m];
                    ofile_ci_dic<<"  ";
                }
                ofile_ci_dic<<'\n';
                break;
            }
            word1->flag=0;
        }
        argall.append(1,'\n');
        word2=word1->word2;
        for(j=0; word2!=NULL; j++)
        {
            tmp_sum=0;
            word=getWord(word2->word);
            if(word=="#")
            {
                word2=word2->next;
                continue;
            }
            argall.append("2222222222222222222222222222222222222222222222");
            argall.append(1,'\n');
            argall.append(word);
            argall.append("arg:  ");
            for(n=0; n<INDEX; n++)
            {
                sprintf(arg,"%d",word2->arg[n]);
                tmp_sum+=word2->arg[n];
                argall.append(arg);
                argall.append("  ");
            }
            for(n=0; n<INDEX; n++)
            {
                if(word2->arg[n]==tmp_sum&&tmp_sum>50)
                {
                    word2->flag=1;
                    argall.append("  flag");
                    //argall.append(word2->flag);
                    ofile_ci_dic<<getWord(word1->word);
                    ofile_ci_dic<<getWord(word2->word);
                    for(m=0; m<INDEX; m++)
                    {
                        ofile_ci_dic<<word2->arg[m];
                        ofile_ci_dic<<"  ";
                    }
                    ofile_ci_dic<<'\n';
                    break;
                }
                word2->flag=0;
            }
            argall.append(1,'\n');
            argall.append("33333333333333333333333333333333333333333333333");
            argall.append(1,'\n');
            word3=word2->word3;
            for(k=0; word3!=NULL; k++)
            {
                word=getWord(word3->word);
                if(word=="#")
                {
                    word3=word3->next;
                    continue;
                }
                argall.append(word);
                argall.append("arg2:  ");
                for(n=0; n<INDEX*2; n++)
                {
                    sprintf(arg,"%d",word3->arg[n]);
                    argall.append(arg);
                    argall.append("  ");
                }
                argall.append(1,'\n');
                word3=word3->next;
                numj++;
            }
            word2=word2->next;
            numj++;
        }
        argall.append(getWord(word1->word));
        argall.append(1,'\n');
        argall.append("------------------------------------------");
        argall.append(1,'\n');
        ofile_arg<<argall;
        word1=word1->next;
    }
    //cout<<"i="<<i<<endl;
    //cout<<"numj="<<numj<<endl;
    //cout<<dic_len<<endl;
    //cout<<dtm.row<<endl;
    ofile_arg.close();
    ofile_ci_dic.close();
}
void bdzm(string pre_word,string next_word,double *word_times)
{
    int n,j,m;
    for(n=0; bd[n]!=""; n++)
    {
        if(!pre_word.compare(bd[n]))
        {
            word_times[6]*=double_max;
            word_times[7]*=double_max;
            break;
        }
    }
    for(n=0; bd[n]!=""; n++)
    {
        if(!next_word.compare(bd[n]))
        {
            word_times[5]*=double_max;
            word_times[7]*=double_max;
            break;
        }
    }
    for(n=0; zm[n]!=""; n++)
    {
        if(!pre_word.compare(zm[n]))
        {
            for(m=0; zm[m]!=""; m++)
            {
                if(!next_word.compare(zm[m]))
                {
                    word_times[0]*=double_max;
                    word_times[2]*=double_max;
                    break;
                }
            }
            if(zm[m]=="")
            {
                word_times[4]*=double_max;
                word_times[5]*=double_max;
            }
            break;
        }
    }
    if(zm[n]=="")
    {
        for(m=0; zm[m]!=""; m++)
        {
            if(!next_word.compare(zm[m]))
            {
                word_times[4]*=double_max;
                word_times[6]*=double_max;
                break;
            }
        }
    }
}
int FenCi(dataToMatrix testDtm)
{
    Data *p;
    p=testDtm.dataSet->next;
    //Data *q;
    //q=stateDtm.dataSet->next;
    int i,j,l,m,n,k;
    int pos1,pos2,pos3;
    string stateall;
    string resultall;
    string stateall_copy;
    DICOS *word1,*word12;
    Word2 *word2;
    Word3 *word3;
    int wordLen=0;
    double deta[ATTR_NUM][INDEX];
    int fai[ATTR_NUM][INDEX];
    double max_deta;
    double temp;
    double pro[ATTR_NUM][INDEX],sum=0;
    double word_times[INDEX];
    double pre_pro[INDEX];
    int max_i;
    string words[2000];
    string states[2000];
    //double no_pre[INDEX]= {0.043,0.042,0.066,0.030,0.39,0.154,0.153,0.123};
    char ch;
    ofstream resultfile;
    ofstream word12no;
    ofstream profile;
    resultfile.open("data\\result.utf8");
    word12no.open("data\\word12no.utf8");
    profile.open("data\\profile.utf8");
    for(i=0; i<testDtm.row&&p!=NULL; i++)
    {
        //cout<<"i="<<i<<endl;
        p->Attr_All->attr.insert(0,"#");
        p->Attr_All->attr.append("$");
        wordLen=senToword(p->Attr_All->attr,words);
        //senToword(p->Attr_All->attr,states);
        profile<<i;
        profile<<"------------------------------";
        profile<<'\n';

        pos1=getPos(words[0],i);
        pos2=getPos(words[1],i);
        for(n=0; n<INDEX; n++)
        {
            pre_pro[n]=0;
        }
        stateall="";
        resultall="";
        stateall_copy="";
        for(j=0; j<wordLen-2; j++)
        {
            pos3=getPos(words[j+2],i);
            if(pos1==-1&&pos2==-1)
            {
                word_no++;
                word12_no++;
                for(n=0; n<INDEX; n++)
                {
                    word_times[n]=ADD_ONE;
                }
                //word_times[0]=1;//人为设置，WTO
                //word_times[1]=1;
                //word_times[2]=1;
                //word_times[3]=1;
                for(n=0; n<INDEX; n++)
                {
                    pre_pro[n]=0;
                }
            }
            if(pos1==-1&&pos2!=-1)
            {
                word_no++;
                word12_no++;
                word12=dicos->next;
                while(word12!=NULL&&word12->word!=pos2)
                {
                    word12=word12->next;
                }
                if(word12!=NULL)
                {
                    for(n=0; n<INDEX; n++)
                    {
                        word_times[n]=word12->arg[n+INDEX]+ADD_ONE;
                    }
                }
                for(n=0; n<INDEX; n++)
                {
                    pre_pro[n]=0;
                }
            }
            if(pos1!=-1&&pos2==-1)
            {
                word_no++;
                word12_no++;
                word12=dicos->next;
                while(word12!=NULL&&word12->word!=pos1)
                {
                    word12=word12->next;
                }
                if(word12!=NULL)
                {
                    for(n=0; n<INDEX; n++)
                    {
                        word_times[n]=word12->arg[n]+ADD_ONE;
                    }
                }
                for(n=0; n<INDEX; n++)
                {
                    pre_pro[n]=0;
                }
            }
            else
            {
                word1=dicos->next;
                for(l=0; word1!=NULL; l++)
                {
                    if(pos1==word1->word)
                    {
                        word2=word1->word2;
                        for(m=0; word2!=NULL; m++)
                        {
                            if(pos2==word2->word)
                            {
                                word12_yes++;
                                for(n=0; n<INDEX; n++)
                                {
                                    if(word2->flag)
                                    {
                                        word_times[n]=word2->arg[n]+ADD_ONE/10;
                                    }
                                    else
                                    {
                                        word_times[n]=word2->arg[n]+ADD_ONE;
                                    }

                                }
                                word3=word2->word3;
                                for(k=0; word3!=NULL; k++)
                                {
                                    if(pos3==word3->word)
                                    {
                                        word123_yes++;
                                        for(n=0; n<INDEX; n++)
                                        {
                                            if(word2->flag)
                                            {
                                                word_times[n]=((word3->arg[2*n]+word3->arg[2*n+1]+pre_pro[n])*MUL)+ADD_ONE/10;
                                            }
                                            else
                                            {
                                                word_times[n]=((word3->arg[2*n]+word3->arg[2*n+1]+pre_pro[n])*MUL)+ADD_ONE;
                                            }

                                        }
                                        pre_pro[0]=word3->arg[8]+word3->arg[12];
                                        pre_pro[1]=word3->arg[9]+word3->arg[13];
                                        pre_pro[2]=word3->arg[0]+word3->arg[4];
                                        pre_pro[3]=word3->arg[1]+word3->arg[5];
                                        pre_pro[4]=word3->arg[2]+word3->arg[6];
                                        pre_pro[5]=word3->arg[3]+word3->arg[7];
                                        pre_pro[6]=word3->arg[10]+word3->arg[14];
                                        pre_pro[7]=word3->arg[11]+word3->arg[15];
                                        break;
                                    }
                                    word3=word3->next;
                                }
                                if(word3==NULL)
                                {
                                    for(n=0; n<INDEX; n++)
                                    {
                                        word_times[n]+=(pre_pro[n]*MUL);
                                    }
                                    for(n=0; n<INDEX; n++)
                                    {
                                        pre_pro[n]=0;
                                    }
                                }
                                break;
                            }
                            word2=word2->next;
                        }
                        if(word2==NULL)
                        {
                            word12_no++;
                            word12=dicos->next;
                            while(word12!=NULL&&word12->word!=pos2)
                            {
                                word12=word12->next;
                            }
                            if(word12!=NULL)
                            {
                                word12no<<getWord(pos1);
                                word12no<<getWord(pos2);
                                word12no<<states[j];
                                word12no<<states[j+1];
                                word12no<<'\n';
                                if(states[j]=="0"&&states[j+1]=="1")
                                    no_state[0]++;
                                if(states[j]=="0"&&states[j+1]=="2")
                                    no_state[1]++;
                                if(states[j]=="1"&&states[j+1]=="1")
                                    no_state[2]++;
                                if(states[j]=="1"&&states[j+1]=="2")
                                    no_state[3]++;
                                if(states[j]=="2"&&states[j+1]=="0")
                                    no_state[4]++;
                                if(states[j]=="2"&&states[j+1]=="3")
                                    no_state[5]++;
                                if(states[j]=="3"&&states[j+1]=="0")
                                    no_state[6]++;
                                if(states[j]=="3"&&states[j+1]=="3")
                                    no_state[7]++;
                                for(n=0; n<INDEX; n++)
                                {
                                    word_times[n]=(word1->arg[n]+word12->arg[n+INDEX])+ADD_ONE;
                                }
                                for(n=0; n<INDEX; n++)
                                {
                                    pre_pro[n]=0;
                                }
                            }
                        }
                        break;
                    }
                    word1=word1->next;
                }
            }
            bdzm(words[j],words[j+1],word_times);
            sum=0;
            for(n=0; n<INDEX; n++)
            {
                sum+=word_times[n];
            }
            for(n=0; n<INDEX; n++)
            {
                pro[j][n]=word_times[n]/sum;
                pro[j][n]=-log(pro[j][n]);
            }
            pos1=pos2;
            pos2=pos3;
        }
        for(n=0; n<INDEX; n++)
        {
            for(j=0; j<wordLen-1; j++)
            {
                profile<<words[j];
                profile<<pro[j][n];
                profile<<"  ";
            }
            profile<<'\n';
        }

        ///***初始化deta，fai
        for(n=0; n<INDEX; n++)
        {
            deta[0][n]=pro[0][n];
        }
        for(j=1; j<wordLen-1; j++)
        {
            for(m=0; m<INDEX; m++)
            {
                max_deta=double_max;
                for(n=0; n<INDEX; n++)
                {
                    temp=double_max+deta[j-1][n];
                    if((n==0||n==2)&&(m==2||m==3))
                    {
                        temp=deta[j-1][n]+pro[j][m];
                    }
                    if((n==1||n==3)&&(m==4||m==5))
                    {
                        temp=deta[j-1][n]+pro[j][m];
                    }
                    if((n==4||n==6)&&(m==0||m==1))
                    {
                        temp=deta[j-1][n]+pro[j][m];
                    }
                    if((n==5||n==7)&&(m==6||m==7))
                    {
                        temp=deta[j-1][n]+pro[j][m];
                    }
                    if(temp<max_deta)
                    {
                        max_deta=temp;
                        max_i=n;
                    }
                }
                deta[j][m]=max_deta;
                fai[j][m]=max_i;
            }
        }
        max_deta=double_max;
        for(n=0; n<INDEX; n++)
        {
            if(deta[wordLen-2][n]<max_deta)
            {
                max_deta=deta[wordLen-2][n];
                max_i=n;
            }
        }
        ch=(char)(max_i/2+48);
        stateall.insert(0,1,ch);
        for(j=wordLen-3; j>=0; j--)
        {
            ch=(char)((fai[j+1][max_i]/2)+48);
            stateall.insert(0,1,ch);
            max_i=fai[j+1][max_i];
        }
        for(j=1; j<wordLen-1; j++)
        {
            resultall.append(words[j]);
            stateall_copy.append(1,stateall[j]);
            stateall_copy.append(" ");
            if(stateall[j]=='2'||stateall[j]=='3')
            {
                resultall.append("  ");
                stateall_copy.append("  ");
            }
        }
        resultfile<<resultall;
        //resultfile<<'\n';
        //resultfile<<stateall_copy;
        //resultfile<<'\n';
        p=p->next;
        //q=q->next;
    }
    resultfile.close();
    word12no.close();
    profile.close();
    return 0;
}

int PingCe(dataToMatrix resultDtm,dataToMatrix goldDtm)
{
    int i,j,k;
    int ACC=0,resultALL=0,goldALL=0;
    Data *p,*q;
    Attr_String *asp,*asq;
    ofstream errfile;
    errfile.open("data\\error.utf8");
    p=resultDtm.dataSet->next;
    q=goldDtm.dataSet->next;
    for(i=0; p!=NULL&&q!=NULL; i++)
    {
        resultALL+=p->len;
        goldALL+=q->len;
        //errfile<<"i=";
        //errfile<<i;
        //errfile<<":";
        asp=p->Attr_All;
        for(j=0; asp!=NULL&&j<p->len; j++)
        {
            asq=q->Attr_All;
            for(k=0; asq!=NULL&&k<q->len; k++)
            {
                //Wordlen=asp->attr
                if(!asp->attr.compare(asq->attr)||asp->attr.find('.',0)!= string::npos)
                {
                    ACC++;
                    break;
                }
                asq=asq->next;
            }
            if(asq==NULL||k==q->len)
            {
                //cout<<"i="<<i<<":"<<asp->attr<<endl;
                errfile<<asp->attr<<"  ";
            }
            asp=asp->next;
        }
        errfile<<'\n';
        p=p->next;//->next;
        q=q->next;
    }
    errfile.close();
    cout<<"resultALL="<<resultALL<<endl;
    cout<<"goldALL="<<goldALL<<endl;
    cout<<"ACC="<<ACC<<endl;
    cout<<"Precision:"<<ACC*1.0/resultALL<<endl;
    cout<<"Recall:"<<ACC*1.0/goldALL<<endl;
    cout<<"F Measure:"<<2*ACC*1.0/resultALL*ACC*1.0/goldALL/(ACC*1.0/resultALL+ACC*1.0/goldALL)<<endl;
    return 0;
}

bool ismush(pair<string,double> &lhs, pair<string,double> &rhs)
{
    return lhs.second > rhs.second;
}

bool iismush(pair<string,vector<double> > &lhs, pair<string, vector<double> > &rhs)
{
    return lhs.second > rhs.second;
}

void TF_IDF(dataToMatrix dtm, dataToMatrix ylkdtm, string filename)
{

    ofstream ofile_TF_IDF;
    ofile_TF_IDF.open(filename.c_str());
    int i,j,k,vl;
    Data *p;
    Attr_String *as;

    double sum_ci=0;
    dic_ci_len=0;
    p=ylkdtm.dataSet->next;

    //语料库
    for(i=0; i<ylkdtm.row&&p!=NULL; i++)
    {

        as=p->Attr_All;
        for(j=0; as!=NULL&&j<p->len; j++)
        {
            sum_ci++;
            for(vl=0; vl<dic_ci_len; vl++)
            {
                if(!as->attr.compare(dic_ci[vl]))
                {
                    dic_ci_num[vl]++;
                    break;
                }
            }
            if(vl==dic_ci_len)
            {
                dic_ci[vl]=as->attr;
                dic_ci_num[vl]=1;
                dic_ci_len++;
            }
            as=as->next;
        }
        p=p->next;
    }
    map<string,double> ylk_ci_map;
    for(i=0;i<dic_ci_len;i++)
    {
        ylk_ci_map[dic_ci[i]]=((double)dic_ci_num[i]);
    }

    //文档
    sum_ci=0;
    dic_ci_len=0;
    p=dtm.dataSet->next;
    for(i=0; i<dtm.row&&p!=NULL; i++)
    {
        as=p->Attr_All;
        for(j=0; as!=NULL&&j<p->len; j++)
        {
            sum_ci++;
            for(vl=0; vl<dic_ci_len; vl++)
            {
                if(!as->attr.compare(dic_ci[vl]))
                {
                    dic_ci_num[vl]++;
                    break;
                }
            }
            if(vl==dic_ci_len)
            {
                dic_ci[vl]=as->attr;
                dic_ci_num[vl]=1;
                dic_ci_len++;
            }
            as=as->next;
        }
        p=p->next;
    }

    ofstream TF("data\\TF.utf8");
    cout<<i<<"&"<<j<<endl;
    cout<<"sum_ci="<<sum_ci<<endl;
    map<string,double> ci_map;
    map<string,double> tf;
    for(i=0;i<dic_ci_len;i++)
    {
        if(ylk_ci_map[dic_ci[i]]==0)
        {
            ylk_ci_map[dic_ci[i]]=MAX;
        }
        ci_map[dic_ci[i]]=((double)dic_ci_num[i])/(ylk_ci_map[dic_ci[i]]+1)/sum_ci;
        tf[dic_ci[i]]=dic_ci_num[i];
    }
    vector<pair<string,double> > ci_vec(ci_map.begin(),ci_map.end());
    vector<pair<string,double> > ci_tf(tf.begin(),tf.end());
    sort(ci_vec.begin(),ci_vec.end(),ismush);
    sort(ci_tf.begin(),ci_tf.end(),ismush);
    i=0;
    for(vector<pair<string,double> >::iterator it=ci_vec.begin(),it2=ci_tf.begin(); it!=ci_vec.end() && it2!=ci_tf.end() && i<500; it++,it2++)
    {
        ofile_TF_IDF<<it->first<<"   ";
        //ofile_TF_IDF<<it->second<<'\n';
        TF<<it2->first<<"   ";
        TF<<it2->second<<'\n';
        i++;
    }
    TF.close();
    ofile_TF_IDF.close();
}

void word2vec(string tfidf_filename, string word2vec_filename, int num)
{
    int i=0;
    string tmpstrline;
    vector<double> vec;
    map<string, vector<double> > word2vec;
    ifstream vectors;
    vectors.open(word2vec_filename.c_str());
    while(!vectors.eof())
    {
        getline(vectors,tmpstrline,'\n');
        stringstream input(tmpstrline);
        string key;
        double value;
        if(tmpstrline!="\0")////由于读取文件结束符同样会继续该操作
        {
            input>>key;
            while(input>>value)
            {
                vec.push_back(value);
            }
            word2vec[key]=vec;
            key="";
            vec.clear();
        }
    }

    vector<string> ci;
    ifstream ci_tfidf;
    ci_tfidf.open(tfidf_filename.c_str());

    while(!ci_tfidf.eof())
    {
        getline(ci_tfidf,tmpstrline,'\n');
        stringstream input(tmpstrline);
        string key;
        double value;
        if(tmpstrline!="\0")////由于读取文件结束符同样会继续该操作
        {
            input>>key;
            ci.push_back(key);
            key="";
        }
    }

    map<string,vector<double> > X;
    for(vector<string>::iterator it=ci.begin(); it!=ci.end(); it++)
    {
        for(map<string, vector<double> >::iterator it_vec=word2vec.begin();it_vec!=word2vec.end();it_vec++)
        {
            if(it_vec->first==*it)
            {
                for(vector<double>::iterator iter=it_vec->second.begin(); iter!=it_vec->second.end(); iter++)
                {
                    X[*it].push_back(*iter);
                }
                i++;
                break;
            }
        }
        if(i==num)
            break;
    }
    vector<pair<string,vector<double> > > cos(X.begin(),X.end());

    ofstream feature;
    feature.open("data\\feature.utf8");
    for(vector<pair<string, vector<double> > > ::iterator it=cos.begin();it!=cos.end();it++)
    {
        feature<<it->first<<"  ";
        for(vector<double>::iterator itv=it->second.begin();itv!=it->second.end();itv++)
        {
            feature<<*itv<<"  ";
        }
        feature<<endl;
    }
    feature.close();
}

double sim(Matrix one, Matrix two)
{
    Matrix oneT;
    oneT.initMatrix(&oneT, one.row, one.col);
    oneT.transposematrix(one, &oneT);

    Matrix twoT;
    twoT.initMatrix(&twoT, two.row, two.col);
    twoT.transposematrix(two, &twoT);

    Matrix one2two;

    one2two.initMatrix(&one2two, one.col, twoT.row);
    one2two.multsmatrix(&one2two, one, twoT);

    Matrix one2one;

    one2one.initMatrix(&one2one, one.col, oneT.row);
    one2one.multsmatrix(&one2one, one, oneT);

    Matrix two2two;

    two2two.initMatrix(&two2two, two.col, twoT.row);
    two2two.multsmatrix(&two2two, two, twoT);

    cout<<one2one.col<<"&"<<one2one.row<<endl;
    cout<<two2two.col<<"&"<<two2two.row<<endl;
    cout<<one2two.col<<"&"<<one2two.row<<endl;

    int i=0,j=0;
    double sim=0;
    for(i=0;i<one2two.col;i++)
    {
        for(j=0; j<one2two.row; j++)
        {
            sim+=one2two.mat[i][j]/(sqrt(one2one.mat[i][i]) * sqrt(two2two.mat[j][j]));
        }
    }
    for(i=0;i<one2one.col;i++)
        cout<<one2one.mat[i][i]<<"  "<< two2two.mat[i][i]<<endl;
    return sim/(one.col *one.col);
}

int main()
{

/*
    ifstream infile_seg_tf("data\\xhs_seg_tf.utf8");
    ofstream ofile_seg_tf("data\\xhs_ss_tf.utf8");
    string line;
    string ci;
    double tf;
    map<string,double> seg_tf;

    while(!infile_seg_tf.eof())
    {
        getline(infile_seg_tf,line);
        stringstream input(line);
        input>>ci;
        input>>tf;
        seg_tf[ci]=tf;
    }
    vector<pair<string, double> > ss_tf(seg_tf.begin(), seg_tf.end());

    sort(ss_tf.begin(),ss_tf.end(),ismush);

    for(vector<pair<string, double> >::iterator it=ss_tf.begin(); it!=ss_tf.end(); it++)
    {
        ofile_seg_tf<<it->first<<"  ";
        ofile_seg_tf<<it->second<<endl;
    }
    ofile_seg_tf.close();

    cout<<"kk"<<endl;
*/
    int i=0;
    string tmpstrline;

    cout<<"-----------training-----------"<<endl;
    dataToMatrix trainDtm;
    trainDtm.loadData(&trainDtm,"data\\pku_training.utf8");
    createVocabList(trainDtm);


    cout<<"------------test-------------"<<endl;
    dataToMatrix testDtm;//加载测试数据
    testDtm.loadData(&testDtm,"data\\yzk.utf8");

    cout<<"------------FenCi--------------"<<endl;

    FenCi(testDtm);

    cout<<"-----------TFIDF---------------"<<endl;

    dataToMatrix resultDtm;
    resultDtm.loadData(&resultDtm,"data\\result.utf8");

    dataToMatrix IDFDtm;
    IDFDtm.loadData(&IDFDtm,"data\\pku_training.utf8");

    TF_IDF(resultDtm,IDFDtm,"data\\TF-IDF.utf8");//采用分词语料库作为TFIDF词频库

    cout<<"-----------Word2vec---------------"<<endl;

    word2vec("data\\TF-IDF.utf8","data\\vectors.utf8",100);


/*
    cout<<"-----------Sim---------------"<<endl;

    dataToMatrix swbj;
    dataToMatrix hlm;
    dataToMatrix yzk;
    dataToMatrix ssq;


    swbj.loadData(&swbj,"feature_swbj.utf8",1);
    hlm.loadData(&hlm,"feature_hlm.utf8",1);
    yzk.loadData(&yzk,"feature_yzk.utf8",1);
    ssq.loadData(&ssq,"feature_ssq.utf8",1);

    Matrix swbjm;
    swbjm.loadMatrix(&swbjm, swbj);

    Matrix hlmm;
    hlmm.loadMatrix(&hlmm, hlm);

    Matrix yzkm;
    yzkm.loadMatrix(&yzkm, yzk);

    Matrix ssqm;
    ssqm.loadMatrix(&ssqm, ssq);

    double ss=sim(swbjm,swbjm);
    cout<<ss<<endl;
*/
    return 0;
}
