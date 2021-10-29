#pragma once

#include "chipdatawindow_global.h"
#include <QDialog>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QProcess>

namespace Ui { class GetChipWindow; };
class ChipDataWindow;

class CHIPDATAWINDOW_EXPORT GetChipWindow : public QDialog
{
	Q_OBJECT

public:
	GetChipWindow(QWidget *parent = Q_NULLPTR);
	~GetChipWindow();

	void init();

signals:
	void sendChipJsonObject(const QJsonObject& object);

protected slots:
	// 设置本地代理，显示提示，开启代理程序
	void setLocalProxy();
	// 设置网络代理，显示提示
	void setNetProxy();
	// 获取数据，异步
	void getData();

	// 从服务器接受到数据
	void recvData(QNetworkReply* reply);

	// 子线程错误
	void processError(QProcess::ProcessError error);
	// 子线程数据可读
	void processDataReady();
	// 启动本地代理
	void startLocalProxy();

	void closeEvent(QCloseEvent*) override;

private:
	void connect();
	// 解析从服务器或者保存的配置中读取的芯片信息
	bool parseChipData(const QByteArray& data);

	// 杀死已经存在的进程
	void killProcess();

	Ui::GetChipWindow *ui;
	QNetworkRequest* request_;
	QNetworkAccessManager* accessManager_;
	QProcess* process_;
	QString localProxyAddr_, localProxyPort_;
};
