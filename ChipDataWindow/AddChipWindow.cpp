#include "stdafx.h"
#include "AddChipWindow.h"
#include "ui_AddChipWindow.h"

AddChipWindow::AddChipWindow(QWidget *parent)
	: QDialog(parent), ui(new Ui::AddChipWindow())
{
	ui->setupUi(this);
	init();
	show();
}

AddChipWindow::~AddChipWindow()
{
	delete ui;
}

void AddChipWindow::init()
{
	auto layout5 = dynamic_cast<QGridLayout*>(this->ui->tab_5block->layout());
	auto layout6 = dynamic_cast<QGridLayout*>(this->ui->tab_6block->layout());
	std::pair<int, int> pos51(0, 0),pos52(2, 0), pos6(0, 0);
	QLayoutItem* item;
	while ((item = layout5->takeAt(0)) != nullptr)
	{
		delete item;
	}
	while ((item = layout6->takeAt(0)) != nullptr)
	{
		delete item;
	}
	ChipConfig::initConfig();
	static const QString url(":/Chip/Resources/img/%1%2.png");
	for(auto i = 0;i < 50;++i)
	{
		auto& config = ChipConfig::getConfig(i);

		if(config.chipClass == GFChip::Class56)
		{
			auto button = new QPushButton(this->ui->tab_6block);
			button->setIcon(QPixmap(url.arg(color_ == GFChip::Blue ? "b" : "o").arg(config.gridID)));
			button->setIconSize(QSize(32, 32));
			button->setFlat(true);
			layout6->addWidget(button, pos6.first, pos6.second);
			pos6.first += (++pos6.second) / 5;
			pos6.second %= 5;
		}
		else if(config.chipClass == GFChip::Class551)
		{
			auto button = new QPushButton(this->ui->tab_5block);
			button->setIcon(QPixmap(url.arg(color_ == GFChip::Blue ? "b" : "o").arg(config.gridID)));
			button->setIconSize(QSize(32, 32));
			button->setFlat(true);
			layout5->addWidget(button, pos51.first, pos51.second);
			pos51.first += (++pos51.second) / 5;
			pos51.second %= 5;
		}
		else if(config.chipClass == GFChip::Class552)
		{
			auto button = new QPushButton(this->ui->tab_5block);
			button->setIcon(QPixmap(url.arg(color_ == GFChip::Blue ? "b" : "o").arg(config.gridID)));
			button->setIconSize(QSize(32, 32));
			button->setFlat(true);
			layout5->addWidget(button, pos52.first, pos52.second);
			pos52.first += (++pos52.second) / 5;
			pos52.second %= 5;
		}
		else
		{
			continue;
		}
	}
}
