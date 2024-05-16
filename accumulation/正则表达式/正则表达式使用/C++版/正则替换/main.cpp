

//replace是替换匹配,即可以将符合匹配规则的子字符串替换为其他字符串。

#include <iostream>
#include <string>
#include <regex>
#include <iterator>

using namespace std;

int main ()
{

  string s ("there is a subsequence in the string\n");

  //原始的正则字符串 \b(sub)([^ ]*) 含义是匹配sub字符串开头，后面一直匹配到有空格出现为止
  regex e ("\\b(sub)([^ ]*)");




  // using string/c-string (3) version:
  // $2 与捕获组有关 上面匹配出来的捕获组为  "subsequence"、"sub"、"sequence"
  // 所以这里是sub-sequence
  cout <<"output1     "<< regex_replace (s,e,"sub-$2");





  // using range/c-string (6) version:
  string result;
  //back_inserter是一个插入迭代器，不在正则内容之中，所以先不深研究
  regex_replace (back_inserter(result), s.begin(), s.end(), e, "$2");
  cout <<"output2     "<< result;





  //使用标志
  //regex_constants::format_no_copy   替换匹配项时，不会复制目标序列中与正则表达式不匹配的部分。
  cout <<"output3     "<< regex_replace (s,e,"$1 and $2",regex_constants::format_no_copy);
  cout << endl;

  return 0;
}
