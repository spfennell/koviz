#ifndef PLOTTITLEVIEW_H
#define PLOTTITLEVIEW_H

#include <QLabel>
#include <QVBoxLayout>
#include <QFontMetrics>
#include <QFontInfo>
#include <QHash>
#include "bookidxview.h"
#include "layoutitem_plottitle.h"

class PlotTitleView : public BookIdxView
{
    Q_OBJECT
public:
    explicit PlotTitleView(QWidget *parent = 0);
    virtual void setModel(QAbstractItemModel *model);

protected:
    virtual QSize minimumSizeHint() const;
    virtual QSize sizeHint() const;
    virtual void paintEvent(QPaintEvent * event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);

private:
    QLabel* _label;
    QVBoxLayout* _vBoxLayout;
    QPoint _mousePressPos;

    Qt::MouseButton _buttonSelectAndPan;
    Qt::MouseButton _buttonRubberBandZoom;
    Qt::MouseButton _buttonResetView;

protected slots:
    virtual void dataChanged(const QModelIndex &topLeft,
                             const QModelIndex &bottomRight);
    virtual void rowsInserted(const QModelIndex &parent, int start, int end);

};

#endif // PLOTTITLEVIEW_H
