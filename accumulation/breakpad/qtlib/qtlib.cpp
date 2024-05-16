#include "qtlib.h"

#ifdef __cplusplus
extern "C" {
#endif

Q_DECL_EXPORT void testfun()
{
    int *i = nullptr;
    *i = 0;
}

#ifdef __cplusplus
}
#endif
