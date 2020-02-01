#pragma once

#include "chipdatawindow_global.h"
#include <QDialog>

namespace Ui { class GetChipWindow; };
class ChipDataWindow;

class CHIPDATAWINDOW_EXPORT GetChipWindow : public QDialog
{
	Q_OBJECT

public:
	GetChipWindow(QWidget *parent = Q_NULLPTR);
	~GetChipWindow();

protected slots:
	// 设置本地代理，显示提示，开启代理程序
	void setLocalProxy();
	// 设置网络代理，显示提示
	void setNetProxy();
	// 获取数据，异步
	void getData();

private:
	void connect();

	Ui::GetChipWindow *ui;
};
