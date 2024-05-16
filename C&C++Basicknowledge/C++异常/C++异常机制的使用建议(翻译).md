[toc]

----

> 参考文章  https://isocpp.org/wiki/faq/exceptions

&emsp;&emsp;参考文章一共23个小节，本次翻译了21个小节，并不是句句翻译，有些我认为用处不大的我就没翻译

<br/>

### 1.为什么使用异常机制 Why use exceptions

&emsp;&emsp;使用异常机制对程序员有什么好处? 当然有好处，使用异常来处理错误会使你的代码更简单、更干净、更不容易漏掉错误(这个是我比较关心的一点)。  

&emsp;&emsp;那么以前使用的`错误码`和`if判断`有什么不足吗？使用这些方式处理错误，**业务代码会和处理错误的代码纠缠在一起**。如此一来，代码将变得混乱，很难确保处理错误的代码可以囊括所有可能会出现的错误。

&emsp;&emsp;**不使用异常机制很多事情就不太好实现。比如在构造函数中检测到一个错误**，你会怎么向外报错？你可以向外抛出一个异常。

&emsp;&emsp;假设我们没有异常，我们应该如何处理发生在构造函数中错误？
```c
    vector<double> v(100000);  //调用构造函数 申请空间
    ofstream os("myfile");     //调用构造函数 获取文件描述符
```

&emsp;&emsp;`vector`或者`ofstream`类的构造函数可能在执行时就失败了，导致后续的操作也失败。比如，在使用`ofstream`时，因为你忘记检查文件打开是否成功(实际上打开文件失败)，导致你输出的内容消失了。如果这种情况发生在别的类上，结果可能更糟。所以，我们必须这么写: 

```c
    vector<double> v(100000); // 分配内存
    if (v.bad()) { /* handle error */ } // 判断申请空间是否出现成功 

    ofstream os("myfile");      // 打开文件
    if (os.bad())  { /* handle error */ }  // 判断文件是否打开
```

&emsp;&emsp;上面那样的操作，基本上每个对象都要测试一遍。**对于由多个类对象组成的类，这会变得非常混乱，尤其是当这些子对象相互依赖时，情况还会更差。**

&emsp;&emsp;所以编写构造函数但不使用异常机制会很棘手，那么以前的普通函数该怎么办呢?可以返回一个错误码或者设置一个全局变量来记录(**比如Linux中的errno**)错误。如果是使用的全局变量，那么每个操作后面需要立即去校验errno是否正常，**但是这种全局错误码的方法并不适合使用多线程的情况。**

&emsp;&emsp;而且使用返回值的问题在于需要选择合适的错误码值，这**需要开发者精心设计错误码值对应的错误含义**，不过一般很难实现。

```c
    int x = my_negate(INT_MIN); // 【考虑考虑这种情况能不能用 函数的功能从字面上理解是取符号相反的值】
```

&emsp;&emsp;上面的代码中`my_negate(INT_MIN)`函数是找不到合适的返回值，INT_MIN是int类型变量的最小值-2^32, 而最大值只到2^32-1,所以`my_negate(INT_MIN)`执行会报错，但是在int类型取值范围内每一个数都是一种正确的返回值，所以没有任何一个数可以单纯的做错误码。我用代码测试了一下。

![avatar][返回值例子]  


&emsp;&emsp;对使用异常的常见反对意见:  

* **"异常机制的开销很大"**，不是真的，与没有错误处理开销相比较，**现代C++实现了将异常开销减少到了百分之几，编写带有错误返回代码和测试的代码也是有开销的。**根据经验，当不抛出异常时，异常处理的开销会很小。在某些实现中它不需要任何成本。所有的成本都是在抛出异常时发生的,也就是说，**正常代码比使用错误返回值并检测是否正确的代码更快。代码只有在出现错误时才会产生成本**。  

* **"但是在JSF++中，Stroustrup(C++之父)完全禁止使用异常!"**，JSF++是用于开发硬实时和安全性至关重要的应用的(比如飞行控制程序)，如果计算时间过长就会有人因此而死，出于这样的原因才禁止使用异常保证响应时间，甚至这种情况下手动释放内存也是被禁止的!

* **"但是从构造函数中抛出异常会导致内存泄漏!"**，胡说八道! 这是一个编译器bug造成的无稽之谈，并且这个bug在十多年前就立刻被修复了。  

#### 关于构造函数中内存泄漏的验证

&emsp;&emsp;在网上查阅了资料，一个类对象A在执行构造函数中抛出异常，A本身的内存会被成功释放，但是A的析构函数不会被调用(**因为对象还没被构造出来，所以析构函数不被调用**)。

&emsp;&emsp;A的内部局部成员对象B清栈时是会被释放掉的，故B的析构函数会被调用，但是用户在构造函数中动态生成的对象不会被delete(堆上开辟的空间不会被自动销毁)。

&emsp;&emsp;如果对象A在构造函数中打开很多**系统资源**，但是构造函数中后续代码抛出了异常，则这些资源将不会被释放，建议在构造函数中加入`try catch`语句，对先前申请的资源进行释放后(也就是做析构函数该做的事情)再次抛出异常，确保内存和其他资源被成功回收。

