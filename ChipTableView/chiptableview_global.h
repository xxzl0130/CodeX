#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(CHIPTABLEVIEW_LIB)
#  define CHIPTABLEVIEW_EXPORT Q_DECL_EXPORT
# else
#  define CHIPTABLEVIEW_EXPORT Q_DECL_IMPORT
# endif
#else
# define CHIPTABLEVIEW_EXPORT
#endif

#pragma comment(lib,"ChipTableView.lib")
