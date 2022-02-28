#pragma once

#include <QDialog>
#include "Chip/Chip.h"
namespace Ui { class AddChipWindow; };

class AddChipWindow : public QDialog
{
	Q_OBJECT

public:
	AddChipWindow(QWidget *parent = Q_NULLPTR);
	~AddChipWindow();

	void init();

private:
	Ui::AddChipWindow *ui;
	GFChip::ChipColor color_ = GFChip::Orange;
};
