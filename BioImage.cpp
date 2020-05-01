#include "BioImage.h"
#include "TiffReader.h"
#include "TiffWriter.h"
#include <string>
#include <QDebug>
#ifdef ZEROTH_USE_QT
#include <QUrl>
#endif
#include <QMap>

namespace zeroth
{

struct HistKey {
    double id;
    bool operator<(const HistKey & o) const {
        return  (id < o.id);
    }
};

struct BioImageData {

    TiffReader* reader;
    void *buffer;

    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint16_t bitDepth;
    uint16_t bitFormat;
    uint32_t imageSize;
    CurrentType dtype;
    bool hasNext;
    double min;
    double max;

    std::string path;
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
    template<typename T>
    double getMin() {
        return *std::min_element((T*)buffer, (T*)buffer+imageSize);

    }

    template<typename T>
    double getMax() {
        return *std::max_element((T*)buffer, (T*)buffer+imageSize);
    }
    /*
    1 = unsigned integer data
    2 = two's complement signed integer data
    3 = IEEE floating point data
    4 = undefined data format
    */
    void update() {
        switch(bitFormat){
        case 1:
        case 4:{
            if(bitDepth == 8){
                // uint8_t
                dtype = CurrentType::ui8;
                min = getMin<uint8_t>();
                max = getMax<uint8_t>();
            } else if(bitDepth == 16) {
                // uint16_t
                dtype = CurrentType::ui16;
                min = getMin<uint16_t>();
                max = getMax<uint16_t>();
            } else if(bitDepth == 32) {
                // uint32_t
                dtype = CurrentType::ui32;
                min = getMin<uint32_t>();
                max = getMax<uint32_t>();
            } else {
                dtype = CurrentType::ui8;
                min = getMin<uint8_t>();
                max = getMax<uint8_t>();
            }
            break;
        }
        case 2: {
            if(bitDepth == 8){
                //int8_t;
                min = getMin<int8_t>();
                max = getMax<int8_t>();
                dtype = CurrentType::i8;
            } else if(bitDepth == 16) {
                // int16_t
                min = getMin<int16_t>();
                max = getMax<int16_t>();
                dtype = CurrentType::i16;
            } else if(bitDepth == 32) {
                // int32_t
                dtype = CurrentType::i32;
                min = getMin<int32_t>();
                max = getMax<int32_t>();
            } else {
                dtype = CurrentType::i8;
                min = getMin<int8_t>();
                max = getMax<int8_t>();
            }
            break;
        }
        case 3: {
            //float
            // TODO :: look for 16 bit float (halp float examples)
            // if you are in mood here is half float http://half.sourceforge.net/
            dtype = CurrentType::ft;
            min = getMin<float>();
            max = getMax<float>();
            break;

        }
        default:{
            dtype = CurrentType::ui8;
            min = getMin<uint8_t>();
            max = getMax<uint8_t>();
        }
        }
    }

    template<typename T>
    uint8_t* toUint8(T thresholdMin = 0, T thresholdMax = 0, bool threshold=false){
        uint8_t* display = new uint8_t[imageSize];
//        T min = *std::min_element((T*)buffer, (T*)buffer+imageSize);
//        T max = *std::max_element((T*)buffer, (T*)buffer+imageSize);

        // the requested thresholdMin & thresholdMax are not in the range of the actual
        // min and max then ignore thresholding

//        if(min < thresholdMin  || thresholdMin == 0) {
//            thresholdMin = min;
//        }
//        if( max > thresholdMax || thresholdMax == 0) {
//            thresholdMax = max;
//        }

        // y = m.x + b
        // m = 255/(max-min)
        // y =  m . x +b
        // therefor if y = 0
        // b = - (m . x )
        float displayMin = convert(thresholdMin, min, max);
        float displayMax = convert(thresholdMax, min, max);
        float m = 255/(displayMax-displayMin);
        float b = (m * displayMin) * -1.0;
//        float b =  displayMin;

        #pragma omp parallel
        {
            #pragma omp loop
            for(uint32_t i =0; i < imageSize; i++) {
                T current = ((T*)buffer)[i];
                if( current <= thresholdMin) {
                    current = thresholdMin;
                } else if(current >= thresholdMax) {
                    current = thresholdMax;
                }
                double cd = convert(current, thresholdMin, thresholdMax);
                // y = m  x +b
                // b = convert(thresholdMin, min, max)
                // newMin = convert(thresholdMin, min, max)
                // newaMax = convert(thresholdMax, min, max)
                // m = 255/(newMax-newMin)
                // cd = m * cd +b
                float displayIntensity = (m * cd) + b;

                if(displayIntensity > 255) {
                    displayIntensity = 255;
                } else if(displayIntensity < 0) {
                    displayIntensity = 0;
                } else {
                    displayIntensity = qRound(displayIntensity);
                }

                display[i] = qRound(displayIntensity);


            }
        }
        return display;
    }

