#include "BioImageProvider.h"
#include "BioImage.h"
#include "BioImageManager.h"
#include <QDebug>

BioImageProvider::BioImageProvider()
    :QQuickImageProvider(QQuickImageProvider::Image)
{

}

QImage BioImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
//    if(id != "a")
//        return QImage();

    zeroth::BioImage* img = BioImageManager::instance().image();
    if(!img) {
        return QImage();
    }
    // find the parametors
    // Bi/img/a/cmap/Greys/min/0/max/0/t/0
    QStringList idSplit = id.split('/');

    QString colorMap = idSplit.contains("cmap") ?idSplit.at(idSplit.indexOf("cmap")+1): "Greys";
    double min = idSplit.contains("min") ? idSplit.at(idSplit.indexOf("min")+1).toDouble(): 0;
    double max = idSplit.contains("max") ? idSplit.at(idSplit.indexOf("max")+1).toDouble(): 0;
    bool threshoald = idSplit.contains("t") ?idSplit.at(idSplit.indexOf("t")+1).toInt(): 0;
    qDebug() << id;
    qDebug() << colorMap << min <<  max << threshoald;

    QVector<QRgb> qtColorTable = BioImageManager::instance().colorTable(colorMap);
    qDebug() << BioImageManager::instance().colorTable("Jet");

    QImage qImg(img->width(), img->height(), QImage::Format_Indexed8);

    memcpy(qImg.bits(), img->displayImage(min, max, threshoald), img->width()*img->height());
    qImg.setColorTable(qtColorTable);
    size->setWidth(img->width());
    size->setHeight(img->height());

    return qImg;
}