[构造函数中内存泄漏验证](../验证代码/../C++异常/验证代码/构造函数中内存泄漏验证.cpp)

[构造函数中内存泄漏修改方案](../验证代码/../C++异常/验证代码/构造函数中内存泄漏修改方案.cpp)

---

### 2.我该如何使用异常机制 How do I use exceptions?

&emsp;&emsp;在C++中，**异常用于向外界通知无法在本地处理的错误**，例如未能获取构造函数中的资源。
```c
    class VectorInSpecialMemory 
    {
            int sz;
            int* elem;

        public:
            VectorInSpecialMemory(int s) : sz(s) , elem(AllocateInSpecialMemory(s))
            { 
                if (elem == nullptr)
                {
                     throw std::bad_alloc();
                }
            }
            ...
    };
```

&emsp;&emsp;<font color="#ff0000">不要将异常简单地视为另一种函数返回值的方式</font>，大部分程序员认为**异常处理代码就是错误处理代码**。

&emsp;&emsp;这里还要提及一个关键的技术`RAII()`，它通过使用带有析构函数的类在资源管理上强制施加顺序。
```c
    void fct(string s)
    {
        File_handle f(s,"r");   //File_handle类的构造函数打开s文件
        // use f

    }// f对象生命周期结束时，析构函数中执行关闭文件
```

&emsp;&emsp;如果上述代码中`use f`的部分抛出了一个异常,`File_handle`的析构函数在`fct`结束时依然会被调用并且文件资源也会被正确地关闭,这与常见的不安全用法形成对比:

```c
    void old_fct(const char* s)
    {
        FILE* f = fopen(s,"r"); // 打开文件s
        // use f
        fclose(f);  // 关闭文件s
    }
```

&emsp;&emsp;此时如果`use f`抛出一个异常，或者执行简单的return，那么文件资源不会关闭。

---

### 3.我不应该使用异常做什么？What shouldn’t I use exceptions for?

&emsp;&emsp;C++异常被设计用来支撑错误处理。  

* 仅使用`throw`向外传递一个错误信号(这意味着函数不能完成它的功能，也不能建立后置条件(`postconditions`))

> 前置条件是在方法运行之前必须为真的条件,也就是说，前置条件告诉调用者，在调用函数前必须满足的条件，如果不满足那么函数不能保证执行正确。

> 后置条件是在函数运行之后能够被保证为真的条件，也就是说，函数在前置条件满足且正确执行后保证达成某些条件成立。


* `catch`仅用来处理你已经知道的指定错误(也可以通过将捕获的异常转换为另一种类型并重新引发该类型的异常，例如捕获`badalloc`并重新引发`nospaceforfilebuffers`)

* 不要使用`throw`来指明函数使用中的**编码错误**。使用断言或其他机制将进程发送到调试器中，或者直接令进程崩溃并收集dump文件供开发人员调试。

* 如果在模块中发现异常已经导致程序状态被违背，请不要使用throw，请使用assert或其他机制终止程序。抛出异常并不能修复已损坏的内存信息，并可能导致重要用户数据的进一步损坏。

&emsp;&emsp;异常还有其他用途——在其他语言中很流行——但在C++中不是惯用用法，C++实现故意不支持(这些实现是基于异常用于错误处理的假设而优化的)。

&emsp;&emsp;**特别注意**，不要使用异常作为控制流。throw不是简单地从函数返回值的另一种方法(就像`return`)。<font color="#ff0000">这样做会很慢</font>，并且会让大多数C++程序员感到困惑，因为他们习惯于仅用于错误处理的异常。同样，throw也不是一种跳出循环的好方法。**从这里可以看出，正确的使用异常和返回值是很重要的**。查了一下stackoverflow，也有说这个问题的，而且还讨论的很深入,目前我个人理解就是`catch`中不要进行`return`操作。
> https://stackoverflow.com/questions/729379/why-not-use-exceptions-as-regular-flow-of-control?r=SearchResults

---

### 4.如何使用(try/catch/throw)可以提高软件质量？What are some ways try / catch / throw can improve software quality?

&emsp;&emsp;使用try、catch、throw代替代码中处理错误的if分支来提高质量。

&emsp;&emsp;try/catch/trow常用的替代方法是返回一个错误码，调用者通过一些条件语句(例如if)显式判断该返回码。调用方通过查看返回值，看看函数是否成功。  

&emsp;&emsp;虽然返回代码技术有时是最合适的错误处理技术，但添加不必要的if语句会带来一些恶劣的副作用:

* **降低质量** : 众所周知，**条件语句包含错误的可能性大约是任何其他类型语句的十倍**。因此，在所有其他条件都相同的情况下，如果可以从代码中去除(条件判断/条件语句)，那么代码可能会更健壮。

* **交付给客户产品变慢** : 因为白盒测试用例的数量与条件语句分支数量成正比的。所有的条件分支都需要测试，这样会增加测试的时间。大体上来说，如果测试不能覆盖到每个分支点，某些分支在用户使用时可能永远都不会再运行了。  

* **增加了开发成本** : 增加一些无关紧要的if分支会提高**测试、查找bug、修改bug**的成本。

