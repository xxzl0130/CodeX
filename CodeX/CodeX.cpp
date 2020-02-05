#include "stdafx.h"
#include "CodeX.h"
#include "ui_CodeX.h"
#include "ChipSolver/ChipSolver.h"
#include "ChipDataWindow/ChipDataWindow.h"

CodeX* CodeX::instance()
{
	if(!CodeX::singleton)
	{
		new CodeX();
	}
	return singleton;
}

void CodeX::solve()
{
	this->ui->solveButton->setEnabled(false);
	this->ui->useEquippedCheckBox->setEnabled(false);
	this->ui->useLockedCheckBox->setEnabled(false);
	this->solver->start();
}

void CodeX::solveFinished()
{
	this->ui->solveButton->setEnabled(true);
	this->ui->useEquippedCheckBox->setEnabled(true);
	this->ui->useLockedCheckBox->setEnabled(true);
}

CodeX::CodeX(QWidget *parent)
	: QMainWindow(parent),
	ui(new Ui::CodeX),
	chipDataWindow(new ChipDataWindow(this)),
	solver(new ChipSolver(this))
{
	CodeX::singleton = this;
	ui->setupUi(this);
	this->ui->chipView->setChipSize(QSize(8,8));
	connect();

	this->chipDataWindow->init();
	this->ui->squadsComboBox->addItems(this->solver->squads());
	this->solver->setTargetBlock(TargetBlock(20,2,4,6,0));
}

void CodeX::connect()
{
	QObject::connect(
		this->ui->chipDataButton,
		&QPushButton::clicked,
		this->chipDataWindow,
		&ChipDataWindow::show
	);
	QObject::connect(
		this->solver,
		&ChipSolver::solvePercentChanged,
		this->ui->progressBar,
		&QProgressBar::setValue
	);
	QObject::connect(
		this->solver,
		&ChipSolver::solveNumberChanged,
		[&](int n)
	{
			this->ui->solveNumberLabel->setText(trUtf8(u8"方案数：") + QString::number(n));
	});
	QObject::connect(
		this->solver,
		&ChipSolver::solveTimeChanged,
		[&](double t)
	{
			this->ui->timeLabel->setText(trUtf8(u8"耗时：") + QString::number(t,'f',2) + "s");
	});
	QObject::connect(
		this->solver,
		&ChipSolver::finished,
		this,
		&CodeX::solveFinished
	);
	QObject::connect(
		this->ui->solveButton,
		&QPushButton::clicked,
		this,
		&CodeX::solve
	);
	QObject::connect(
		this->ui->squadsComboBox,
		&QComboBox::currentTextChanged,
		[&](const QString& squad)
	{
			this->ui->configsComboBox->clear();
			this->ui->configsComboBox->addItems(this->solver->configs(squad));
	});
	QObject::connect(
		this->ui->configsComboBox,
		&QComboBox::currentTextChanged,
		this->solver,
		&ChipSolver::setTargetConfig
	);
	QObject::connect(
		this->ui->useLockedCheckBox,
		&QCheckBox::stateChanged,
		this->solver,
		&ChipSolver::setUseLocked
	);
	QObject::connect(
		this->ui->useEquippedCheckBox,
		&QCheckBox::stateChanged,
		this->solver,
		&ChipSolver::setUseEquipped
	);
}
