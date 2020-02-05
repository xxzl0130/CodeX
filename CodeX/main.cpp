#include "stdafx.h"
#include "CodeX.h"
#include <QtWidgets/QApplication>

CodeX* CodeX::singleton = nullptr;

int main(int argc, char *argv[])
{
	QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication a(argc, argv);
	QApplication::setOrganizationName("xuanxuan.tech");
	QApplication::setApplicationName("CodeX");
	QSettings::setDefaultFormat(QSettings::IniFormat);
	CodeX::instance()->show();
	return a.exec();
}