&emsp;&emsp;<font color="#ff0000">与返回错误码和if分支处理错误比较而言，使用try/catch/theow可以减少代码bug,降低错误成本以及提高开发效率。当然，如果你的开发团队没有使用try/catch/theow的经验积累，你需要先找一些简单的小项目练练手，在应用与实际项目前，应该了解如何使用。</font>

---

### 5.我仍然不相信:一个4行代码片段显示返回代码并不比异常差; 那么，我为什么要在大几个数量级的应用程序上使用异常？ I’m still not convinced: a 4-line code snippet shows that return-codes aren’t any worse than exceptions; why should I therefore use exceptions on an application that is orders of magnitude larger? 

&emsp;&emsp;因为异常处理的伸缩性优于返回错误码。  

&emsp;&emsp;你只用4行的代码作比较，这看不出问题，如果是4000行，你就会发现两者的不同。 

&emsp;&emsp;下面用两种方法来处理一段代码，首先是异常处理:
```c
    try 
    {
      f();
      // ...
    } 
    catch (std::exception& e) 
    {
      // ...code that handles the error...
    }
```

&emsp;&emsp;接下来是返回错误码处理方式
```c
    int rc = f(); //接收函数返回的错误码
    if (rc == 0) 
    {
        // ...
    } 
    else 
    {
        // ...code that handles the error...
    }
```
&emsp;&emsp;有人喜欢先从**玩具**代码中进行比较，然后再去实际项目中使用，但是异常处理可以帮助你处理实际应用程序，所以不太可能在玩具代码中看到任何好处。

&emsp;&emsp;实际使用中，运行过程中检测到问题后，程序需要将错误码传递给处理错误的函数中。这种**错误信息传播**通常需要通过几十个函数(比如f1()中调用f2(),f2()中调用f3()，以此类推),可能问题会出现在f10()甚至f100()。错误信息会被一直传播到外层函数f1(),因为只有f1()函数可以将错误码传递给附近的上下文资源，它们知道如何处理发生的错误。在交互式应用程序中，f1()通常位于主事件循环附近，但无论如何，检测问题的代码通常与处理问题的代码不是在同一个函数中，错误信息需要在两者之间的所有栈中传播。  

&emsp;&emsp;异常处理使得错误处理变得简单:

```c
        void f1()
        {
            try 
            {
                // ...
                f2();
                // ...
            } 
            catch (some_exception& e) 
            {
                // ...code that handles the error...
            }
        }

        void f2() { ...; f3(); ...; }
        void f3() { ...; f4(); ...; }
        void f4() { ...; f5(); ...; }
        void f5() { ...; f6(); ...; }
        void f6() { ...; f7(); ...; }
        void f7() { ...; f8(); ...; }
        void f8() { ...; f9(); ...; }
        void f9() { ...; f10(); ...; }

        void f10()
        {
            // ...
            if ( /*...some error condition...*/ )  //判断是否出现异常
            {
                throw some_exception();
            }  
            // ...
        }
```

&emsp;&emsp;f10()仅需要检测错误、f1()仅需要处理错误。  

&emsp;&emsp;然而，使用返回错误码会强制将错误信息传递的逻辑带给到这两者之间的所有函数中。下面是使用返回代码的等效代码：

```c

int f1()
{
  // ...
  int rc = f2();
  if (rc == 0) {
    // ...
  } else {
    // ...code that handles the error...
  }
}

int f2()
{
  // ...
  int rc = f3();
  if (rc != 0)
    return rc;
  // ...
  return 0;
}

int f3()
{
  // ...
  int rc = f4();
  if (rc != 0)
    return rc;
  // ...
  return 0;
}

int f4()
{
  // ...
  int rc = f5();
  if (rc != 0)
    return rc;
  // ...
  return 0;
}

int f5()
{
  // ...
  int rc = f6();
  if (rc != 0)
    return rc;
  // ...
  return 0;
}

int f6()
{
  // ...
  int rc = f7();
  if (rc != 0)
    return rc;
  // ...
  return 0;
}

int f7()
{
  // ...
  int rc = f8();
  if (rc != 0)
    return rc;
  // ...
  return 0;
}

int f8()
{
  // ...
  int rc = f9();
  if (rc != 0)
    return rc;
  // ...
  return 0;
}

int f9()
{
  // ...
  int rc = f10();
  if (rc != 0)
    return rc;
  // ...
  return 0;
}

int f10()
{
  // ...
  if (...some error condition...)
    return some_nonzero_error_code;
  // ...
  return 0;
}

```
&emsp;&emsp;返回错误码解决方案在中间的函数接口中扩展了处理错误的逻辑。从f2()到f9()都存在手动编写的有关回传错误信息到f1()的代码。这种解决方案很糟糕的，它体现在:

* f2()到f9()的每个函数中都有了额外的if决策分支，它和原先的业务分支混在了一起，这样就很容易出现bug。 

* 这种写法增加了代码体积。

* f2()到f9()函数的复杂性增加了。 

* 要求f2()到f10()这些函数的返回值负责两种执行状态。返回xxx表示功能执行成功的结果，返回yyy表示功能执行错误且yyy是错误信息。当返回值xxx和yyy的类型不同时，比如功能执行成功与否返回值是int类型，而错误信息返回值string类型，此时需要额外的引用参数来向调用者传播有关成功和不成功的信息。  

