### 静态多态
&emsp;&emsp;也称为编译期间的多态，编译器在编译期间完成的，编译器根据函数实参的类型(可能会进行隐式类型转换)，可推断出要调用那个函数，如果有对应的函数就调用该函数，否则出现编译错误。

1. 函数重载 包括普通函数的重载和成员函数的重载
2. 函数模板



### 动态多态
&emsp;&emsp;即运行时的多态，在程序执行期间(非编译期)判断所引用对象的实际类型，根据其实际类型调用相应的方法。
                    
1. 通过基类类型的引用或者指针调用虚函数
2. 必须是虚函数（派生类一定要重写基类中的虚函数）