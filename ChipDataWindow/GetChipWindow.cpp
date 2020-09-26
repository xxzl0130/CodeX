#include "stdafx.h"
#include "GetChipWindow.h"
#include "ui_GetChipWindow.h"
#include <QFile>

constexpr auto pacUrl = "http://static.xuanxuan.tech/GF/GF.pac";
constexpr auto serverHost = "https://codex.xuanxuan.tech:8080/";
constexpr auto localHost = "http://127.0.0.1:8080/";
constexpr auto jsonPath = "chipJson";
#ifdef Q_OS_WIN
constexpr auto exeName = "GF_Tool_Server.exe";
#else
constexpr auto exeName = "GF_Tool_Server";
#endif

GetChipWindow::GetChipWindow(QWidget *parent)
	: QDialog(parent),
	ui(new Ui::GetChipWindow),
	request_(new QNetworkRequest()),
	accessManager_(new QNetworkAccessManager(this)),
	process_(new QProcess(this)),
	localProxyAddr_(trUtf8(u8"程序待启动"))
{
	ui->setupUi(this);
	connect();
}

GetChipWindow::~GetChipWindow()
{
	delete ui;
	delete request_;
	killProcess();
}

void GetChipWindow::init()
{
	this->ui->localProxyRadioButton->setChecked(true);
	request_->setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");

	QSslConfiguration config = request_->sslConfiguration();
	config.setPeerVerifyMode(QSslSocket::VerifyNone);
	config.setProtocol(QSsl::SecureProtocols);
	request_->setSslConfiguration(config);

	process_->setProgram(QString("./") + exeName);

	killProcess();

	// 启动后尝试加载数据
	QSettings settings;
	parseChipData(settings.value("/User/Chip").toByteArray());
}

void GetChipWindow::startLocalProxy()
{
	killProcess();
	this->ui->startLocalPushButton->setEnabled(false);
	process_->start(QIODevice::ReadOnly);
}

void GetChipWindow::closeEvent(QCloseEvent* e)
{
	killProcess();
	QDialog::closeEvent(e);
}

bool GetChipWindow::parseChipData(const QByteArray& bytes)
{
	if (bytes.isEmpty() || bytes[0] != '#')
	{
		return false;
	}
	auto data = QByteArray::fromBase64(
		QByteArray(bytes.data() + 1, bytes.size() - 1), QByteArray::Base64Encoding);
	try {
		auto res = gzip::decompress(data.data(), data.size());
		/*QFile file("chip.json");
		file.open(QIODevice::WriteOnly);
		file.write(res.c_str());
		file.close();*/
		QJsonParseError jsonError;
		QJsonDocument doucment = QJsonDocument::fromJson(res.c_str(), &jsonError);  // 转化为 JSON 文档
		if (!doucment.isNull() && jsonError.error == QJsonParseError::NoError && doucment.isObject())
		{  // 解析未发生错误 JSON 文档为对象
			auto obj = doucment.object();
			emit sendChipJsonObject(obj);
			QSettings settings; // 解析成功则保存数据
			settings.setValue("/User/Chip", bytes);
			return true;
		}
		else
		{
			return false;
		}
	}
	catch (const std::runtime_error & e)
	{
		qDebug() << trUtf8(u8"数据解析失败！\n") + e.what();
		return false;
	}
}

void GetChipWindow::setLocalProxy()
{
	this->ui->noteLabel->setText(trUtf8(
		u8"说明：将手机连接与电脑相同的WiFi，长按打开WiFi的高级设置，选择“手动代理”，地址如下："));
	this->ui->proxyAddressLineEdit->setText(trUtf8(u8"地址:") + localProxyAddr_ + trUtf8(u8" 端口:") + localProxyPort_);
	this->ui->startLocalPushButton->setEnabled(this->process_->state() != QProcess::Running);
	request_->setUrl(QUrl(QString(localHost) + jsonPath));
}

void GetChipWindow::setNetProxy()
{
	this->ui->noteLabel->setText(trUtf8(
		u8"说明：将手机连接任意WiFi，长按打开WiFi的高级设置，代理选择“自动代理”，代理地址为："));
	this->ui->proxyAddressLineEdit->setText(pacUrl);
	request_->setUrl(QUrl(QString(serverHost) + jsonPath));
	process_->kill();
	this->ui->startLocalPushButton->setEnabled(false);
}

void GetChipWindow::getData()
{
	auto data = QString("uid=%1&name=%2&locked=0&equipped=0")
		.arg(this->ui->uidLineEdit->text())
		.arg(QString(QTextCodec::codecForName("UTF8")->fromUnicode(this->ui->nameLineEdit->text()).toPercentEncoding()));
	this->accessManager_->post(*request_, data.toLatin1());
}

