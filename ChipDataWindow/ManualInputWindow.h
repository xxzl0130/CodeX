#pragma once

#include <QDialog>
#include "Chip/Chip.h"
namespace Ui { class ManualInputWindow; };

class ManualInputWindow : public QDialog
{
	Q_OBJECT

public:
	ManualInputWindow(QWidget *parent = Q_NULLPTR);
	~ManualInputWindow();

signals:
	void sendChipJsonObject(const QJsonObject& object);

protected slots:
	void onConfirm();
	void onCancel();
	bool code2Chips(const QString& inputCode, Chips& outChips);

private:
	Ui::ManualInputWindow *ui;
};
