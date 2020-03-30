#pragma once

#include <QDialog>
#include <QVersionNumber>
namespace Ui { class AboutDialog; };

class QNetworkAccessManager;
class QNetworkRequest;

class AboutDialog : public QDialog
{
	Q_OBJECT

public:
	AboutDialog(QWidget *parent = Q_NULLPTR);
	~AboutDialog();

	void checkUpdate();

	QVersionNumber version;

private slots:
	void showOnStart(bool b);

private:
	Ui::AboutDialog *ui;
	QNetworkAccessManager* accessManager_;
	QNetworkRequest request_;
};
