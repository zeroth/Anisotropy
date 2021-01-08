#ifndef BIOIMAGEMANAGER_H
#define BIOIMAGEMANAGER_H

#include <QObject>
#include "BioImage.h"
#include <QQmlEngine>
#include <QJSEngine>
#include <QHash>
#include "ColorTable.h"
#include "DataModel.h"

class BioImageManager : public QObject
{
    Q_OBJECT
public:
    BioImageManager(const BioImageManager&) = delete;
    BioImageManager& operator=(const BioImageManager &) = delete;
    BioImageManager(BioImageManager &&) = delete;
    BioImageManager & operator=(BioImageManager &&) = delete;

    static BioImageManager& instance(){
        static BioImageManager test;
        return test;
    }

    Q_INVOKABLE void createImage(const QString &path);
    Q_INVOKABLE void saveImage(const QString& id, const QString& path);

    Q_INVOKABLE void setImage(const QString& id, zeroth::BioImage* img);
    void setImage(const QString& id, const QString& path);

    Q_INVOKABLE zeroth::BioImage* image(const QString& id);
    Q_INVOKABLE float intensity(const QString& id, qreal x, qreal y);
    Q_INVOKABLE double imgMin(const QString& id);
    Q_INVOKABLE double imgMax(const QString& id);
    Q_INVOKABLE QList<qreal> hist(const QString& id);
    Q_INVOKABLE QList<int> bins(const QString& id);
    Q_INVOKABLE int range(const QString& id);

    Q_INVOKABLE void exeAnisotropy(const QString& parallelBackGround, const QString& perpendicularBackground,
                                   const QString& parallel,const QString& perpendicular, double subtractVal);

    Q_INVOKABLE void saveAnisotropy(const QUrl& path, const QString& idR, const QString& idT);
    Q_INVOKABLE void openAnisotropy(const QUrl& path);

    Q_INVOKABLE QStringList availableColorMaps();
    QVector<QRgb> colorTable(const QString& name);
    Q_INVOKABLE bool hasImage();
    Q_INVOKABLE DataModel* mode() {return  m_model;}
    Q_INVOKABLE void setModelBinSize(int val) { m_model->setBinSize(val); }
    Q_INVOKABLE int modelBinSize() { return  m_model->binSize(); }
    Q_INVOKABLE float modelYMin() { return  m_model->yMin(); }
    Q_INVOKABLE float modelYMax() { return  m_model->yMax(); }
    Q_INVOKABLE int modelBinCount() { return  m_model->totatlNumberOfBins(); }


signals:
    void imageLoaded(const QString& id);
    void imageUpdated();
    void anisotropyReady(const QString& rId, const QString& tId);
    void dataModelUpdated();

private slots:
    void updateModel(const QString& rId, const QString& tId);

private:
    BioImageManager(QObject *parent = nullptr);

private:
    QHash<QString, zeroth::BioImage*> m_img;
    ColorTable* cTable;
    DataModel* m_model;

};

#endif // BIOIMAGEMANAGER_H
