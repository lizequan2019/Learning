QTextTable 用于描述 QTextDocument 中的表格元素，表格是按行和列按序的一组单元格，
一个表格至少有一行和一列。每个单元格包含一个块，并被框架(frame)包围。


框架在外观上就是一个矩形边框。
一、文本框架(简称框架)基础
1、文本框架有一个边框，它把一个或多个文本块组合在一起，从而提供比段落更大的结构层，
因此框架被用作其他文档元素的通用容器。框架使用 QTextCursor:: insertFrame()创建。
2、每个框架至少包含一个文本块，以使文本光标可以在其中插入新的文档元素。
3、框架可用于在文档中创建分层结构，每个文档都有一个根框架(QTextDocument :: 
rootFrame())，且根框架下的每个框架都有一个父框架和一个(可能是空的)子框架列表。也
就是说文档是由一个或多个框架嵌套组成的。
4、父框架可以通过 QTextFrame::parentFrame()找到，QTextFrame::childFrames()函数提供了一
个子框架列表。
5、框架通常不直接创建，而是使用 QTextCursor:: insertFrame()创建，原型如下：
QTextFrame *insertFrame(const QTextFrameFormat &format)




表格创建的方法似乎有两种，而且表格上面还有空余出的一段，是不是标题