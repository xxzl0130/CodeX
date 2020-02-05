#include "stdafx.h"
#include "SettingWindow.h"
#include "ui_SettingWindow.h"
#include "CodeX.h"

SettingWindow::SettingWindow(QWidget *parent)
	: QDialog(parent),
	ui(new Ui::SettingWindow())
{
	ui->setupUi(this);
	connect();
}

SettingWindow::~SettingWindow()
{
	delete ui;
}

TargetBlock SettingWindow::getTargetBlock(const QString& squad)
{
	TargetBlock target;
	for(auto i = 0;i < this->ui->gridLayout->rowCount();++i)
	{
		if(dynamic_cast<QLabel*>(this->ui->gridLayout->itemAtPosition(i,0)->widget())->text() == squad)
		{
			target.damageBlock = dynamic_cast<QSpinBox*>(this->ui->gridLayout->itemAtPosition(i, 1)->widget())->value();
			target.defbreakBlock = dynamic_cast<QSpinBox*>(this->ui->gridLayout->itemAtPosition(i, 2)->widget())->value();
			target.hitBlock = dynamic_cast<QSpinBox*>(this->ui->gridLayout->itemAtPosition(i, 3)->widget())->value();
			target.reloadBlock = dynamic_cast<QSpinBox*>(this->ui->gridLayout->itemAtPosition(i, 4)->widget())->value();
			target.error = dynamic_cast<QSpinBox*>(this->ui->gridLayout->itemAtPosition(i, 5)->widget())->value();
			break;
		}
	}
	target.upper = this->ui->numbersSpinBox->value();
	return target;
}

void SettingWindow::reset()
{
	auto squads = CodeX::instance()->solver_->squadList();
	for(auto i = 0;i < squads.size();++i)
	{
		dynamic_cast<QLabel*>(this->ui->gridLayout->itemAtPosition(i + 1, 0)->widget())->setText(squads[i]);
		auto value = CodeX::instance()->solver_->squadMaxValue(squads[i]);
		dynamic_cast<QSpinBox*>(this->ui->gridLayout->itemAtPosition(i + 1, 1)->widget())->setValue(value.damageBlock);
		dynamic_cast<QSpinBox*>(this->ui->gridLayout->itemAtPosition(i + 1, 2)->widget())->setValue(value.defbreakBlock);
		dynamic_cast<QSpinBox*>(this->ui->gridLayout->itemAtPosition(i + 1, 3)->widget())->setValue(value.hitBlock);
		dynamic_cast<QSpinBox*>(this->ui->gridLayout->itemAtPosition(i + 1, 4)->widget())->setValue(value.reloadBlock);
		dynamic_cast<QSpinBox*>(this->ui->gridLayout->itemAtPosition(i + 1, 5)->widget())->setValue(0);
	}
	this->ui->numbersSpinBox->setValue(1000);
}

void SettingWindow::connect()
{
	QObject::connect(
		this->ui->resetButton,
		&QPushButton::clicked,
		this,
		&SettingWindow::reset
	);
	QObject::connect(
		this->ui->okButton,
		&QPushButton::clicked,
		this,
		&SettingWindow::accept
	);
}
