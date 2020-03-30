#pragma once

#include "chipsolver_global.h"
#include <QThread>
#include <QString>
#include <map>
#include <set>
#include <vector>
#include <queue>
#include "Chip/Chip.h"

struct CHIPSOLVER_EXPORT TargetBlock
{
	// 伤害格数
	int damageBlock;
	// 破防格数
	int defbreakBlock;
	// 精度格数
	int hitBlock;
	// 装填格数
	int reloadBlock;
	// 偏差格数
	int error;
	// 最大显示方案数量
	int showNumber;
	// 最大计算方案数量
	int maxNumber;

	explicit TargetBlock(int dmg = 0, int dbk = 0, int hit = 0, int r = 0,  int e = 0, int u = 1e3,int m = 1e9) :
		damageBlock(dmg), defbreakBlock(dbk), hitBlock(hit), reloadBlock(r), error(e), showNumber(u),maxNumber(m)
	{}
};

class CHIPSOLVER_EXPORT ChipSolver : public QThread
{
	Q_OBJECT
public:
	explicit ChipSolver(QObject* parent = nullptr);
	~ChipSolver() = default;

	SquadSolution solutions;
	
	// 重装列表
	QStringList squadList() const;
	// 该重装的方案列表，同时会设置成当前重装
	QStringList configList(const QString& squad);
	// 返回该重装的最大属性
	GFChip squadMaxValue(const QString& squad);

	// 将一个方案转为可显示的图形信息
	ChipViewInfo solution2ChipView(const Solution& solution, const QString& squad = "");

public slots:
	// 设置目标格数
	void setTargetBlock(const TargetBlock& block);
	// 设置目标的配置方案
	void setTargetConfig(const QString& name);
	void setUseEquipped(bool b);
	void setUseLocked(bool b);
	void setUseAlt(bool b);
	void stop();

signals:
	// 解决方案百分比
	void solvePercentChanged(int percent);
	// 总方案数
	void solveNumberChanged(int number);
	// 解算用时
	void solveTimeChanged(double time);
	
protected:
	void run() override;

private:	
	// 一组配置
	typedef std::vector<ChipPuzzleOption> Config;
	// 一个重装的所有配置
	struct SquadConfig
	{
		std::vector<Config> configs;
		// 总格数
		int blocks = 38;
		// 可空格数
		int optional = 0;
		// 颜色
		int color;
		// 旋转对称，0为不对称，>0为需要旋转几次
		int palindrome;
		// 最大属性
		GFChip maxValue;
	};
	// 所有配置方案，以名称为key
	std::map<QString, SquadConfig> configs_;
	// 所有重装的最大值
	std::map<QString, GFChip> maxValues_;
	// 重装和配置方案记录信息
	QJsonObject configInfo_;
	// 重装的基本格子
	std::map<QString, ChipViewInfo> squadView_;
	// 目标重装
	QString targetSquadName_;
	// 目标方案
	QString targetConfigName_;
	// 目标格数
	TargetBlock targetBlock_;
	// 临时记录目标
	TargetBlock tmpTarget_;
	// 临时记录方案
	Solution tmpSolution_;
	// 临时选择的方案
	Config tmpConfig_;
	// 临时选择的重装方案
	SquadConfig tmpSquadConfig_;
	// 去重用set
	std::set<std::vector<int>> solutionSet_;
	// 上一次上报的求解数量
	int lastSolveNumber_;
	// 开始运行的时间
	time_t t0_;
	// 使用已装备
	bool useEquipped_;
	// 使用已锁定
	bool useLocked_;
	// 使用备选
	bool useAlt_;
	// 运行标志，用于停止解算
	bool running_;
	// 当前芯片
	std::vector<int> tmpChips_;
	// 优先级队列
	std::priority_queue<Solution> queue_;
	// 方案数
	int tmpSolutionNumber_;

	//检查当前芯片数量是否满足该拼法最低需要
	bool satisfyConfig(const Config& config);
	// 检查芯片方案是否满足目标的格数溢出要求，只计算上溢，溢出返回true
	bool checkOverflow(const TargetBlock& target, const GFChip& chips) const;
	// 递归求解方案
	void findSolution(int k);
};
