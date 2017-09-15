    #include "score.h"
    namespace MyUtility
    {
        Score::Score()
        {

        }
        Score::~Score()
        {
            dictionaryList.clear();
            Clear();
            if(!fout.is_open())
            {
                fout.clear();
                fout.close();
            }
        }
            /*
            *************************************************
            功能   :评测分词结果的效果。
            参数   : gold_file标准的分词结果；test_file用户分词结果；dic_file词典文件；report_file报告文件
            返回值 :
            -------------------------------------------------
            备注   :
            -------------------------------------------------
            作者   : Li Yachao
            时间   : 2012-4-13
            *************************************************
        */
        Score::Score(const std::string& gold_file,const std::string& test_file,
                  const std::string& dic_file  ,const std::string& report_file)
        {
            goldFile = gold_file;
            testFile = test_file;
            dictionaryFile = dic_file ;
            if(report_file.empty())
            {
                reportFile="";
            }
            else
            {
                reportFile = report_file;
            }
            Clear();
            if(!Init())
            {
                std::cout<<"Use score [gold] [test] [dic] [report]"<<std::endl;
            }

        }
        /*取得分词的准确度*/
        double Score::GetPrecise()
        {
            double total = 0;
            double total_c = 0;
            for(int i=0;i<listScore.size();i++)
            {
                total += listScore[i].TestTotal;
                total_c += listScore[i].TestCorrect;
            }
            if(total!= 0)
            {
                return total_c/total;
            }
            else
            {
                return -1;
            }
        }
        /*取得分词的召回率*/
        double Score::GetRecall()
        {
            double total = 0;
            double total_c = 0;
            for(int i=0;i<listScore.size();i++)
            {
                total += listScore[i].GoldTotal;
                total_c += listScore[i].TestCorrect;
            }
            if(total!= 0)
            {
                return total_c/total;
            }
            else
            {
                return -1;
            }
        }
        /*取得分词的F值*/
        double Score::GetFMeasure()
        {
            double a = GetPrecise();
            double b = GetRecall();
            if((a <= 0)&& (b <= 0))
            {
                return -1;
            }
            return 2*a*b/(a+b);
        }
        /*取得分词的标准切分数量*/
        int Score::GetTrueWords()
        {
            int val = 0;
            for(int i=0;i<listScore.size();i++)
            {
                val += listScore[i].GoldTotal;
            }
            return val;
        }
        /*取得分词的用户切分数量*/
        int Score::GetTestWords()
        {
            int val = 0;
            for(int i=0;i<listScore.size();i++)
            {
                val += listScore[i].TestTotal;
            }
            return val;
        }
        /*测试语料未登录词比例*/
        double Score::GetTestOOVRate()
        {
            double t = GetTrueWords();
            if(t != 0)
            {
                return totalOOVTokens/t;
            }
            else
            {
                return -1;
            }
        }
        /*未登录词的召回率*/
        double Score:: GetOOVRecallRate()
        {
            if(totalOOVTokens <= 0)
            {
                return 0;
            }
            else
            {
                return (double)totalOOVCorrectTokens/(double)totalOOVTokens;
            }
        }
        /*登录词的召回率*/
        double Score::GetIVRecallRate()
        {
            double t = (GetTrueWords() - totalOOVTokens);
            if(t==0)
            {
                return 0;
            }
            else
            {
                return (double)(totalIVCorrectTokens)/t;
            }
        }
        void Score::Clear()
        {
            totalOOVTokens = 0;
            totalOOVCorrectTokens = 0;
            totalIVCorrectTokens = 0;
            goldLines.clear();
            testLines.clear();
            listScore.clear();
            listScore.clear();
            dictionaryList.clear();
        }
        bool Score::Init()
        {
            if(!reportFile.empty())
            {
                fout.open(reportFile.c_str());
                if(!fout.is_open())
                {
                    return false;
                }
            }
            if(dictionaryFile.empty())
            {
                return false;
            }
            else
            {
                if(!InitDict(dictionaryFile))
                {
                    std::cout<<"Dict Empty!";
                }
            }
            if(!FileReader(goldFile,goldLines))
            {
                return false;
            }
            if(!FileReader(testFile,testLines))
            {
                return false;
            }
            if(goldLines.size() != testLines.size())
            {
                std::cerr <<"File lines size fault.";
                return false;
            }
            std::cout<<"--------------------"<<std::endl;
            //fout1.open("error.txt");
            int line_size = goldLines.size();
            std::vector<std::string>gold_tokens;
            std::vector<std::string>test_tokens;
            std::string seg_flags[]={" ","/"};/*分词的切分标志可以是空格，也可以是斜杠*/
            for(int i=0;i<line_size;i++)
            {
                SplitByTokens(gold_tokens,goldLines[i],seg_flags,2,false);
                SplitByTokens(test_tokens,testLines[i],seg_flags,2,false);
                struct ScoreItem score;
                if(!Parse(gold_tokens,test_tokens, score))
                {
                    std::cerr<<"Parse error at line "<<i+1<<std::endl;
                }
                gold_tokens.clear();
                test_tokens.clear();
                /*********Score评分************/
                listScore.push_back(score);
                /*S****************************/
            }
            if(!reportFile.empty())
            {
                fout<<"/**************************************************/"<<std::endl ;

                fout<<"True Word Count :"<<GetTrueWords()<<std::endl ;
                fout<<"Test Word Count :"<<GetTestWords()<<std::endl ;
                fout<<std::fixed<<std::setprecision(4)<<"Precision:"<<GetPrecise()<<std::endl<<"Recall:"<<GetRecall()<<std::endl<<"F Measure:"<<GetFMeasure()<<std::endl ;

                fout<<"OOV Rate : "<<GetTestOOVRate()<<std::endl ;
                fout<<"OOV Recall Rate : "<<GetOOVRecallRate()<<std::endl ;
                fout<<"IV Recall Rate : "<<GetIVRecallRate()<<std::endl ;

                std::cout<<"True Word Count :"<<GetTrueWords()<<std::endl ;
                std::cout<<"Test Word Count :"<<GetTestWords()<<std::endl ;
                std::cout<<std::fixed<<std::setprecision(4)<<"Precision:"<<GetPrecise()<<std::endl<<"Recall:"<<GetRecall()<<std::endl<<"F Measure:"<<GetFMeasure()<<std::endl ;

                std::cout<<"OOV Rate : "<<GetTestOOVRate()<<std::endl ;
                std::cout<<"OOV Recall Rate : "<<GetOOVRecallRate()<<std::endl ;
                std::cout<<"IV Recall Rate : "<<GetIVRecallRate()<<std::endl ;
                return true;
            }

        }
        bool Score::Parse(const std::vector<std::string>& gold_tokens,const std::vector<std::string>& test_tokens, struct ScoreItem& score)
        {
            if(gold_tokens.size() == 0 || test_tokens.size() == 0)
            {
                return false;
            }
            //totalGoldTokens += gold_tokens.size();
            /*****未登录词**************************/
            for(int i=0;i<gold_tokens.size();i++)
            {
                if(!IsEntryExist(gold_tokens[i].c_str()))
                {
                    totalOOVTokens ++;
                }
            }
            /******************************/
            std::vector<bool>gold_val;
            /*****************************/
            score.GoldTotal = gold_tokens.size();
            score.TestCorrect = test_tokens.size();
            score.TestTotal = test_tokens.size();
            /*****************************/
            size_t length_gold = 0;
            size_t length_test = 0;
            size_t gold_last_success = 0;
            size_t gold_cur_pos = 0;
            size_t gold_cur_prior = 0;
            size_t test_last_success = 0;
            size_t test_cur_pos = 0;
            size_t test_cur_prior = 0;
            std::string buffer_gold = gold_tokens[0];
            std::string buffer_test = test_tokens[0];
            std::string str_out ="";
            for(int i=0;i<gold_tokens.size();i++)
            {
                length_gold += gold_tokens[i].length();
            }
            for(int i=0;i<test_tokens.size();i++)
            {
                length_test += test_tokens[i].length();
            }
            if(length_gold != length_test)
            {
                return false;/*切分有误，丢失了数据*/
            }
            if(!reportFile.empty())
            {
                fout << gold_tokens[gold_cur_pos];
                fout <<"\t";
            }
            if(gold_tokens[gold_cur_pos]  != test_tokens[test_cur_pos])
            {
                score.TestCorrect --;
                gold_val.push_back(false);
                if(!reportFile.empty())
                {
                    fout <<"|"<<test_tokens[test_cur_pos];
                }
            }
            else
            {
                if(!reportFile.empty())
                {
                    fout <<test_tokens[test_cur_pos];
                }
                //if((NULL != dic) && (dic->IsEntryExist(test_tokens[test_cur_pos].c_str())))
                //{
                //  //totalOOVCorrectTokens ++;
                //}
                gold_val.push_back(true);
            }
            if(!reportFile.empty())
            {
                fout<<std::endl ;
            }
            while(true)
            {
                if((gold_last_success < gold_cur_pos) && (gold_cur_prior < gold_cur_pos ))
                {
                    buffer_gold += gold_tokens[gold_cur_pos];
                    gold_cur_prior++;
                    if(!reportFile.empty())
                    {
                        fout <<gold_tokens[gold_cur_pos]<<"\t<"<<std::endl;

                    }
                    gold_val.push_back(false);
                }
                if((test_last_success < test_cur_pos) && (test_cur_prior < test_cur_pos ))
                {
                    buffer_test += test_tokens[test_cur_pos];
                    test_cur_prior++;
                    if(!reportFile.empty())
                    {
                        fout <<"\t>"<< test_tokens[test_cur_pos]<<std::endl;
                    }
                }
                if((buffer_gold.length () == buffer_test.length()) && (buffer_gold == buffer_test))
                {
                    /************************************/
                    gold_cur_pos ++;
                    gold_last_success = gold_cur_pos;
                    gold_cur_prior = gold_cur_pos;
                    /************************************/
                    test_cur_pos ++;
                    test_last_success = test_cur_pos;
                    test_cur_prior = test_cur_pos;
                    /************************************/
                    if(gold_cur_pos < gold_tokens.size())
                    {
                        buffer_gold = gold_tokens[gold_cur_pos] ;
                        if(!reportFile.empty())
                        {
                            fout << gold_tokens[gold_cur_pos];
                            fout <<"\t";
                        }
                    }
                    if(test_cur_pos < test_tokens.size())
                    {
                        buffer_test = test_tokens[test_cur_pos] ;
                        if(buffer_gold != buffer_test)
                        {
                            score.TestCorrect --;
                        }
                        else
                        {
                            //if((NULL != dic) && (dic->IsEntryExist(test_tokens[test_cur_pos].c_str())))
                            //{
                            //  //totalOOVCorrectTokens ++;
                            //}
                        }
                        if(!reportFile.empty())
                        {
                            if(gold_tokens[gold_cur_pos] != test_tokens[test_cur_pos])
                            {
                                fout <<"|"<<test_tokens[test_cur_pos];
                                gold_val.push_back(false);
                            }
                            else
                            {
                                fout <<test_tokens[test_cur_pos];
                                gold_val.push_back(true);
                            }
                            fout <<std::endl ;
                        }
                    }
                    if((gold_cur_pos >= gold_tokens.size()) && (test_cur_pos >= test_tokens.size()))
                    {
                        break;
                    }
                    continue;
                }
                else if(buffer_gold.length () < buffer_test.length())
                {
                    /********************************************/
                    gold_cur_prior = gold_cur_pos;
                    gold_cur_pos ++;
                }
                else if(buffer_gold.length () > buffer_test.length())
                {
                    /********************************************/
                    //fout<<test_tokens[test_cur_pos]<<std::endl;
                    score.TestCorrect --;
                    test_cur_prior = test_cur_pos;
                    test_cur_pos ++;
                }
            }
            if(fout.is_open())
            {
                double precise = (double)score.TestCorrect/ (double)score.TestTotal;
                double recall = (double)score.TestCorrect / (double)score.GoldTotal;
                double f = 0;
                if((precise!=0) || (recall != 0))
                {
                    f = 2*precise*recall/(recall+precise);
                }
                fout<<std::fixed<<std::setprecision(4)<<"Precision:"<<precise<<" Recall:"<<recall<<" F Measure:"<<f<<std::endl ;
            }
            /**************************************************/

            if(gold_tokens.size() != gold_val.size())
            {
                return false;
            }
            for(int i=0;i<gold_tokens.size();i++)
            {
                if(gold_val[i])
                {
                    if(IsEntryExist(gold_tokens[i].c_str()))
                    {
                        totalIVCorrectTokens++;
                    }
                    else
                    {
                        totalOOVCorrectTokens++;
                    }

                }
                else
                {
                    //fout1<<gold_tokens[i]<<std::endl;
                }
            }
            //fout1<<std::endl ;
            //fout1.clear();
            return true;
        }
        bool Score::IsEntryExist(const char * entry)
        {
            bool val = false;
            if(NULL == entry)
            {
               return val;
            }
            std::set<std::string>::iterator pos;
            pos = dictionaryList.find(entry);
            if(pos != dictionaryList.end())
            {
                val = true;
            }
            else
            {
                /*处理藏文词条中后面有音节点的情况*/
            /*  if(Postfix(entry,"\xe0\xbc\x8b"))
                {
                    tmp = tmp.substr(0,tmp.size() - 3);
                }
                else
                {
                    tmp += "\xe0\xbc\x8b";
                }*/
                pos = dictionaryList.find(entry);
                if(pos != dictionaryList.end())
                {
                    val = true;
                }
            }

            return val;
        }
        /*初始化词典*/
        bool Score::InitDict(const std::string& filePath)
        {
            std::vector<std::string>lines;
            if(!FileReader(filePath,lines))
            {
                //std::cerr << "Open Dict [" << filePath << "] error!" << std::endl;
                return false;
            }
            for(int i=0;i<lines.size();i++)
            {
                dictionaryList.insert(lines[i]);
            }
            lines.clear();
            return true;
        }
        bool Score::FileReader(const std::string& path,std::vector<std::string>& lines)
        {
            std::ifstream fin;
            fin.open(path.c_str());
            if(!fin.is_open())
            {
                std::cerr << "Open [" << path << "] error!" << std::endl;
                return false;
            }
            lines.clear();
            std::string utf8flag = "\xef\xbb\xbf";
            std::string utf16flag = "\xff\xfe";
            std::string myLine = "";
            int lineIndex = 1;
            while (getline(fin, myLine))
            {
                if(lineIndex == 1)
                {
                    if(IsPrefix(myLine,utf8flag))
                    {
                        myLine = myLine.substr(3,myLine.length() - 3);
                    }
                    else if(IsPrefix(myLine,utf16flag))
                    {
                        std::cerr <<"["<<path<<"], File type error,need UTF8 or ANSI file."<<std::endl;
                        return false;
                    }
                }
                lines.push_back(myLine);
                myLine.clear();
                lineIndex++;
                //std::cout<<lineIndex <<"\t";
            }
            fin.close();
            return true;
        }
        bool Score::Postfix(const std::string &src, const std::string &postfix)
        {
            size_t s1=postfix.size();
            size_t s2=src.size();
            if( s1>s2 )
            {
                return false;
            }
            int i=(int)postfix.size()-1;
            int j=(int)src.size()-1;
            while( i>=0 && src[j]==postfix[i])
            {
                --i, --j;
            }
            return (i == -1);
        }
        bool Score::IsPrefix(const std::string &src, const std::string &prefix)
        {
            size_t s1=prefix.size();
            size_t s2=src.size();
            if(s1 == 0)
            {
                return true ;
            }
            if( s1>s2 )
            {
                return false;
            }
            size_t i=0;
            while( i<s1 && src[i]==prefix[i])
            {
                ++i;
            }
            return (i == s1);
        }
        void Score::SplitByTokens(std::vector<std::string> &vecstr, const std::string &str, const std::string tokens[],const  int tokensnumber, bool withtoken)
        {
            vecstr.clear();
            if((str.empty()) ||tokensnumber <=0 )
            {
                return  ;
            }
            std::string buffer="";
            int textLength = str.length();
            int start = 0;
            int offset = 0;
            while(start < textLength)
            {
                offset = textLength;
                int subLength =0;
                std::string tmp ="";
                for(int i=0;i< tokensnumber;i++)
                {
                    if(tokens[i].empty())
                    {
                        continue;
                    }
                    int curr = str.find(tokens[i],start);
                    if((curr >= 0) &&(curr < offset))
                    {
                        offset = curr;
                        subLength  = tokens[i].length();

                    }
                }
                if(start == offset)
                {
                    tmp = str.substr(start,0);
                    start = offset + subLength;
                }
                else if(start < offset)
                {
                    int len = 0;
                    if(withtoken)
                    {
                        len = subLength + ( offset - start);
                    }
                    else
                    {
                        len = ( offset - start);
                    }
                    tmp = str.substr(start,len);
                    start = (offset + subLength  );
                }
                /*这个影响多个空格连在一块，并且切分标志位空格的情况*/
                if(!tmp.empty())
                {
                    vecstr.push_back(tmp);
                }
            }

        }
    }

