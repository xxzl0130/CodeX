#include "SolutionTableModel.h"
#include "ChipSolver/ChipSolver.h"
#include "Chip/Chip.h"
#include <QList>
#include "CodeX/CodeX.h"

SolutionTableModel::SolutionTableModel(QObject *parent)
	: QAbstractItemModel(parent),
	solution_(nullptr)
{
	font_.setFamily(QString::fromUtf8("\347\255\211\347\272\277"));
	font_.setPointSize(12);
}

SolutionTableModel::~SolutionTableModel()
{
}

QModelIndex SolutionTableModel::index(int row, int column, const QModelIndex& parent) const
{
	return hasIndex(row, column, parent) ? createIndex(row, column, nullptr) : QModelIndex();
}

QModelIndex SolutionTableModel::parent(const QModelIndex& child) const
{
	return QModelIndex();
}

int SolutionTableModel::rowCount(const QModelIndex& parent) const
{
	if (!solution_)
		return 0;
	return solution_->size();
}

int SolutionTableModel::columnCount(const QModelIndex& parent) const
{
	return 8;
}

bool SolutionTableModel::hasChildren(const QModelIndex& parent) const
{
	return false;
}

inline int min(int a,int b)
{
	return a < b ? a : b;
}

QVariant SolutionTableModel::data(const QModelIndex& index, int role) const
{
	if(!solution_)
		return QVariant();
	if (index.row() >= solution_->size())
		return QVariant();
	const auto& solution = (*solution_)[index.row()];
	switch (role)
	{
	case Qt::DisplayRole:
	case Qt::EditRole:
		switch (index.column())
		{
		case 0:
			return index.column();
		case 1:
			return QString("%1 (%2)").arg(solution.totalValue.damageValue)
			.arg(min(0, solution.totalValue.damageValue - maxValue_.damageValue));
		case 2:
			return QString("%1 (%2)").arg(solution.totalValue.defbreakValue)
				.arg(min(0, solution.totalValue.defbreakValue - maxValue_.defbreakValue));
		case 3:
			return QString("%1 (%2)").arg(solution.totalValue.hitValue)
				.arg(min(0, solution.totalValue.hitValue - maxValue_.hitValue));
		case 4:
			return QString("%1 (%2)").arg(solution.totalValue.reloadValue)
		.arg(min(0, solution.totalValue.reloadValue - maxValue_.reloadValue));
		case 5:
			return solution.totalValue.id;
		case 6:
			return solution.totalValue.level;
		case 7:
			return solution.totalValue.no * 50;
		default:
			return QVariant();
		}
	case Qt::TextAlignmentRole:
		return int(Qt::AlignHCenter | Qt::AlignVCenter);
	case Qt::BackgroundRole:
		if (index.row() % 2)
		{
			return QBrush(QColor(Qt::lightGray));
		}
		else
		{
			return QColor(Qt::white);
		}
	case Qt::FontRole:
		return font_;
	default:
		return QVariant();
	}
}

bool SolutionTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	return false;
}

Qt::ItemFlags SolutionTableModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;
	return QAbstractItemModel::flags(index) & ~Qt::ItemIsEditable;
}

QVariant SolutionTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::FontRole)
	{
		return font_;
	}
	if (orientation == Qt::Vertical || role != Qt::DisplayRole)
		return QAbstractItemModel::headerData(section, orientation, role);
	switch (section)
	{
	case 0:
		return trUtf8(u8"编号");
	case 1:
		return trUtf8(u8"杀伤");
	case 2:
		return trUtf8(u8"破防");
	case 3:
		return trUtf8(u8"装填");
	case 4:
		return trUtf8(u8"命中");
	case 5:
		return trUtf8(u8"总属性");
	case 6:
		return trUtf8(u8"总等级");
	case 7:
		return trUtf8(u8"校准券");
	default:
		return QAbstractItemModel::headerData(section, orientation, role);
	}
}

void SolutionTableModel::setSolution(SquadSolution const* ptr)
{
	solution_ = ptr;
	refresh();
}

void SolutionTableModel::setMaxValue(const GFChip& value)
{
	maxValue_ = value;
	refresh();
}