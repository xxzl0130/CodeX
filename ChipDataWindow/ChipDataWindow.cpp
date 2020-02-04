#include "stdafx.h"
#include "ChipDataWindow.h"
#include "ui_ChipDataWindow.h"
#include "GetChipWindow.h"

ChipDataWindow::ChipDataWindow(QWidget* parent, Qt::WindowFlags f):
	QDialog(parent,f),
	ui(new Ui::ChipDataWindow),
	getChipWindow(new GetChipWindow(this))
{
	ui->setupUi(this);
	connect();
	getChipWindow->init();
}

ChipDataWindow::~ChipDataWindow()
{
}

void ChipDataWindow::recvChipJsonObject(const QJsonObject& object)
{
	//TODO
}

void ChipDataWindow::connect()
{
	QObject::connect(
		this->ui->getDataPushButton,
		&QPushButton::clicked,
		this->getChipWindow,
		&GetChipWindow::show
	);
	QObject::connect(
		this->getChipWindow,
		&GetChipWindow::sendChipJsonObject,
		this,
		&ChipDataWindow::recvChipJsonObject
	);
}
