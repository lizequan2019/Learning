


&emsp;&emsp;这个问题这么想，C++中的`struct`和`class`的差异就是默认权限(**struct为public**,**class为private**)，所以问题中的区别基本就是`C++ class`和`C struct`的区别。

&emsp;&emsp;下面是网上找的区别,总结就是`C struct`中没有函数且默认权限是public,`C++ class`有函数且默认权限是private。

``` c
/*
    1.C的结构体不允许有成员函数，C++允许，并且还可以是虚函数。

    2.C的结构体没有权限这个说法，默认public；C++的结构体就和类一样，有权限，默认是public。

    3.C的结构体可以继承,C++的结构体可以和其他结构体或类继承 【下面代码验证  C的结构体可以继承并不是和C++继承一样】

    4.C++的结构体可以初始化，而C不可以 【感觉和第一条重复了】

    5.C要加上struct关键字或者使用typedef取别名来使用，而C++直接使用结构体名字。【下面代码验证】
*/
```


&emsp;&emsp;下面验证一下第五条结论，发现确实C++中`struct`对象创建方法和`class`一样。

```c
//==========================================【验证第五条】==============================================

//这是C语言的
#include <stdio.h>

typedef struct A{
    int a;
}strA;

int main(){
     struct A aa; //<============================ 不使用typedef别名
     printf("aa.a = %d\n",aa.a);

     strA bb;
     printf("bb.a = %d\n",bb.a);

     return 0;
}


#include <iostream>
using namespace std;

struct A{
    int a;
	A(){
		a = 10;
	}     
};

int main(){
    A aa;  //<===================================直接创建类对象就行了
	cout<<"aa.a = "<<aa.a<<endl;
	return 0;
}

```



&emsp;&emsp;`C sturct`不能像`C++ class`一样继承,只能是将需要继承的struct结构体型变量A放到自己内部(**A变量必须做结构体内部第一个变量**)，神奇。

```c
//==========================================【验证第三条】==============================================
//参考网站 https://www.cnblogs.com/lknlfy/archive/2013/01/06/2848348.html

    #include <stdio.h>

    //父结构体
    struct father{
        int f1;
        int f2;
    };

    //子结构体
    struct son{
        //【子结构体里定义一个父结构体变量，必须放在子结构体里的第一位】
        struct father fn;
        //子结构体的扩展变量
        int s1;
        int s2;
    };

    void test(struct son *t){
        //将子结构体指针强制转换成父结构体指针
        struct father *f = (struct father *)t;
        //打印原始值
        printf("f->f1 = %d\n",f->f1);
        printf("f->f2 = %d\n",f->f2);
        //修改原始值
        f->f1 = 30;
        f->f2 = 40;
    }

    int main(void){
        struct son s;
        s.fn.f1 = 10;
        s.fn.f2 = 20;

        test(&s);
        //打印修改后的值
        printf("s.fn.f1 = %d\n",s.fn.f1);
        printf("s.fn.f2 = %d\n",s.fn.f2);

        return 0;
    }

    /*
            f->f1 = 10
            f->f2 = 20
            s.fn.f1 = 30
            s.fn.f2 = 40
     */
```