```c

    #include <iostream>

    class U
    {
    public:
        int test = 0;
        void func()
        {
            std::cout << "func called" << std::endl;
            ++test;
        }
    };

    int main()
    {
        U u;
        U *rawp = &u;

        auto pfunc = &U::func; //类成员函数指针  函数指针类型是 void (U::*xxx)()

        (rawp->func)();
        (*rawp).func();
        (rawp->*pfunc)();
        ((*rawp).*pfunc)();

        auto ptest = &U::test;
        std::cout << rawp->test << std::endl;
        std::cout << (*rawp).test << std::endl;
        std::cout << rawp->*ptest << std::endl;
        std::cout << (*rawp).*ptest << std::endl;
        return 0;
    }

```

#### 补充:
&emsp;&emsp;&class::member,对于member为`nonstatic`的变量，取值后返回的是变量在对象内的偏移位置。对于`static`的成员变量，返回的是成员地址。nonstatic和static变量取值都可以直接用*。 

&emsp;&emsp;&class::func,返回的是成员函数的地址。对于`nonstatic`函数，通过返回的指针调用函数需要用二元运算符.*或者->\*，对于`static`函数则直接用\*解引用即可，`static`函数和普通C函数一样的调用。
