#include <iostream>
#include <string>
#include <vector>
#include <regex>

using namespace std;

int main ()
{

        regex  e ("[0-9]+");




        //形式1  被检查字符串是一个string类型
        cout <<"1.=========================="<<endl;
        string s ("3123ter18retret903r8190re3890te338et213ert1890");
        if(regex_search(s,e))
        {
            cout <<"search "<<endl;
        }
        else
        {
            cout <<"no search "<<endl;
        }




        //形式2 被检查字符串是一个char * 类型 (const char * 也行)
        cout <<"2.=========================="<<endl;
        char chars[] = "3123ter18retret903r8190re3890te338et213ert1890";
        if(regex_search(chars,e))
        {
            cout <<"search"<<endl;
        }
        else
        {
            cout <<"no search"<<endl;
        }




        //形式3 被检查字符串是一个容器类型  单纯的这样用感觉鸡肋
        cout <<"3.=========================="<<endl;
        string ss ("weqweqefdfsfsfsdfs");
        if(regex_search(ss.begin(),ss.end(),e))
        {
            cout <<"search"<<endl;
        }
        else
        {
            cout <<"no search"<<endl;
        }



        //==================================================================================
        //上传的使用方法只是能告诉使用者这段字符串中有没有匹配规则的子串(或者整个都匹配)，但是使用者却不知道匹配的结果，下面的方法加入了匹配结果
        smatch m;



        //形式4 加入smatch参数
        cout <<"4.=========================="<<endl;
        string sss ("3123ter18retret903r8190re3890te338et213ert1890");
        if(regex_search(s,m,e))
        {
            if(m.ready())
            {
                cout <<"search num  = "<<m.size()<<endl;
                cout <<"search string is = "<<m.str()<<endl;
            }
        }



        //通过执行上边的代码，发现形式4只检测到了一个匹配的子串，就是第一个，但这显然不是我们想要的结果，我想要显示所有匹配的字符串，所以还需要换一种方法



        cout <<"5.=========================="<<endl;
        //形式5
        string::iterator iterbegin = sss.begin();
        string::iterator iterend   = sss.end();

        std::match_results<string::iterator> m3;
        //这里我们需要特别注意match_results的类型选取
        //如果直接使用smatch 它等价match_results<string::const_iterator>
        //那么iterbegin和iterend 的类型也就必须是string::const_iterator
        while(regex_search(iterbegin,iterend,m3,e))
        {
            if(m3.ready())
            {
                cout <<"search string is = "<<m3.str()<<endl;
                iterbegin = m3[0].second;
            }
        }




        //形式6 另一种遍历所有匹配子串项的方法
        cout <<"6.=========================="<<endl;
        while(regex_search(sss,m,e))
        {
            if(m.ready())
            {
                for (auto x:m)  //这里的m中其实只有匹配结果，这样for循环的好处是兼容了捕获组
                {
                    cout <<"search string is = "<<x<<endl;
                }
                sss = m.suffix().str();  //suffix 意为后缀，就是返回匹配字符串之后其他的字符串
            }
        }



        //关于捕获组
        //(sub)(.) 是一个有捕获组的正则匹配，捕获组索引为0(匹配的整个字符串)、1(匹配(sub)的字符串)、2(匹配(.)的字符串)
        /*
            search string is = sub3   //第一个匹配的捕获组
            search string is = sub
            search string is = 3
            ========
            search string is = subj   //第二个匹配的捕获组
            search string is = sub
            search string is = j
            ========
        */
        cout <<"7.=========================="<<endl;
        string str = "sub3123231jectsubject";
        regex  e2 ("(sub)(.)");
        while(regex_search(str,m,e2))
        {
            if(m.ready())
            {
                for (auto x:m)
                {
                    cout <<"search string is = "<<x<<endl;
                }
                str = m.suffix().str();
                cout <<"========"<<endl;
            }
        }

        return 0;
}
