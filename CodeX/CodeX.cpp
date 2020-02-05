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
	this->solutionTableModel_->setSolution(&this->solver->solutions);
	this->solutionTableModel_->setMaxValue(this->solver->squadMaxValue(this->ui->squadsComboBox->currentText()));
}

CodeX::CodeX(QWidget *parent)
	: QMainWindow(parent),
	ui(new Ui::CodeX),
	chipDataWindow(new ChipDataWindow(this)),
	solver(new ChipSolver(this)),
	progressBar_(new QProgressBar(this)),
	solveNumberLabel_(new QLabel(u8"方案数：0",this)),
	timeLabel_(new QLabel(u8"耗时：0s", this)),
	chipTableModel_(new ChipTableModel(this)),
	chipTableDelegate_(new ChipTableDelegate(this)),
	solutionTableModel_(new SolutionTableModel(this))
{
	CodeX::singleton = this;
	ui->setupUi(this);
	this->ui->statusBar->addWidget(progressBar_);
	this->ui->statusBar->addWidget(solveNumberLabel_);
	this->ui->statusBar->addWidget(timeLabel_);
	
	this->ui->chipView->setChipSize(QSize(8,8));

	this->ui->chipsTable->setModel(chipTableModel_);
	this->ui->chipsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	this->ui->chipsTable->setItemDelegate(chipTableDelegate_);
	this->ui->chipsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	this->ui->chipsTable->verticalHeader()->hide();
	this->ui->chipsTable->setColumnHidden(0,true);
	this->ui->chipsTable->setColumnHidden(2,true);
	this->chipTableModel_->setShowBlocks(false);
	this->chipTableModel_->setShowStatus(false);
	this->ui->solutionTable->setModel(solutionTableModel_);
	this->ui->solutionTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	this->ui->solutionTable->verticalHeader()->hide();
	this->ui->solutionTable->setSelectionBehavior(QAbstractItemView::SelectRows);

	connect();

	this->chipDataWindow->init();
	this->ui->squadsComboBox->addItems(this->solver->squadList());
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
		this->progressBar_,
		&QProgressBar::setValue
	);
	QObject::connect(
		this->solver,
		&ChipSolver::solveNumberChanged,
		[&](int n)
	{
			this->solveNumberLabel_->setText(trUtf8(u8"方案数：") + QString::number(n));
	});
	QObject::connect(
		this->solver,
		&ChipSolver::solveTimeChanged,
		[&](double t)
	{
			this->timeLabel_->setText(trUtf8(u8"耗时：") + QString::number(t,'f',2) + "s");
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
			this->ui->configsComboBox->addItems(this->solver->configList(squad));
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
