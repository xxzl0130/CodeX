#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(CHIPSOLVER_LIB)
#  define CHIPSOLVER_EXPORT Q_DECL_EXPORT
# else
#  define CHIPSOLVER_EXPORT Q_DECL_IMPORT
# endif
#else
# define CHIPSOLVER_EXPORT
#endif

#pragma comment(lib,"ChipSolver.lib")