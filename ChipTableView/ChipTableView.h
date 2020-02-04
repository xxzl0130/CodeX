#pragma once

#include "chiptableview_global.h"
#include <QAbstractItemModel>
#include "Chip/Chip.h"

class CHIPTABLEVIEW_EXPORT ChipTableModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	explicit ChipTableModel(QObject* parent = nullptr);

    QModelIndex index(int row, int column,
        const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& child) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    bool hasChildren(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const;
	
    void setChips(const QList<GFChip>& chips);
	// true显示格数 false显示数值（默认)
    void setShowBlocks(bool b = false);
	// true显示锁定与装备状态（默认），false不显示
    void setShowStatus(bool b = true);
    void refresh()
    {
        beginResetModel();
        endResetModel();
    }

private:
	
    QList<GFChip> chips_;
    bool showBlocks_;
    bool showStatus_;
};

inline Qt::ItemFlags ChipTableModel::flags(const QModelIndex& index) const
{
	if(!index.isValid())
        return Qt::NoItemFlags;
	return QAbstractItemModel::flags(index) & ~Qt::ItemIsEditable;
}
