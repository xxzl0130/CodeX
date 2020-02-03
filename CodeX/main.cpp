#include "stdafx.h"
#include "CodeX.h"
#include <QtWidgets/QApplication>

CodeX* CodeX::singleton = nullptr;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	CodeX::instance()->show();
	return a.exec();
}
