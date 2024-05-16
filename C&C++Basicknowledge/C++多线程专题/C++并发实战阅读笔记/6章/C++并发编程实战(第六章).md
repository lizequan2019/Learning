[TOC]

---

# 6. 基于锁的并发数据结构设计

&emsp;&emsp;本章内容 

* 为并发操作设计数据结构意味着什么 
  
* 并发数据结构设计指导 
  
* 并发数据结构设计的实例

&emsp;&emsp;为编程问题选择合适的数据结构将会成为整个解决方案的关键，并行编程也是如此。


---

## 6.1 并发设计的意义？

&emsp;&emsp;在最基本层面上，并发数据结构的设计就意味着多个线程可以同时地访问数据结构，各个线程进行相同或不同的操作时观察到的数据结构都是不冲突的，不会出现竞态条件。 

&emsp;&emsp;在高层面上讲，真正的并发在于提供更高的并发程度，让各个线程有更多机会按并发方式访问数据结构。  

&emsp;&emsp;在前面使用的互斥每次仅能让一个线程获取锁进行操作，这种方式是明令阻止真正的并发访问，也被称为串行化。**核心思想只有一个，减少保护区域，减少序列化操作，就能提升并发访问的潜力。**


### 6.1.1 并发数据结构设计指南

&emsp;&emsp;在设计时需要考虑两个方面，**线程安全**和**真正的并发**。  

&emsp;&emsp;在第三章已经介绍过了构建线程安全的数据结构的基础方法:  

* 确保没有线程可以看到数据结构的`不变量`被另一个线程的操作破坏的状态。我对这个`不变量`的概念有一点理解了，例如有一个共享数据b，b此时没有被任何线程操作，就叫做x状态，当线程A在操作b时，b在被其他线程观察时应该是处于x状态，其他线程是看不到A修改b的过程(当然也就无法参与其修改过程)。  

> 【不变量】就是一定为真的的东西。比如，队列头一定指向队首元素或者为空。它不可能指向队列中间的某个元素。数据元素包含的指针一定指向队列中的下个元素，或者为空，而不可能指向比如说，下下个元素。然而，程序有时为了方便，可能会临时的破坏这种规定，比如说，往队列中元素A后面插入元素的时候，就需要A指向新元素，然后新元素指向A原先指向的元素，这样，在新元素指向A原先指向的元素之前，这个不变量就被破坏了，因为A不是指向队列中下一个元素(A指向同时指向了新元素和下下个元素)。在多线程的时候，这个`破坏`不能被其他线程看到，所以，就必须在插入元素前加锁，使得在别的线程看来，A指向的是队列中下一个元素，即，`不变量是永远正确的`。虽然在当前线程来看，不变量被临时破坏了，但是在被其他线程看见前，该线程将不变量恢复正确，所以从别的线程来看，不变量总是为真的。

* 提供的函数**操作应该完整、独立、而非零散的分解步骤**，以避免函数接口固有的竞态条件。

* 注意数据结构在抛出异常时会出现的行为，不要破坏`不变量`

* 将死锁的概率降到最低。使用数据结构时，需要限制锁的范围，且避免嵌套锁的存在。

&emsp;&emsp;在考虑这些细节之前，还需要考虑你希望在用户的数据结构上增加什么约束条件;当一个线程通过一个特殊的函数对数据结构进行访问时，那么还有哪些函数能被其他的线程安全调用呢？

&emsp;&emsp;这个问题至关重要，普通的构造函数和析构函数需要独立访问数据结构，所以用户在使用的时候，就不能在构造函数完成前，或析构函数完成后对数据结构进行访问。当数据结构支持赋值操作，swap()，或拷贝构造时，作为数据结构的设计者，即使数据结构中有大量的函数被线程所操纵时，你也需要保证这些操作在并发环境下是安全的(或确保这些操作能够独立访问)，以保证并发访问时不会出现错误。

&emsp;&emsp;接着要考虑的是实现真正的并发访问，作者提供了一些在设计时要扪心自问的问题。这些问题可以归结为一个核心问题:如何才可以只**保留最必要的串行操作**,将串行操作减少至最低程度而达到最大限度地实现真正的并发访问。

* 锁保护区域中的某些操作是否可以在锁保护的范围之外执行？

* 数据结构中不同的区域是否能被不同的互斥量所保护？

* 所有操作都需要同级互斥量保护吗？

* 能否通过简单的改动，提高数据结构的并发程度，为并发操作增加机会，而不影响操作语义(**可以理解为修改后程序原本的含义不变**)?

&emsp;&emsp;所有这些问题都以一个单一的想法为指导:如何最大限度地减少必须发生的串行操作，并实现最大程度的真正并法。


---


## 6.2 基于锁的并发数据结构

&emsp;&emsp;基于锁的并法数据结构完全是为了确保在访问数据时锁定正确的互斥对象，并确保锁保持的时间最短。在数据结构中只使用一个互斥对象保护数据是很困难的，需要确保数据不能在互斥锁保护之外被访问，还要提防接口固有的竞态条件。当你使用多个互斥量来保护数据结构中不同的区域时，问题会暴露的更加明显，当操作需要获取多个互斥锁时，就有可能产生死锁。所以，在设计时，使用多个互斥量时需要格外小心。

&emsp;&emsp;我们先来看看在第3章中栈的实现，这个实现就是一个十分简单的数据结构，它只使用了一个互斥量。但是，这个结构是线程安全的吗？它离真正的并发访问又有多远呢？

### 6.2.1 采用锁实现线程安全的栈容器

&emsp;&emsp;第三章曾经介绍过线程安全的栈容器，我们意在编写类似`std::stack<>`的线程安全的栈容器，它支持将`push`和`pop`函数。

