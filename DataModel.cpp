#include "DataModel.h"
#include <QDebug>
#include <QMap>
#include <algorithm>

struct DataModelData {
    QMap<float, float> rawDataMap;
    QList<QPair<float,float>>rawData;

    zeroth::BioImage *imageR;
    zeroth::BioImage *imageT;
    int binsize = 500;
    uint32_t imageSize =0;

    QList<QPair<QString, float>> displayData;
    QVector<float> displayDataVals;

    void loadRawData() {

        imageSize = imageR->width() * imageR->height();

        for (uint32_t i = 0; i < imageSize; i++) {
            rawDataMap.insertMulti(imageT->intensityAt(i), imageR->intensityAt(i));
        }
        QMapIterator<float, float> it(rawDataMap);
        while (it.hasNext()) {
            it.next();
            QPair<float, float> pair(it.key(), it.value());
            rawData.append(pair);
        }
        qDebug() << "Size of raw data" << rawData.size();
        createDisplayData();
    }

    void createDisplayData() {
        if(!imageSize) return;
        displayData.clear();

        //        uint32_t totalNumberOfBins = imageSize/binsize;
        //        for(uint32_t bin = 0; bin < totalNumberOfBins; bin++) {
        //            QString binName = QString("%1-%2").arg(QString::number(bin)).arg(QString::number(bin*binsize));
        //            float binValue = 0;
        //            for(int index = bin*binsize; index < (bin*binsize) +binsize; index++) {
        //                if(index < rawData.size()) {
        //                    binValue +=rawData.value(index).second;
        //                }
        //            }
        //            QPair<QString, float> dpair(binName, binValue);
        //            displayData.append(dpair);
        //        }

        for(int index =0; index < rawData.size(); index++) {
            int startIndex = index;
            float binVal = 0;
            int binValCount = 0;
            for(int binIndex = index; binIndex < startIndex+binsize; binIndex++) {
                if(binIndex < rawData.size()) {
                    binVal +=rawData.value(binIndex).second;
                    binValCount++;
                } else{
                    break;
                }
            }
            QString binName = QString("%1-%2").arg(QString::number(startIndex)).arg(QString::number(startIndex+binValCount));
            binVal = binVal / binValCount;
            QPair<QString, float> dpair(binName, binVal);
            displayDataVals.append(binVal);
            displayData.append(dpair);
            index = (startIndex + binValCount) -1;
        }
    }
};

DataModel::DataModel(QObject *parent )
    :QAbstractTableModel(parent), d(new DataModelData)
{

}

int DataModel::rowCount(const QModelIndex &) const
{
    return d->displayData.size();
}

int DataModel::columnCount(const QModelIndex &) const
{
    return 3;
}

QVariant DataModel::data(const QModelIndex &index, int role) const
{
    if(role == I || (role  == Qt::DisplayRole && index.column() == 0)){
        return index.row();
    }
    else if(role == X || (role  == Qt::DisplayRole && index.column() == 1)){
        return d->displayData[index.row()].first;
    } else {
        // Y
        return d->displayData[index.row()].second;
    }

    return QVariant();
}

QHash<int, QByteArray> DataModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "Display";
    roles[I] = "I";
    roles[X] = "X";
    roles[Y] = "Y";
    return roles;
}

void DataModel::setRawData(zeroth::BioImage *imageR, zeroth::BioImage *imageT)
{
    d->imageR = imageR;
    d->imageT = imageT;
    beginResetModel();
    d->loadRawData();
    endResetModel();
    emit modelUpdated();
}

void DataModel::setBinSize(int val)
{
    if(val > d->imageSize)
        val = d->imageSize;
    d->binsize = val;
    beginResetModel();
    d->createDisplayData();
    endResetModel();
    emit modelUpdated();
}

int DataModel::binSize()
{
    return d->binsize;
}

int DataModel::totatlNumberOfBins()
{
    return d->imageSize / d->binsize;
}

float DataModel::yMin()
{
    float m = *std::min_element(d->displayDataVals.constBegin(), d->displayDataVals.constEnd());
    return m;
}

float DataModel::yMax()
{
    float m = *std::max_element(d->displayDataVals.constBegin(), d->displayDataVals.constEnd());
    return m;
}
