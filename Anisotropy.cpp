#include "Anisotropy.h"
#include "TiffReader.h"
#include <functional>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <array>
#include <QUrl>

typedef float operatorFn(float, float);

float add(float a, float b) {
    return  a+b;
}

float subtract(float a, float b) {
    return  a - b;
}

float multiply(float a, float b) {
    return  a * b;
}

float divide(float a, float b) {
    return  a / b;
}

/*double convert(double number, double min, double max) {

//        rmin denote the minimum of the range of your measurement
//        rmax denote the maximum of the range of your measurement
//        tmin denote the minimum of the range of your desired target scaling
//        tmax denote the maximum of the range of your desired target scaling
//        m ∈ [rmin,rmax] denote your measurement to be scaled

//        Then m = ((m− rmin)/ (rmax−rmin))×(tmax−tmin)+tmin

    return ( (number-min)/(max-min) ) * 255.0;
}*/

struct AnisotropyData {
    zeroth::BioImage* imgR;
    zeroth::BioImage* imgT;

    QString paraBg;
    QString perpBg;
    QString paraImg;
    QString perpImg;
    double subtractVal =0;

    float* imageTofloat(zeroth::BioImage* im) {
        void* data = im->data();
        uint32_t size = im->width()* im->height();

        if(im->bitDepth() == 8){
            float *result = new float[size];
            uint8_t* buff = (uint8_t*)data;
            for(uint32_t i = 0; i < size; i++) {
                result[i] = (float)buff[i];
            }
            return  result;
        } else if (im->bitDepth() == 16) {
            float *result = new float[size];
            uint16_t* buff = (uint16_t*)data;
            for(uint32_t i = 0; i < size; i++) {
                result[i] = (float)buff[i];
            }
            return result;
        } else if (im->bitDepth() == 32) {
            float *result = new float[size];
            uint32_t* buff = (uint32_t*)data;
            for(uint32_t i = 0; i < size; i++) {
                result[i] = (float)buff[i];
            }
            return result;
        }
        return nullptr;
    }

    float* calculate(float* x, float* y, uint32_t size, operatorFn fn) {
        float* r = new float[size];
        for(uint32_t i =0; i < size; i++) {
            r[i] = fn(x[i], y[i]);
        }

        return r;
    }

    float* calculate(float* x, float y, uint32_t size, operatorFn fn) {
        float* r = new float[size];
        for(uint32_t i =0; i < size; i++) {
            r[i] = fn(x[i], y);
        }

        return r;
    }

    bool checkArray(uint32_t *a, uint32_t size){
        for(uint32_t i =1; i < size; i++){
            if(a[0] != a[i]) return false;
        }
        return true;
    }

};

Anisotropy::Anisotropy(QObject *parent) : QObject(parent)
{
    d = new AnisotropyData;
}

QString Anisotropy::parallelBackground() const
{
    return d->paraBg;
}

void Anisotropy::setParallelBackground(const QString &val)
{
    QUrl url(val);
    if(url.isValid()) {
        d->paraBg = url.toLocalFile();
    } else {
        d->paraBg = val;
    }

}

QString Anisotropy::perpendicularBackground() const
{
    return d->perpBg;
}

void Anisotropy::setPerpendicularBackground(const QString &val)
{
    QUrl url(val);
    if(url.isValid()) {
        d->perpBg = url.toLocalFile();
    } else {
        d->perpBg = val;
    }
}

QString Anisotropy::parallel() const
{
    return d->paraImg;
}

void Anisotropy::setParallel(const QString &val)
{
    QUrl url(val);
    if(url.isValid()) {
        d->paraImg = url.toLocalFile();
    } else {
        d->paraImg = val;
    }
}

QString Anisotropy::perpendicular() const
{
    return d->perpImg;
}

void Anisotropy::setPerpendicular(const QString &val)
{
    QUrl url(val);
    if(url.isValid()) {
        d->perpImg = url.toLocalFile();
    } else {
        d->perpImg = val;
    }
}

double Anisotropy::subtractVal() const
{
    return d->subtractVal;
}

void Anisotropy::setSubtractVal(double val)
{
    d->subtractVal = val;
}

void Anisotropy::apply()
{

    zeroth::BioImage* parlBg = new zeroth::BioImage();
    parlBg->setSource(d->paraBg);

    zeroth::BioImage* perpBg = new zeroth::BioImage();
    perpBg->setSource(d->perpBg);


    zeroth::BioImage* parlImg = new zeroth::BioImage();
    parlImg->setSource(d->paraImg);

    zeroth::BioImage* perpImg = new zeroth::BioImage();
    perpImg->setSource(d->perpImg);

    uint32_t widthArray[4] = {parlBg->width() , perpBg->width() , parlImg->width() , perpImg->width()};
    uint32_t heightArray[4] = {parlBg->height() , perpBg->height() , parlImg->height() , perpImg->height()};

    if(!d->checkArray(widthArray, 4) || !d->checkArray(heightArray, 4)) {
        qErrnoWarning("All the images must be of same size");
        return;
    }

    uint32_t width = parlBg->width();
    uint32_t height = parlBg->height();
    uint32_t size = width * height;
    /*
    G = b_parl/b_perp

    parl = parl - 1800
    perp = perp - 1800

    perp = G*perp



    T = perp + (2 * perp)
    R = (parl - perp)/ T

    */

    float *perpB   = d->imageTofloat(perpBg);
    float *parlB   = d->imageTofloat(parlBg);

    float *perpIm  = d->imageTofloat(perpImg);
    float *parlIm  = d->imageTofloat(parlImg);
    // parl = parl - 1800
    // perp = perp - 1800
    perpIm = d->calculate(perpIm, d->subtractVal, size, subtract);
    parlIm = d->calculate(parlIm, d->subtractVal, size, subtract);

    // G = b_parl/b_perp
    float *g = d->calculate(parlB, perpB, size, divide);

    // perp = G*perp
    perpIm = d->calculate(g, perpIm, size, multiply);


    // T = perp + (2 * perp)

    // m2 = (2 * perp)
    float *m2 = d->calculate(perpIm, 2, size, multiply);

    // T = perp + m2
    float* T = d->calculate(perpIm, m2, size, add);

    // R = (parl - perp)/ T
    // sub = (parl - perp)
    float* sub = d->calculate(parlIm, perpIm, size, subtract);

    // R = sub /T
    float* R = d->calculate(sub, T, size, divide);

//    float min = *std::min_element(R, R+size);
//    float max = *std::max_element(R, R+size);

    d->imgR = new zeroth::BioImage;
    d->imgR->setData(R);
    d->imgR->setBitDepth(32);
    d->imgR->setWidth(width);
    d->imgR->setHeight(height);
    d->imgR->setDepth(1);
    d->imgR->setBitFormat(3);
    d->imgR->update();

    d->imgT = new zeroth::BioImage;
    d->imgT->setData(T);
    d->imgT->setBitDepth(32);
    d->imgT->setWidth(width);
    d->imgT->setHeight(height);
    d->imgT->setDepth(1);
    d->imgT->setBitFormat(3);
    d->imgT->update();
}

zeroth::BioImage *Anisotropy::getImageR()
{
    return d->imgR;
}

zeroth::BioImage *Anisotropy::getImageT()
{
    return d->imgT;
}

