#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include "cJSON.h"
#include <string>

//节点的类型是目录还是文件类型
enum NodeType{
   TYPE_UNKONW, //初始化时为未知类型
   TYPE_FILE,
   TYPE_DIR
};

//目录树节点数据结构
typedef struct node
{
	struct node * m_nextNode;   //指向下一级节点
	NodeType m_nodeType;        //当前节点的类型
	std::string m_parentPath;   //父级节点的路径
	std::string m_name;         //当前节点的名称

	node(){
		m_nextNode = NULL;
		m_nodeType = TYPE_UNKONW;
	}
}filenode;

/*
   des:遍历目录树并将遍历的内容存储filenode数据结构中
   par:_dirname 要遍历的目录，注意后面不用加/字符
       _root    是输出参数,数据结构指针
   ret:true执行成功 false执行失败
*/
bool TaverseDirectory(const std::string & _dirname, filenode * _root)
{
	if(_root == NULL){
		printf("filenode对象为NULL\n");
		return false;
	}

	DIR *dirp = NULL;
	if(NULL== (dirp=opendir(_dirname.data()))){
		printf("opendir %s 目录错误\n",_dirname.data());
		closedir(dirp);
		return false;
	}	
	
	filenode * temp = _root;
	struct dirent * entp = NULL;
	while ( NULL != (entp =readdir(dirp))){
		std::string name = entp->d_name;//文件和目录的名称

        char buf[PATH_MAX];
		filenode * pNode = new filenode();
		if(pNode == NULL){
			printf("创建filenoded对象失败\n");
			closedir(dirp);
			return false;
		}
		
		//节点的名称
		pNode->m_name = name;

        //节点的父级
		memset(buf,'\0',PATH_MAX);
		realpath(_dirname.c_str(),buf);
		pNode->m_parentPath = buf;
 
        //当前节点绝对路径 = 父级节点路径 + 当前节点名称
        std::string absolutePath = pNode->m_parentPath + "/" + pNode->m_name;

        if(entp->d_type == DT_REG){//普通文件
			pNode->m_nodeType = TYPE_FILE;
			temp->m_nextNode = pNode;
			temp = temp->m_nextNode;
		} else if(entp->d_type == DT_DIR){//目录
		    if(name == ".git"){//忽略.git目录
                continue;
			}else if(name == ".." || name == "."){//忽略目录 ./ 和 ../
			    continue;
			}else{
				pNode->m_nodeType = TYPE_DIR;
				temp->m_nextNode = pNode;
				temp = temp->m_nextNode;
				TaverseDirectory(absolutePath, temp);
			}
		}
	}

	closedir(dirp);
	return true;
}



bool DirTreeToJson(const std::string & _dirname, std::string & _json)
{
	struct stat stat_src;
	// 第一个参数是指定展示的目录  第二个参数是系统的一个结构体 
	// 执行lstat()可以填充结构体
	if (lstat(_dirname.data(), &stat_src) != 0) {
      printf("lstat执行错误 error(%s)\n",strerror(errno));
      return false;
    }

	filenode * pNode = new filenode();
	if(pNode == NULL){
		printf("创建filenoded对象失败\n");
		return false;
	}
	
    //判断是普通文件类型还是目录
	if (S_ISREG(stat_src.st_mode)){//regular(常规、普通文件) file
		 printf("%s 是普通文件不是目录\n",_dirname.data());
		 if(pNode == NULL){
		     delete pNode;
	     }	
		 return false;
	}else if(S_ISDIR(stat_src.st_mode)){//目录
         char buf[PATH_MAX];
		 //节点的名称
		 memset(buf,'\0',PATH_MAX);
		 realpath(_dirname.data(),buf);
		 pNode->m_name = buf;

		 //节点类型
		 pNode->m_nodeType = TYPE_DIR;
 
         //便利目录树
         TaverseDirectory(_dirname.data(),pNode);
	}

	cJSON * jsonRoot = cJSON_CreateArray();
	if(jsonRoot == NULL){
		printf("创建cJSON数组失败\n");
		if(pNode == NULL){
	         delete pNode;
	    }	
		return false;
	}

    filenode * temp = pNode;
	while(temp){
       cJSON * jsonTemp = cJSON_CreateObject();
	   if(jsonTemp == NULL){
		  cJSON_Delete(jsonRoot);
		  if(pNode == NULL){
		   	 delete pNode;
		  }	
		  return false;
	   }
       cJSON_AddStringToObject(jsonTemp,"curNodeName",temp->m_name.data());
	   cJSON_AddNumberToObject(jsonTemp,"curNodeType",temp->m_nodeType);
	   cJSON_AddStringToObject(jsonTemp,"curNodePar",temp->m_parentPath.data());
       cJSON_AddItemToArray(jsonRoot,jsonTemp);

	   temp = temp->m_nextNode;
	}

	_json = cJSON_PrintUnformatted(jsonRoot);

	cJSON_Delete(jsonRoot);
	if(pNode == NULL){
		delete pNode;
	}	
     
	return true;
}

int main(int argc,char *argv[])
{
	if (argc != 2 ){
		printf("输入参数应该是两个，是否输入遍历目录参数\n");
		return 0;
	}
	
    std::string jsonContent;
	DirTreeToJson(argv[1],jsonContent);

	printf("jsonContent = %s\n",jsonContent.data());
     
	return 0;
}
