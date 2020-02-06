#pragma once

#include <QDialog>
#include <QVersionNumber>
namespace Ui { class AboutDialog; };

class AboutDialog : public QDialog
{
	Q_OBJECT

public:
	AboutDialog(QWidget *parent = Q_NULLPTR);
	~AboutDialog();

	void checkUpdate();

	QVersionNumber version;

private:
	Ui::AboutDialog *ui;
};
