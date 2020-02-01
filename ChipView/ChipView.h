#pragma once

#include "chipview_global.h"
#include <QtWidgets/qwidget.h>
#include "ChipBlock.h"

namespace Ui 
{
	class ChipView;
}
class QPushButton;

class CHIPVIEW_EXPORT ChipView : public QWidget
{
	Q_OBJECT

	Q_PROPERTY(QSize chipSize READ chipSize WRITE setChipSize)
public:
	ChipView(QWidget* parent = Q_NULLPTR);

	QSize chipSize();
	void setChipSize(const QSize& s);

	enum ChipColor
	{
		Blue = 0x6699ff,
		Red = 0xff9900,
		Dodgerblue = 0x1e90ff,
		Deepskyblue = 0x00bfff,
		Greenyellow = 0xadff2f,
		Limegreen = 0x32cd32,
		Cornsilk = 0xfff8dc,
		Darkorchild = 0x9932cc,
		Lightpink = 0xffb6c1,
		Salmon = 0xfa8072,
	};

	void setChipColor(int i, int j, ChipColor color);

protected:
	void resizeEvent(QResizeEvent* event) override;
	ChipBlock* createChip(QWidget* parent = Q_NULLPTR);

	Ui::ChipView* ui;
	QSize size;
	int spacing, marging;
};
