#pragma once
#pragma warning(disable:26812)
#include <QtWidgets/QMainWindow>
#include <QList>
#include <QMap>
#include <Chip/Chip.h>

namespace Ui { class CodeX; }
class GFChip;
class ChipSolver;
class ChipDataWindow;
class QProgressBar;
class QLabel;
class ChipTableModel;
class ChipTableDelegate;
class SolutionTableModel;
class SettingWindow;
class AboutDialog;
class AltSolutionWindow;

class CodeX : public QMainWindow
{
	Q_OBJECT

public:
	static CodeX* instance();

	// 芯片列表
	Chips chips;
	// 外层按颜色分类，内层按grid编号分类的芯片，保存强制+20的属性
	std::map<int,std::map<int, std::vector<GFChip>>> gridChips;
	// 按小队分类的已装备的芯片
	std::map<int, std::vector<GFChip>> squadChips;
	
	// 求解器
	ChipSolver* solver_;

	bool chipUsed(int no);

protected slots:
	void solve();
	void solveFinished();
	void selectSolution(int index);
	void addAltSolution();
	void chipsChanged();
	
private:
	CodeX(QWidget* parent = Q_NULLPTR);
	static CodeX* singleton;
	
	void connect();
	
	Ui::CodeX *ui;
	ChipDataWindow* chipDataWindow_;
	SettingWindow* settingWindow_;

	QProgressBar* progressBar_;
	QLabel* solveNumberLabel_;
	QLabel* timeLabel_;
	// 保存解决方案
	std::map < QString, std::map<QString, SquadSolution> > solutions_;

	ChipTableModel* chipTableModel_;
	ChipTableDelegate* chipTableDelegate_;
	SolutionTableModel* solutionTableModel_;
	AboutDialog* aboutDialog_;
	AltSolutionWindow* altSolutionWindow_;
};
