&emsp;&emsp;尽管牛客上面的题是很奇怪，但是还是自己了解的不足，就比如说switch case的语法。目前先总结两点 

1. default 是在所有case都不匹配的时候才会匹配的
   
2. 每一个分支都要加上break，否则会一直向下执行直至执行完或者遇到break，此时就和是否匹配case无关了 

&emsp;&emsp;实际代码测试
```c
    #include <iostream>
    using namespace std;

    int main()
    {
        int i = 1;
        switch (i) 
        {
                case 1:
                {
                    cout <<" this is 1"<<endl;
                }
                // break;
            
                case 2:
                {
                    cout <<" this is 2"<<endl;
                }
                // break;

                case 3:
                {
                    cout <<" this is 3"<<endl;
                }
                // break;

                case 4:
                {
                    cout <<" this is 4"<<endl;
                }
                // break;

                default:
                {
                    cout <<"this is default"<<endl;
                }
                break;
        }
        return 0;
    }

/*  执行结果
    this is 1
    this is 2
    this is 3
    this is 4
    this is default
*/
```