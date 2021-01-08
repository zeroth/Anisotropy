#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <QAbstractTableModel>
#include "BioImage.h"

struct DataModelData;

class DataModel: public QAbstractTableModel
{
    Q_OBJECT
public:
    enum ColRoles {
        I = Qt::UserRole + 1,
        X,
        Y
    };

    DataModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;

    void setRawData(zeroth::BioImage* imageR, zeroth::BioImage* imageT);
    void setBinSize(int val);
    int binSize();
    int totatlNumberOfBins();
    float yMin();
    float yMax();

signals:
    void modelUpdated();


private:
    DataModelData *d;
};

#endif // DATAMODEL_H