```c
    #include <exception>

    //报告错误的结构体
    struct empty_stack: std::exception
    {
        const char* what() const throw();
    };

    template<typename T>
    class threadsafe_stack
    {
        private:
            std::stack<T> data;    //原生的stack数据结构
            
            mutable std::mutex m;  //单一的互斥量 保证在同一时间内，只有一个线程可以访问到数据，所以能够保证，数据结构的“不变量”被破坏时，不会被其他线程看到

        public:
            threadsafe_stack(){}   //构造函数

            threadsafe_stack(const threadsafe_stack& other) //拷贝构造函数
            {
                std::lock_guard<std::mutex> lock(other.m);
                data=other.data;
            }

            threadsafe_stack& operator=(const threadsafe_stack&) = delete; //赋值构造函数删除

            void push(T new_value) 
            {
                std::lock_guard<std::mutex> lock(m);
                data.push(std::move(new_value));  // 1
            }

            std::shared_ptr<T> pop()
            {
                std::lock_guard<std::mutex> lock(m);
                if(data.empty()) 
                    throw empty_stack();  // 2

                std::shared_ptr<T> const res(std::make_shared<T>(std::move(data.top())));  // 3
                data.pop();  // 4
                return res;
            }

            void pop(T& value)
            {
                std::lock_guard<std::mutex> lock(m);
                if(data.empty()) 
                     throw empty_stack();
                     
                value=std::move(data.top());  // 5
                data.pop();  // 6
            }

            bool empty() const
            {
                std::lock_guard<std::mutex> lock(m);
                return data.empty();
            }
    };
```

&emsp;&emsp;`std::mutex m`是数据结构中唯一的互斥量，这种方式保证了任何时刻都仅有一个线程访问数据。**保持了不变性**。
&emsp;&emsp;在`empty()`和多个重载的`pop()`之间，潜藏着竞态条件的危险，
```c
//我认为这里提到的危险是这样的变量来自代码6.1

     //线程A                             //线程B
     threadsafe_stack A;                threadsafe_stack  B;        

     //这样在多线程中是存在固有的竞态条件,当栈容器中只有一个元素时
     if(A.empty())  
     {
                                         if(B.empty())
                                         {
        A.pop();//会执行
                                            B.top();//会执行，但是此时没有元素了，错误
     }
                                         }
```

&emsp;&emsp;但是`pop()`函数本身就可以判断栈容器是否为空，所以一般也不会像上面这样用，按书中的说法，**这不属于恶性的竞态条件**。

<br><br>

&emsp;&emsp;有以下几个操作可能出现异常，这里分析一下抛出异常是否安全。

&emsp;&emsp;<font color=#ff0000>给互斥加锁可能出现异常，但这很罕见，因为只有互斥本身存在问题或系统资源耗尽才会出现。</font>而且每个成员函数内部的第一个操作就是加锁，即使抛出异常此时也没有改动栈容器的数据，所以抛出异常也是安全的。由于使用**std::lock_guard<>**保证了每个互斥量都会解锁。

&emsp;&emsp;其余可能出现异常代码参见[6.1](./第六章代码例子/6-1code.cpp)，我对这些的理解是**1.在代码抛出异常时，如果没有操作共享数据，这可以算作是安全的 2.如果是C++内置类型发生了异常(当然几率很低)，那么C++运行时库确保不会出现内存泄露并正确地销毁创建的对象。**

&emsp;&emsp;<u>最后作者还是指出了这段代码还是有可能引发死锁，原因在于<font color=#ff0000>我们在持锁期间执行用户代码</font>。大致意思是，[6.1](./第六章代码例子/6-1code.cpp)代码中的`std::stack<T> data`中存储的数据类型是用户自定义的，当然这些类型不是简单的int和string，大多数都是一个**结构体、类**这样的复合类型。那么用户肯定会在这些复合类型中增加符合自己业务的方法，这些方法中也有可能存在的获取锁的操作。**这些是引发死锁的一个前提**，当调用`threadsafe_stack`中一个方法且为持有锁`std::mutex m`状态,然后还调用了`std::stack<T> data`中元素(可能是个类)的内部方法，如果这些方法中也有获取锁的步骤，此时就会有两个不同的锁(一个是`threadsafe_stack`类中的，一个是用户代码中自定义的)，两个不同的锁形成了嵌套锁，此时会引发死锁。合理的解决方式是对栈容器的使用者提出要求，由他们负责保证避免以上死锁场景。</u>

&emsp;&emsp;栈容器的成员函数中都使用了`std::lock_guard<>`保护数据，这没问题。只是其中的构造函数和析构函数不是安全的成员函数，必须由使用者自己保证:若栈容器还未构建完成，则其他线程不得访问数据，并且，只有当全部线程都停止访问后，才可以销毁栈容器。

&emsp;&emsp;在并发性上，代码中**仅仅使用锁迫使线程实现串行化**，没有使用类似`条件变量`的机制，所以代码的并发性很低，线程的大部分时间都是等待。


<br><br><br>




## 6.2.2采用锁和条件变量实现线程安全的队列容器

&emsp;&emsp;本章节使用第4章代码举例说明,代码参见[6.2](./第六章代码例子/6-2code.cpp)

&emsp;&emsp;本例代码使用了**条件变量机制**，使得线程函数pop时不需要多次调用empty()进行确认。其他的和上面的栈容器差不多。

