#ifndef BIOIMAGEMANAGER_H
#define BIOIMAGEMANAGER_H

#include <QObject>
#include "BioImage.h"
#include <QQmlEngine>
#include <QJSEngine>
#include <QHash>
#include "ColorTable.h"

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

    Q_INVOKABLE void setImage(const QString& id, zeroth::BioImage* img);
    Q_INVOKABLE zeroth::BioImage* image();
    Q_INVOKABLE float intensity(qreal x, qreal y);

    Q_INVOKABLE void createImage(const QString &path);
    Q_INVOKABLE QStringList availableColorMaps();
    QVector<QRgb> colorTable(const QString& name);
    Q_INVOKABLE void saveImage(const QString& path);
    Q_INVOKABLE bool hasImage();

    Q_INVOKABLE double imgMin();
    Q_INVOKABLE double imgMax();
    Q_INVOKABLE QList<qreal> hist();
    Q_INVOKABLE QList<int> bins();
    Q_INVOKABLE int range();

    Q_INVOKABLE void exeAnisotropy(const QString& parallelBackGround, const QString& perpendicularBackground,
                                   const QString& parallel,const QString& perpendicular, double subtractVal);

signals:
    void imageLoaded(const QString& id);
    void imageUpdated();

private:
    BioImageManager(QObject *parent = nullptr);

private:
    QHash<QString, zeroth::BioImage*> m_img;
    ColorTable* cTable;

};

#endif // BIOIMAGEMANAGER_H
