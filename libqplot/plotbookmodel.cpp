#include "plotbookmodel.h"

PlotBookModel::PlotBookModel(QObject *parent) :
    QStandardItemModel(parent)
{
}

PlotBookModel::PlotBookModel(int rows, int columns, QObject *parent) :
    QStandardItemModel(rows,columns,parent)
{
}

QModelIndexList PlotBookModel::plotIdxs(const QModelIndex &pageIdx) const
{
    QModelIndexList idxs;
    int rc = rowCount(pageIdx);
    // Start at 3 because page title=0, startTime=1, stopTime=2
    for ( int i = 3 ; i < rc; ++i ) {
        idxs.append(index(i,0,pageIdx));
    }

    return idxs;
}
