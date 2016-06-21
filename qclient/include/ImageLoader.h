//
// Created by Nikita Somenkov on 15/06/16.
//

#ifndef SOCCER_SEGMENTATION_IMAGELOADER_H
#define SOCCER_SEGMENTATION_IMAGELOADER_H

#include "SegInfo.h"

#include <QImage>
#include <QTcpSocket>

class ImageLoader  {
public:
    enum Status {
        NETWORK_ERROR,
        SUCCESS,
        DONE
    };

    Status load(const QImage &image) const;
    Status get(QImage &image) const;

    void setPort(quint16 port);
    void setHost(const QString &host);

    bool ok() const;
    bool connect() const;
    
private:
    bool doRequest(const QByteArray &request, QByteArray &response) const;

    QString mHost = "localhost";
    quint16 mPort = 31415;
    mutable qint32 mId = -1;
    mutable QTcpSocket mTcpSocket;
    QImage mImage;

    static int readID(const QByteArray &bytes);
    static QImage readImage(QByteArray &image);
    static char* number(int &num);
};


#endif //SOCCER_SEGMENTATION_IMAGELOADER_H
