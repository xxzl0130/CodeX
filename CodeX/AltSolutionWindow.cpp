#include "stdafx.h"
#include "AltSolutionWindow.h"
#include "ui_AltSolutionWindow.h"
#include "CodeX.h"

AltSolutionWindow::AltSolutionWindow(QWidget *parent)
	: QDialog(parent),
	ui(new Ui::AltSolutionWindow()),
	chipTableModel_(new ChipTableModel(this)),
	chipTableDelegate_(new ChipTableDelegate(this)),
	solutionTableModel_(new SolutionTableModel(this))
{
	ui->setupUi(this);
}

AltSolutionWindow::~AltSolutionWindow()
{
	delete ui;
}

void AltSolutionWindow::addSolution(const Solution& s)
{
	for(const auto& it : s.chips)
	{
		chipCount_[it.no]++;
	}
	this->solutions_.push_back(s);
	this->solutionTableModel_->refresh();
	saveSolutions();
}

void AltSolutionWindow::clearSolution()
{
	this->solutions_.clear();
	this->chipCount_.clear();
	this->solutionTableModel_->refresh();
	saveSolutions();
}

void AltSolutionWindow::init()
{
	this->ui->chipView->setChipSize(QSize(8, 8));
	this->ui->chipsTable->setModel(chipTableModel_);
	this->ui->chipsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	this->ui->chipsTable->setItemDelegate(chipTableDelegate_);
	this->ui->chipsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	this->ui->chipsTable->verticalHeader()->hide();
	this->ui->chipsTable->setColumnHidden(2, true);
	this->chipTableModel_->setShowBlocks(false);
	this->chipTableModel_->setShowStatus(false);
	this->chipTableModel_->setShowColor(true);
	this->ui->solutionTable->setModel(solutionTableModel_);
	this->ui->solutionTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	this->ui->solutionTable->verticalHeader()->hide();
	this->ui->solutionTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	this->ui->solutionTable->setSelectionMode(QAbstractItemView::SingleSelection);
	this->ui->solutionTable->setSortingEnabled(true);
	this->solutionTableModel_->setSolution(&this->solutions_);
	this->solutionTableModel_->setShowError(false);
	connect();
	QSettings settings;
	auto data = settings.value("/User/Solutions", "[]").toByteArray();
	auto doc = QJsonDocument::fromJson(data);
	this->solutions_.clear();
	for(const auto& it : doc.array())
	{
		this->addSolution(Solution::fromJsonObject(it.toObject()));
	}
	this->solutionTableModel_->refresh();
}

bool AltSolutionWindow::chipUsed(int no)
{
	return chipCount_[no] > 0;
}

void AltSolutionWindow::closeEvent(QCloseEvent* e)
{
	saveSolutions();
	QDialog::closeEvent(e);
}

void AltSolutionWindow::saveSolutions()
{
	QJsonArray arr;
	for(const auto& it : solutions_)
	{
		arr.append(it.toObject());
	}
	QSettings settings;
	QJsonDocument doc(arr);
	settings.setValue("/User/Solutions", doc.toJson(QJsonDocument::Compact));
}

void AltSolutionWindow::connect()
{
	QObject::connect(
		this->ui->solutionTable,
		&QTableView::clicked,
		[&](QModelIndex index)
		{
			this->selectSolution(index.row());
		});
	QObject::connect(
		this->ui->delButton,
		&QPushButton::clicked,
		this,
		&AltSolutionWindow::delSolution
	);
}

void AltSolutionWindow::selectSolution(int index)
{
	Chips solutionChips;
	const auto& solution = solutions_[index];
	for (const auto& it : solution.chips)
	{
		solutionChips.push_back(CodeX::instance()->chips[it.no]);
	}
	this->chipTableModel_->setChips(solutionChips);
	this->ui->chipView->setView(CodeX::instance()->solver_->solution2ChipView(solution, solution.squad));
}

void AltSolutionWindow::delSolution()
{
	auto index = this->ui->solutionTable->currentIndex().row();
	if (index < 0 || index >= solutions_.size())
		return;
	for(const auto& it : solutions_[index].chips)
	{
		chipCount_[it.no]--;
	}
	solutions_.erase(solutions_.begin() + index);
	solutionTableModel_->refresh();
}
