#pragma once

#include "chipdatawindow_global.h"
#include <qdialog.h>

namespace Ui {	class ChipDataWindow; }
class GetChipWindow;

class CHIPDATAWINDOW_EXPORT ChipDataWindow : public QDialog
{
	Q_OBJECT

public:
	explicit ChipDataWindow(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	~ChipDataWindow();

private:
	void connect();

	Ui::ChipDataWindow* ui;
	GetChipWindow* getChipWindow;
};
