    /********************************************************************
    * Copyright (C) 2012 Li Yachao
    * Contact: liyc7711@gmail.com or harry_lyc@foxmail.com
    *
    * Permission to use, copy, modify, and distribute this software for
    * any non-commercial purpose is hereby granted without fee, provided
    * that the above copyright notice appear in all copies and that both
    * that copyright notice.
    * It is provided "as is" without express or implied warranty.
    *
    * Version: 0.1
    * Last update: 2012-4-13
    *********************************************************************/
    /*********************************************************************
    用于分析分词结果
    *********************************************************************/
    #ifndef SCORE_H
    #define SCORE_H

    #include <iomanip>
    #include <iostream>
    #include <fstream>
    #include <vector>
    #include <set>
    #include <string>

    namespace MyUtility
    {
        struct ScoreItem
        {
            int GoldTotal;
            int TestTotal;
            int TestCorrect;
        };
        class Score
        {
        public:
            /*如果报告、词典文件输入为空，则表示不用*/
            Score(const std::string& gold_file,const std::string& test_file,
                  const std::string& dic_file ,const std::string& report_file="");
            Score();
            ~Score();
            void Clear();
            double GetRecall();
            double GetPrecise();
            double GetFMeasure();
            int GetTrueWords();
            int GetTestWords();
            double GetTestOOVRate();/*测试语料未登录词比例*/
            double GetOOVRecallRate();/*未登录词的召回率*/
            double GetIVRecallRate();/*登录词的召回率*/
        private:
            std::ofstream fout;/*输出文件流*/
            //std::ofstream fout1;/*输出文件流*/
            std::string reportFile;/*报告结果件路径*/
            std::string goldFile;/*标准文件路径*/
            std::string testFile;/*测试文件路径*/
            std::string dictionaryFile;/*词典文件路径*/
            int totalOOVTokens ;/*未登录tokens数量*/
            int totalOOVCorrectTokens;/*正确的未登录tokens数量*/
            int totalIVCorrectTokens;/*正确的登录词tokens数量*/
            std::vector<std::string>goldLines;/*标准文件的文本行*/
            std::vector<std::string>testLines;/*测试文件的文本行*/
            std::vector<struct ScoreItem> listScore;/*每行的评测结果*/
            std::set<std::string> dictionaryList;/*词典数据结构*/
            /*************************************************/
            bool IsPrefix(const std::string &src, const std::string &prefix);
            bool Postfix(const std::string &src, const std::string &postfix);
            bool Init();
            bool InitDict(const std::string& filePath);/*初始化词典*/
            bool IsEntryExist(const char * entry);
            bool Parse(const std::vector<std::string>& gold_tokens,const std::vector<std::string>& test_tokens,struct ScoreItem& score);
            bool FileReader(const std::string& path,std::vector<std::string>& lines);
            void SplitByTokens(std::vector<std::string> &vecstr, const std::string &str, const std::string tokens[],const  int tokensnumber, bool withtoken);

        };
    }
    #endif
