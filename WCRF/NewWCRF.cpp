/**

局部归一化

信息熵定义混乱度

四字上下文

**/
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <fstream>
#include <sstream>
#include "matrix.h"
#define VEC_LEN 8000 //最大字典数
#define CI_LEN 1000000
#define INDEX 8
#define STATE 4
#define QJBJ 30
#define ZM 54
#define BD 20
#define double_max 10000
#define A 0
#define B 1
#define C 2
#define D 3
#define AB 4
#define BC 5
#define CD 6
#define ABC 7
#define BCD 8
#define ABCD 9
#define CONTEXT 10
//#define ADD_ONE 0.01
//#define MUL 4.8
using namespace std;

typedef struct Word4
{
    short int word;
    Word4 *next;
    int arg[INDEX];
    double entropy;
}*four;

typedef struct Word3
{
    short int word;
    four word4;
    Word3 *next;
    int arg[INDEX*2];
    double entropy;
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
    double entropy;
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
    double entropy;
}*one;

string dic[VEC_LEN];///utf8字典
string dic_ci[CI_LEN];
int dic_len=2;
DICOS *dicos;
int NONO=0;
int word_no=0;
int word12_no=0;
int word12_yes=0;
int word123_yes=0;
int word1234_yes=0;
int stateTostate[INDEX];
double stateTostate_two[INDEX][INDEX];
double ADD_ONE=0.01;
double MUL=2;
ofstream f1file;
bool flag=0;
string bd[BD]= {"，","。","”","：","、","《","》","（","）","？","『","』","℃","●","！","＊"};
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
    NONO++;
    return -1;
}

