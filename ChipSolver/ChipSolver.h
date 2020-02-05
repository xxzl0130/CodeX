#pragma once

#include "chipsolver_global.h"
#include <QThread>
#include <QList>
#include <QMap>
#include <QString>
#include <QJsonObject>
#include <vector>
#include "Chip/Chip.h"

// 一组可行解
struct CHIPSOLVER_EXPORT Solution
{
	// 使用的芯片参数
	std::vector<ChipPuzzleOption> chips;
	// 得到的重装总属性
	GFChip totalValue;
};

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
	// 最大方案数量
	int upper;

	explicit TargetBlock(int dmg = 0, int dbk = 0, int hit = 0, int r = 0,  int e = 0, int u = 1e9) :
		damageBlock(dmg), defbreakBlock(dbk), hitBlock(hit), reloadBlock(r), error(e), upper(u)
	{}
};

class CHIPSOLVER_EXPORT ChipSolver : public QThread
{
	Q_OBJECT
public:
	explicit ChipSolver(QObject* parent = nullptr);
	~ChipSolver() = default;

	// 一个重装的所有解
	typedef QList<Solution> SquadSolution;

	SquadSolution solutions;
	
	// 重装列表
	QStringList squads() const;
	// 该重装的方案列表，同时会设置成当前重装
	QStringList configs(const QString& squad);

	// 将一个方案转为可显示的图形信息
	ChipViewInfo solution2ChipView(const Solution& solution);

public slots:
	// 设置目标格数
	void setTargetBlock(const TargetBlock& block);
	// 设置目标的配置方案
	void setTargetConfig(const QString& name);
	void setUseEquipped(bool b);
	void setUseLocked(bool b);

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
	typedef QList<ChipPuzzleOption> Config;
	// 一个重装的所有配置
	struct SquadConfig
	{
		std::vector<Config> configs;
		// 总格数
		int blocks = 38;
		// 可空格数
		int optional = 0;
		// 最大属性
		GFChip maxValue;
	};
	// 所有配置方案，以名称为key
	QMap<QString, SquadConfig> configs_;
	// 重装和配置方案记录信息
	QJsonObject configInfo_;
	// 重装的基本格子
	QMap<QString, ChipViewInfo> squadView_;
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
	// 记录芯片使用
	std::vector<bool> chipUsed_;
	// 使用已装备
	bool useEquipped_;
	// 使用已锁定
	bool useLocked_;

	//检查当前芯片数量是否满足该拼法最低需要
	static bool satisfyConfig(const Config& config);
	// 检查芯片方案是否满足目标的格数溢出要求，只计算上溢，溢出返回true
	bool checkOverflow(const TargetBlock& target, const GFChip& chips) const;
	// 递归求解方案
	void findSolution(int k);
};
