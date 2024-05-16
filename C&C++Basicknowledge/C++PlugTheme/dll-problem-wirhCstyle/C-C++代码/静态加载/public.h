#pragma once

#define DLL_PUBLIC __attribute__((visibility("default")))

// 定义表示    使用extern "C" ; 不定义表示  不使用extern "C"
#define myextern 1

//定义表示    使用命名空间 ; 不定义表示  不使用命名空间
#define mynamespace 1