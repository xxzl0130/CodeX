#pragma once

#include "chip_global.h"
#include <QJsonObject>
#include <QPixmap>
#include <QList>
#include <string>
#include <vector>
#include <map>

// 游戏内的芯片数据
class CHIP_EXPORT GFChip
{
public:
	explicit GFChip(const QJsonObject& object);
	GFChip();
	static GFChip fromJsonObject(const QJsonObject& object);
	QPixmap icon() const;
	QString name() const;
	QString squadName() const;

	enum ChipClass
	{
		Class56 = 5061,
		Class551 = 5051,
		Class552 = 5052,
	};
	enum ChipColor
	{
		Orange = 1,
		Blue = 2,
	};
	struct Point
	{
		int x, y;
	};

	// 游戏内id
	int id;
	// 顺序编号
	int no;
	// 芯片类别，5061/5051/5052...
	int chipClass;
	// 芯片等级
	int level;
	// 芯片颜色
	int color;
	// 芯片形状ID
	int gridID;
	// 小队装备信息，0未装备
	int squad;
	// 芯片位置
	Point position;
	// 芯片旋转，顺时针，90度为单位
	int rotate;
	// 伤害格数
	int damageBlock;
	// 装填格数
	int reloadBlock;
	// 命中格数
	int hitBlock;
	// 破防格数
	int defbreakBlock;
	// 伤害数值
	int damageValue;
	// 装填数值
	int reloadValue;
	// 命中数值
	int hitValue;
	// 破防数值
	int defbreakValue;
	// 锁定
	bool locked;
	
	void calcValue();

	GFChip operator + (const GFChip& t) const;
	GFChip operator += (const GFChip& t);
	GFChip operator - (const GFChip& t) const;
	GFChip operator -= (const GFChip& t);
};

typedef std::vector<GFChip> Chips;

Chips CHIP_EXPORT getChips(const QJsonObject& obj);

// 小队序号转为对应名称，需要先从squad_with_user_info中读取id并转换为1-6
QString CHIP_EXPORT squadString(int i);

// 每类芯片的基本参数
class CHIP_EXPORT ChipConfig
{
public:
	typedef std::vector<std::string> Map;

	ChipConfig() = default;
	
	// 芯片形状ID
	int gridID;
	// 芯片类别，5061/5051/5052...
	int chipClass;
	// 芯片宽度
	int width;
	// 芯片高度
	int height;
	// 芯片格数
	int blocks;
	// 旋转方向数
	int direction;

	// 芯片名字
	std::string name;
	// 芯片形状
	Map map;

	// 通过gridID获取芯片配置
	static const ChipConfig& getConfig(int id);

	ChipConfig rotate90(int n = 1) const; // clockwise

private:
	explicit ChipConfig(const QJsonObject& object);

	static std::map<int, ChipConfig> configs_;
};

// 芯片在拼图解法中的参数
struct CHIP_EXPORT ChipPuzzleOption
{
	int id;
	uint8_t x, y;
	// 顺时针旋转90度的次数
	uint8_t rotate;
	explicit ChipPuzzleOption(int _x = 0, int _y = 0, int _r = 0, int _no = 0) :id(_no), x(_x), y(_y), rotate(_r) {}
	explicit ChipPuzzleOption(const QJsonObject& object);
};

// 芯片摆放信息，0为空，>0为芯片编号（下标+1),<0为无法使用
struct CHIP_EXPORT ChipViewInfo
{
	int width;
	int height;
	std::vector<std::vector<int>> map;
};

// 一组可行解
struct CHIP_EXPORT Solution
{
	// 使用的芯片参数
	std::vector<ChipPuzzleOption> chips;
	// 得到的重装总属性，以level为总等级，id为属性偏差，no为旋转次数
	GFChip totalValue;

	bool operator <(const Solution& r) const;
};
