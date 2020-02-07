#include "stdafx.h"
#include "AboutDialog.h"
#include "ui_AboutDialog.h"

AboutDialog::AboutDialog(QWidget *parent)
	: QDialog(parent),
	ui(new Ui::AboutDialog()),
	version(1,5)
{
	ui->setupUi(this);
	ui->nameLabel->setText(trUtf8(u8"CodeX重装芯片计算器 v") + version.toString());
}

AboutDialog::~AboutDialog()
{
	delete ui;
}

void AboutDialog::checkUpdate()
{
	QNetworkAccessManager accessManager;
	QNetworkRequest request;
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
	QSslConfiguration config = request.sslConfiguration();
	config.setPeerVerifyMode(QSslSocket::VerifyNone);
	config.setProtocol(QSsl::SecureProtocols);
	request.setSslConfiguration(config);
	request.setUrl(QUrl("https://api.github.com/repos/xxzl0130/CodeX/releases/latest"));
	auto onReply =
		QObject::connect(
			&accessManager,
			&QNetworkAccessManager::finished,
			[&](QNetworkReply* reply)
			{
				if (reply->error() != QNetworkReply::NoError)
				{
					qDebug() << reply->errorString();
					reply->deleteLater();
					return;
				}
				auto json = QJsonDocument::fromJson(reply->readAll()).object();
				auto ver = json["tag_name"].toString("v1.0").toLower().split("v");
				if (!ver.isEmpty())
				{
					auto newVersion = QVersionNumber::fromString(ver[0]);
					if (newVersion > version)
					{
						auto ret = QMessageBox::information(nullptr, u8"提示", u8"检测到新版本，是否下载？", QMessageBox::Yes | QMessageBox::No);
						if (ret == QMessageBox::Yes)
						{
							QDesktopServices::openUrl(QUrl("https://github.com/xxzl0130/CodeX/releases"));
						}
					}
				}
				reply->deleteLater();
			});
	accessManager.get(request);
}
