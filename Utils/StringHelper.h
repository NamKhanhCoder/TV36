#pragma once
#include<vector>
#include<string>
using namespace std;
inline vector<string> splitString(const string& data, char delimiter = '|')
{
    vector<string> words;
    string word = "";
    for (int i = 0; i < data.size(); i++)
    {
        if (data[i] == delimiter)
        {
            words.push_back(word);
            word.clear();
        }
        else
        {
            word += data[i];
        }
    }
    words.push_back(word);
    return words;
}
inline string convertToUpper(const string& inp)
{
    string res="";
    for(char i :inp)
    {
        res+=toupper(i);
    }
    return res;
}