&emsp;&emsp;作者也对此代码做出了点评，[6.2](./第六章代码例子/6-2code.cpp)中的`push()`中的条件变量中唤醒其他线程函数操作`data_cond.notify_one();`有瑕疵。如果被唤醒的线程函数出现了异常退出，那么剩余的线程函数也不会被唤醒。也可以使用`data_cond.notify_all();`，但是开销会增加(**感觉大佬就是对性能要求极致，膜拜一下**)，因为大部分线程函数发队列依然为空(push每次只放一个数据，这么多线程不够分)，然后重新睡眠。第二种方式，是在捕获异常操作中继续调用`data_cond.notify_one();`再唤醒另一个线程，就像击鼓传花一样。<font color=#ff0000>第三种处理方式</font>是将`std::shared_ptr<>`的初始化语句移动到push()的调用处，令队列容器改为存储`std::shared_ptr<>`而不是直接存储数据的值。将`std:：shared_ptr<>`从`std:：queue<>`中复制出来，不会抛出异常(**可以看cplusplus上面对shared_ptr的讲解，拷贝构造函数声明就是 noexcept**)。

&emsp;&emsp;书中采用了第三种优化方式，参见代码[6.3](./第六章代码例子/6-3code.cpp) 【TODO 我对代码中第五点的注释不太明白，为什么使用shared_ptr<T>就可以在锁外分配内存】  

&emsp;&emsp;在并发性上，和前面栈容器一样都是由唯一的互斥量保护整个数据结构，迫使每次只能容许一个线程操作线程数据

<br><br><br>

## 6.2.3 使用细粒度锁和条件变量实现线程安全的队列

&emsp;&emsp;在[6.2](./第六章代码例子/6-2code.cpp)和[6.3](./第六章代码例子/6-3code.cpp)代码中有一个需要被保护的数据队列而我们只为此使用了一个互斥量。为了使用颗粒更加细腻的锁，你需要了解队列内部的组成部分，并将每个数据项关联到互斥量。

&emsp;&emsp;队列数据结构最简单的形式莫过于一个单链表，如下图

![avatar][单链表数据结构]

&emsp;&emsp;[6.4](./第六章代码例子/6-4code.cpp)代码是基于[6.2](./第六章代码例子/6-2code.cpp)代码清单简单实现的一个队列，仅仅支持单线程使用。

&emsp;&emsp;请注意，代码[6.4](./第六章代码例子/6-4code.cpp)中使用`std::unique_ptr<node>`来管理节点，因为这可以确保它们以及它们引用的数据在不需要时被删除，而无需编写显式删除。从队列的头节点开始一致到队列末端，相邻节点之间都按前后顺序形成归属关系，末端节点划归前方节点的`std::unique_ptr<node>`所有，但仍需对末端节点直接进行控制，所以还需要一个指向它的原生指针`node* tail`。

&emsp;&emsp;虽然这个代码实现在单线程环境中可以很好的工作，但你在多线程中使用细粒度锁则会出现一些问题。[6.4](./第六章代码例子/6-4code.cpp)中你有两个数据项，head节点和tail节点，原则上你可以使用两个互斥量分别保护`head`、`tail`,可问题就会随着而来。  

&emsp;&emsp;最明显的问题就是`push()`函数能同时修改`head`和`tail`,所以这就需要两个互斥量来分别锁住它们。

&emsp;&emsp;这看起来问题不大，但很不幸，因为很容易出现死锁的情况(**locking both mutexes would be possible**)。在队列只有一个元素的时，`try_pop()`访问`head->next`弹出元素，`push()`访问`tail->next`增添元素，如果没有去比较头节点和尾节点的内部数据，就无法判断它是不是同一个节点，在同时执行`push()`和`try_pop()`，头节点和尾节点的互斥量会形成死锁。如果还是采用一个互斥量对两个变量进行互斥操作，那么与以前相比并无提升。那如何破解之？ 

<br><br>

&emsp;**通 过 分 离 数 据 启 用 并 发 性**

&emsp;&emsp;可以通过预先分配一个没有数据的虚拟节点来解决这个问题，以确保队列中始终至少有一个节点，将头部访问的节点与尾部访问的节点分开。对于空队列，头和尾现在都指向虚拟节点，而不是NULL。这很好，因为如果队列为空，try_pop()不会访问head->next。如果向队列中添加一个节点，那么头部和尾部现在指向不同的节点，因此head->next和tail->next上没有竞争。缺点是为了虚拟节点，需要通过指针间接存储数据，增加了访问路径长度。代码实现参见[6-5](./第六章代码例子/6-5code.cpp)  

&emsp;&emsp;`try_pop()`函数的变化相当小。首先，不再是将`head`与NULL比较，而是`head`和`tail`进行比较【3】。其次，现在节点通过指针存储数据，所以可以直接通过指针获取数据【4】，而不是创建一个实例用来接收数据。  

&emsp;&emsp;`push()`函数有大的变化，首先必须创建一个数据对象并且使用`shared_ptr<>`获取其所有权【7】。代码中创建的新节点会成为新的虚拟节点，所以代码中的`node`数据结构不需要有自定义构造函数【8】。节点的数据都是通过赋值修改的，代码中旧的虚拟节点的数据来源于`new_data`的赋值【9】。最后，为了依然存在虚拟节点，还必须创建一个，链表的`tail`指针会指向虚拟节点。

&emsp;&emsp;现在来分析一下，这些改变的好处以及如何做到队列的线程安全。  

&emsp;&emsp;`push()`函数现在只能访问`tail`指针，这算是一个提升。`try_pop()`可以访问`head`和`tail`，但是`tail`也仅仅是在函数开始位置需要，所以锁的粒度会很小。最大的好处是，虚拟节点意味着`try_pop()`和`push()`永远不会在同一个节点上操作同时操作，所以`head`和`tail`可以分别使用一个互斥量进行保护而非统一使用一个互斥量(**说明不会发生死锁了**)。但是具体应该在哪一处加锁呢？

