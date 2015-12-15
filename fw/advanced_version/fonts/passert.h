#ifndef PASSERT_H
#define PASSERT_H

#ifdef QT_CORE_LIB
#define passert(condition,message) Q_ASSERT(condition)
#else
#include "ch.h"
#include <string.h>
#define abs(v) v
#define passert(condition,message) chDbgAssert(condition,message)
#endif

#ifdef QT_CORE_LIB
#include "qdebugprint.h"
#define qlog(text) qDebugPrint::print(text)
#else
#define EMBEDDED_TARGET
#define qlog(text)
#endif

//one of this must be set as global project variable
//#define DRAWING_MODE_LOOPED
//#define DRAWING_MODE_CHANGESONLY


#define writeenableassert() passert(IsReadOnly() == 0,"is readonly")
#define wea() writeenableassert()

#endif // PASSERT_H
