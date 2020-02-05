#pragma once
#pragma warning(disable:26812)
#include <QtWidgets/QMainWindow>
#include <QList>
#include <QMap>

namespace Ui { class CodeX; }
class GFChip;
class ChipSolver;
class ChipDataWindow;
class QProgressBar;
class QLabel;
class ChipTableModel;
class ChipTableDelegate;
class SolutionTableModel;

class CodeX : public QMainWindow
{
	Q_OBJECT

public:
	static CodeX* instance();

	// 芯片列表
	Chips chips;
	// 按grid编号分类的芯片，保存强制+20的属性
	QMap<int, QList<GFChip>> gridChips;

protected slots:
	void solve();
	void solveFinished();
	void selectSolution(int index);
	
private:
	CodeX(QWidget* parent = Q_NULLPTR);
	static CodeX* singleton;
	
	void connect();
	
	Ui::CodeX *ui;
	ChipDataWindow* chipDataWindow;
	ChipSolver* solver;

	QProgressBar* progressBar_;
	QLabel* solveNumberLabel_;
	QLabel* timeLabel_;

	ChipTableModel* chipTableModel_;
	ChipTableDelegate* chipTableDelegate_;
	SolutionTableModel* solutionTableModel_;
};
