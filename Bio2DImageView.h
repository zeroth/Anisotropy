#ifndef BIO2DIMAGEVIEW_H
#define BIO2DIMAGEVIEW_H

#include <QtQuick/QQuickPaintedItem>
#include <inttypes.h>
#include "BioImage.h"

struct Bio2DImageViewData;

class Bio2DImageView : public QQuickPaintedItem
{
    Q_OBJECT
//    Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource)
    Q_PROPERTY(int min READ min WRITE setMin)
    Q_PROPERTY(int max READ max WRITE setMax)
//    Q_PROPERTY(QImage displayData READ displayData WRITE setDisplayData NOTIFY displayUpdated)

public:
    Bio2DImageView(QQuickItem* parent = 0);

    /*QString dataSource() const;
    void setDataSource(const QString& path);*/

    Q_INVOKABLE void setImage(zeroth::BioImage* image);
//    Q_INVOKABLE zeroth::BioImage* image();
    Q_INVOKABLE void saveImage(const QString& path);

    int min() const;
    void setMin(int val);

    uint32_t max() const;
    void setMax(uint32_t val);

    void paint(QPainter *painter) override;
    virtual QRectF boundingRect() const;

    Q_INVOKABLE float pixelValue(qreal x, qreal y);

public slots:
    void updateDisplay();

signals:
    void displayUpdated();

private:
    Bio2DImageViewData* d;

};

#endif // BIO2DIMAGEVIEW_H
