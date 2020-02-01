#pragma once
#pragma warning(disable:26812)
#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(CHIPVIEW_LIB)
#  define CHIPVIEW_EXPORT Q_DECL_EXPORT
# else
#  define CHIPVIEW_EXPORT Q_DECL_IMPORT
# endif
#else
# define CHIPVIEW_EXPORT
#endif

#pragma comment(lib,"ChipView.lib")