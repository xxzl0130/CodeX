#pragma once

#include "chipsolver_global.h"
#include <QThread>
#include <QList>
#include <QMap>
#include <QString>
#include <QJsonObject>
#include "Chip/Chip.h"

// 一组可行解
struct CHIPSOLVER_EXPORT Solution
{
	// 使用的芯片参数
	QList<ChipPuzzleOption> chips;
	// 得到的重装总属性
	GFChip totalValue;
};

class CHIPSOLVER_EXPORT ChipSolver : public QThread
{
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

	struct TargetBlock
	{
		// 伤害格数
		int damageBlock;
		// 装填格数
		int reloadBlock;
		// 命中格数
		int hitBlock;
		// 破防格数
		int defbreakBlock;
		// 偏差格数
		int error;
		// 最大方案数量
		int upper;

		explicit TargetBlock(int dmg = 0,int r = 0,int hit = 0,int dbk = 0,int e = 0,int u = 1e9):
			damageBlock(dmg), reloadBlock(r), hitBlock(hit),defbreakBlock(dbk),error(e),upper(u)
		{}
	};

	// 将一个方案转为可显示的图形信息
	ChipViewInfo solution2ChipView(const Solution& solution);

public slots:
	// 设置目标格数
	void setTargetBlock(const TargetBlock& block);
	// 设置目标的配置方案
	void setTargetConfig(const QString& name);

signals:
	void solvePercentChanged(int percent);
	
protected:
	void run() override;

private:
	// 一组配置
	typedef QList<ChipPuzzleOption> Config;
	// 一个重装的所有配置
	struct SquadConfig
	{
		QList<Config> configs;
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
};
