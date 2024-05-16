/* 包含在 #include <functional> 中。

   参考引用 https://www.airchip.org.cn/index.php/2022/03/05/cpp-example-callback/

  
   以前没有这个类的时候，我们在想定义一个回调函数指针，非常的麻烦。我们通常这样的定义：

   typedef void(*ptr)(int，int); 这里的ptr就是一个函数指针，有一个缺陷【如果需要去回调一个类成员函数，函数指针则无法指向类成员函数】



   在C++11中，引入了std::function与std::bind来配合进行回调函数实现。
   
   例如 : function<void(int ,int)> a; 这里要注意a只是一个function类型的对象，并不是指针
*/

#include <iostream>
#include <functional>
#include <stdio.h>

using namespace std;

class BrainToolBox{
    public:
        BrainToolBox(){}
        int Add(int a, int b) { return a + b; }
};


int Add(int a, int b) { return a + b;}

void output() {printf("无入参函数\n");}


int main(){

    int a = 4;
    int b = 6;
    BrainToolBox tool;

    //普通有参函数
    std::function<int(int, int)> addFunc_1 = Add;
    int c = addFunc_1(3,4);
    std::cout << "c Value: " << c << std::endl;

    //普通无参函数
    std::function<void()> addFunc_2 = output;
    addFunc_2();

    //类成员有参函数
    std::function<int(int, int)> addFunc = std::bind(&BrainToolBox::Add, &tool, std::placeholders::_1, std::placeholders::_2);
    int d = addFunc(a, b);
    std::cout << "d Value: " << d << std::endl;

    return 0;
}