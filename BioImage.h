#ifndef BIO2DIMAGE_H
#define BIO2DIMAGE_H

#include <QObject>
#include <QMetaType>


namespace zeroth {

typedef enum CurrentType {
    ui8,
    ui16,
    ui32,
    i8,
    i16,
    i32,
    ft
} CurrentType;

class Histogram: public QObject {
    Q_OBJECT
public:
    double begin;
    double end;
    QList<qreal> hist;
    QList<int> bins;
    int range;
};

struct BioImageData;
#ifdef ZEROTH_USE_QT
class BioImage : public QObject
#else
class BioImage
#endif
{

    #ifdef ZEROTH_USE_QT
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource)
    Q_PROPERTY(uint32_t width READ width WRITE setWidth)
    Q_PROPERTY(uint32_t height READ height WRITE setHeight)
    Q_PROPERTY(uint32_t depth READ depth WRITE setDepth)
    Q_PROPERTY(bool hasNext READ hasNext)
    #endif
public:
    BioImage();
    ~BioImage();

//    BioImage(const BioImage&) = default;
//    BioImage& operator=(const BioImage &) = default;
//    BioImage(BioImage &&) = default;
//    BioImage & operator=(BioImage &&) = default;


    #ifdef ZEROTH_USE_QT
    void setSource(const QString& path);
    QString source() const;
    #else
    void setSource(const std::string& path);
    std::string source() const;
    #endif

    void *data();
    void setData(void* buffer);

    uint32_t width() const;
    void setWidth(uint32_t val);

    uint32_t height() const;
    void setHeight(uint32_t val);

    uint32_t depth() const;
    void setDepth(uint32_t val);

    uint16_t bitDepth() const;
    void setBitDepth(uint16_t val);

    uint16_t bitFormat() const;
    void setBitFormat(uint16_t val);

    double min() const;
    double max() const;

    Q_INVOKABLE Histogram* histogram();

    uint8_t *displayImage(double thresholdMin = 0, double thresholdMax = 0, bool threshold=false);
    float intensityAt(uint32_t x, uint32_t y) const;

    CurrentType dtype();

    void update();

    bool hasNext();
    #ifdef ZEROTH_USE_QT
    Q_INVOKABLE void next();
    #else
    void next();
    #endif

    void save(const std::string& path);

private:
    BioImageData *d;
};

}

//Q_DECLARE_METATYPE(zeroth::BioImage);
#endif // BIO2DIMAGE_H