&emsp;&emsp;如果您在上面的示例中仔细观察f1()和f10()，异常不会给您带来很大的改进。但是，如果从整体上看(观察f1()...f10())，你会发现两者之间的所有功能都有很大的不同。  
&emsp;&emsp;结论 : 异常处理的好处之一是，可以用更干净、更简单的方式将错误信息传回给可以处理错误的调用函数。另一个好处是，函数内部不需要额外的机制来将"成功"和"不成功"情况传回给调用者。玩具代码通常不强调错误传播或两种返回类型问题的处理，因此它们不代表真实世界的代码。

---

### 6.异常如何简化函数返回类型和参数类型？How do exceptions simplify my function return type and parameter types?

&emsp;&emsp;使用返回错误码时，通常需要两个或多个不同的返回值:一个用于指示函数成功并给出计算结果，另一个用于将错误信息传回给调用者。例如，如果函数有5种可能失败的原因，那么您可能需要多达6个不同的返回值 : 执行成功返回值，以及5种错误情况的值。  

&emsp;&emsp;现在只简化为两种情况: 

* 函数执行成功结果是XXX

* 函数执行失败错误信息是YYY 

&emsp;&emsp;让我们做一个简单的例子:我们想创建一个支持四种算术运算的Number类：加法、减法、乘法和除法。定义如下:

```c
class Number 
{
  public:
    friend Number operator+ (const Number& x, const Number& y);
    friend Number operator- (const Number& x, const Number& y);
    friend Number operator* (const Number& x, const Number& y);
    friend Number operator/ (const Number& x, const Number& y);
    // ...
};

void f(Number x, Number y)
{
  // ...
  Number sum  = x + y;
  Number diff = x - y;
  Number prod = x * y;
  Number quot = x / y;
  // ...
}
```

&emsp;&emsp;然后我们发现了一个问题，如何处理错误。两个数相加可能会出现数值溢出问题，两个数相除可能会出现被除数为0造成下溢问题，等等。如何既可以向外通知**函数执行成功，结果是xxx**也可以向外界通知**函数执行失败，错误信息是yyy**呢？  

&emsp;&emsp;将异常视为仅在需要时使用的返回类型。因此，我们只需定义所有异常并在需要时抛出它们：

```c
  void f(Number x, Number y)
  {
      try 
      {
        // ...
        Number sum  = x + y;
        Number diff = x - y;
        Number prod = x * y;
        Number quot = x / y;
        // ...
      }

      catch (Number::Overflow& exception) 
      {
        // ...code that handles overflow...
      }

      catch (Number::Underflow& exception) 
      {
        // ...code that handles underflow...
      }

      catch (Number::DivideByZero& exception) 
      {
        // ...code that handles divide-by-zero...
      }
  }
```

&emsp;&emsp;如果我们使用错误码而不是异常，处理起来就很难。当在Number对象内不能同时推送正确结果和带有详细描述的错误信息时，可能最终会使用额外的引用参数来处理两种情况中的一种:"执行成功"或"执行失败"，或两者兼而有之。在不损失通用性的情况下，我将使用一个正常的返回值处理计算结果，通过一个引用参数处理**执行失败**的情况。

```c
  class Number 
  {
      public:

        enum ReturnCode  // 执行结果情况的枚举值
        {
            Success,
            Overflow,
            Underflow,
            DivideByZero
        };

        //每个函数的返回值代表计算结果  rc代表执行成功或失败原因
        Number add(const Number& y, ReturnCode& rc) const; 
        Number sub(const Number& y, ReturnCode& rc) const;
        Number mul(const Number& y, ReturnCode& rc) const;
        Number div(const Number& y, ReturnCode& rc) const;
        // ...
  };

  //使用 Number 类
  int f(Number x, Number y)
  {
      // ...
      Number::ReturnCode rc;
      Number sum = x.add(y, rc);

      //这里只列举加法  剩下三种都一样
      if (rc == Number::Overflow) 
      {
        // ...code that handles overflow...
        return -1;
      } 
      else if (rc == Number::Underflow) 
      {
        // ...code that handles underflow...
        return -1;
      } 
      else if (rc == Number::DivideByZero) 
      {
        // ...code that handles divide-by-zero...
        return -1;
      }

      return y;
  }

```

&emsp;&emsp;在使用错误码时，有时不得不修改函数接口，尤其是需要向调用者传递更多的错误信息。例如，如果有5种错误情况，而不同情况的"错误信息"需要使用不同的数据结构，那么您可能会得到一个相当混乱的函数接口(函数引用参数此时需要增加)。

&emsp;&emsp;使用异常处理就不会出现这些混乱。异常处理可被视作一个独立的返回值，就像函数会根据自身抛出的内容自动生成新的返回类型及其返回值。  

&emsp;&emsp;注意:使用错误码时请不要将错误信息存储在命名空间范围、全局或静态变量中，例如`Number::lastError()`。这不是线程安全的。当然，如果你这样做了，你应该写大量注释，警告未来的程序员，你的代码不是线程安全的，如果不进行大量的重写，它可能无法成为线程安全的。


### 7.异常机制将"good path"（或"happy path"）与"bad path"分开意味着什么？What does it mean that exceptions separate the “good path” (or “happy path”) from the “bad path”?

