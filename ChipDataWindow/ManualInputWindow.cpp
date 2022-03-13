#include "stdafx.h"
#include "ManualInputWindow.h"
#include "ui_ManualInputWindow.h"

ManualInputWindow::ManualInputWindow(QWidget *parent)
	: QDialog(parent), ui(new Ui::ManualInputWindow())
{
	ui->setupUi(this);
	QObject::connect(this->ui->buttonBox, &QDialogButtonBox::accepted, this, &ManualInputWindow::onConfirm);
	QObject::connect(this->ui->buttonBox, &QDialogButtonBox::rejected, this, &ManualInputWindow::onCancel);
}

ManualInputWindow::~ManualInputWindow()
{
	delete ui;
}

void ManualInputWindow::onConfirm()
{
	Chips chips;
	auto code = this->ui->inputTextEdit->toPlainText().trimmed();
	if (code.isEmpty())
	{
		this->accept();
		return;
	}
	if (code2Chips(code, chips))
	{
		QJsonObject obj;
		QJsonObject chipObj;
		for (auto& chip : chips)
		{
			chipObj[QString::number(chip.id)] = chip.toObject();
		}
		obj["chip_with_user_info"] = chipObj;
		emit sendChipJsonObject(obj);
		this->accept();
	}
	else
	{
		QMessageBox::warning(this, u8"´íÎó", u8"´íÎóµÄÐ¾Æ¬´úÂë£¡");
	}
}

void ManualInputWindow::onCancel()
{
	this->reject();
}

bool ManualInputWindow::code2Chips(const QString& inputCode, Chips& outChips)
{
	auto innerStr = inputCode.trimmed();
	if (innerStr.isEmpty() || innerStr.length() < 10)
		return false;
	if (innerStr.front() != '[' || innerStr.back() != ']')
		return false;
	if (innerStr[2] != '!' || innerStr[innerStr.length() - 3] != '?')
		return false;
	innerStr = innerStr.split("!").back().split("?").front();
	auto chipCodes = innerStr.split("&", QString::SkipEmptyParts);
	std::vector<GFChip> chips;
	chips.reserve(chipCodes.size());
	for (auto& str : chipCodes)
	{
		auto chip = GFChip::fromHycdesCode(str);
		if(chip.id < 0)
			return false;
		chips.push_back(std::move(chip));
	}
	outChips = std::move(chips);

	return true;
}
