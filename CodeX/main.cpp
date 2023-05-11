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
	const QLocale locale = QLocale::system();
	// workaround QTBUG-85409 by overwriting default locale
	if (QStringLiteral(u"\u3008") == locale.toString(1)) {
		QLocale::setDefault(QLocale::system().name());
	}

	CodeX::instance()->show();
	return a.exec();
}
