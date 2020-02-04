#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(CHIP_LIB)
#  define CHIP_EXPORT Q_DECL_EXPORT
# else
#  define CHIP_EXPORT Q_DECL_IMPORT
# endif
#else
# define CHIP_EXPORT
#endif

#pragma comment(lib,"Chip.lib")