&emsp;&emsp;`try_pop()`函数不太好处理。首先，你需要使用互斥量加锁保护`head`，然后保持直到不再使用`head`。因为会有多个线程要执行`try_pop()`，所以第一步要加锁来明确哪个线程先执行。一旦`head`在【5】被改变，此时可以解锁互斥量。返回值操作【6】无需保护。  

&emsp;&emsp;还有对`tail`访问时也需要使用对应的互斥量加锁。因为函数中仅访问了一次`tail`，所以在临近访问`tail`的位置加锁即可，最好的做法是对`tail`加锁和访问封装在一个函数。这样使得代码清晰，如代码[6-6](./第六章代码例子/6-6code.cpp)。

&emsp;&emsp;代码中，`push()`函数较简单，仅做的修改是使用互斥量`tail_mutex`保护数据结构，而且不变量没有被破坏。

&emsp;&emsp;回顾6.1.1节中的指引，按严格的标准评判这段代码。先明确程序含有哪些不变量，接着再查证它们是否被破坏。  

* tail->next == nullptr
* tail->data == nullptr
* head == tail 说明队列为空
* 对于每个节点x,只要x!=tail，则x->data指向一个T类型的实例，且x->next指向后续节点。x->next==tail说明x是最后一个节点。
* 从head指针指向的节点出发，我们沿着next指针反复访问后续节点，最终会到达tail指针指向的节点。  

<br>

&emsp;&emsp;`try_pop()`函数很有趣，事实证明，对`tail_mutex`加锁不但是保护了`tail`访问所必要的，而且还确保了不会在读取`head`数据时发生竞态条件。如果没有`tail_mutex`互斥量，就会有极大可能出现两个线程分别同时调用`try_pop()`和`push()`,这时候操作顺序就是未定义的。虽然每个成员函数都在互斥上加锁，但是它们锁住的互斥各不相同，所以它们有可能访问相同的数据。毕竟队列里的全部数据都来自`push()`的调用，数据由它增加。多个线程可能会出现以未定义的顺序访问同一项数据，根据第五章的分析，这有可能构成数据竞争。幸好`get_tail()`函数中使用了`tail_mutex`互斥量解决了问题。`get_tail()`和`push()`是对同一个互斥量加锁，所以调用有了明确的顺序。先调用`get_tail()`则会获取旧`tail`节点，先调用`push()`则会获取新`tail`节点以及新数据赋值到原来的tail节点上。 

&emsp;&emsp;在`head_mutex`互斥加锁后再调用`get_tail()`也是很重要的一点。如不这么做，就会像下面线程B一样调用`pop_head()`，但因为线程A先一步调用并且持锁而被阻塞在调用`get_tail()`和`head_mutex`互斥量加锁之间。这会招致更严重的问题。

```c
//下面是一个有缺陷的实例

    //线程A                                                              //线程B
    std::unique_ptr<node> pop_head()   
    {
        node* const old_tail=get_tail(); //【1】在锁保护区外获取旧的尾节点
                                                                        node* const old_tail=get_tail(); //阻塞
        std::lock_guard<std::mutex> head_lock(head_mutex);              //线程B需要等到线程A 取消head_mutex持锁才能继续
                                                                        
        if(head.get()==old_tail) //【2】
        {
            return nullptr;
        }

        std::unique_ptr<node> old_head=std::move(head);
        head=std::move(old_head->next); //【3】
        return old_head;
    }
                                                                       std::lock_guard<std::mutex> head_lock(head_mutex);
```

&emsp;&emsp;在锁保护区域外调用`get_tail()`【1】，等到当前线程可以在使用`head_mutex`加锁初始化线程，你会发现`head`和`tail`全都被修改，`get_tail()`返回的节点可能不再是尾节点，甚至不再是队列的节点。即使`head`就是指向最后一个节点，这也意味着`head`和`old_tail`的比较操作会出错。因此，当你更新`head`节点【3】，你可能会将`head`指针向后移动超出`tail`到队列的末尾，从而破坏了数据结构。   

&emsp;&emsp;[6-6](./第六章代码例子/6-6code.cpp)中，在`head_mutex`互斥量保护区域内调用`get_tail()`，这确保了不再会有其他线程能修改`head`指针，还保证了在调用`push()`加入新节点时，`tail`指针仅能从队列末尾向外移动，该行为绝对安全。`head`指针不会越过`get_tail()`所返回的位置，不变量保持成员。

&emsp;&emsp;一旦`pop_head()`将头节点从队列中移除，互斥量将被解锁。假如头节点是真实节点，`try_pop()`可以获取数据并销毁节点，假如是虚拟节点那就返回一个含有NULL值的`std::shared_ptr<>`实例。同时仅有一个线程可以访问节点，这样是安全的。

&emsp;&emsp;下一个设计是队列的对外接口，它们来自与代码[6.2](./第六章代码例子/6-2code.cpp)，所以这里的分析与前文相同，这里不存在接口固有的竞态条件。  

&emsp;&emsp;异常处理是变得更复杂了。因为你已经改变了数据的内存分配模式，所以异常现在可以从任意代码位置抛出。`try_pop()`中能抛出异常的操作就是互斥量加锁了，但是在互斥量持锁前数据是不会被修改的。因此`try_pop()`是线程安全的。另一方面，`push()`中在堆上为T类型变量分配内存和创建新的节点都会可能出现异常抛出。但是这些新创建的对象是被赋值到智能指针上，所以当异常发生时它们的内存会被自动释放。一旦互斥量持锁，`push()`中剩余的操作都不会抛出异常，任务圆满完成，`push()`是异常安全的函数。

&emsp;&emsp;我们没有改变接口的外在形式，所以不会出现死锁的情况。成员函数内部同样没有机会，唯一需要获取两个锁的操作位于`pop_head()`函数内，而它总是先锁住`head_mutex`然后再对`tail_mutex`加锁，故死锁不会出现。

