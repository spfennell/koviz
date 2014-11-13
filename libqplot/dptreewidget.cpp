#include "dptreewidget.h"

DPTreeWidget::DPTreeWidget(const QString &dirName,
                           MonteModel* monteModel,
                           QStandardItemModel *plotModel,
                           QItemSelectionModel *plotSelectModel,
                           QWidget *parent) :
    QWidget(parent),
    _dirName(dirName),
    _monteModel(monteModel),
    _plotModel(plotModel),
    _plotSelectModel(plotSelectModel),
    _gridLayout(0),
    _searchBox(0)
{
    _setupModel();

    _gridLayout = new QGridLayout(parent);

    _searchBox = new QLineEdit(parent);
    connect(_searchBox,SIGNAL(textChanged(QString)),
            this,SLOT(_searchBoxTextChanged(QString)));
    _gridLayout->addWidget(_searchBox,0,0);


    _dpTreeView = new QTreeView(parent);
    _dpTreeView->setModel(_dpFilterModel);
    QModelIndex proxyRootIdx = _dpFilterModel->mapFromSource(_dpModelRootIdx);
    _dpTreeView->setRootIndex(proxyRootIdx);
    _dpTreeView->setFocusPolicy(Qt::ClickFocus);
    _gridLayout->addWidget(_dpTreeView,1,0);
    connect(_dpTreeView,SIGNAL(clicked(QModelIndex)),
            this, SLOT(_dpTreeViewClicked(QModelIndex)));
    connect(_dpTreeView->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(_dpTreeViewCurrentChanged(QModelIndex,QModelIndex)));

    // This doesn't work :( Can't hide timestamp column
    for ( int col = 1; col < _dpModel->columnCount(); ++col) {
        _dpTreeView->hideColumn(col);
    }
}

DPTreeWidget::~DPTreeWidget()
{
    delete _dpModel;
}

//
// DP File/Sys Model and Filter Proxy Model
//
void DPTreeWidget::_setupModel()
{
    _dir = new QDir(_dirName);
    _dir->cdUp();
    _dpModel = new QFileSystemModel;
    _dpModelRootIdx = _dpModel->setRootPath(_dir->path());
    QStringList filters;
    //filters  << "DP_*" << "SET_*"; // _dpFilterModel does additional filtering
    _dpModel->setNameFilters(filters);
    _dpModel->setNameFilterDisables(false);
    _dpModel->setFilter(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot);
    _dpFilterModel = new DPFilterProxyModel(_monteModel);
    _dpFilterModel->setDynamicSortFilter(true);
    _dpFilterModel->setSourceModel(_dpModel);
    QRegExp dprx(QString(".*"));  // DP_ and SET_ are filtered by _dpModel
    _dpFilterModel->setFilterRegExp(dprx);
    _dpFilterModel->setFilterKeyColumn(0);
}

void DPTreeWidget::_searchBoxTextChanged(const QString &rx)
{
    _dpTreeView->expandAll();
    _dpFilterModel->setFilterRegExp(rx);
}

void DPTreeWidget::_dpTreeViewClicked(const QModelIndex &idx)
{
    Q_UNUSED(idx);

    QModelIndexList idxs =  _dpTreeView->selectionModel()->selectedRows();
    foreach ( QModelIndex idx, idxs ) {
        QModelIndex srcIdx = _dpFilterModel->mapToSource(idx);
        QString fp = _dpModel->filePath(srcIdx);
        if ( _isDP(fp) ) {
            bool isCreated = false;
            for ( int row = 0; row < _plotModel->rowCount(); ++row) {
                QModelIndex pageIdx = _plotModel->index(row,0);
                QString pageName = _plotModel->data(pageIdx).toString();
                if ( pageName == fp ) {
                    _plotSelectModel->setCurrentIndex(pageIdx,
                                                  QItemSelectionModel::Select);
                    isCreated = true;
                    break;
                }
            }
            if ( !isCreated ) {
                _createDPPages(fp);
            }
        }
    }
}

void DPTreeWidget::_dpTreeViewCurrentChanged(const QModelIndex &currIdx,
                                             const QModelIndex &prevIdx)
{
    Q_UNUSED(prevIdx);
    _dpTreeViewClicked(currIdx);
}

