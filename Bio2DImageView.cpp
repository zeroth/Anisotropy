#include "Bio2DImageView.h"
#include "TiffReader.h"

#include <QPainter>
#include <QFileInfo>
#include <QUrl>
#include <QFileSelector>
#include <QQmlFileSelector>
#include <QQmlEngine>
#include <QQmlFile>
#include <QtMath>
#include<QtConcurrent/QtConcurrent>
//#include <omp.h>
#include <QDebug>
#include <iostream>
#include <QtWidgets/QWidget>

//using namespace zeroth;
double convert(double number, double min, double max) {
    /*
        rmin denote the minimum of the range of your measurement
        rmax denote the maximum of the range of your measurement
        tmin denote the minimum of the range of your desired target scaling
        tmax denote the maximum of the range of your desired target scaling
        m ∈ [rmin,rmax] denote your measurement to be scaled

        Then m = ((m− rmin)/ (rmax−rmin))×(tmax−tmin)+tmin
    */
    return ( (number-min)/(max-min) ) * 255.0;
}

struct Bio2DImageViewData {
    QString path;
    int min;
    uint32_t max;
//    zeroth::TiffReader* tiffReader;
    void* imageBuffer;
    int width;
    int height;
    uint32_t availableMax;
    uint8_t *display;
    zeroth::BioImage* currentImage;

    uint32_t currentBitDepth;

    QImage getBlankImage(int width, int height) {
        QImage img(width, height, QImage::Format_Grayscale8);
        const uint16_t tableSize = 256;
        QVector<QRgb> qtColorTable(tableSize);

        for (int i = 0; i<tableSize; ++i) {

            qtColorTable[i] = qRgb(i, i, i);
        }
        img.setColorTable(qtColorTable);
        for(int x = 0; x < width; x++) {
            for(int y = 0; y < height; y++) {
                img.setPixel(x, y, qRgb(0,0,0));
            }
        }
        return img;
    }

    /*
     * if we need to resize the image
    const nearestNeighbor = (src, dst) => {
      let pos = 0;
      for (let y = 0; y < dst.height; y++) {
        for (let x = 0; x < dst.width; x++) {
          const srcX = Math.floor(x * src.width / dst.width);
          const srcY = Math.floor(y * src.height / dst.height);
          let srcPos = (srcY * src.width + srcX) * 4;
          dst.data[pos++] = src.data[srcPos++]; // R
          dst.data[pos++] = src.data[srcPos++]; // G
          dst.data[pos++] = src.data[srcPos++]; // B
          dst.data[pos++] = src.data[srcPos++]; // A
        }
      }
    };
    */

    template<typename T>
    void createDisplayImage() {
        int imageSize = width * height;
        display = new uint8_t[imageSize];
        max = qPow(2, sizeof (T)*8) -1;
        availableMax = qPow(2, sizeof (T)*8) -1;

        T min = *std::min_element((T*)imageBuffer, (T*)imageBuffer+imageSize);
        T max = *std::max_element((T*)imageBuffer, (T*)imageBuffer+imageSize);
        #pragma omp parallel
        {
            #pragma omp loop
            for(int i =0; i < imageSize; i++) {
                T in = convert(((T*)imageBuffer)[i], min, max);
                display[i] = in;
            }
        }
    }

};

Bio2DImageView::Bio2DImageView(QQuickItem* parent) :
    QQuickPaintedItem(parent)
{
    d = new Bio2DImageViewData;
    d->min = 0;
    d->max = 255;
//    this->setWidth(100);
//    this->setHeight(100);
//    connect(this, &Bio2DImageView::displayUpdated, this, &Bio2DImageView::displayUpdated);
    connect(this, &QQuickPaintedItem::scaleChanged, this, &Bio2DImageView::displayUpdated);
}

