#include "stdafx.h"
#include "ChipDataWindow.h"
#include "ui_ChipDataWindow.h"
#include "GetChipWindow.h"
#include "CodeX/CodeX.h"

ChipDataWindow::ChipDataWindow(QWidget* parent, Qt::WindowFlags f):
	QDialog(parent,f),
	ui(new Ui::ChipDataWindow),
	getChipWindow(new GetChipWindow(this)),
	tableModel_(new ChipTableModel(this)),
	tableDelegate_(new ChipTableDelegate(this))
{
	ui->setupUi(this);
	connect();
}

ChipDataWindow::~ChipDataWindow()
{
}

void ChipDataWindow::init()
{
	getChipWindow->init();
	this->ui->tableView->setModel(this->tableModel_);
	this->ui->tableView->setItemDelegate(this->tableDelegate_);
	this->ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	this->ui->tableView->verticalHeader()->hide();
	this->ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void ChipDataWindow::recvChipJsonObject(const QJsonObject& object)
{
	auto squads = object["squad_with_user_info"].toObject();
	std::map<int, int> squadID;
	for(const auto& it : squads)
	{
		auto obj = it.toObject();
		squadID[obj["id"].toString().toInt(0)] = obj["squad_id"].toString().toInt(0);
	}

	auto& chips = CodeX::instance()->chips;
	auto& gridChips = CodeX::instance()->gridChips;
	gridChips.clear();
	chips = getChips(object["chip_with_user_info"].toObject());
	for(auto i = 0;i < chips.size();++i)
	{
		auto& chip = chips[i];
		chip.no = i + 1;
		if(chip.squad > 0)
		{
			chip.squad = squadID[chip.squad];
		}
		// ¿½±´Ò»·Ý+20
		auto t = chip;
		t.no = i;
		t.level = 20;
		t.calcValue();
		gridChips[t.gridID].push_back(t);
	}
	this->tableModel_->setChips(chips);
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
