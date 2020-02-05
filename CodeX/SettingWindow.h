#pragma once

#include <QDialog>
namespace Ui { class SettingWindow; };
struct TargetBlock;

class SettingWindow : public QDialog
{
	Q_OBJECT

public:
	SettingWindow(QWidget *parent = Q_NULLPTR);
	~SettingWindow();

	TargetBlock getTargetBlock(const QString& squad);

public slots:
	void reset();

private:
	void connect();
	
	Ui::SettingWindow *ui;
};