&emsp;&emsp;再有就是并发的可能性了，与[6.2](./第六章代码例子/6-2code.cpp)中的数据结构相比，[6.6](./第六章代码例子/6-6code.cpp)数据结构的并发范围要大得多，因为锁的粒度更细，更多的操作是在锁之外完成的。例如，在`push()`中，新节点和新数据项是在不持有锁的情况下分配的。这意味着多个线程可以同时分配新节点和数据项，而不会出现问题。每次只有一个线程可以将新节点添加到列表中，但这样做的代码只是一些简单的指针赋值，因此锁根本不会保持太长时间，与[6.2](./第六章代码例子/6-2code.cpp)基于`std::queue`的实现相比，内部的所有内存分配操作都是在持锁状态完成的。

&emsp;&emsp;同样，`try_pop()`只在互斥`tail_mutex`上短暂持锁，以抱回`tail`指针的读取。因此，`try_pop()`的整个调用过程几乎都可以与`push()`并发执行。队列节点通过`std::unique_ptr<node>`的析构函数删除，该操作开销高，所以在互斥`mutex`的保护范围以外执行，因而在持锁范围内的操作被缩减至最少。这样就增加了tyr_pop()的并发调用数目，虽然每次只允许一个线程调用`pop_head()`，但多个线程可以并发执行`try_pop()`的其他部分，安全地删除各自旧有的头节点并返回数据。


&emsp;**等 待 数 据 弹 出**

&emsp;&emsp;目前[6.6](./第六章代码例子/6-6code.cpp)提供了一个具有细粒度锁且线程安全的队列，但它只支持`try_pop()`(并且只有一个重载)。那可以将[6.2](./第六章代码例子/6-2code.cpp)代码中`wait_and_pop()`函数以细粒度锁的方式实现吗？

&emsp;&emsp;答案是肯定的，如果要实现`wait_and_pop()`函数，那么`push()`函数也需要修改，看起来这很容易，在函数最后面加上`data_cond.notify_one()`即可，就像[6.2](./第六章代码例子/6-2code.cpp)。事情其实没那么简单，你之所以使用细粒度锁是为了实现最大数量的并发。如果在调用`notify_one()`时保持互斥锁定(就像[6.2](./第六章代码例子/6-2code.cpp)一样)，假设线程被唤醒早于解锁互斥量，那么线程还是必须等待互斥量解锁。另一方面，假设互斥量解锁早于线程被唤醒，那么互斥量是可以等待线程被唤醒并加锁的(假设此时没有其他线程去抢占互斥量)。这是一个小改进，但在某些情况下可能很重要。参见代码[6.8](./第六章代码例子/6-8code.cpp)

&emsp;&emsp;`wait_and_pop()`更加复杂，因为你必须确定在哪里等待被唤醒，判断条件的`predicate`是什么，需要哪个互斥量被加锁。等待唤醒的条件是"队列非空"，用代码表示为`head!=tail`。按照这种写法，要求两个互斥`head_mutex`和`tail_mutex`都被锁住，但是在[6.6](./第六章代码例子/6-6code.cpp)中就已经决定仅仅在访问`tail`指针时对`tail_mutex`加锁，头节点和尾节点的比较是不需要加锁的，这里也应该一样。如果我们将判断条件`predicate`设置为`head!=get_tail()`,则仅仅需要对`head_mutex`加锁，加锁后再调用`data_cond.wait()`。只要加入了等待的逻辑，这种实现就与`try_pop()`一样。  

&emsp;&emsp;对于`try_pop()`的另一个重载和对应的`wait_and_pop()`的重载，我们也要谨慎思考和设计。如果你用`old_head`的数据拷贝赋值给入参`value`来取代向外返回`old_head`，就可能出现与异常有关的问题。根据这种改动，在拷贝赋值操作执行时，数据以及从队列中删除，互斥量已经解锁，剩下的是就是将数据返回给调用者。但是，如果拷贝赋值操作抛出了异常(完全有可能)，则该项数据丢失，因为它无法回到队列本来的位置上。(std::shared_ptr<>的拷贝赋值操作是不会出现异常的)

&emsp;&emsp;若队列模板使用时采用了实例类型T，而此类型支持不抛出异常的移动赋值操作，或不抛出异常的交换操作，我们即可使用类型T。但还是有一个通用的解决方案(可以用于任何类型的T)比较好。要实现这种方案，需要在队列移除节点之前，将可能抛出异常的操作移动到锁的保护范围之内。这就意味着需要额外重载`pop_head()`,在改动队列之前就获取其存储的值。 参见代码[6.9](./第六章代码例子/6-9code.cpp)

&emsp;&emsp;相比而言，`empty()`就很简单了，只需要锁住互斥`head_mutex`，然后检查`head == get_tail()`，参见代码[6.10](./第六章代码例子/6-10code.cpp)。

&emsp;&emsp;此小结的类的声明参见[6.7](./第六章代码例子/6-7code.cpp)




## 总结

&emsp;&emsp;这个队列实现将作为第7章中介绍的无锁队列的基础。它是一个**无限队列**；只要有可用内存，线程就可以继续将新值推送到队列中。无界队列的替代方案是有界队列，其中队列的最大长度在创建队列时是固定的。一旦有界队列已满，将更多元素推送到队列的操作将失败或阻塞，直到队列自身弹出以前的元素以腾出空间。当基于要执行的任务在线程之间划分工作时，有界队列可用于多线程的分配工作，它能够依据待执行的任务的数量，确保工作在各线程中均匀分配。它能防止一下情形发生:某些线程向队列添加任务的速度过快，远超线程从队列领取任务的速度。

