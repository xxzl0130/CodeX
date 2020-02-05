#include "stdafx.h"
#include "CodeX.h"
#include "ui_CodeX.h"
#include "SettingWindow.h"

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
	this->ui->solutionTable->setEnabled(false);
	this->ui->chipsTable->setEnabled(false);
	this->ui->squadsComboBox->setEnabled(false);
	this->ui->configsComboBox->setEnabled(false);
	this->ui->useLockedCheckBox->setEnabled(false);
	this->ui->useEquippedCheckBox->setEnabled(false);
	this->ui->solveButton->setEnabled(false);
	this->ui->useEquippedCheckBox->setEnabled(false);
	this->ui->useLockedCheckBox->setEnabled(false);
	
	this->solver_->setTargetBlock(this->settingWindow_->getTargetBlock(this->ui->squadsComboBox->currentText()));
	this->solver_->start();
}

void CodeX::solveFinished()
{
	this->ui->solutionTable->setEnabled(true);
	this->ui->chipsTable->setEnabled(true);
	this->ui->squadsComboBox->setEnabled(true);
	this->ui->configsComboBox->setEnabled(true);
	this->ui->useLockedCheckBox->setEnabled(true);
	this->ui->useEquippedCheckBox->setEnabled(true);
	this->ui->solveButton->setEnabled(true);
	this->ui->useEquippedCheckBox->setEnabled(true);
	this->ui->useLockedCheckBox->setEnabled(true);
	this->solutionTableModel_->setSolution(&this->solver_->solutions);
	this->solutionTableModel_->setMaxValue(this->solver_->squadMaxValue(this->ui->squadsComboBox->currentText()));
}

void CodeX::selectSolution(int index)
{
	Chips solutionChips;
	const auto& solution = this->solver_->solutions[index];
	for(const auto& it : solution.chips)
	{
		solutionChips.push_back(this->chips[it.id]);
	}
	this->chipTableModel_->setChips(solutionChips);
	this->ui->chipView->setView(this->solver_->solution2ChipView(solution));
}

CodeX::CodeX(QWidget *parent)
	: QMainWindow(parent),
	solver_(new ChipSolver(this)),
	ui(new Ui::CodeX),
	chipDataWindow_(new ChipDataWindow(this)),
	settingWindow_(new SettingWindow(this)),
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
	//this->ui->chipsTable->setColumnHidden(0,true);
	this->ui->chipsTable->setColumnHidden(2,true);
	this->chipTableModel_->setShowBlocks(false);
	this->chipTableModel_->setShowStatus(false);
	this->chipTableModel_->setShowColor(true);
	this->ui->solutionTable->setModel(solutionTableModel_);
	this->ui->solutionTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	this->ui->solutionTable->verticalHeader()->hide();
	this->ui->solutionTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	this->ui->solutionTable->setSelectionMode(QAbstractItemView::SingleSelection);
	this->ui->solutionTable->setSortingEnabled(true);

	connect();

	this->chipDataWindow_->init();
	this->ui->squadsComboBox->addItems(this->solver_->squadList());
	this->solver_->setTargetBlock(TargetBlock(20,2,4,6,0));
	this->settingWindow_->reset();
}

void CodeX::connect()
{
	QObject::connect(
		this->ui->chipDataButton,
		&QPushButton::clicked,
		this->chipDataWindow_,
		&ChipDataWindow::show
	);
	QObject::connect(
		this->solver_,
		&ChipSolver::solvePercentChanged,
		this->progressBar_,
		&QProgressBar::setValue
	);
	QObject::connect(
		this->solver_,
		&ChipSolver::solveNumberChanged,
		[&](int n)
	{
			this->solveNumberLabel_->setText(trUtf8(u8"方案数：") + QString::number(n));
	});
	QObject::connect(
		this->solver_,
		&ChipSolver::solveTimeChanged,
		[&](double t)
	{
			this->timeLabel_->setText(trUtf8(u8"耗时：") + QString::number(t,'f',2) + "s");
	});
	QObject::connect(
		this->solver_,
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
			this->ui->configsComboBox->addItems(this->solver_->configList(squad));
	});
	QObject::connect(
		this->ui->configsComboBox,
		&QComboBox::currentTextChanged,
		this->solver_,
		&ChipSolver::setTargetConfig
	);
	QObject::connect(
		this->ui->useLockedCheckBox,
		&QCheckBox::stateChanged,
		this->solver_,
		&ChipSolver::setUseLocked
	);
	QObject::connect(
		this->ui->useEquippedCheckBox,
		&QCheckBox::stateChanged,
		this->solver_,
		&ChipSolver::setUseEquipped
	);
	QObject::connect(
		this->ui->solutionTable,
		&QTableView::clicked,
		[&](QModelIndex index)
	{
			this->selectSolution(index.row());
	});
	QObject::connect(
		this->ui->settingPushButton,
		&QPushButton::clicked,
		this->settingWindow_,
		&SettingWindow::show
	);
}
