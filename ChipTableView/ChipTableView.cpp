#include "ChipTableView.h"
#include <QBrush>
#include <QPainter>
#include <QFont>
#include <QColor>
#include "ChipView/ChipView.h"

ChipTableModel::ChipTableModel(QObject* parent):
	QAbstractItemModel(parent),
	showBlocks_(false),
	showStatus_(true),
	showColor_(false)
{
	font_.setFamily(QString::fromUtf8("\347\255\211\347\272\277"));
	font_.setPointSize(12);
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
			//return QAbstractItemModel::data(index, role);
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
			return QString(chip.locked ? u8"√" : "");
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
			return QColor::fromRgb(ChipView::index2color(0));
		}
		else
		{
			return QColor(Qt::white);
		}
	case Qt::FontRole:
		return font_;
	case Qt::ForegroundRole:
		if(index.column() == 0 && showColor_)
		{
			return QColor::fromRgb(ChipView::index2color(index.row() + 1));
		}
		return QVariant();
	default:
		return QVariant();
	}
}

bool ChipTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	return false;
}

void ChipTableModel::setChips(const Chips& chips)
{
	chips_ = chips;
	refresh();
}

const Chips& ChipTableModel::chips() const
{
	return chips_;
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

void ChipTableModel::setShowColor(bool b)
{
	showColor_ = b;
	refresh();
}

ChipTableDelegate::ChipTableDelegate(QObject* parent):
	QItemDelegate(parent)
{
	
}

void ChipTableDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if(index.column() != 1)
	{
		QItemDelegate::paint(painter, option, index);
		return;
	}
	auto model = dynamic_cast<const ChipTableModel*>(index.model());
	const auto& icon = model->chips()[index.row()].icon();
	auto width = icon.width();
	auto height = icon.height();
	const auto& rect = option.rect;
	auto x = rect.x() + rect.width() / 2 - width / 2;
	auto y = rect.y() + rect.height() / 2 - height / 2;

	painter->drawPixmap(x, y, icon);
}

Qt::ItemFlags ChipTableModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;
	return QAbstractItemModel::flags(index) & ~Qt::ItemIsEditable;
}

QVariant ChipTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role == Qt::FontRole)
	{
		return font_;
	}
	if (orientation == Qt::Vertical || role != Qt::DisplayRole)
		return QAbstractItemModel::headerData(section, orientation, role);
	switch (section)
	{
	case 0:
		return trUtf8(u8"编\n号");
	case 1:
		return trUtf8(u8"形\n状");
	case 2:
		return trUtf8(u8"名\n称");
	case 3:
		return trUtf8(u8"强\n化");
	case 4:
		return trUtf8(u8"精\n度");
	case 5:
		return trUtf8(u8"装\n填");
	case 6:
		return trUtf8(u8"伤\n害");
	case 7:
		return trUtf8(u8"破\n防");
	case 8:
		return trUtf8(u8"锁\n定");
	case 9:
		return trUtf8(u8"装\n备");
	default:
		return QAbstractItemModel::headerData(section, orientation, role);
	}
}
