#include "stdafx.h"
#include "CodeX.h"

CodeX::CodeX(QWidget *parent)
	: QMainWindow(parent),
	ui(new Ui::CodeX)
{
	ui->setupUi(this);
	this->ui->chipView->setChipSize(QSize(10,10));
	this->ui->chipView->setChipSize(QSize(6,6));
}