&emsp;&emsp;这是另一个异常处理优于使用错误码的点。 

&emsp;&emsp;`good path`就是执行时没有任何错误代码分支。`bad path`就是执行有错误的代码分支。

&emsp;&emsp;如果异常处理正确，则将`good path`与`bad path`分开。下面举一个简单的例子: f函数可以调用g()、h()、i()、j()函数。当出现`foo`或者`bar`错误时，f函数会立刻处理错误并成功地返回。如果发生了其他的异常错误，f函数也能将错误信息传播给调用者。

```c
  void f()  // f函数使用异常进行处理
  {
    try {
      GResult gg = g();
      HResult hh = h();
      IResult ii = i();
      JResult jj = j();
    }
    catch (FooError& e) {
      // ...code that handles "foo" errors...
    }
    catch (BarError& e) {
      // ...code that handles "bar" errors...
    }
  }
```

&emsp;&emsp;`good path`和`bad path`可以很清楚地分辨出来。try块中的代码分支属于`good path`,当没有错误发生时，你可以清楚的看到代码的执行逻辑,而catch块中的代码以及调用catch块代码的地方都属于`bad path`。 

&emsp;&emsp;现在来看使用返回错误码的方式，使用返回代码会使代码变得混乱，以至于很难看到相对简单的算法。`good path`和`bad path`被混杂在一起

```c
    int f()  //使用错误码
    {
        int rc;  // 存储要返回的错误码

        GResult gg = g(rc);

        if (rc == FooError) {
          // ...code that handles "foo" errors...
        } else if (rc == BarError) {
          // ...code that handles "bar" errors...
        } else if (rc != Success) {
          return rc;
        }

        HResult hh = h(rc);
        if (rc == FooError) {
          // ...code that handles "foo" errors...
        } else if (rc == BarError) {
          // ...code that handles "bar" errors...
        } else if (rc != Success) {
          return rc;
        }

        IResult ii = i(rc);
        if (rc == FooError) {
          // ...code that handles "foo" errors...
        } else if (rc == BarError) {
          // ...code that handles "bar" errors...
        } else if (rc != Success) {
          return rc;
        }

        JResult jj = j(rc);
        if (rc == FooError) {
          // ...code that handles "foo" errors...
        } else if (rc == BarError) {
          // ...code that handles "bar" errors...
        } else if (rc != Success) {
          return rc;
        }
        // ...
        return Success;
    }
```

&emsp;&emsp;上述代码中`good path`和`bad path`都混合起来了，很难看出代码原本的业务逻辑。而使用异常机制的代码的基本功能非常明显。



### 8.我将前面的常见问题解释为异常处理简单易用;我做对了吗？I’m interpreting the previous FAQs as saying exception handling is easy and simple; did I get it right?  
&emsp;&emsp;这么理解是错误的。  

&emsp;&emsp;这并不是说异常处理简单易行。正确的理解是，使用异常处理是值得的。收益大于成本。使用异常机制的成本如下:  

* **异常机制处理不是免费的午餐。**使用异常机制，需要有规范的代码编写和严谨的考虑，为了理解如何正确使用，还是要多读有关这部分的优秀书籍。  
  
* **异常处理不是万能的**。在一个邋遢且无代码编程规范的团队中，无论使用异常机制还是错误码，都会遇到问题。  

* **并非要绝对的使用异常处理错误。**即使决定使用异常而不是返回代码，这并不意味着可以将它们用于所有代码。需要知道何时应通过返回代码报告条件，何时应通过异常报告条件。

&emsp;&emsp;幸运的是，整个行业中对于如何正确使用异常机制方面有很多智慧和见解。


### 9.Exception handling seems to make my life more difficult; that must mean exception handling itself is bad; clearly I’m not the problem, right??


### 10.我有太多的try块;我该怎么办？I have too many try blocks; what can I do about it?

&emsp;&emsp;编码中出现如下代码，极大可能是`返回错误码`思想导致的。

```c 
  void myCode(){
    try {
      foo();
    }
    catch (FooException& e) {
      // ...
    }
    try {
      bar();
    }
    catch (BarException& e) {
      // ...
    }
    try {
      baz();
    }
    catch (BazException& e) {
      // ...
    }
  }
```

&emsp;&emsp;虽然上述代码使用了异常处理的语法，但是代码的整体结构与返回错误码很相似，那么随后开发/测试/维护成本就基本与返回错误码形式的相同了。换句话说，这样确实能减少使用返回错误码，但是这样用缺很糟糕。

&emsp;&emsp;为了避免出现上面的情况，使用每一个try块时都需要扪心自问，<font color="#ff0000">我为什么要在这里使用try块?</font>。以下列出一些可能的回答。  

* **"我之所以这么写是因为，代码可以及时处理异常。catch块处理错误并继续执行，而不抛出任何额外的异常。上层的调用者永远不知道异常已经发生。catch块不会抛出任何异常，也不会返回任何错误代码"**，在这种情况下，这么用没毛病。 

* **"我之所以这么写是因为，可以使用catch块做一些操作(可能是释放掉一些资源的操作)，之后我会重新抛出异常"**，对于这种情况，请考虑使用析构函数来代替try块的操作。比如，一个try块捕获异常，对应的catch块只是关闭了一个文件，然后重新抛出异常，那么可以考虑使用一个File类对象代替try/catch，在File对象析构时关闭文件。这就是RAII。  

