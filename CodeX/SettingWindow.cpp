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
	this->ui->threadLabel->setText(u8"推荐值：" + QString::number(QThread::idealThreadCount()));
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
	this->show();
	target.showNumber = this->ui->numbersSpinBox->value();
	target.maxNumber = this->ui->unlimitCheckBox->isChecked() ? -1 : this->ui->maxSpinBox->value();
	this->hide();
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
	this->ui->maxSpinBox->setValue(10000);
}

void SettingWindow::show()
{
	QSettings settings;
	this->ui->numbersSpinBox->setValue(settings.value(IniShowLimit, 1000).toInt());
	this->ui->maxSpinBox->setValue(settings.value(IniCalcLimit, 10000).toInt());
	this->ui->unlimitCheckBox->setChecked(settings.value(IniNoLimit, false).toBool());
	this->ui->threadSpinBox->setValue(settings.value(IniThreads, 1).toInt());
	this->ui->threadSpinBox->setMaximum(QThread::idealThreadCount() * 5);
	QDialog::show();
}

void SettingWindow::accept()
{
	QSettings settings;
	settings.setValue(IniShowLimit, this->ui->numbersSpinBox->value());
	settings.setValue(IniCalcLimit, this->ui->maxSpinBox->value());
	settings.setValue(IniNoLimit, this->ui->unlimitCheckBox->isChecked());
	settings.setValue(IniThreads, this->ui->threadSpinBox->value());
	QDialog::accept();
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
	QObject::connect(
		this->ui->unlimitCheckBox,
		&QCheckBox::stateChanged,
		[&](int state)
		{
			this->ui->maxSpinBox->setEnabled(state != Qt::CheckState::Checked);
		}
	);
}
