// ===============================================================
// Hoc vien Cong nghe Buu chinh Vien thong
// Hoc phan: Ky thuat lap trinh
// De tai 7: Quan ly dich vu truyen hinh so IPTV
// Tac gia: Le Tran Nam Khanh (B24DCVT193)
// Tien ich: StringHelper (Xu ly chuoi & tach cot delimiter)
// ===============================================================
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