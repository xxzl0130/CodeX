#include "ChipView.h"
#include "ui_ChipView.h"
#include <QDebug>

ChipView::ChipView(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::ChipView),
	size(0, 0),
	spacing(2),
	marging(2)
{
	ui->setupUi(this);

	this->ui->gridLayout->setSpacing(spacing);
	this->ui->gridLayout->setMargin(marging);
}

QSize ChipView::chipSize()
{
	return size;
}

void ChipView::setChipSize(const QSize& s)
{
	// clear items
	for (auto i = 0; i < size.height(); ++i)
	{
		for (auto j = 0; j < size.width(); ++j)
		{
			auto item = this->ui->gridLayout->itemAtPosition(i,j);
			if (!item)
				continue;
			this->ui->gridLayout->removeItem(item);
			auto widget = item->widget();
			if (!widget)
				continue;
			this->ui->gridLayout->removeWidget(widget);
			delete item;
			delete widget;
		}
	}

	// add items
	size = s;
	for (auto i = 0; i < size.height(); ++i)
	{
		for (auto j = 0; j < size.width(); ++j)
		{
			this->ui->gridLayout->addWidget(createChip(this), i, j);
		}
	}
	resizeEvent(nullptr);
}

void ChipView::setChipColor(int i, int j, uint32_t color)
{
	if (i >= size.height() || j >= size.width())
		return;
	dynamic_cast<ChipBlock*>(this->ui->gridLayout->itemAtPosition(i, j)->widget())
		->setColor(color);
}

void ChipView::setView(const ChipViewInfo& view)
{
	auto x0 = (this->size.width() - view.width) / 2;
	auto y0 = (this->size.height() - view.height) / 2;
	for(auto i = 0;i < this->size.height();++i)
	{
		for(auto j = 0;j < this->size.width();++j)
		{
			this->setChipColor(i, j, Gray);
		}
	}
	for(auto y = 0;y < view.height;++y)
	{
		for(auto x = 0;x < view.width;++x)
		{
			if(view.map[y][x] >= 0)
				this->setChipColor(y + y0, x + x0, index2color(view.map[y][x]));
		}
	}
}

uint32_t ChipView::index2color(int i)
{
	switch (i)
	{
	case 0:
		return 0xdfe4ea;
	case 1:
		return 0xeccc68;
	case 2:
		return 0xff7f50;
	case 3:
		return 0xff6b81;
	case 4:
		return 0xffa502;
	case 5:
		return 0x7bed9f;
	case 6:
		return 0x70a1ff;
	case 7:
		return 0x5352ed;
	case 8:
		return 0x3742fa;
	default:
		return 0xdfe4ea;
	}
}

void ChipView::resizeEvent(QResizeEvent* event)
{
	this->setFixedHeight(this->width());
	int bHeight = (this->height() - marging * 2 - spacing * (this->size.height() - 1)) / this->size.height();
	int bWidth = (this->width() - marging * 2 - spacing * (this->size.width() - 1)) / this->size.width();
	int bSize = std::min(bHeight, bWidth);
	auto sHeight = bSize * size.height() + spacing * (size.height() - 1);
	auto sWidth = bSize * size.width() + spacing * (size.width() - 1);
	auto vLeft = this->height() - sHeight; // ´¹Ö±Ê£Óà
	auto hLeft = this->width() - sWidth; // Ë®Æ½Ê£Óà
	this->ui->gridLayout->setContentsMargins(hLeft / 2, vLeft / 2, hLeft - hLeft / 2, vLeft - vLeft / 2);
	QWidget::resizeEvent(event);
}

ChipBlock* ChipView::createChip(QWidget* parent)
{
	return new ChipBlock(parent);
}