string getWord(int pos)
{
    if(pos>=0)
        return dic[pos];
    else
    {
        cout<<"pos="<<pos<<endl;
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

/**独立的词转换为字串**/
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
/**状态下标转换*/
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
    cin>>state1;
    return -1;
}

/**建立四字上下文参数链表**/

void createVocabList(dataToMatrix dtm)
{
    ofstream ofile;
    //ofstream ofile_ci;
    //ofstream ofile_ci_dic;
    ofstream ofile_arg;
    ofstream ofile_state;
    ofile.open("data\\dic.utf8");
    //ofile_ci.open("data\\dic_ci.utf8");
    //ofile_ci_dic.open("data\\ci_dic.utf8");
    ofile_state.open("data\\state.utf8");
    ofile_arg.open("data\\arg.utf8");
    int i,j,k,vl,n,m,s;
    Data *p;
    Attr_String *as;
    string words[2000];
    int wordLen=0;
    dicos=new DICOS;
    dicos->next=NULL;
    dicos->word=-1;
    dicos->word2=new Word2;
    dicos->word2->next=NULL;
    dicos->word2->word=-1;
    dicos->word2->word3=new Word3;
    dicos->word2->word3->word=-1;
    dicos->word2->word3->next=NULL;
    dicos->word2->word3->word4=new Word4;
    dicos->word2->word3->word4->word=-1;
    dicos->word2->word3->word4->next=NULL;
    string senall;
    string stateall;
    DICOS *word1;
    Word2 *word2;
    Word3 *word3;
    Word4 *word4;
    DICOS *tmp1,*pre1;
    Word2 *tmp2,*pre2;
    Word3 *tmp3,*pre3;
    Word4 *tmp4,*pre4;
    int pos1=0,pos2=0,pos3=0,pos4=0;
    int state0=-1,state1=0,state2=0,state3=0,state4=0;
    int index;
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
        cout<<"i="<<i<<endl;
        stateall="";
        stateall.append("3");
        senall="";
        senall.append("#");
        as=p->Attr_All;
        for(j=0; as!=NULL&&j<p->len; j++)
        {
            /*for(vl=0;vl<dic_ci_len;vl++)
            {
                if(!as->attr.compare(dic_ci[vl]))
                {
                    break;
                }
            }
            if(vl==dic_ci_len)
            {
                dic_ci[vl]=as->attr;
                dic_ci_len++;
                ofile_ci<<dic_ci[vl];
                ofile_ci<<'\n';
            }*/
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
        pos3=getPos(words[2],i);
        state0=-1;
        state1=getState(stateall[0]);
        state2=getState(stateall[1]);
        state3=getState(stateall[2]);
        for(j=0; j<wordLen-3; j++)
        {
            //cout<<"j="<<j<<endl;
            word1=dicos;
            pos4=getPos(words[j+3],i);
            state4=getState(stateall[j+3]);
            for(vl=0; word1!=NULL; vl++)
            {
                if(word1->word==pos1)
                {
                    //cout<<"word1"<<endl;
                    word2=word1->word2;
                    for(n=0; word2!=NULL; n++)
                    {
                        if(word2->word==pos2)
                        {
                            //cout<<"word2"<<endl;
                            word3=word2->word3;
                            for(m=0; word3!=NULL; m++)
                            {
                                if(word3->word==pos3)
                                {
                                    //cout<<"word3"<<endl;
                                    word4=word3->word4;
                                    for(s=0; word4!=NULL; s++)
                                    {
                                        if(word4->word==pos4)
                                        {
                                            index=stateToindex(state2,state3);
                                            word4->arg[index]++;
                                            //index=stateToindex()
                                            //cout<<"word4"<<endl;
                                            word3->arg[index+INDEX]++;
                                            index=stateToindex(state1,state2);
                                            word3->arg[index]++;

                                            //index=stateToindex(state1,state2);
                                            word2->arg[index]++;
                                            word1->arg[index]++;
                                            if(state0!=-1)
                                            {
                                                index=stateToindex(state0,state1);
                                                word1->arg[index+INDEX]++;
                                            }
                                            break;
                                        }
                                        pre4=word4;
                                        word4=word4->next;
                                    }
                                    if(word4==NULL)
                                    {
                                        tmp4=new Word4;
                                        tmp4->next=NULL;
                                        tmp4->word=pos4;
                                        for(m=0; m<INDEX; m++)
                                        {
                                            tmp4->arg[m]=0;
                                        }
                                        index=stateToindex(state2,state3);
                                        tmp4->arg[index]=1;
                                        //index=stateToindex(state2,state3);
                                        word3->arg[index+INDEX]++;
                                        index=stateToindex(state1,state2);
                                        word3->arg[index]++;

                                        //index=stateToindex(state1,state2);
                                        word2->arg[index]++;
                                        word1->arg[index]++;
                                        if(state0!=-1)
                                        {
                                            index=stateToindex(state0,state1);
                                            word1->arg[index+INDEX]++;
                                        }
                                        pre4->next=tmp4;
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
                                tmp3->word4=new Word4;

                                tmp3->word4=new Word4;
                                tmp3->word4->next=NULL;
                                tmp3->word4->word=pos4;
                                for(m=0; m<INDEX; m++)
                                {
                                    tmp3->word4->arg[m]=0;
                                }
                                index=stateToindex(state2,state3);
                                tmp3->word4->arg[index]=1;
                                //index=stateToindex(state2,state3);
                                tmp3->arg[index+INDEX]=1;
                                index=stateToindex(state1,state2);
                                tmp3->arg[index]=1;

                                //index=stateToindex(state1,state2);
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
                        tmp2->word3->word4=new Word4;
                        tmp2->word3->word4->next=NULL;
                        tmp2->word3->word4->word=pos4;
                        for(n=0; n<INDEX; n++)
                        {
                            tmp2->word3->word4->arg[n]=0;
                        }
                        index=stateToindex(state2,state3);
                        tmp2->word3->word4->arg[index]=1;

                        //index=stateToindex(state2,state3);
                        tmp2->word3->arg[index+INDEX]=1;
                        index=stateToindex(state1,state2);
                        tmp2->word3->arg[index]=1;

                        //index=stateToindex(state1,state2);
                        tmp2->arg[index]=1;
                        word1->arg[index]++;
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

                tmp1->word2->word3->word4=new Word4;
                tmp1->word2->word3->word4->word=pos4;
                tmp1->word2->word3->word4->next=NULL;
                for(n=0; n<INDEX; n++)
                {
                    tmp1->word2->word3->word4->arg[n]=0;
                }
                index=stateToindex(state2,state3);
                tmp1->word2->word3->word4->arg[index]=1;

                //index=stateToindex(state2,state3);
                tmp1->word2->word3->arg[index+INDEX]=1;
                index=stateToindex(state1,state2);
                tmp1->word2->word3->arg[index]=1;

                index=stateToindex(state1,state2);
                tmp1->arg[index]=1;
                tmp1->word2->arg[index]=1;
                if(state0!=-1)
                {
                    index=stateToindex(state0,state1);
                    tmp1->arg[index+INDEX]=1;
                }
                pre1->next=tmp1;
            }
            pos1=pos2;
            pos2=pos3;
            pos3=pos4;
            state0=state1;
            state1=state2;
            state2=state3;
            state3=state4;
        }
        p=p->next;
    }

    ofile.close();
    ofile_state.close();
    //ofile_ci.close();

    cout<<"writing arg"<<endl;
    string argall;
    char arg[20];
    string word;
    int numi=0;
    int numj=0;
    word1=dicos->next;
    double tmp_sum;
    double tmp_p;
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
        word1->entropy=0;
        for(n=0; n<INDEX; n++)
        {
            tmp_p=(word1->arg[n]+ADD_ONE)/(tmp_sum/2+INDEX*ADD_ONE);
            word1->entropy+=(-log(tmp_p)*tmp_p);
            //word1->entropy+=ADD_ONE;
            //word1->entropy/=(tmp_sum/100);
        }
        sprintf(arg,"%8.4f",word1->entropy);
        argall.append("entropy:");
        argall.append(arg);
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
            argall.append("arg2:  ");
            for(n=0; n<INDEX; n++)
            {
                sprintf(arg,"%d",word2->arg[n]);
                tmp_sum+=word2->arg[n];
                argall.append(arg);
                argall.append("  ");
            }
            word2->entropy=0;
            for(n=0; n<INDEX; n++)
            {
                tmp_p=(word2->arg[n]+ADD_ONE)/(tmp_sum+INDEX*ADD_ONE);
                word2->entropy+=(-log(tmp_p)*tmp_p);
                //word2->entropy+=ADD_ONE;
                //word2->entropy/=(tmp_sum/100);
            }
            sprintf(arg,"%8.4f",word2->entropy);
            argall.append("entropy:");
            argall.append(arg);
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
                argall.append("33333333333333333333333333333333333333333333333");
                argall.append(1,'\n');
                argall.append(word);
                argall.append("arg3:  ");
                for(n=0; n<INDEX*2; n++)
                {
                    sprintf(arg,"%d",word3->arg[n]);
                    argall.append(arg);
                    argall.append("  ");
                }
                argall.append(1,'\n');
                argall.append("444444444444444444444444444444444444444444");
                argall.append(1,'\n');
                word4=word3->word4;
                for(s=0; word4!=NULL; s++)
                {
                    word=getWord(word4->word);
                    if(word=="#")
                    {
                        word4=word4->next;
                        continue;
                    }
                    argall.append(word);
                    argall.append("arg4:  ");
                    for(n=0; n<INDEX; n++)
                    {
                        sprintf(arg,"%d",word4->arg[n]);
                        argall.append(arg);
                        argall.append("  ");
                    }
                    argall.append(1,'\n');
                    word4=word4->next;
                }
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
    //ofile_ci_dic.close();
}

/**标点字母重新考虑**/

void bdzm(string pre_word,string next_word,double *pro)
{
    int n,j,m;
    for(n=0; bd[n]!=""; n++)
    {
        if(!pre_word.compare(bd[n]))
        {
            pro[6]/=double_max;
            pro[7]/=double_max;
            break;
        }
    }
    for(n=0; bd[n]!=""; n++)
    {
        if(!next_word.compare(bd[n]))
        {
            pro[5]/=double_max;
            pro[7]/=double_max;
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
                    pro[0]*=double_max;
                    pro[2]*=double_max;
                    break;
                }
            }
            if(zm[n]=="")
            {
                pro[4]/=double_max;
                pro[5]/=double_max;
            }
        }
        break;
    }
    if(zm[n]=="")
    {
        for(m=0; zm[m]!=""; m++)
        {
            if(!next_word.compare(zm[m]))
            {
                pro[4]/=double_max;
                pro[6]/=double_max;
                break;
            }
        }
    }
}

/**分词实现**/

int FenCi(dataToMatrix testDtm)
{
    Data *p;
    p=testDtm.dataSet->next;
    int i,j,l,m,n,k,s;
    int pos1,pos2,pos3,pos4;
    string stateall;
    string resultall;
    string stateall_copy;
    DICOS *word1,*word_1;
    Word2 *word2,*word_2;
    Word3 *word3,*word_3;
    Word4 *word4,*word_4;
    int wordLen=0;
    double deta[ATTR_NUM][INDEX];
    int fai[ATTR_NUM][INDEX];
    double max_deta;
    double temp;
    double pro[ATTR_NUM][INDEX],sum=0;
    double word_times[CONTEXT][INDEX];
    int max_i;
    string words[2000];
    char ch;
    ofstream resultfile;
    ofstream word12no;
    ofstream profile;
    resultfile.open("data\\result.utf8");
    word12no.open("data\\word12no.utf8");
    profile.open("data\\profile.utf8");
    for(i=0; i<testDtm.row&&p!=NULL; i++)
    {
        cout<<"i="<<i<<endl;
        p->Attr_All->attr.insert(0,"#");
        p->Attr_All->attr.append("$");
        wordLen=senToword(p->Attr_All->attr,words);

        profile<<i;
        profile<<"------------------------------";
        profile<<'\n';

        pos1=getPos(words[0],i);
        pos2=getPos(words[1],i);
        pos3=getPos(words[2],i);

        stateall="";
        resultall="";
        stateall_copy="";
        for(j=0; j<wordLen-3; j++)
        {
            //cout<<"j="<<j<<endl;
            pos4=getPos(words[j+3],i);
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
                            word3=word2->word3;
                            for(k=0; word3!=NULL; k++)
                            {
                                if(pos3==word3->word)
                                {
                                    word123_yes++;
                                    word4=word3->word4;
                                    for(s=0; word4!=NULL; s++)
                                    {
                                        if(pos4==word4->word)
                                        {
                                            word1234_yes++;
                                            for(n=0; n<INDEX; n++)
                                            {
                                                word_times[A][n]=word1->arg[n];
                                                word_times[AB][n]=word2->arg[n];
                                                word_times[ABC][n]=word3->arg[n+INDEX];
                                                word_times[ABCD][n]=word4->arg[n];
                                            }
                                            break;
                                        }
                                        word4=word4->next;
                                    }
                                    if(word4==NULL)
                                    {
                                        //cout<<"word14"<<endl;
                                        for(n=0; n<INDEX; n++)
                                        {
                                            word_times[A][n]=word1->arg[n];
                                            word_times[AB][n]=word2->arg[n];
                                            word_times[ABC][n]=word3->arg[n+INDEX];
                                            word_times[ABCD][n]=0;
                                        }
                                    }
                                    break;
                                }
                                word3=word3->next;
                            }
                            if(word3==NULL)
                            {
                                //cout<<"word13"<<endl;
                                for(n=0; n<INDEX; n++)
                                {
                                    word_times[A][n]=word1->arg[n];
                                    word_times[AB][n]=(word2->arg[n]);
                                    word_times[ABC][n]=0;
                                    word_times[ABCD][n]=0;
                                }
                            }
                            break;
                        }
                        word2=word2->next;
                    }
                    if(word2==NULL)
                    {
                        //cout<<"word12"<<endl;
                        for(n=0; n<INDEX; n++)
                        {
                            word_times[A][n]=word1->arg[n];
                            word_times[AB][n]=0;
                            word_times[ABC][n]=0;
                            word_times[ABCD][n]=0;
                        }
                    }
                    break;
                }
                word1=word1->next;
            }
            if(word1==NULL)
            {
                //cout<<"word11"<<endl;
                for(n=0; n<INDEX; n++)
                {
                    word_times[A][n]=0;
                    word_times[AB][n]=0;
                    word_times[ABC][n]=0;
                    word_times[ABCD][n]=0;
                }
            }
            word1=dicos->next;
            for(n=0; word1!=NULL; n++)
            {
                if(pos2==word1->word)
                {
                    word2=word1->word2;
                    for(m=0; word2!=NULL; m++)
                    {
                        if(pos3==word2->word)
                        {
                            for(n=0; n<INDEX; n++)
                            {
                                word3=word2->word3;
                                for(s=0; word3!=NULL; s++)
                                {
                                    if(pos4==word3->word)
                                    {
                                        for(n=0; n<INDEX; n++)
                                        {
                                            word_times[B][n]=word1->arg[n];
                                            word_times[BC][n]=word2->arg[n];
                                            word_times[BCD][n]=word3->arg[n];
                                        }
                                        break;
                                    }
                                    word3=word3->next;
                                }
                                if(word3==NULL)
                                {
                                    //cout<<"word23"<<endl;
                                    for(n=0; n<INDEX; n++)
                                    {
                                        word_times[B][n]=word1->arg[n];
                                        word_times[BC][n]=word2->arg[n];
                                        word_times[BCD][n]=0;
                                    }
                                }
                            }
                            break;
                        }
                        word2=word2->next;
                    }
                    if(word2==NULL)
                    {
                        //cout<<"word22"<<endl;
                        for(n=0; n<INDEX; n++)
                        {
                            word_times[B][n]=word1->arg[n];
                            word_times[BC][n]=0;
                            word_times[BCD][n]=0;
                        }
                    }
                    break;
                }
                word1=word1->next;
            }
            if(word1==NULL)
            {
                //cout<<"word21"<<endl;
                for(n=0; n<INDEX; n++)
                {
                    word_times[B][n]=0;
                    word_times[BC][n]=0;
                    word_times[BCD][n]=0;
                }
            }
            word1=dicos->next;
            for(n=0; word1!=NULL; n++)
            {
                if(pos3==word1->word)
                {
                    word2=word1->word2;
                    for(n=0; word2!=NULL; n++)
                    {
                        if(pos4==word2->word)
                        {
                            for(n=0; n<INDEX; n++)
                            {
                                word_times[C][n]=word1->arg[n+INDEX];
                                word_times[CD][n]=word2->arg[n];
                            }
                            break;
                        }
                        word2=word2->next;
                    }
                    if(word2==NULL)
                    {
                        //cout<<"word32"<<endl;
                        for(n=0; n<INDEX; n++)
                        {
                            word_times[C][n]=word1->arg[n];
                            word_times[CD][n]=0;
                        }
                    }
                    break;
                }
                word1=word1->next;
            }
            if(word1==NULL)
            {
                //cout<<"word31"<<endl;
                for(n=0; n<INDEX; n++)
                {
                    word_times[C][n]=0;
                    word_times[CD][n]=0;
                }
            }
            word1=dicos->next;
            for(n=0; word1!=NULL; n++)
            {
                if(pos4==word1->word)
                {
                    for(n=0; n<INDEX; n++)
                    {
                        word_times[D][n]=word1->arg[n];
                    }
                    break;
                }
                word1=word1->next;
            }
            if(word1==NULL)
            {
                for(n=0; n<INDEX; n++)
                {
                    word_times[D][n]=0;
                }
            }
            sum=0;
            for(n=0; n<INDEX; n++)
            {
                sum+=(word_times[BC][n]+word_times[ABC][n]*2+word_times[BCD][n]*2+word_times[ABCD][n]*10+ADD_ONE);
            }
            for(n=0; n<INDEX; n++)
            {
                pro[j][n]=(word_times[BC][n]+word_times[ABC][n]*2+word_times[BCD][n]*2+word_times[ABCD][n]*10+ADD_ONE)/sum;
                pro[j][n]=-log(pro[j][n]);
            }
            bdzm(words[j+1],words[j+2],pro[j]);
            pos1=pos2;
            pos2=pos3;
            pos3=pos4;
        }
        for(n=0; n<INDEX; n++)
        {
            for(j=0; j<wordLen-1; j++)
            {
                profile<<words[j+1];
                profile<<pro[j][n];
                profile<<"  ";
            }
            profile<<'\n';
        }
        ///Viterbi求解最短路径
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
        for(j=0; j<wordLen-2; j++)
        {
            resultall.append(words[j+1]);
            stateall_copy.append(1,stateall[j]);
            stateall_copy.append(" ");
            if(stateall[j]=='2'||stateall[j]=='3')
            {
                resultall.append("  ");
                stateall_copy.append("  ");
            }
        }
        resultfile<<resultall;
        resultfile<<'\n';
        resultfile<<stateall_copy;
        resultfile<<'\n';
        p=p->next;
    }
    resultfile.close();
    word12no.close();
    profile.close();
    return 0;
}

/**评测程序**/

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
        errfile<<"i=";
        errfile<<i;
        errfile<<":";
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
        p=p->next->next;
        q=q->next;
    }
    errfile.close();
    cout<<"resultALL="<<resultALL<<endl;
    cout<<"goldALL="<<goldALL<<endl;
    cout<<"ACC="<<ACC<<endl;
    cout<<"Precision:"<<ACC*1.0/resultALL<<endl;
    cout<<"Recall:"<<ACC*1.0/goldALL<<endl;
    cout<<"F Measure:"<<2*ACC*1.0/resultALL*ACC*1.0/goldALL/(ACC*1.0/resultALL+ACC*1.0/goldALL)<<endl;
    /*
        f1file<<ADD_ONE;
        f1file<<"&";
        f1file<<MUL;
        f1file<<"------------------------";
        f1file<<'\n';
        f1file<<"resultALL=";
        f1file<<resultALL;
        f1file<<'\n';

        f1file<<"goldALL=";
        f1file<<goldALL;
        f1file<<'\n';

        f1file<<"ACC=";
        f1file<<ACC;
        f1file<<'\n';

        f1file<<"精度:";
        f1file<<ACC*1.0/resultALL;
        f1file<<'\n';

        f1file<<"召回率:";
        f1file<<ACC*1.0/goldALL;
        f1file<<'\n';

        f1file<<"F1:";
        f1file<<2*ACC*1.0/resultALL*ACC*1.0/goldALL/(ACC*1.0/resultALL+ACC*1.0/goldALL);
        f1file<<'\n';*/
    return 0;
}

