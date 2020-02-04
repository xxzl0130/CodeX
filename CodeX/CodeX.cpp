#include "stdafx.h"
#include "CodeX.h"
#include "ui_CodeX.h"

CodeX* CodeX::instance()
{
	if(!CodeX::singleton)
	{
		new CodeX();
	}
	return singleton;
}

CodeX::CodeX(QWidget *parent)
	: QMainWindow(parent),
	ui(new Ui::CodeX),
	chipDataWindow(new ChipDataWindow(this))
{
	CodeX::singleton = this;
	ui->setupUi(this);
	this->ui->chipView->setChipSize(QSize(8,8));
	connect();

	this->chipDataWindow->init();
}

void CodeX::connect()
{
	QObject::connect(
		this->ui->chipDataButton,
		&QPushButton::clicked,
		this->chipDataWindow,
		&ChipDataWindow::show
	);
}