    uint8_t* createDisplayImage(double thresholdMin = 0, double thresholdMax = 0, bool threshold=false) {
        switch (dtype) {
        case ui8 : {
            return toUint8<uint8_t>(thresholdMin,thresholdMax,threshold);
        }
        case ui16: {
            return toUint8<uint16_t>(thresholdMin,thresholdMax,threshold);
        }
        case ui32:{
            return toUint8<uint32_t>(thresholdMin,thresholdMax,threshold);
        }
        case i8: {
            return toUint8<int8_t>(thresholdMin,thresholdMax,threshold);
        }
        case i16:{
            return toUint8<int16_t>(thresholdMin,thresholdMax,threshold);
        }
        case i32: {
            return toUint8<int32_t>(thresholdMin,thresholdMax,threshold);
        }
        case ft:{
            //float
            // TODO :: look for 16 bit float (halp float examples)
            return toUint8<float>(thresholdMin,thresholdMax,threshold);
        }
        default: {
            return toUint8<uint8_t>(thresholdMin,thresholdMax,threshold);
        }

        }
    }

    template<typename T>
    void copy(T *data) {
        (T*)buffer = new T[imageSize];

        #pragma omp parallel
        {
        #pragma omp loop
            for(uint32_t i =0; i < imageSize; i++) {
                ((T*)buffer)[i] = data[i];
            }
        }
    }


    float getIntensityAt(uint32_t x, uint32_t y) {
        uint32_t index = y * width + x;
        if(index > imageSize)
            return 0;
        switch (dtype) {
        case ui8 : {
            return ((uint8_t*)(buffer))[index];
        }
        case ui16: {
            return ((uint16_t*)(buffer))[index];
        }
        case ui32:{
            return ((uint32_t*)(buffer))[index];
        }
        case i8: {
            return ((int8_t*)(buffer))[index];
        }
        case i16:{
            return ((int16_t*)(buffer))[index];
        }
        case i32: {
            return ((int32_t*)(buffer))[index];
        }
        case ft:{
            //float
            // TODO :: look for 16 bit float (halp float examples)
            return ((float*)(buffer))[index];
        }
        default: {
            return ((uint8_t*)(buffer))[index];
        }

        }
    }

