&emsp;&emsp;C++中，final关键字用于修饰类时，有以下作用:

1. 禁止继承 : c++11特性中，将类标记为`final`，意味着无法继承。
   
```c
    class test final
    {

    };
```

2. 禁止重写方法 : 当方法被标记为`final`时，在子类中无法重写该方法。

```c
class test
{
  public:
    test();
    virtual void func() final;
};
```