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

	void init();

signals:
	// 芯片数据更新了
	void chipsChanged();

public slots:
	void recvChipJsonObject(const QJsonObject& object);

private slots:
	void squadChanged(int index);

private:
	void connect();

	Ui::ChipDataWindow* ui;
	GetChipWindow* getChipWindow;
	ChipTableModel* tableModel_;
	ChipTableModel* squadModel_;
	ChipTableDelegate* tableDelegate_;
	ChipTableDelegate* squadDelegate_;
};