void GetChipWindow::recvData(QNetworkReply* reply)
{
	if (reply->error() == QNetworkReply::NoError)
	{
		QByteArray bytes = reply->readAll();
		if (bytes[0] != '#')
		{
			QMessageBox::warning(this, trUtf8(u8"失败"), trUtf8(u8"获取数据失败！请检查代理操作和用户信息！"));
		}
		else if(parseChipData(bytes))
		{
			QMessageBox::information(this, trUtf8(u8"成功"), trUtf8(u8"获取芯片数据成功！"));
			this->close();
		}
		else
		{
			QMessageBox::warning(this, trUtf8(u8"失败"), trUtf8(u8"解析数据失败！"));
		}
	}
	else
	{
		QMessageBox::warning(this, trUtf8(u8"错误"),
			trUtf8(u8"网络连接错误！") + " code:" + QString::number(reply->error()) + "\n" + reply->errorString());
	}
	reply->deleteLater();
}

void GetChipWindow::processError(QProcess::ProcessError error)
{
	localProxyAddr_ = trUtf8(u8"程序待启动");
	localProxyPort_ = "";
	if (this->ui->netProxyRadioButton->isChecked())
		return;
	this->ui->startLocalPushButton->setEnabled(true);
	switch (error)
	{
	case QProcess::FailedToStart:
		QMessageBox::warning(this, trUtf8(u8"错误"), trUtf8(u8"程序启动失败！"));
		break;
	case QProcess::Crashed:
		QMessageBox::warning(this, trUtf8(u8"错误"), trUtf8(u8"程序崩溃！"));
		break;
	case QProcess::Timedout:
		QMessageBox::warning(this, trUtf8(u8"错误"), trUtf8(u8"程序超时！"));
		break;
	case QProcess::ReadError:
		QMessageBox::warning(this, trUtf8(u8"错误"), trUtf8(u8"程序读取失败！"));
		break;
	case QProcess::WriteError:
		QMessageBox::warning(this, trUtf8(u8"错误"), trUtf8(u8"程序写入失败！"));
		break;
	case QProcess::UnknownError:
		QMessageBox::warning(this, trUtf8(u8"错误"), trUtf8(u8"程序未知错误！"));
		break;
	}
}

void GetChipWindow::processDataReady()
{
	auto data = QString::fromUtf8(process_->readAllStandardOutput());
	// 解析本地地址
	if(data.indexOf(u8"代理地址") != -1)
	{
		auto list = data.split(" ");
		list = list[2].split(":");
		this->localProxyAddr_ = list[0];
		this->localProxyPort_ = list[1];
		setLocalProxy();
		QMessageBox::information(this, trUtf8(u8"成功"), trUtf8(u8"本地代理程序启动成功！"));
	}
}

void GetChipWindow::connect()
{
	QObject::connect(
		this->ui->getDataPushButton,
		&QPushButton::clicked,
		this,
		&GetChipWindow::getData
	);
	QObject::connect(
		this->ui->startLocalPushButton,
		&QPushButton::clicked,
		this,
		&GetChipWindow::startLocalProxy
	);
	QObject::connect(
		this->ui->localProxyRadioButton,
		&QRadioButton::toggled,
		this,
		&GetChipWindow::setLocalProxy
	);
	QObject::connect(
		this->ui->netProxyRadioButton,
		&QRadioButton::toggled,
		this,
		&GetChipWindow::setNetProxy
	);
	QObject::connect(
		this->accessManager_,
		&QNetworkAccessManager::finished,
		this,
		&GetChipWindow::recvData
	);
	QObject::connect(
		this->process_,
		&QProcess::readyReadStandardOutput,
		this,
		&GetChipWindow::processDataReady
	);
	QObject::connect(
		this->process_,
		&QProcess::errorOccurred,
		this,
		&GetChipWindow::processError
	);
	/*QObject::connect(
		this->accessManager_,
		&QNetworkAccessManager::sslErrors,
		[&](QNetworkReply* reply, const QList<QSslError>& errors)
	{
		for(auto& it : errors)
		{
			qDebug() << it;
		}
		QByteArray bytes = reply->readAll();
		qDebug() << QString::fromUtf8(bytes);
	});*/
}

void GetChipWindow::killProcess()
{
#ifdef Q_OS_WIN
	QString cmd = "taskkill /im ";
	cmd += exeName;
	cmd += " /f";
#else
	QString cmd = "killall -9 ";
	cmd += exeName;
#endif
	process_->execute(cmd);
}
