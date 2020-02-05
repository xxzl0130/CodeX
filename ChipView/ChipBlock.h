#pragma once

#include "chipview_global.h"
#include <QtWidgets/QLabel>

class CHIPVIEW_EXPORT ChipBlock : public QLabel
{
	Q_OBJECT

	Q_PROPERTY(int color READ color WRITE setColor)
public:
	ChipBlock(QWidget *parent);
	~ChipBlock();

	int heightForWidth(int width) const override;
	bool hasHeightForWidth() const override;

	int color() { return colorHex; }
	void setColor(uint32_t c);

protected:
	void resizeEvent(QResizeEvent* event) override;

private:
	int colorHex;
};