//
// The PlotBookView's model is _plotModel.
// The _plotModel tree has to mesh (hard-coded basically) with
// PlotBookView::rowInserted()'s tree.
//
// For instance, in PlotBookView::rowInserted() there is a line:
//        } else if ( ! g2pidx.isValid() ) {
//            if ( idx.row() == 0 ) {
//            // X axis label
//
// which corresponds to _createDPPages (note it's the 0th row below plot):
//
//        plotItem->appendRow(xAxisLabelItem);
//
void DPTreeWidget::_createDPPages(const QString& dpfile)
{
    QStandardItem *rootItem = _plotModel->invisibleRootItem();
    QCursor currCursor = this->cursor();
    this->setCursor(QCursor(Qt::WaitCursor));

    DPProduct dp(dpfile);
    int numRuns = _monteModel->rowCount();
    int pageNum = 0 ;
    foreach (DPPage* page, dp.pages() ) {
        QString pageTitle = dpfile;
        if ( pageNum > 0 ) {
            pageTitle += QString("_%0").arg(pageNum);
        }
        QStandardItem *pageItem = new QStandardItem(pageTitle);
        rootItem->appendRow(pageItem);
        foreach (DPPlot* plot, page->plots() ) {

            QString plotTitle = _descrPlotTitle(plot);
            QStandardItem *plotItem = new QStandardItem(plotTitle);
            pageItem->appendRow(plotItem);

            QString xAxisLabel = plot->xAxisLabel();
            QStandardItem *xAxisLabelItem = new QStandardItem(xAxisLabel);
            plotItem->appendRow(xAxisLabelItem);

            QString yAxisLabel = plot->yAxisLabel();
            QStandardItem *yAxisLabelItem = new QStandardItem(yAxisLabel);
            plotItem->appendRow(yAxisLabelItem);

            QStandardItem *curvesItem = new QStandardItem("Curves");
            plotItem->appendRow(curvesItem);

            QString title = plot->title();
            QStandardItem *titleItem = new QStandardItem(title);
            plotItem->appendRow(titleItem);

            int curveId = -1;
            foreach (DPCurve* dpcurve, plot->curves() ) {
                ++curveId;
                for ( int run = 0; run < numRuns; ++run) {
                    QString curveTitle = QString("Curve_%0_%1").
                                                   arg(curveId).arg(run);
                    QStandardItem *curveItem = new QStandardItem(curveTitle);
                    curvesItem->appendRow(curveItem);

                    QString tName = dpcurve->t()->name();
                    QString xName = dpcurve->x()->name();
                    QString yName = dpcurve->y()->name();
                    QString tUnit = dpcurve->t()->unit();
                    QString xUnit = dpcurve->x()->unit();
                    QString yUnit = dpcurve->y()->unit();

                    QStandardItem *tItem       = new QStandardItem(tName);
                    QStandardItem *xItem       = new QStandardItem(xName);
                    QStandardItem *yItem       = new QStandardItem(yName);
                    QStandardItem *tUnitItem   = new QStandardItem(tUnit);
                    QStandardItem *xUnitItem   = new QStandardItem(xUnit);
                    QStandardItem *yUnitItem   = new QStandardItem(yUnit);
                    QStandardItem *runIDItem   = new QStandardItem(
                                                     QString("%0").arg(run));

                    curveItem->appendRow(tItem);
                    curveItem->appendRow(xItem);
                    curveItem->appendRow(yItem);
                    curveItem->appendRow(tUnitItem);
                    curveItem->appendRow(xUnitItem);
                    curveItem->appendRow(yUnitItem);
                    curveItem->appendRow(runIDItem);
                }
            }
            // TODO: does this need to be here???
            //_selectCurrentRunOnPageItem(pageItem);
        }
        pageNum++;
    }

    this->setCursor(currCursor);
}

bool DPTreeWidget::_isDP(const QString &fp)
{
    bool ret = false ;
    QFileInfo fi(fp);
    if ( (fi.baseName().left(3) == "DP_" && fi.suffix() == "xml" ) ) {
        ret = true;
    } else if ( fi.baseName().left(3) == "DP_" &&
                fi.suffix().isEmpty() && fi.isFile()) {
        ret = true;
    }
    return ret;
}

QString DPTreeWidget::_descrPlotTitle(DPPlot *plot)
{
    QString plotTitle = "Plot_";
    if ( plot->title() != "Plot" )  {
        plotTitle += plot->title();
    } else {
        QStringList vars;
        foreach ( DPCurve* curve, plot->curves() ) {
            vars.append(curve->y()->name());
        }
        QString var0 = vars.at(0);
        int dotCnt = 0 ;
        QString sub;
        for ( int i = 1 ; i < var0.size(); ++i) {
            sub = var0.right(i);
            if ( sub.at(0) == '.' ) {
                dotCnt++;
            }
            bool is = true;
            foreach ( QString var, vars ) {
                if ( ! var.endsWith(sub) ) {
                    is = false;
                    break;
                }
            }
            if ( ! is || dotCnt == 2 ) {
                break;
            }
        }
        if ( dotCnt == 2 ) {
            sub.remove(0,1);
        }
        plotTitle += sub;
    }

    return plotTitle;
}
