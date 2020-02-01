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

void ChipView::setChipColor(int i, int j, ChipColor color)
{
	if (i >= size.height() || j >= size.width())
		return;
	dynamic_cast<ChipBlock*>(this->ui->gridLayout->itemAtPosition(i, j)->widget())
		->setColor(color);
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
	__super::resizeEvent(event);
}

ChipBlock* ChipView::createChip(QWidget* parent)
{
	return new ChipBlock(parent);
}
