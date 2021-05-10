#include "stdafx.h"
#include "AboutDialog.h"
#include "ui_AboutDialog.h"

AboutDialog::AboutDialog(QWidget *parent)
	: QDialog(parent),
	version(2,2,2),
	ui(new Ui::AboutDialog()),
	accessManager_(new QNetworkAccessManager(this))
{
	ui->setupUi(this);
	ui->nameLabel->setText(trUtf8(u8"CodeX重装芯片计算器 v") + version.toString());
	QSettings settings;
	auto s = settings.value("/Sys/AboutShowOnStart", true).toBool();
	this->ui->checkBox->setChecked(s);
	if (s)
		this->show();
	QObject::connect(this->ui->checkBox, &QCheckBox::stateChanged, this, &AboutDialog::showOnStart);
}

AboutDialog::~AboutDialog()
{
	delete ui;
}

void AboutDialog::checkUpdate()
{
	request_.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
	QSslConfiguration config = request_.sslConfiguration();
	config.setPeerVerifyMode(QSslSocket::VerifyNone);
	config.setProtocol(QSsl::SecureProtocols);
	request_.setSslConfiguration(config);
	request_.setUrl(QUrl("https://gitee.com/api/v5/repos/zaxs0130/CodeX/releases"));
	auto onReply =
		QObject::connect(
			accessManager_,
			&QNetworkAccessManager::finished,
			[&](QNetworkReply* reply)
			{
				try {
					if (reply->error() != QNetworkReply::NoError)
					{
						reply->deleteLater();
						return;
					}
					auto json = QJsonDocument::fromJson(reply->readAll()).array()[0].toObject();
					auto ver = json["tag_name"].toString("v1.0").toLower().split("v", QString::SkipEmptyParts);
					if (ver.empty())
					{
						reply->deleteLater();
						return;
					}
					auto newVersion = QVersionNumber::fromString(ver[0]);
					if (newVersion > version)
					{
						auto ret = QMessageBox::information(nullptr, u8"提示", u8"检测到新版本，是否下载？",
							QMessageBox::Yes | QMessageBox::No);
						if (ret == QMessageBox::Yes)
						{
							QDesktopServices::openUrl(
								QUrl(json["assets"].toArray()[0].toObject()["browser_download_url"].toString()));
						}
					}
					reply->deleteLater();
				}
				catch (std::exception e)
				{
					reply->deleteLater();
				}
			});
	accessManager_->get(request_);
}

void AboutDialog::showOnStart(bool b)
{
	QSettings settings;
	settings.setValue("/Sys/AboutShowOnStart", b);
}
