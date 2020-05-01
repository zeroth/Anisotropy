#include "BioImageProvider.h"
#include "BioImage.h"
#include "BioImageManager.h"
#include <QDebug>

BioImageProvider::BioImageProvider()
    :QQuickImageProvider(QQuickImageProvider::Image)
{

}

QImage BioImageProvider::requestImage(const QString &id, QSize *size, const QSize &)
{
    // find the parametors

    // Bi/img/{imageid}/cmap/{colormapname}/min/{min}/max/{max}/t/{on/off}
    QStringList idSplit = id.split('/');
    QString imageId = idSplit.contains("img") ? idSplit.at(idSplit.indexOf("img")+1) : "";

    if(imageId == "") {
        return QImage();
    }
    zeroth::BioImage* img = BioImageManager::instance().image(imageId);
    if(!img) {
        return QImage();
    }

    QString colorMap = idSplit.contains("cmap") ?idSplit.at(idSplit.indexOf("cmap")+1): "Greys";
    double min = idSplit.contains("min") ? idSplit.at(idSplit.indexOf("min")+1).toDouble(): BioImageManager::instance().imgMin(imageId);
    double max = idSplit.contains("max") ? idSplit.at(idSplit.indexOf("max")+1).toDouble(): BioImageManager::instance().imgMax(imageId);
    bool threshoald = idSplit.contains("t") ?idSplit.at(idSplit.indexOf("t")+1).toInt(): 0;
    qDebug() << id;
    qDebug() << colorMap << min <<  max << threshoald;

    QVector<QRgb> qtColorTable = BioImageManager::instance().colorTable(colorMap);

    QImage qImg(img->width(), img->height(), QImage::Format_Indexed8);

    memcpy(qImg.bits(), img->displayImage(min, max, threshoald), img->width()*img->height());
    qImg.setColorTable(qtColorTable);
    size->setWidth(img->width());
    size->setHeight(img->height());

    return qImg;
}