* **"我之所以这么写是因为，可以重新包装异常，我捕获一个Xyz类型异常，然后我可以抛出一个Pqr类型异常"**，请不要这样做，考虑使用一个更好的异常抛出的层次结构，尽量不要使用重新包装异常的思想。

&emsp;&emsp;重点在于问问自己**为什么要这样做**？如果你能发掘出自己的真实想法，也许可以找到更好的替代方法来实现。  


### 10.我可以从构造函数中抛出异常吗？析构函数呢？

&emsp;&emsp;对于构造函数来说是的。当无法在构造函数中正确地初始化一个对象时，你应该抛出一个异常。除了通过抛出退出构造函数之外，没有真正令人满意的选择。

&emsp;&emsp;对于析构函数，<<Effective C++>>的条款8认为，当析构函数中向外抛出异常，程序依旧执行则会出现未定义行为，并且提供了两点建议:
1. 析构函数绝对不要向外暴露异常，如果一个被析构函数调用的函数可能抛出异常，析构函数应该捕捉所有异常，然后在**内部消化(依然不向外传播)**或**直接结束程序**。 
   
2. 如果客户(**注意 这里所说的客户是调用函数的程序员**)需要对某个被操作的函数运行时抛出的异常做出反应，那么class应该提供一个普通函数(而非在析构函数中)执行该操作。下面用代码说明。
```c

//这是一个数据库连接，根据RAII思想，在析构函数中使用close()函数，但是close()会抛出异常

class DB
{
    //关闭数据库连接，创建一个单独的函数，其他人可以调用此函数，即使抛出异常，他们可以自己
    //写程序来弥补出现的错误，很可能让程序继续正常运行，这就是给调用者做出反应
    void close();

    ~DB()
    {
      if(//判断是否关闭连接)
      {//没有关闭
            try()
            {
              close(); //这里依旧要调用是为了双保险，万一调用者忘记调用close()函数
            }
            
            catch(...)
            {
              //捕获异常 内部消化(依然不向外传播)或直接结束程序
            }
      }
    }
}
```


### 11.如何处理构造函数执行失败？

&emsp;&emsp;抛出一个异常。  

&emsp;&emsp;构造函数没有返回类型，因此不可能返回错误码。所以，向外通知构造函数执行失败的最佳方法是抛出异常。 

&emsp;&emsp;注意:如果构造函数通过抛出异常而结束，则与对象本身相关联的内存将被清除，不会出现内存泄漏。例如:

```c
      void f()
      {
        X x;             // 如果 X::X() 抛出异常, x的内存空间不会泄漏
        Y* p = new Y();  // 如果 Y::Y() 抛出异常, *p的内存空间不会泄漏
      }
```

### 12.如何处理析构函数执行失败？

&emsp;&emsp;<font color="#ff0000">向日志中写入消息，终止程序运行。但不要向外抛出异常。</font>下面会解释为什么:

&emsp;&emsp;C++规则是，在另一个异常栈解旋的过程中，决不能从正在调用的析构函数中抛出异常。比如，如果调用`throw Foo()`，堆栈中所有的栈帧会被解旋在下面两段代码之间。

```c
     throw Foo()

     .
     .
     .

     catch (Foo e)
```

&emsp;&emsp;在栈解旋期间，所有栈帧中的局部对象将被销毁。如果某个对象的析构函数抛出了一个异常(比如Bar对象的异常)，C++运行时库就会面临困境，无论如何操作都会丢失信息(为什么会丢失信息没看懂)。所以C++语法保证此时此刻会通过`terminate()`终止进程。  

&emsp;&emsp;避免这种情况的简单方法是不要在析构函数中抛出异常。这种经验足以应付99%的场景。


### 13.如果我的构造函数可能抛出异常，我应该如何处理资源？

&emsp;&emsp;对象中的每个数据成员都应该清理自己的混乱。 

&emsp;&emsp;如果构造函数抛出异常，则对象的析构函数不会运行。如果此时对象已经做了一些需要撤回的事情(比如分配一些内存、打开文件),对象内部的数据成员必须要记录需要撤回的操作。

&emsp;&emsp;比如，与其将内存空间分配给原始指针，不如将空间分配给智能指针对象，智能指针被销毁时，智能指针的析构函数会销毁原始指针。`std::unique_ptr`类就是一种智能指针。【TODO 有链接】

&emsp;&emsp;顺便说一下，如果要将一个Fred类对象分配到一个智能指针中，那么对你调用者好一点，在Fred类中进行typedef声明:
```c
      #include <memory>
      class Fred {
      public:
        typedef std::unique_ptr<Fred> Ptr;  //这就相当于this指针了
        // ...
      };
```

&emsp;&emsp;typedef简化了使用对象的所有代码的语法: 调用者可以使用`Fred::Ptr`而不是`std::unique_Ptr<Fred>`:

```c
      #include "Fred.h"
      void f(std::unique_ptr<Fred> p);  // 冗余的
      void f(Fred::Ptr             p);  // 更简便的
      void g()
      {
        std::unique_ptr<Fred> p1( new Fred() );  
        Fred::Ptr             p2( new Fred() );  
        // ...
      }
```

