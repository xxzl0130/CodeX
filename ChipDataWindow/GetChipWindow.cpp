#include "stdafx.h"
#include "GetChipWindow.h"
#include "ui_GetChipWindow.h"

GetChipWindow::GetChipWindow(QWidget *parent)
	: QDialog(parent),
	ui(new Ui::GetChipWindow)
{
	ui->setupUi(this);
	connect();
	this->ui->netProxyRadioButton->setChecked(true);
}

GetChipWindow::~GetChipWindow()
{
	delete ui;
}

void GetChipWindow::setLocalProxy()
{
	this->ui->noteLabel->setText(trUtf8(
		u8"说明：将手机连接与电脑相同的WiFi，长按打开WiFi的高级设置，选择“手动代理”，地址如下："));
	this->ui->proxyAddressLineEdit->setText(trUtf8(u8"地址:") + "192.168.3.1" + trUtf8(u8" 端口:") + "8081");
}

void GetChipWindow::setNetProxy()
{
	this->ui->noteLabel->setText(trUtf8(
		u8"说明：将手机连接任意WiFi，长按打开WiFi的高级设置，代理选择“自动代理”，代理地址为："));
	this->ui->proxyAddressLineEdit->setText("http://static.xuanxuan.tech/GF/GF.pac");
}

void GetChipWindow::getData()
{
}

void GetChipWindow::connect()
{
	QObject::connect(
		this->ui->getDataPushButton,
		&QPushButton::clicked,
		this,
		&GetChipWindow::getData
	);
	QObject::connect(
		this->ui->localProxyRadioButton,
		&QRadioButton::toggled,
		this,
		&GetChipWindow::setLocalProxy
	);
	QObject::connect(
		this->ui->netProxyRadioButton,
		&QRadioButton::toggled,
		this,
		&GetChipWindow::setNetProxy
	);
}
