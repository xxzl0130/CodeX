#pragma once
#pragma warning(disable:26812)
#include <QtWidgets/QMainWindow>
#include "ui_CodeX.h"

class CodeX : public QMainWindow
{
	Q_OBJECT

public:
	static CodeX* instance();

private:
	CodeX(QWidget* parent = Q_NULLPTR);
	static CodeX* singleton;
	
	void connect();
	
	Ui::CodeX *ui;
	ChipDataWindow* chipDataWindow;
};
