#pragma once

#include <QAbstractItemModel>
#include "chiptableview_global.h"
#include "Chip/Chip.h"
#include <QFont>

class CHIPTABLEVIEW_EXPORT SolutionTableModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	SolutionTableModel(QObject *parent);
	~SolutionTableModel();

    QModelIndex index(int row, int column,
        const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& child) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    bool hasChildren(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const;

    QVariant headerData(int section, Qt::Orientation orientation,
        int role = Qt::DisplayRole) const override;

    void setSolution(std::vector<Solution> const* ptr);
    void setMaxValue(const GFChip& value);
	
    void refresh()
    {
        beginResetModel();
        endResetModel();
    }

private:
    std::vector<Solution> const* solution_;
    GFChip maxValue_;
    QFont font_;
};