&emsp;&emsp;要实现这个功能，仅需简单地扩展本节讲解的无线队列代码:只需限制`push()`中的条件变量上的等待数量。我们需要等待队列中的数据被弹出，所含数据数目小于其最大容量，而不是等待`pop()`执行完后必须使队列非空而进行数据被压入。



# 6.3 设计更复杂的基于锁的数据结构

&emsp;&emsp;堆栈和队列很简单: 1.接口非常有限 2.它们紧紧围绕特定用途。  

&emsp;&emsp;并非所有的数据结构都那么简单；大多数数据结构支持多种操作。原则上，这会带来更大的并发机会，但也会使保护数据的任务更加困难，因为有多种访问模式要顾虑周全。在设计为并发访问服务数据结构时，精准把握各种操作的特性非常重要。

&emsp;&emsp;为了了解其中的一些问题，让我们看看查找表(lookup table)的设计。


## 6.3.1使用锁编写线程安全的查找表

&emsp;&emsp;一个查找表或者字典是将一种类型值(key所代表的值)与另一个相同或不同类型的值做映射关联。通常，这种结构背后的含义是允许代码查询key值关联的数据。在C++标准库中，与此功能相关的容器有,`std::map<>,std::multimap<>, std::unordered_map<>, and std::unordered_multimap<>`。

&emsp;&emsp;查找表与堆栈或队列具有不同的使用模式。堆栈或队列容器中的每个方法几乎都会修改容器本身，而查找表很少修改。曾经提到过得DNS缓存就是使用查找表的场景，它的接口比std::map还少。  

&emsp;&emsp;正如你在堆栈和队列中看到的,当有多个线程同时访问数据结构时，标准容器的接口并不合适，因为接口设计中存在固有的条件竞争，所以需要减少和修改它们。  

&emsp;&emsp;从并发角度看，std::map接口最大的问题就是迭代器。尽管其在他线程访问或者修改容器时，迭代器可以安全的访问容器，但这依然是一个棘手的命题。正确处理迭代器需要你处理一些问题，例如另一个线程删除迭代器所引用的元素，这可能会非常复杂。为了查找表接口的线程安全，首先要跳过迭代器。考虑到std::map(或者其他关联容器)的接口是严重依赖迭代器的，所以专门的从头设计一遍接口很有价值。  

&emsp;&emsp;对于查找表来说，这里仅有一些基本操作。 

* 增加一对key-value 
* 通过key去修改关联的value
* 移除key以及它关联的value
* 获取key关联的value  
  
&emsp;&emsp;还有一些容器的操作也很有用，比如检测容器是否为空，一组key的快照或者一组<key,value>的快照。【TODO 没看懂】  
&emsp;&emsp;如果你坚持简单的线程安全准则，比如不返回引用，并且使用互斥量锁住每一个成员函数，这些操作都是安全的，不是在其他线程调用前调用就是在调用后调用。一个新的<key,value>对被增加时会存在竞态条件。如果两个线程增加一个key对应的value,仅有第一个有效，第二个会失败。一种可能的做法是将增加函数和修改函数合并到一个成员函数中，正如[3.13](./第三章代码例子/3-13code.cpp)  


&emsp;&emsp;从接口的角度来看，还有一点要特别处理，使用if判断获取关联key的value。当查询的key不存在时，可以返回用户提供的默认值

```c
    mapped_type get_value(key_type const& key, mapped_type default_value);
```  

&emsp;&emsp;在这种情况下，没有显式提供默认值则可以使用`mapped_type`的默认构造函数中的默认值。同时还可以扩展返回参数为`std::pair<mapped_type,bool>`而不仅仅是`mapped_type`实例，其中bool表示value是否存在。另一种选择是返回一个引用value的智能指针，如果指针为null,则表示没有value存在。  

&emsp;&emsp;如前面所述，一旦确定了接口，就可以通过在每个函数中使用一个互斥量和锁保证底层数据结构的线程安全(这里假设没有接口固有的竞态条件)。但是将读数据结构函数和修改数据结构函数分离则会损失并发的可能性。有一个方法是，使用一个同时允许运行多个读线程和同时仅允许运行一个写线程的锁，比如[3.13](./第三章代码例子/3-13code.cpp)中使用的`std::shared_mutex`。虽然是提高了访问数据的并发性，但是修改数据的并发性没有变化。还可以做的更好  




**基 于 细 粒 度 锁 的 MAP 数 据 结 构 设 计**

&emsp;&emsp;与6.2.3节中讨论队列一样，为了能够使用细粒度锁，我们需要仔细观察数据结构的细节而不是包装一个例如std::map<>的内置容器。实现像查找表这样的关联容器有三种常见的方法。  

* 二叉树、如红黑树
* 有序数组
* 哈希表 

&emsp;&emsp;二叉树并没有为扩展并发机会提供太多发挥空间，每一次查询或修改必须从访问根节点开始，所以根节点必须被锁保护。访问线程会逐层向下移动，根节点上的锁会随之释放。尽管如此，比起为整个数据结构单独使用一个锁，这种情形好不了多少。

&emsp;&emsp;排序数组更差，因为它无法预支所查找的目标位置，我们只能对整个数组使用单一的锁。  

&emsp;&emsp;只剩下哈希表了，假设一个哈希表具有一定数量的哈希桶【TODO 了解哈希表与哈希桶】，每个key都属于一个桶，key本身的值和哈希函数决定key具体属于哪个哈希桶。这意味这您可以安全地为每个哈希桶设置单独的锁。若再采用共享锁，支持多个读线程或一个写线程，就会令并发操作的机会增加N倍，其中N是哈希桶的数目。缺点是需要一个好的为key服务的哈希函数。C++标准库提供了`std::hash<>`模板，您可以将其用于此目的。它已经特化了基本类型(如int)和常见库类型(如std:：string)，用户也可以轻松地特化其他键类型。如果使用标准库的无序容器，并将用于进行哈希处理的函数对象的类型作为模板参数，则用户可以选择是将其键类型专用于`std::hash`，还是提供单独的哈希函数。

