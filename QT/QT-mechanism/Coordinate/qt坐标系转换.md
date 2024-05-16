# QT控件坐标转换、坐标移动

## 常见问题

* 制作界面时，弹出的右键菜单需要自定义弹出的位置而不是系统默认给出的位置
* 界面使用Qlabel制作界面的标题栏，标题栏要求和界面一同运动

&emsp;&emsp;类似上面的问题都需要开发者自己去设置控件的坐标位置

## Qt坐标转换函数接口

&emsp;&emsp;上述问题需要使用qt提供接口来修改位置,**首先了解qt的坐标向右向下坐标值逐渐增大，反之变小**    

&emsp;&emsp;Qt提供的接口有 **mapTo、mapFrom、mapToParent   mapToGlobal、mapFromParent   mapFromGlobal**，这些接口的作用都是将一个坐标系的位置转换到另一个坐标系中对应的位置。下面将介绍一下这些接口的区别。


* mapTo         将当前坐标系坐标转换到任意一级父界面的坐标系
  
* mapFrom       将任意一级父界面的坐标转换到当前坐标系
  
* mapToParent   将当前坐标系坐标转换到直接父级界面的坐标系
  
* mapFromParent 将直接父级界面的坐标转换到当前坐标系
  
* mapToGlobal   将当前坐标系坐标转换到全屏幕界面的坐标系
  
* mapFromGlobal 将全屏幕界面的坐标转换到当前坐标系


## 代码展示
&emsp;&emsp;这里介绍一下代码中的一些元素， widget界面是整个QMainWindow界面的子控件，pButton按钮是widget的子控件。一下代码使用上述介绍的函数进行坐标的转换，**详见Qt工程代码Coordinate**  

&emsp;&emsp;在使用的时候，**需要注意调用mapToxxx函数的都是子界面而不是父界面**。进行坐标的转换一般来说都是界面控件调用mapToxxx函数，界面此时可以理解为一个坐标系。


``` C++
            //使用mapTo mapFrom
            QPoint p = ui->widget->mapTo(this,pButton->pos());
            qDebug()<<"mapTo    pButton按钮坐标系转换  widget->this  Pos = "<<p;

            p = ui->widget->mapFrom(this,p);
            qDebug()<<"mapFrom  pButton按钮坐标系转换  this->widget  Pos = "<<p;

            //使用mapToParent mapFromParent
            p = ui->widget->mapToParent(pButton->pos());
            qDebug()<<"使用mapToParent    pButton按钮坐标系转换  widget->widget的直接父级窗口  Pos = "<<p;

            p = ui->widget->mapFromParent(p);
            qDebug()<<"mapFromParent     pButton按钮坐标系转换  widget的直接父级窗口->widget  Pos = "<<p;

            //使用mapToGlobal  mapFromGlobal
            p = ui->widget->mapToGlobal(pButton->pos());
            qDebug()<<"使用mapToGlobal    pButton按钮坐标系转换  widget->Global  Pos = "<<p;

            p = ui->widget->mapFromGlobal(p);
            qDebug()<<"mapFromGlobal     pButton按钮坐标系转换  Global->widget  Pos = "<<p;
```
