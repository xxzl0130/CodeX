#include "stdafx.h"
#include "CodeX.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	CodeX w;
	w.show();
	return a.exec();
}
