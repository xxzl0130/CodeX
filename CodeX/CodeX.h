#pragma once
#pragma warning(disable:26812)
#include <QtWidgets/QMainWindow>
#include <QList>
#include <QMap>

namespace Ui { class CodeX; }
class GFChip;
class ChipSolver;

class CodeX : public QMainWindow
{
	Q_OBJECT

public:
	static CodeX* instance();

	// 芯片列表
	QList<GFChip> chips;
	// 按grid编号分类的芯片，保存强制+20的属性
	QMap<int, QList<GFChip>> gridChips;
private:
	CodeX(QWidget* parent = Q_NULLPTR);
	static CodeX* singleton;
	
	void connect();
	
	Ui::CodeX *ui;
	ChipDataWindow* chipDataWindow;
	ChipSolver* solver;
};