&emsp;&emsp;线程安全的查找表实现参见[6.11](./第六章代码例子/6-11code.cpp),这个实现采用`std::vector<std::unique_ptr<bucket_type>>`数据结构来存储哈希桶，在构造函数中可以指定哈希桶的数量(默认值是19，这是一个任意的质数,哈希表适合使用质数个的哈希桶)。每个哈希桶被实例化的`std::shared_mutex`对象所保护，为了实现对每个哈希桶的多并发读取或者单个修改数据。

&emsp;&emsp;因为哈希桶的数量固定，所以`get_bucket()`函数被调用时不需要锁保护(【8】【9】【10】)，并且根据各个函数的不同操作特性，可以使用共享锁保护哈希桶(只读)【3】，也可以使用独占锁保护(读/写)【4】【5】。

&emsp;&emsp;`value_for`、`add_or_update_mapping`、`remove_mapping`这三个函数都使用`find_entry_for`函数判断哈希桶中是否有key对应的value。每一个哈希桶内部包含的是一个<key,value>为单个元素的队列，所以增加和移除元素都是很简单的。  

&emsp;&emsp;我们已经从并发的角度分析过，所有数据都被互斥锁适当的保护了，那么异常是否安全呢？`value_for`函数没有修改任何数据，所以它是安全的，即使它抛出了异常也不会影响数据结构。`remove_mapping`函数通过标准库内置的`erase`修改队列，但是删除操作是保证不会抛出异常的，所以这也是安全的。还剩下`add_or_update_mapping`函数，它可能在if的两个分支中抛出异常。`push_back`是异常安全的，如果抛出，它将使列表保持原始状态，这样此分支是安全的。另一个分支的问题是在替换当前队列中key对应的value,如果赋值过程中抛出异常，我们就只能期望value并没有被修改。但这不会影响整个数据结构，而且值的类型完全由使用者提供，因此可以放心地让用户来处理异常。

&emsp;&emsp;在本节开始时，我提到过查找表有一个很好的功能**数据快照**，比如`std::map<>`。这要求锁住整个容器，即所有的哈希桶都被锁住，这是为了以确保获取的副本数据与查找表的状态保持一致。因为查找表上的**正常**操作一次只需要锁定一个哈希桶，所以这将是唯一需要锁定所有哈希桶的操作。因此，假如每次都以相同的顺序锁定它们(例如，每次都以增加哈希桶的索引来加锁)，就不会出现死锁。代码参见[6.12](./第六章代码例子/6-12code.cpp)。此代码的实现中，在访问哈希桶前对每个桶都进行了加锁，而且使用的是`std::shared_mutex`互斥量，这样就可以提高对同一个哈希桶并发访问的机会。然而，怎样才能运用更细粒度的锁操作，令桶的并发潜力进一步释放呢？在下一节将做到这点:实现支持迭代器的线程安全的链表。  


## 6.3.2 使用多种锁编写线程安全的链表  

&emsp;&emsp;链表是最基本的数据结构之一，所以实现一个线程安全的列表应该是很简单的吧，事实如此吗？其实这取决于你需要实现的功能，如果要实现的链表需要提供支持迭代器支持，那就太复杂了，所以在前面的哈希表中回避了这个功能的实现。如果容器提供了支持STL风格的迭代器，那么根本问题是容器必须按某种形式持有引用，只向内部数据结构。若允许别的线程同时改动容器，该引用就必须保持有效，进而要求迭代器在数据结构的部件上持锁。以上思路并不可取，因为我们必须考虑到，STL风格的迭代器的生命周期完全不受其容器控制。【TODO不太了解STL容器这部分】 

&emsp;&emsp;另一种方法是，让容器以成员函数形式提供迭代功能，例如`for_each`函数。**这使得容器直接负责迭代操作和锁操作，但却违反了第三章中避免死锁的准则。**为了让`for_each`的操作用意义，它就必须在持有内部锁的时候运行用户代码。不仅如此，为了让用户代码处理item数据，`for_each`还必须把每个item的引用传递给此用户代码。数据应该按值传递，而非按引用的方式，但若数据过于庞大，则会造成高昂的开销。  

&emsp;&emsp;所以，我们唯有寄希望于使用者，由他们自己保证，在`for_each`函数中不会通过用户代码访问锁造成死锁以及不会通过存储从访问锁保护区域外的引用造成数据竞争。此时查找表使用链表是安全的。  

&emsp;&emsp;剩下的问题就是，链表需要对外提供什么操作?参考[6.11](./第六章代码例子/6-11code.cpp)和[6.12](./第六章代码例子/6-12code.cpp)了解链表所需要的操作。

* 向链表中加入数据项 
* 从链表中移除指定项
* 从链表中查找指定项
* 修改链表中的指定项
* 将列表中的每个项目复制到另一个容器。

&emsp;&emsp;为了使它成为一个好的通用列表容器，添加进一步的操作(例如位置插入)将很有帮助，但这对于查找表来说是不必要的，所以我将把它留给读者作为练习。  

&emsp;&emsp;链表若要具备精细粒度的锁操作，则基本思想是让每个节点都具备一个互斥量。随着链表的增长，互斥的数量也会变多!这种做法的好处是，可以在链表的不同部分执行真正的并发操作:每个操作仅仅需要锁住目标节点，当操作转移到下一个目标节点时，原来的锁即可解开。参见代码[6.13](./第六章代码例子/6-13code.cpp)。  

