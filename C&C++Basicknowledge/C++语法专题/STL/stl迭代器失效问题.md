
&emsp;&emsp;stl数据结构分为序列型和关联型容器，下面代码展示了容器可能会出现的迭代器失效的情况。

```c++
#include <iostream>
#include <vector>
#include <map>
#include <list>

using namespace std;

vector<int> Vec;
vector<int>::iterator itVec;
map<int,int> Map;
map<int,int>::iterator itMap;
list<int> List;
list<int>::iterator itList;

int main(){

    for(int i = 10; i >= 0; i--){
        Vec.push_back(i);
        Map[i]=i;
        List.push_back(i);
    }


//vector  deque和这个类似

    for(itVec = Vec.begin(); itVec != Vec.end(); )
    {
        if (*itVec == 8)
            // Vec.erase(itVec);       //<====此处造成vector迭代器失败
            itVec = Vec.erase(itVec);  //<====修改
        else
            itVec++;
    }

    cout<<"vec = ";
    for(itVec = Vec.begin(); itVec != Vec.end(); )
    {
        cout<<" "<<*itVec;
        itVec++;
    }
    cout<<endl;


    itVec = Vec.begin();
    cout<<"执行 reserve 前 itVec = "<<*itVec<<endl;
    Vec.reserve(100);                                    //<====此处造成vector迭代器失败，需要重新给迭代器赋值
    cout<<"执行 reserve 后 itVec = "<<*itVec<<endl;


//map  set和这个类型

    for(itMap = Map.begin(); itMap != Map.end(); )
    {
        if (itMap->first == 8)
            //Map.erase(itMap);   /<====此处造成map迭代器失败
            Map.erase(itMap++);
        else
            itMap++;
    }

    cout<<"map = ";
    for(itMap = Map.begin(); itMap != Map.end(); )
    {
        cout<<" "<<itMap->second;
        itMap++;
    }
    cout<<endl;

//list  既可以使用序列型方法也可以使用关联型方法
    for(itList = List.begin(); itList != List.end(); )
    {
        if (*itList == 8)
            itList = List.erase(itList);  
        else
            itList++;
    }

    for(itList = List.begin(); itList != List.end(); )
    {
        if (*itList == 4)
            List.erase(itList++);  
        else
            itList++;
    }

    cout<<"list = ";
    for(itList = List.begin(); itList != List.end(); )
    {
        cout<<" "<<*itList;
        itList++;
    }
    cout<<endl;

    return 0;
}


/*
   执行结果:   vector map  和 list 删除 8

                vec =  10 9 7 6 5 4 3 2 1 0
                执行 reserve 前 itVec = 10
                执行 reserve 后 itVec = 0

                map =  0 1 2 3 4 5 6 7 9 10

                list =  10 9 7 6 5 3 2 1 0
 */
```