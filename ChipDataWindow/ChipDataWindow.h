#pragma once

#include "chipdatawindow_global.h"
#include <QDialog>
#include <QJsonObject>
#include "ChipTableView/ChipTableView.h"

namespace Ui {	class ChipDataWindow; }
class GetChipWindow;

class CHIPDATAWINDOW_EXPORT ChipDataWindow : public QDialog
{
	Q_OBJECT

public:
	explicit ChipDataWindow(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	~ChipDataWindow();

public slots:
	void recvChipJsonObject(const QJsonObject& object);

private:
	void connect();

	Ui::ChipDataWindow* ui;
	GetChipWindow* getChipWindow;
	ChipTableModel* tableModel_;
	QList<GFChip> chips_;
};