int main()
{
    int i=0,j;
    cout<<"-----------training-----------"<<endl;
    dataToMatrix testDtm;
    testDtm.loadData(&testDtm,"data\\msr_test_gold.utf8");
    createVocabList(testDtm);

    int step1,step2;
    //f1file.open("data\\f1.utf8");
    //for(step1=1; step1<10; step1++)
    {
        //ADD_ONE=step1*0.01;
        //for(step2=1; step2<5; step2++)
        {
            //MUL=step2*1.2;
            cout<<"------------FenCi--------------"<<endl;
            dataToMatrix MMDtm;//加载测试数据
            MMDtm.loadData(&MMDtm,"data\\msr_test.utf8");
            FenCi(MMDtm);
            dataToMatrix resultDtm;
            resultDtm.loadData(&resultDtm,"data\\result.utf8");

            cout<<"------------testing---------------"<<endl;
            dataToMatrix goldDtm;
            goldDtm.loadData(&goldDtm,"data\\msr_test_gold.utf8");

            PingCe(resultDtm,goldDtm);
        }
    }
    f1file.close();

    cout<<"NONO="<<NONO<<endl;
    cout<<"word1234_yes="<<word1234_yes<<endl;
    cout<<"word123_yes="<<word123_yes<<endl;
    cout<<"word12_yes="<<word12_yes<<endl;
    cout<<"word12_no="<<word12_no<<endl;
    cout<<"word_no="<<word_no<<endl;
    /*
    for(i=0; i<INDEX; i++)
    {
        cout<<stateTostate[i]<<"  ";
    }
    cout<<endl;
    for(i=0; i<INDEX; i++)
    {
        for(j=0; j<INDEX; j++)
        {
            cout<<stateTostate_two[i][j]<<"  ";
        }
        cout<<endl;
    }
    int state[INDEX]= {0,0,0,0,0,0,0,0};
    DICOS *word1;
    word1=dicos->next;
    while(word1)
    {
        for(j=0; j<INDEX; j++)
            state[j]+=word1->arg[j];
        word1=word1->next;
    }
    for(i=0; i<INDEX; i++)
    {
        cout<<state[i]<<"  ";
    }
    */
    return 0;
}