### 14.捕获到异常时应该抛出什么内容?

&emsp;&emsp;C++与其他编程语言的异常机制不同，会很好的包容你要抛出的内容。事实上，你可以抛出任何内容。但这就引出了一个问题，你应该抛出什么？  

&emsp;&emsp;通常来说，**最好是抛出对象变量，而不是内置类型变量**。如果可能，最终应该抛出的是继承`std::exception`的子类对象，这样其他人调用你封装的异常类会轻松(因为继承`std::exception`所以可以捕获很多内容)，其实你还可以向他们提供更多信息(比如，你封装的异常类是对`std::runtimeerror`的增加)。

&emsp;&emsp;最常见的做法是抛出临时变量 : 
```c
  #include <stdexcept>
  class MyException : public std::runtime_error 
  {
    public:
      MyException() : std::runtime_error("MyException") { }
  };

  void f()
  {
    // ...
    throw MyException();
  }
```

&emsp;&emsp;这里，创建并抛出一个`MyException`类型的临时变量。`MyException`类继承自`std::runtime_error`类，后者继承自`std::exception`类。



### 15.发生异常时应该捕获什么？

&emsp;&emsp;可以捕获的内容有，数值(避免)、引用(推荐)、指针(很少用)



### 16.在throw关键字之后没有异常对象是什么意思？它抛出了什么内容？

&emsp;&emsp;你看到的代码可能类似下面这样的:
```c
      class MyException 
      {
        public:
          // ...
          void addInfo(const std::string& info);
          // ...
      };

      void f()
      {
          try 
          {
            // ...
          }
          catch (MyException& e) 
          {
            e.addInfo("f() failed");
            throw; //捕获到了异常 throw后面没有对象变量
          }
      }
```

&emsp;&emsp;上面例子中，当前`throw`使用的含义是重新抛出当前异常。`catch`块中捕获了异常，然后向异常对象中增加了其他信息`e.addInfo("f() failed")`,最后又重新抛出异常。

&emsp;&emsp;通过在程序的重要函数中添加适当的`catch`块，可以实现简单形式的堆栈追踪。这里提到的简单形式的堆栈追踪，应该就是在外界抛出异常时，向即将被抛出的异常对象增加一些信息。感觉就和日志一样。
> **什么是堆栈追踪** : C++异常中的堆栈跟踪就是当程序抛出异常时，能够把导致抛出异常的语句所在的文件名和行号打印出来，以及把调用抛出异常的语句的函数以及其它上层函数信息都打印出来。
> 参考博客              https://www.cnblogs.com/lidabo/p/3635646.html

&emsp;&emsp;另一种重新抛出的用法是异常调度器(exception dispatcher)  

```c
    void handleException()
    {
        try 
        {
          throw;  //重新抛出异常
        }
          
          //下面是处理对应异常的catch块
        catch (MyException& e) 
        {
          // ...code to handle MyException...
        }
        catch (YourException& e) 
        {
          // ...code to handle YourException...
        }
    }
    void f()
    {
        try 
        {
          // ...something that might throw...
        }
        catch (...) {
          handleException();
        }
    }
```
&emsp;&emsp;这个用法允许重复使用单个函数(handleException())来处理其他函数中的异常。


### 17.如何多态化的抛出异常？

&emsp;&emsp;有人会编写如下代码:

```c
      #include <iostream>
      using namespace std;

      class MyExceptionBase 
      {//自定义异常基类
          public:
          
              MyExceptionBase()
              {
                  cout <<"执行 MyExceptionBase()"<<endl;
              }
              
              ~MyExceptionBase()
              {
                  cout <<"执行 ~MyExceptionBase()"<<endl;
              }
      };

      class MyExceptionDerived : public MyExceptionBase 
      {//自定义异常子类
        public:

              MyExceptionDerived()
              {
                  cout <<"执行 MyExceptionDerived()"<<endl;
              }

              ~MyExceptionDerived()
              {
                    cout <<"执行 ~MyExceptionDerived()"<<endl;
              }
      }; 

      void f(MyExceptionBase& e)
      {
          throw e;
      }

      void g()
      {
          MyExceptionDerived e;

          try 
          {
            f(e);
          }

          catch (MyExceptionDerived & e) 
          {
              cout <<"捕获到了MyExceptionDerived 类型异常"<<endl;
          }

          catch (...) 
          {
              cout <<"捕获到了其他类型异常"<<endl;
          }
          cout<<"********************"<<endl;
      }


      int main()
      {
          cout <<"=====start g()====="<<endl;
          g();
          cout <<"=====end g()====="<<endl;

          cout <<"===========end==========="<<endl;
          return 0;
      }

/*执行结果
            =====start g()=====
            执行 MyExceptionBase()
            执行 MyExceptionDerived()       <==创建MyExceptionDerived类型对象
            捕获到了其他类型异常            <==进入的catch分支
            执行 ~MyExceptionBase()         <==这个地方很有意思，应该是在catch(...)中的一个MyExceptionBase副本被销毁了
            ********************
            执行 ~MyExceptionDerived()
            执行 ~MyExceptionBase()
            =====end g()=====
            ===========end===========
*/
```

