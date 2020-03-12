#ifndef TTVIDEOIMAGEPROVIDER_H
#define TTVIDEOIMAGEPROVIDER_H

#include <QQuickPaintedItem>
#include <QImage>

class QuickPaintedItem:public QQuickPaintedItem
{
    Q_OBJECT
public:
    QuickPaintedItem(QQuickItem *parent = nullptr);
};

#endif
