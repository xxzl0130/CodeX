#include "Chip.h"
#include <QString>
#include <QStringList>
#include <cmath>
#include <QJsonArray>
#include <QJsonDocument>

// 各属性系数
static constexpr double ArgDmg = 4.4, ArgDbk = 12.7, ArgAcu = 7.1, ArgFil = 5.7;
// 密度系数
static constexpr double Den56 = 1.0, Den551 = 1.0, Den552 = 0.92;
// 等级系数
static constexpr double ArgLv[21] = { 1.0,1.08,1.16,1.24,1.32,1.4,1.48,1.56,1.64,1.72,1.8,1.87,1.94,2.01,2.08,2.15,2.22,2.29,2.36,2.43,2.5 };


GFChip::GFChip(const QJsonObject& object)
{
	chipClass = object.value("chip_id").toInt();
	level = object.value("chip_level").toInt();
	color = object.value("color_id").toInt();
	gridID = object.value("grid_id").toInt();
	squad = object.value("squad_with_user_id").toInt();
	auto t = object.value("position").toString().split(",");
	position.x = t[0].toInt();
	position.y = t[1].toInt();
	rotate = object.value("shape_info").toString()
		.split(",")[0].toInt();
	damageBlock = object.value("assist_damage").toInt();
	reloadBlock = object.value("assist_reload").toInt();
	hitBlock = object.value("assist_hit").toInt();
	defbreakBlock = object.value("assist_def_break").toInt();
	locked = object.value("is_locked").toBool();
	calcValue();
}

GFChip GFChip::fromJsonObject(const QJsonObject& object)
{
	return GFChip(object);
}

QIcon GFChip::icon() const
{
	static const QString url(":/Chip/Resources/img/%1%2.png");
	return QIcon(url.arg(color == Blue ? "b" : "o").arg(gridID));
}

GFChip::GFChip(): chipClass(0), level(0), color(Orange), gridID(0), squad(0), position({0,0}), rotate(0), damageBlock(0),
              reloadBlock(0), hitBlock(0), defbreakBlock(0), damageValue(0), reloadValue(0), hitValue(0),
              defbreakValue(0), locked(false)
{
}

void GFChip::calcValue()
{
	auto den = Den56;
	switch (this->chipClass)
	{
	default:
	case Class56:
		den = Den56;
		break;
	case Class551:
		den = Den551;
		break;
	case Class552:
		den = Den552;
		break;
	}
	this->damageValue = ceil(ceil(this->defbreakBlock * ArgDmg * den) * ArgLv[this->level]);
	this->defbreakValue = ceil(ceil(this->defbreakBlock * ArgDbk * den) * ArgLv[this->level]);
	this->hitValue = ceil(ceil(this->hitBlock * ArgAcu * den) * ArgLv[this->level]);
	this->reloadValue = ceil(ceil(this->reloadBlock * ArgFil * den) * ArgLv[this->level]);
}

QList<GFChip> getChips(const QJsonObject& obj)
{
	QList<GFChip> chips;
	for(const auto& k : obj.keys())
	{
		auto chip = GFChip::fromJsonObject(obj[k].toObject());
		if ((chip.gridID == GFChip::Class56 || chip.gridID == GFChip::Class551 || chip.gridID == GFChip::Class552) && chip.
			gridID > 11)
		{
			chips.push_back(chip);
		}
	}
	return chips;
}

const ChipConfig& ChipConfig::getConfig(int id)
{
	if(configs_.empty())
	{
		// 初始化
		QFile json(":/Chip/Resources/chips.json");
		json.open(QIODevice::ReadOnly);
		auto doc = QJsonDocument::fromJson(json.readAll());
		for(const auto& it : doc.object())
		{
			auto config = ChipConfig(it.toObject());
			configs_[config.gridID] = config;
		}
	}
	return configs_[id];
}

ChipConfig ChipConfig::rotate90(int n) const
{
	auto rotate = [](const Map& map) -> std::vector<std::string>
	{
		Map r;
		std::string t;
		t.resize(map.size(), '0');
		r.resize(map[0].length(), t);
		for (auto i = 0u; i < r.size(); ++i)
		{
			for (auto j = 0u; j < r[i].length(); ++j)
			{
				r[i][j] = map[map.size() - j - 1][i];
			}
		}
		return r;
	};
	auto t = *this;
	for (auto i = 0; i < n; ++i)
	{
		t.map = rotate(t.map);
	}
	return t;
}

ChipConfig::ChipConfig(const QJsonObject& object)
{
	gridID = object.value("ID").toInt();
	chipClass = object.value("class").toInt();
	width = object.value("width").toInt();
	height = object.value("height").toInt();
	blocks = object.value("blocks").toInt();
	name = object.value("name").toString().toStdString();
	auto oMap = object["map"].toArray();
	for(const auto& it : oMap)
	{
		map.push_back(it.toString().toStdString());
	}
}

ChipPuzzleOption::ChipPuzzleOption(const QJsonObject& object)
{
	gridID = object.value("ID").toInt();
	x = object.value("x").toInt();
	y = object.value("y").toInt();
	rotate = object.value("rotate").toInt();
}
