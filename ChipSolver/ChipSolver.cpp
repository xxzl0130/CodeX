#include "ChipSolver.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <vector>
#include <ctime>
#include "CodeX/CodeX.h"
#include <cstdlib>

constexpr auto QrcBase = ":/ChipSolver/Resources/";

ChipSolver::ChipSolver(QObject* parent):
	QThread(parent),
	useEquipped_(false),
	useLocked_(false)
{
	Q_INIT_RESOURCE(ChipSolver);

	qRegisterMetaType<TargetBlock>("TargetBlock");
	
	QFile file(QString(QrcBase) + "squads.json");
	file.open(QIODevice::ReadOnly);
	configInfo_ = QJsonDocument::fromJson(file.readAll()).object();
	file.close();

	for(const auto& squad : configInfo_.keys())
	{
		file.setFileName(QrcBase + squad + ".json");
		file.open(QIODevice::ReadOnly);
		auto obj = QJsonDocument::fromJson(file.readAll()).object();
		file.close();
		
		// 读入格子信息
		ChipViewInfo info;
		info.width = obj["width"].toInt();
		info.height = obj["height"].toInt();
		auto map = obj["map"].toArray();
		for(auto i = 0;i < map.size();++i)
		{
			info.map.emplace_back();
			for(const auto& it : map[i].toString().toLatin1())
			{
				info.map[i].push_back(-(it - '0'));
			}
		}
		squadView_[squad] = info;

		// 读取芯片拼图方案
		const auto& configFile = configInfo_[squad].toObject();
		for(const auto& name : configFile.keys())
		{
			SquadConfig squadConfig;
			// 重装的基本信息，实际上和方案无关，但是没地方存了，懒得单独再存
			squadConfig.blocks = obj["blocks"].toInt();
			squadConfig.optional = obj["optional"].toObject()[name].toInt();
			auto maxBlocks = obj["MaxBlocks"].toObject();
			squadConfig.maxValue.damageBlock = maxBlocks["damage"].toInt();
			squadConfig.maxValue.defbreakBlock = maxBlocks["def_break"].toInt();
			squadConfig.maxValue.hitBlock = maxBlocks["hit"].toInt();
			squadConfig.maxValue.reloadBlock = maxBlocks["reload"].toInt();
			auto maxValues = obj["MaxValues"].toObject();
			squadConfig.maxValue.damageValue = maxValues["damage"].toInt();
			squadConfig.maxValue.defbreakValue = maxValues["def_break"].toInt();
			squadConfig.maxValue.hitValue = maxValues["hit"].toInt();
			squadConfig.maxValue.reloadValue = maxValues["reload"].toInt();
			squadConfig.color = obj["color"].toInt();
			
			file.setFileName(QrcBase + configFile[name].toString());
			file.open(QIODevice::ReadOnly);
			auto configArray = QJsonDocument::fromJson(file.readAll()).array();
			file.close();
			for(const auto& it : configArray)
			{
				Config config;
				for(const auto& c : it.toArray())
				{
					config.push_back(ChipPuzzleOption(c.toObject()));
				}
				squadConfig.configs.push_back(config);
			}
			// name是方案名
			configs_[name] = squadConfig;
		}
	}
}

QStringList ChipSolver::squadList() const
{
	return configInfo_.keys();
}

QStringList ChipSolver::configList(const QString& squad)
{
	targetSquadName_ = squad;
	return configInfo_[squad].toObject().keys();
}

GFChip ChipSolver::squadMaxValue(const QString& squad)
{
	return configs_[configList(squad)[0]].maxValue;
}

void ChipSolver::setTargetBlock(const TargetBlock& block)
{
	targetBlock_ = block;
}

void ChipSolver::setTargetConfig(const QString& name)
{
	targetConfigName_ = name;
}

void ChipSolver::setUseEquipped(bool b)
{
	useEquipped_ = b;
}

void ChipSolver::setUseLocked(bool b)
{
	useLocked_ = b;
}

ChipViewInfo ChipSolver::solution2ChipView(const Solution& solution)
{
	auto view = squadView_[targetSquadName_];
	for(auto i = 0;i < solution.chips.size();++i)
	{
		const auto& chip = solution.chips[i];
		const auto& chipConfig = ChipConfig::getConfig(CodeX::instance()->chips[chip.id].gridID).rotate90(chip.rotate);
		for(auto x = 0;x < chipConfig.width;++x)
		{
			for(auto y = 0;y < chipConfig.height;++y)
			{
				if(chipConfig.map[y][x] == '1')
				{
					view.map[y + chip.y][x + chip.x] = i + 1;
				}
			}
		}
	}
	return view;
}