&emsp;&emsp;代码[6.13](./第六章代码例子/6-13code.cpp)所示的` threadsafe_list<>`是单项链表，每一个节点都是`node`结构体类型。链表的头节点使用node的默认构造函数生成，头节点的next指针指向NULL。通过`push_front`函数将新的节点加入到链表中;首先新节点在堆上被创建，它的next指针变量指向NULL。然后我们锁住头节点的互斥，以便正确读取其next指针，最后将新节点插入到头节点后面，新节点的next指针再指向NULL。 到目前为止，一切安好: 为了向队列增加新的数据项你只需要使互斥量持锁即可，这里没有死锁的风险。另外，进行缓慢的内存分配过程是在锁保护区域之外进行的，保护区域内进行的只是几个指针的更新，它们肯定不会出问题。其他代码说明参见[6.13](./第六章代码例子/6-13code.cpp)。

&emsp;&emsp;那么[6.13](./第六章代码例子/6-13code.cpp)中是否存在死锁或竞争条件？如果**用户**提供的谓词和函数性能良好，那么这里的答案肯定是否定的。迭代操作总是单向的，总是从head节点开始，总是在释放当前互斥锁之前锁定下一个互斥锁，因此在不同的线程中不可能有不同的锁定顺序。`remove_if`函数中删除节点是有可能出现竞态条件的，因为删除节点(**if分支生命周期结束后，节点自动销毁**)是在互斥量解锁之后进行的(**为什么要在互斥量解锁后销毁，因为这个互斥量是属于被删除节点的，不能在节点互斥量持锁时删除，否则是未定义行为**)。但是深入思考就会发现，这其实是安全的操作，因为`current`指针所指向节点的互斥量依然是持锁的，所以在删除过程中，没有线程可以越过当前节点，从被删除节点上获取锁。    

![avatar][remove_if的潜在风险]   


&emsp;&emsp;并发操作的几率又怎么样？这个链表采用了精细粒度的锁操作，目的是摆脱单一的全局互斥，从而增加并发操作的几率。此代码做到了这点 : 不同的线程可以同时处理列表中的不同节点，无论它们是使用`for_each`处理每个项，还是使用`find_first_if`搜索，还是使用`remove_if`删除项。  

&emsp;&emsp;但由于**互斥的加锁顺序必须按照链表中节点的前后顺序进行**，因此多线程间不能出现跨过其他线程正在处理的节点。如果一个线程在处理某个特定节点花费很长时间，其他线程在达该特定节点时将阻塞。


# 6.4 总结 

&emsp;&emsp;分析了几种常见的数据结构的设计，包括栈、队列、链表和查找表，通过锁操作保护数据，预防数据竞争，并着眼与并发访问的设计方法，探讨了如何将本章前面部分的指引用于具体实现。  








[remove_if的潜在风险]:data:image/png;base64,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b9zyo767lJOu+X750GyeM7/1/fLtHwzeX7654a7ZebeVG35wT7nph4Nz2ffv/mk5/+oby533/qzctXGw3Rt//oty930/Lz+7/xfl57/45dx6f/nLUn7R99TcP+v5yU/vLz+6577y41n1ps2aL//Oj8th595Yvnb1bb1pwVuOvqL8j9n++bNPXFAun+3D4D+/4/jyrA+fVbaf1b0/u783jb/vuiu/V4687Du9dbQR+eajax/4+MioPnC+jOoD58uoPugy34q7PcysHhayH3R9UI0rWkIXbPWActqj/p9y1xVX9JdceuFy8StfOfR6sQULLGf/dekD58uoPpimfN+9857ybxd8uxw7O0i+7vuDQT+D6z/84Jnl0fudVC699Udz7f3BgaeX7T5wRnnJYeeX9bf/pO8u5cNnXF/2OPLycswVs4PtC67tTy29QuX22UKhiZqvax+spL8vhRLF1WnX3d6fUsql37qpvODj5/aKvV/7h3X9qZvY74Rryt/MFogX3viDXsE0H0vN14aP34zqA+fLqD5wvozqgy7zuWgxi2Yh+0HXB5VStKBLt5opxz34QeV7Rx3VX3rphUtoKQULLGf/dekD58uoPliL+T7/jVvLSz99YXnN5y4p1942KE62//CZ5fffd1p54ewg+fRvDQbOR176nfKxs24o184WMj+892ebPd8ouvbBas130w/vKW8/9qqy/YfO6l2puue+QTH4d1++orzy8IvLeTf+sD9lE5PMV9P1elUfOF9G9YHzZVQfTFM+Fy1m0SxkP+j6oFKLFnT1w2bKCf/blmX9AQf0W1h64bLUggWWs/+69IHzZVQfrOZ8h5377d5/6f/oX84qJ129oT+1lF3+9aLyl//29fLxs4dvXZp0vqDr9ao+WGv5vnX7T8oBJ1/Xu83s9w88rTcNLr7lzrL3cVeVv//y5eWGHwyuirWxOfONo2sfkK/5+de4TZvpMK49Po8ay7b5mrd88+VBz3ve8+Zec1t47eM1ivmoviW+7fO/4/JB87b5+tZ1aLZJW1ddddVQH9Tgi22O3+vP5dbrY34zX9xO38wx33YEqg/8/pJRfdBlvlS0MBisB1uLlVm7tP29l0Nq0YJu+PWZcvo2Dy+X7rprfysWX7h0UbDAch30XfvA+TKqD1ZDPm7N+syFN5fnHXxO2fXfv96fU8qrPvuNstcxV5Z1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


































[单链表数据结构]:data:image/png;base64,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



