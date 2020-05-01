#ifndef BIOIMAGEVIEWER_H
#define BIOIMAGEVIEWER_H
#include <QQuickImageProvider>

class BioImageProvider : public QQuickImageProvider
{
public:
    BioImageProvider();

    // QQuickImageProvider interface
public:
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
};

#endif // BIOIMAGEVIEWER_H
