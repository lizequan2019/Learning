#include <iostream>
#include <string>
#include <regex>

using namespace std;

int main ()
{

  //简便写法，确定字符串是否匹配规则
  if (regex_match ("subject", regex("(sub)(.*)") ))
  {
      cout << "char matched" << endl;
  }



  //使用迭代器的写法1
  const char cstr[] = "subject";
  string s ("subject");
  regex  e ("(sub)(.*)");

  if(regex_match (s,e))
  {
      cout << "string object matched"<<endl;
  }

  if (regex_match(s.begin(), s.end(), e ))
  {
      cout << "string range matched"<<endl;
  }



  //使用迭代器的写法2
  smatch sm;    //代码等价于  std::match_results<string::const_iterator> sm;
  regex_match (s,sm,e);
  cout << "string object with " << sm.size() << " matches"<<endl;


  //使用迭代器的写法3
  //这里使用迭代器  cbegin cend 都是获取的const_iterator迭代器
  regex_match ( s.cbegin(), s.cend(), sm, e);
  cout << "range with " << sm.size() << " matches"<<endl;





  //===================正则查找和正则匹配的区别=================================

  //在这里我们要明确正则匹配和正则查找的功能是什么?
  //查找是在一个字符串中找到若干个符合正则规则的子字符串
  //匹配是判断一整个字符串是否符合正则规则

  string str = "sub3123231jectsubject";
  regex  e2 ("(sub).+");    //regex  e2 ("(sub)(.)");  sub3123231jectsubject整个字符串满足"(sub).+"规则但是不满足"(sub)(.)"
                            //如果是正则查找则"(sub).+" 和  "(sub)(.)" 都能找到符合正则的子串

  while(regex_match(str,sm,e2))
  {
      if(sm.ready())
      {
            for (auto x:sm)
            {
                cout <<"match string is = "<<x<<endl;
            }
            str = sm.suffix().str();
            cout <<"========"<<endl;
      }
  }

  return 0;
}
