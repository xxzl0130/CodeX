#pragma once
#pragma warning(disable:26812)
#include <QtWidgets/QMainWindow>
#include "Chip/Chip.h"

namespace Ui { class CodeX; }

class CodeX : public QMainWindow
{
	Q_OBJECT

public:
	static CodeX* instance();

	QList<GFChip> chips;

private:
	CodeX(QWidget* parent = Q_NULLPTR);
	static CodeX* singleton;
	
	void connect();
	
	Ui::CodeX *ui;
	ChipDataWindow* chipDataWindow;
};
