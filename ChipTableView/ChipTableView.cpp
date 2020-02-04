#include "ChipTableView.h"
#include <QBrush>

ChipTableModel::ChipTableModel(QObject* parent):
	QAbstractItemModel(parent),
	showBlocks_(false)
{
}

QModelIndex ChipTableModel::index(int row, int column, const QModelIndex& parent) const
{
	return hasIndex(row, column, parent) ? createIndex(row, column, nullptr) : QModelIndex();
}

QModelIndex ChipTableModel::parent(const QModelIndex& child) const
{
	return QModelIndex();
}

int ChipTableModel::rowCount(const QModelIndex& parent) const
{
	return chips_.size();
}

int ChipTableModel::columnCount(const QModelIndex& parent) const
{
	return showStatus_ ? 10 : 8;
}

bool ChipTableModel::hasChildren(const QModelIndex& parent) const
{
	return false;
}

QVariant ChipTableModel::data(const QModelIndex& index, int role) const
{
	if (index.row() >= chips_.size())
		return QVariant();
	const auto& chip = chips_[index.row()];
	switch (role)
	{
	case Qt::DisplayRole:
	case Qt::EditRole:
		switch (index.column())
		{
		case 0:
			return QString::number(chip.no);
		case 1:
			return chip.icon();
		case 2:
			return chip.name();
		case 3:
			return QString("+%1").arg(chip.level);
		case 4:
			return QString::number(showBlocks_ ? chip.hitBlock : chip.hitValue);
		case 5:
			return QString::number(showBlocks_ ? chip.reloadBlock : chip.reloadValue);
		case 6:
			return QString::number(showBlocks_ ? chip.damageBlock : chip.damageValue);
		case 7:
			return QString::number(showBlocks_ ? chip.defbreakBlock : chip.defbreakValue);
		case 8:
			return QString(chip.locked ? u8"¡Ì" : "");
		case 9:
			return chip.squadName();
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
	default:
		return QVariant();
	}
}

bool ChipTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	return false;
}

void ChipTableModel::setChips(const QList<GFChip>& chips)
{
	chips_ = chips;
	refresh();
}

void ChipTableModel::setShowBlocks(bool b)
{
	showBlocks_ = b;
	refresh();
}

void ChipTableModel::setShowStatus(bool b)
{
	showStatus_ = b;
	refresh();
}
