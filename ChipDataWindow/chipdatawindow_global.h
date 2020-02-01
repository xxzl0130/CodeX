#pragma once
#pragma warning(disable:26812)
#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(CHIPDATAWINDOW_LIB)
#  define CHIPDATAWINDOW_EXPORT Q_DECL_EXPORT
# else
#  define CHIPDATAWINDOW_EXPORT Q_DECL_IMPORT
# endif
#else
# define CHIPDATAWINDOW_EXPORT
#endif

#pragma comment(lib,"ChipDataWindow.lib")