    template<typename T>
    QMap<qreal, int> hist()
    {
        QMap<qreal, int> h;


            for(uint32_t i = 0; i < imageSize; i++) {
                T intensity = ((T*)(buffer))[i];


//                if(intensity > )
                if(h.contains(intensity)){
                    h[intensity] = h[intensity]+1;
                }
                else {
                    h[intensity] = 1;
                }
            }

//            qDebug() << h;
        return h;
    }
    Histogram* histogram() {
        Histogram* hg = new Histogram;
        hg->begin = min;
        hg->end = max;

//        QMap<double, uint32_t> histMap = hist();
        QMap<qreal, int> histMap;
        switch (dtype) {
        case ui8 : {
            histMap = hist<uint8_t>();
            break;
        }
        case ui16: {
            histMap = hist<uint16_t>();
            break;
        }
        case ui32:{
            histMap = hist<uint32_t>();
            break;
        }
        case i8: {
            histMap = hist<int8_t>();
            break;
        }
        case i16:{
            histMap = hist<int16_t>();
            break;
        }
        case i32: {
            histMap = hist<int32_t>();
            break;
        }
        case ft:{
            //float
            // TODO :: look for 16 bit float (halp float examples)
            histMap = hist<float>();
            break;
        }
        default: {
            histMap = hist<uint8_t>();
        }
        }

        hg->hist = histMap.keys();
        hg->bins = histMap.values();
        hg->range = histMap.size();

        return hg;
    }
};

BioImage::BioImage()
{
    d = new BioImageData;
    d->bitDepth = 8; // default it will be 8bit
    d->bitFormat = 1;
    d->buffer   = nullptr;
    d->width    = 0;
    d->height   = 0;
    d->imageSize = 0;
    d->depth    = 0;
    d->hasNext  = false;
    d->path = "";
    d->dtype = CurrentType::ui8;
}

BioImage::~BioImage()
{
    delete d;
}


#ifdef ZEROTH_USE_QT
void BioImage::setSource(const QString &path)
#else
//void setSource(const std::string& path);
void BioImage::setSource(const std::string &path)
#endif
{

#ifdef ZEROTH_USE_QT
    QUrl url(path);
    if(url.isValid() && url.scheme().compare("file", Qt::CaseInsensitive) == 0) {
        d->path = url.toLocalFile().toStdString();
    } else{
        d->path = path.toStdString();
    }
#else
    d->path = path;
#endif
    // TODO : copy data from all the slices
    d->reader = new TiffReader(d->path);
    d->buffer = d->reader->readImage();
    d->bitDepth = d->reader->bitsPerSample();
    d->bitFormat = d->reader->sampleFormat();

    d->width = d->reader->width();
    d->height = d->reader->height();
    d->imageSize = d->width* d->height;
    d->depth = d->reader->sliceCount();

    this->update();
}

#ifdef ZEROTH_USE_QT

QString BioImage::source() const
{
    return QString(d->path.c_str());
}

#else
std::string BioImage::source() const
{
    return  d->path;
}
#endif

void *BioImage::data()
{
    return d->buffer;
}

void BioImage::setData(void *buffer)
{
    d->buffer = buffer;
}

uint32_t BioImage::width() const
{
    return d->width;
}

void BioImage::setWidth(uint32_t val)
{
    d->width = val;
    if(d->width >0 && d->height >0)
        d->imageSize = d->width* d->height;
    else
        d->imageSize = d->width;
}

uint32_t BioImage::height() const
{
    return d->height;
}

void BioImage::setHeight(uint32_t val)
{
    d->height = val;
    if(d->width >0 && d->height >0)
        d->imageSize = d->width* d->height;
    else
        d->imageSize = 0;
}

uint32_t BioImage::depth() const
{
    return d->depth;
}

void BioImage::setDepth(uint32_t val)
{
    d->depth = val;
}

uint16_t BioImage::bitDepth() const
{
    return d->bitDepth;
}

void BioImage::setBitDepth(uint16_t val)
{
    d->bitDepth = val;
}

uint16_t BioImage::bitFormat() const
{
    return d->bitFormat;
}

void BioImage::setBitFormat(uint16_t val)
{
    d->bitFormat = val;
}

double BioImage::min() const
{
    return d->min;
}

double BioImage::max() const
{
    return d->max;
}

Histogram *BioImage::histogram()
{
    return d->histogram();
}

uint8_t *BioImage::displayImage(double thresholdMin , double thresholdMax , bool threshold)
{
    return d->createDisplayImage(thresholdMin,thresholdMax , threshold);
}

float BioImage::intensityAt(uint32_t x, uint32_t y) const
{
    return d->getIntensityAt(x, y);
}

CurrentType BioImage::dtype()
{
    return d->dtype;
}

void BioImage::update()
{
    d->update();
}

bool BioImage::hasNext()
{
    d->hasNext = d->reader->hasNext();
    return d->hasNext;
}

void BioImage::next()
{
    if(this->hasNext()) {
        d->reader->next();
    }
}

void BioImage::save(const std::string &path)
{
    zeroth::TiffWriter* w = new zeroth::TiffWriter(path, d->bitDepth,d->bitFormat, d->width, d->height);
    w->addNextSlice(d->buffer);
    delete  w;
}

}
