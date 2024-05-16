#ifndef QTLIB_H
#define QTLIB_H

#include "qtlib_global.h"

// 定义了插件DLL导出函数
#ifdef __cplusplus
extern "C" {
#endif

Q_DECL_EXPORT void testfun();

#ifdef __cplusplus
}
#endif

#endif // QTLIB_H
