//
// Created by Nikita Somenkov on 15/06/16.
//

#include "ImageLoader.h"

ImageLoader::Status ImageLoader::load(const QImage &image) const
{
    if (!ok()) return NETWORK_ERROR;

    uchar * imageData = const_cast<uchar*>(image.bits());
    QByteArray request("ANS@"), response;
    request.append(number(mId), sizeof(mId));
	for (int i = 0; i < image.height(); i++)
		for (int j = 0; j < image.width(); j++)
		{
			QColor color = image.pixelColor(i, j);
			request.append(uchar(color.red()));
		}

    if(doRequest(request, response))
        return SUCCESS;

    return NETWORK_ERROR;
}

void ImageLoader::setPort(quint16 port) { mPort = port; }

void ImageLoader::setHost(const QString &host) { mHost = host; }

ImageLoader::Status ImageLoader::get(QImage &image) const
{
    if (!ok()) return NETWORK_ERROR;

    QByteArray request("GET@"), response;
    request.append(number(mId), sizeof(mId));

    if (!doRequest(request, response))
        return NETWORK_ERROR;

    const int bytesSize = 3 * sizeImage.height * sizeImage.width;
    const int gotBytesSize = response.size();
    if (gotBytesSize == bytesSize)
    {
        qDebug() << "got image";
        image = readImage(response).copy();
        return SUCCESS;
    }
    else if (gotBytesSize == 4 && response == "->-@")
    {
        qDebug() << "done";
        return DONE;
    }
    return NETWORK_ERROR;
}

bool ImageLoader::connect() const
{
    mId = -1;

    QByteArray request("GET@"), response;
    request.append(number(mId), sizeof(mId));

    if (!doRequest(request, response) ||
        response.size() != sizeof(int))
        return false;

    return (mId = readID(response)) != -1;
}

QImage ImageLoader::readImage(QByteArray &image)
{
    uchar *data = reinterpret_cast<uchar*>(image.data());
    return QImage(data, sizeImage.width, sizeImage.height,
                  QImage::Format::Format_RGB888);
}

int ImageLoader::readID(const QByteArray &bytes)
{
    QByteArray reverse = bytes;
    std::reverse(reverse.begin(), reverse.end());

    const int id = *reinterpret_cast<int*>(reverse.data());
    return id;
}

bool ImageLoader::doRequest(const QByteArray &request, QByteArray &response) const
{
    static const int timeout = 5000;
    mTcpSocket.connectToHost(mHost, mPort);

    if (mTcpSocket.waitForConnected(timeout))
    {
        mTcpSocket.write(request);
        if (mTcpSocket.waitForBytesWritten(timeout))
        {
            while(mTcpSocket.waitForReadyRead(timeout))
                response.append(mTcpSocket.readAll());

            mTcpSocket.disconnectFromHost();
            return true;
        }
    }

    mTcpSocket.disconnectFromHost();

    return false;
}

bool ImageLoader::ok() const
{
    return mId != -1;
}

char *ImageLoader::number(int &num)
{
    return reinterpret_cast<char*>(&num);
}
