#include "stdafx.h"
#include "AboutDialog.h"
#include "ui_AboutDialog.h"

AboutDialog::AboutDialog(QWidget *parent)
	: QDialog(parent),
	ui(new Ui::AboutDialog()),
	version(2,0,2),
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
#ifdef _DEBUG
	QNetworkProxy proxy;
	proxy.setType(QNetworkProxy::Socks5Proxy);
	proxy.setHostName("127.0.0.1");
	proxy.setPort(10808);
	QNetworkProxy::setApplicationProxy(proxy);
#endif
	request_.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
	QSslConfiguration config = request_.sslConfiguration();
	config.setPeerVerifyMode(QSslSocket::VerifyNone);
	config.setProtocol(QSsl::SecureProtocols);
	request_.setSslConfiguration(config);
	request_.setUrl(QUrl("https://api.github.com/repos/xxzl0130/CodeX/releases/latest"));
	auto onReply =
		QObject::connect(
			accessManager_,
			&QNetworkAccessManager::finished,
			[&](QNetworkReply* reply)
			{
				if (reply->error() != QNetworkReply::NoError)
				{
					reply->deleteLater();
					return;
				}
				auto json = QJsonDocument::fromJson(reply->readAll()).object();
				auto ver = json["tag_name"].toString("v1.0").toLower().split("v");
				if (ver.size() > 1)
				{
					auto newVersion = QVersionNumber::fromString(ver[1]);
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
	accessManager_->get(request_);
}

void AboutDialog::showOnStart(bool b)
{
	QSettings settings;
	settings.setValue("/Sys/AboutShowOnStart", b);
}