void ChipSolver::run()
{
	const auto& plans = configs_[targetConfigName_];
	tmpTarget_ = targetBlock_;
	tmpTarget_.error += plans.optional; // 附加额外的可空格
	int percent = 0;
	chipUsed_.resize(CodeX::instance()->chips.size(), false);
	solutions.clear();
	auto t0 = clock();
	for(auto i = 0;i < plans.configs.size();++i)
	{
		// 计算当前进度百分比
		int per = round((i + 1) * 100.0 / plans.configs.size());
		if(per > percent)
		{
			percent = per;
			emit solvePercentChanged(percent);
		}

		if(!satisfyConfig(plans.configs[i]))
		{
			continue;
		}
		// 临时记录方案
		tmpSolution_.chips.resize(8, ChipPuzzleOption());

		// 当前配置方案
		tmpConfig_ = plans.configs[i];
		tmpMaxValue_ = configs_[targetConfigName_].maxValue;
		tmpColor_ = configs_[targetConfigName_].color;
		solutionSet_.clear();
		findSolution(0);
		emit solveNumberChanged(solutions.size());
		auto t1 = clock();
		emit solveTimeChanged(double(t1 - t0) / CLOCKS_PER_SEC);
		if (solutions.size() >= targetBlock_.upper)
			break;
	}
	emit solvePercentChanged(100);
}

bool ChipSolver::satisfyConfig(const Config& config)
{
	QMap<int, int> require;
	bool ans = true;
	for(const auto& it : config)
	{
		++require[it.id];
	}
	for(const auto& it : require.keys())
	{
		ans = ans && (CodeX::instance()->gridChips[it].size() >= require[it]);
	}
	return ans;
}

bool ChipSolver::checkOverflow(const TargetBlock& target, const GFChip& chips) const
{
	int over = 0;
	over += std::max(0, chips.defbreakBlock - target.defbreakBlock);
	over += std::max(0, chips.damageBlock - target.damageBlock);
	over += std::max(0, chips.reloadBlock - target.reloadBlock);
	over += std::max(0, chips.hitBlock - target.hitBlock);
	return over > target.error;
}

void ChipSolver::findSolution(int k)
{
	if (k >= tmpConfig_.size())
	{
		if (solutionSet_.count(chipUsed_) > 0)
			return;
		solutionSet_.insert(chipUsed_);
		tmpSolution_.totalValue.no = 0;
		tmpSolution_.totalValue.id = 0;
		tmpSolution_.totalValue.level = 0;
		for(const auto& it : tmpSolution_.chips)
		{
			const auto& chip = CodeX::instance()->chips[it.id];
			tmpSolution_.totalValue.no += abs(it.rotate - chip.rotate);
			tmpSolution_.totalValue.level += chip.level;
		}
		tmpSolution_.totalValue.id += std::min(0, tmpSolution_.totalValue.defbreakValue - tmpMaxValue_.defbreakValue);
		tmpSolution_.totalValue.id += std::min(0, tmpSolution_.totalValue.damageValue - tmpMaxValue_.damageValue);
		tmpSolution_.totalValue.id += std::min(0, tmpSolution_.totalValue.reloadValue - tmpMaxValue_.reloadValue);
		tmpSolution_.totalValue.id += std::min(0, tmpSolution_.totalValue.hitValue - tmpMaxValue_.hitValue);
		auto t = tmpSolution_.chips.size();
		tmpSolution_.chips.resize(k);
		solutions.push_back(tmpSolution_);
		tmpSolution_.chips.resize(t);
		return;
	}
	//获取当前所需型号的芯片列表
	const auto& chips = CodeX::instance()->gridChips[tmpConfig_[k].id];
	// 先保存这一步的芯片配置，后续更新id
	tmpSolution_.chips[k] = tmpConfig_[k];
	for (const auto& chip : chips)
	{
		if((chip.color != tmpColor_) // 颜色不同
			|| (chip.locked && !useLocked_)// 已锁定且不使用已锁定
			|| (chip.squad && !useEquipped_))// 已装备且不使用已装备
		{
			continue;
		}
		if (!chipUsed_[chip.no])
		{
			// 溢出了不满足要求
			if (checkOverflow(tmpTarget_, tmpSolution_.totalValue + chip))
			{
				continue;
			}
			chipUsed_[chip.no] = true;
			tmpSolution_.totalValue += chip;
			tmpSolution_.chips[k].id = chip.no; // 更新id
			findSolution(k + 1);
			chipUsed_[chip.no] = false;
			tmpSolution_.totalValue -= chip;
		}
	}
}
