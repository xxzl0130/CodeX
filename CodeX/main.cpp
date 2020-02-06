#include "stdafx.h"
#include "CodeX.h"
#include <QtWidgets/QApplication>
#include <QVersionNumber>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QObject>
#include <QNetworkProxy>

CodeX* CodeX::singleton = nullptr;
QVersionNumber version(1, 1);

void checkUpdate();

int main(int argc, char *argv[])
{
	QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication a(argc, argv);
	QApplication::setOrganizationName("xuanxuan.tech");
	QApplication::setApplicationName("CodeX");
	QSettings::setDefaultFormat(QSettings::IniFormat);
	CodeX::instance()->show();
	checkUpdate();
	return a.exec();
}

void checkUpdate()
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
			if(!ver.isEmpty())
			{
				auto newVersion = QVersionNumber::fromString(ver[0]);
				if(newVersion > version)
				{
					auto ret = QMessageBox::information(nullptr, u8"提示", u8"检测到新版本，是否下载？", QMessageBox::Yes | QMessageBox::No);
					if(ret == QMessageBox::Yes)
					{
						QDesktopServices::openUrl(QUrl("https://github.com/xxzl0130/CodeX/releases"));
					}
				}
			}
			reply->deleteLater();
		});
	accessManager.get(request);
}