&emsp;&emsp;如果按照上面的写法测试，结果会令你意想不到，抛出`MyExceptionBase`类型对象，但是却进入了catch(...)分支。出现这种现象的原因是，这根本就不是多态。在f()函数中抛出e,实际上是向外抛出了与表达式e的静态类型相同类型的对象，就像拷贝一样，所以给f()函数传入的是`MyExceptionDerived`类型,但抛出的仍然是`MyExceptionBase`类型。

&emsp;&emsp;正确写法如下:

```c
    #include <iostream>
    using namespace std;

    class MyExceptionBase 
    {//自定义异常基类
      public:
      
        virtual void throwmyslef()
        {
            throw *this;
        }

        MyExceptionBase()
        {
            cout <<"执行 MyExceptionBase()"<<endl;
        }
        
        ~MyExceptionBase()
        {
            cout <<"执行 ~MyExceptionBase()"<<endl;
        }
    };

    class MyExceptionDerived : public MyExceptionBase 
    {//自定义异常子类
      public:

        virtual void throwmyslef()
        {
            throw *this;
        }

        MyExceptionDerived()
        {
            cout <<"执行 MyExceptionDerived()"<<endl;
        }

        ~MyExceptionDerived()
        {
              cout <<"执行 ~MyExceptionDerived()"<<endl;
        }
    }; 

    void f(MyExceptionBase& e)
    {
        e.throwmyslef();
    }

    void g()
    {
        MyExceptionDerived e;

        try 
        {
          f(e);
        }

        catch (MyExceptionDerived & e) 
        {
            cout <<"捕获到了MyExceptionDerived 类型异常"<<endl;
        }

        catch (...) 
        {
            cout <<"捕获到了其他类型异常"<<endl;
        }
        cout<<"********************"<<endl;
    }

    int main()
    {
        cout <<"=====start g()====="<<endl;
        g();
        cout <<"=====end g()====="<<endl;

        cout <<"===========end==========="<<endl;
        return 0;
    }


/*执行结果
            =====start g()=====
            执行 MyExceptionBase()
            执行 MyExceptionDerived()
            捕获到了MyExceptionDerived 类型异常  <=== 这是我们想要的结果
            执行 ~MyExceptionDerived()
            执行 ~MyExceptionBase()
            ********************
            执行 ~MyExceptionDerived()
            执行 ~MyExceptionBase()
            =====end g()=====
            ===========end===========
*/
```

&emsp;&emsp;注意,`throw`语句已移动到虚函数中。`raise()`函数将呈现多态行为，因为`raise()`被声明为`virtual`，e对象是通过引用传递的(应该可以达到指针传递的效果)。`throw`语句抛出的对象依然静态类型，但在`MyExceptionDerived::raise()`中，该静态类型是`MyExceptionDerived`，而不是`MyExceptionBase`。

### 18.当我抛出这个对象时，它会被复制多少次？

&emsp;&emsp;视情况而定，可能是0次

&emsp;&emsp;被抛出的对象必须具有一个公有可被访问的拷贝构造函数。编译器可以生成多次复制抛出对象的代码，包括零次。然而，即使编译器从未实际复制被抛出对象，也必须确保异常类的复制构造函数存在且可访问。

### 19.为什么C++不提供“finally”构造？  

&emsp;&emsp;因为C++支持一种更好的选择:RAII(资源获取即初始化)技术。基本思想就是使用一个局部对象代表一个资源，对象通过析构函数销毁那么资源也被释放。这样程序员就不会忘记释放资源了。  

```c
    // wrap a raw C file handle and put the resource acquisition and release  包装原始C文件句柄，并进行资源获取和释放

    // in the C++ type's constructor and destructor, respectively 资源获取和释放分别在C++的构造函数和析构函数中进行

    class File_handle 
    {
        FILE* p;

        public:
            File_handle(const char* n, const char* a)
                { p = fopen(n,a); if (p==0) throw Open_error(errno); }
            File_handle(FILE* pp)
                { p = pp; if (p==0) throw Open_error(errno); }
            ~File_handle() { fclose(p); } //注意这里进行了文件的关闭!!!!
            operator FILE*() { return p; } 
    };
    // use File_handle: uses vastly outnumber the above code  使用文件句柄
    void f(const char* fn)
    {
        File_handle f(fn,"rw"); // open fn for reading and writing
        
    } // automatically destroy f here  在这里自动销毁f
      // even if there's an exception, so this is exception-safe by construction 即使存在异常，这在构造上也是异常安全的
```

&emsp;&emsp;在编写代码中，最坏的情况是每个需要的资源都要有一个"资源句柄"类。然而，我们不必为每个已获取的资源使用"finally"来释放它们。在实际使用中，资源获取远多于资源种类，因此使用RAII比"finally"需要的代码量更少。


### 20.为什么捕获异常后不能继续？

&emsp;&emsp;换句话说，为什么C++不提供一个机制，用于返回抛出异常的点并从那里继续执行？

&emsp;&emsp;为了正确地编写这样的代码，抛出异常的编写者和处理异常的编写者需要对彼此的代码和上下文有深入的了解。这造成了一种复杂的相互依赖关系，只要允许，就会导致严重的维护问题。



[返回值例子]:data:image/png;base64,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