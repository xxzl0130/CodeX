#include "ChipBlock.h"
#include "ChipView.h"

ChipBlock::ChipBlock(QWidget *parent)
	: QLabel(parent)
{
	auto sizePolicy = this->sizePolicy();
	sizePolicy.setHeightForWidth(true);
	sizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
	sizePolicy.setHorizontalPolicy(QSizePolicy::Preferred);
	this->setSizePolicy(sizePolicy);
	this->setMinimumSize(QSize(0, 0));
	this->setEnabled(false);
	this->setColor(ChipView::Red);
	this->setText("");
}

ChipBlock::~ChipBlock()
{
}

int ChipBlock::heightForWidth(int width) const
{
	return width;
}

bool ChipBlock::hasHeightForWidth() const
{
	return true;
}

void ChipBlock::setColor(int c)
{
	this->setStyleSheet(QString("background-color: #%1;\nborder:none;").arg(c, 6, 16));
}

void ChipBlock::resizeEvent(QResizeEvent* event)
{
	this->setFixedHeight(this->width());
	__super::resizeEvent(event);
}
