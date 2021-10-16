#pragma once

#include "chipsolver_global.h"
#include <QThread>
#include <QString>
#include <QReadWriteLock>
#include <QQueue>
#include <map>
#include <qmutex.h>
#include <set>
#include <vector>
#include <queue>
#include <atomic>
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
	virtual ~ChipSolver() = default;

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
	void solveNumberChanged(long long number);
	
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
	std::map<QString, std::map<QString, SquadConfig>> configs_;
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
	std::atomic_int64_t lastSolveNumber_{};
	// 方案数
	std::atomic_int64_t tmpSolutionNumber_{};
	// 百分比
	std::atomic_int percent;
	// 当前配置序号
	int configIndex_;
	// 锁
	std::mutex configIndexLock_;
	// 使用已装备
	bool useEquipped_;
	// 使用已锁定
	bool useLocked_;
	// 使用备选
	bool useAlt_;
	// 运行标志，用于停止解算
	bool running_;
	bool solveRunning_;
	// 当前芯片
	std::vector<int> tmpChips_;

	//检查当前芯片数量是否满足该拼法最低需要
	bool satisfyConfig(const Config& config);
	// 检查芯片方案是否满足目标的格数溢出要求，只计算上溢，溢出返回true
	bool checkOverflow(const TargetBlock& target, const GFChip& chips) const;

	struct SolverParam
	{
		int k;
		// 临时记录方案
		Solution solution;
		// 选择的方案
		Config config;
		// 当前芯片列表，用于去重
		std::vector<int> curChips;
		// 去重用set
		std::set<std::vector<int>> solutionSet;
		// 芯片列表
		Chips chips;
		// 外层按颜色分类，内层按grid编号分类的芯片，保存强制+20的属性
		std::map<int, std::map<int, std::vector<GFChip>>> gridChips;
		// 优先级队列
		std::shared_ptr<std::priority_queue<Solution>> queue;

		SolverParam()
		{
			k = 0;
		}
	};
	void findSolution(SolverParam& param);
	void startSolve();
	// 各个线程给出的结果的队列
	std::queue<std::shared_ptr<std::priority_queue<Solution>>> thSolutionQueue_;
	// 锁
	std::mutex queueMutex_;
	std::condition_variable queueCV_;
	void merge();
};
