#pragma once
#include <QtCore/QtGlobal>

#if defined(BACKEND_LIBRARY)
#  define BACKEND_EXPORT Q_DECL_EXPORT
#else
#  define BACKEND_EXPORT Q_DECL_IMPORT
#endif
