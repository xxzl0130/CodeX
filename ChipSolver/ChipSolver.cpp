#include "ChipSolver.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

constexpr auto QrcBase = ":/ChipSolver/Resources/";

ChipSolver::ChipSolver(QObject* parent):
	QThread(parent)
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
			info.map.push_back(QList<int>());
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
			
			file.setFileName(QrcBase + configFile[name].toString());
			file.open(QIODevice::ReadOnly);
			auto configArray = QJsonDocument::fromJson(file.readAll()).array();
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

QStringList ChipSolver::squads() const
{
	return configInfo_.keys();
}

QStringList ChipSolver::configs(const QString& squad)
{
	targetSquadName_ = squad;
	return configInfo_[squad].toObject().keys();
}

void ChipSolver::setTargetBlock(const TargetBlock& block)
{
	targetBlock_ = block;
}

void ChipSolver::setTargetConfig(const QString& name)
{
	targetConfigName_ = name;
}

ChipViewInfo ChipSolver::solution2ChipView(const Solution& solution)
{
	return ChipViewInfo();
}

void ChipSolver::run()
{
	//TODO
}