void Bio2DImageView::setImage(zeroth::BioImage *img)
{
    d->currentImage = img;
    d->width = img->width();
    d->height = img->height();
    d->imageBuffer = img->data();
    d->currentBitDepth = img->bitDepth();
    if(d->currentBitDepth == 8) {
//        d->createDisplayImage<uint8_t>();
        d->display = (uint8_t*)d->imageBuffer;
    } else if(d->currentBitDepth == 16 ) {
        d->createDisplayImage<uint16_t>();
    } else if(d->currentBitDepth == 32) {
        d->createDisplayImage<float>();
    }
    this->setWidth(d->width);
    this->setHeight(d->height);

    this->setImplicitWidth(d->width);
    this->setImplicitHeight(d->height);

    this->update();

}

void Bio2DImageView::saveImage(const QString &path)
{
    QString file;
    QUrl url(path);
    if(url.isValid()) {
        file = url.toLocalFile();
    } else {
        file = path;
    }
    d->currentImage->save(file.toStdString());

}

int Bio2DImageView::min() const
{
    return d->min;
}

void Bio2DImageView::setMin(int val)
{
    if(val <0) d->min = 0;
    else d->min = val;

}

uint32_t Bio2DImageView::max() const
{
    return d->max;
}

void Bio2DImageView::setMax(uint32_t val)
{
    if(val <0) d->max = 0;
    else d->max = val;
}

void Bio2DImageView::paint(QPainter *painter)
{
    painter->setRenderHints(QPainter::Antialiasing, true);
    qDebug() << this->boundingRect();

    /*qreal newWidth = this->width();
    double ratio = (double)d->width/(double)d->height;
    double h = newWidth/ratio;
    qreal newHeight = h;

    qDebug() << "Ratio : " << ratio << " new H " << h;
    qDebug() << "New Width : " << newWidth << " Old width " << d->width;
    qDebug() << "New Height : " << newHeight << " Old height " << d->height;

    bool useNewSize = false;
    if(d->width != newWidth || d->height != newHeight) {
        useNewSize = true;
    }

//  if(d->tiffReader->bitsPerSample() == 8) {
//      intensity = ((uint8_t*)d->imageBuffer)[index];
//  }
//  else if(d->tiffReader->bitsPerSample() == 16) {
//      intensity = ((uint16_t*)d->imageBuffer)[index];
//  }
//  else if(d->tiffReader->bitsPerSample() == 32) {
//      intensity = ((uint32_t*)d->imageBuffer)[index];
//  }

//  if(intensity < d->min) {
//      intensity = 0;
//  } else if(intensity > d->max) {
//      intensity = d->max;
//  }

*/
//    this->setScale(1.2);
    for(int y = 0; y< d->height; y++) {
        for(int x =0; x < d->width; x++) {
            uint32_t index = y * d->width + x;
            uint8_t intensity =d->display[index];
//            this->setScale();
            painter->setPen(QPen(QColor(intensity, intensity, intensity ), 1));
            painter->drawPoint(x, y);
        }
    }
//    updateDisplay();
}

QRectF Bio2DImageView::boundingRect() const
{

    return QRectF(0, 0, d->width, d->height);
}

float Bio2DImageView::pixelValue(qreal x, qreal y)
{
    uint32_t index = y * d->width + x;
    if(d->currentBitDepth == 8)
        return (float)((uint8_t*)(d->imageBuffer))[index];
    else if(d->currentBitDepth == 16)
        return (float)((uint16_t*)(d->imageBuffer))[index];
    else if (d->currentBitDepth == 32)
        return ((float*)(d->imageBuffer))[index];
    else
        return 0;
}

//QImage Bio2DImageView::displayData() const
//{
//    //QImage(const uchar *data, int width, int height, QImage::Format format, QImageCleanupFunction cleanupFunction = nullptr, void *cleanupInfo = nullptr)
//    return QImage(d->display, d->width, d->height, QImage::Format_Grayscale8);
//}

//void Bio2DImageView::setDisplayData(const QImage & data)
//{
//    d->display = (uint8_t*)data.bits();
//}

void Bio2DImageView::updateDisplay()
{
    qDebug() << "scalling";
    this->update();
}
