//
// Created by Nikita Somenkov on 16/06/16.
//

#include "GCModule.h"

#include <QDebug>
#include <QRgb>

cv::Mat GCModule::cast(const QImage &qImage)
{
	const int h = qImage.height();
	const int w = qImage.width();
	cv::Mat result(h, w, CV_8UC3);
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			const QColor qColor = qImage.pixelColor(j, i);
			const cv::Vec3b color(qColor.red(), qColor.green(), qColor.blue());
			result.at<cv::Vec3b>(i, j) = color;
		}
			
	}
	return result;
}

QImage GCModule::cast(const cv::Mat &cvMat)
{
	const int h = cvMat.rows;
	const int w = cvMat.cols;

	if (cvMat.type() == CV_8UC3)
	{
		QImage result(w, h, QImage::Format_RGB888);
		for (int i = 0; i < h; i++)
		{
			for (int j = 0; j < w; j++)
			{
				cv::Vec3b color = cvMat.at<cv::Vec3b>(i, j);
				const QColor qColor(color[0], color[1], color[2]);
				result.setPixelColor(j, i, qColor);
			}
		}
		return result;
	}
	QImage result(w, h, QImage::Format_Grayscale8);
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			uchar color = cvMat.at<uchar>(i, j);
			const QColor qColor(color, color, color);
			result.setPixelColor(j, i, qColor);
		}
	}
    qDebug() << result.byteCount();
	return result;
	//return QImage::fromData(cvMat.data, QImage::Format_Grayscale8);
}

cv::Rect GCModule::cast(const QRect &qRect)
{
    return cv::Rect(qRect.x(), qRect.y(), qRect.width(), qRect.height());
}

QRect GCModule::cast(const cv::Rect &cvRect)
{
    return QRect(cvRect.x, cvRect.y, cvRect.width, cvRect.height);
}

cv::Point GCModule::cast(const QPoint &qPoint)
{
    return cv::Point(qPoint.x(), qPoint.y());
}

QPoint GCModule::cast(const cv::Point &cvPoint)
{
    return QPoint(cvPoint.x, cvPoint.y);
}

QImage GCModule::getCurrentImage()
{
    cv::Mat result;
    cv::Mat mask = mMasks[mIdx] == cv::GC_FGD |
                   mMasks[mIdx] == cv::GC_PR_FGD;
    mInputImage.copyTo(result, mask);
    return cast(result);
}

QImage GCModule::setCorrectionsPoints(const QVector<QPoint> &points, CorrectionType type)
{
    qDebug() << "GCModule::setCorrectionsPoints(" << points << ");";
    static const int radius = 1;
    static const int thickness = -1;
    uchar value = type == BackgroundSelect ? cv::GC_BGD : cv::GC_FGD;
    for (auto & point : points)
        cv::circle( mMasks[mIdx], cast(point), radius, value, thickness);
    cv::grabCut(mInputImage, mMasks[mIdx], cv::Rect(),
                mBgdModel, mFgdModel, 5, cv::GC_INIT_WITH_MASK);
    return getCurrentImage();
}


void GCModule::set(const QImage &image)
{
    mIdx = 0;
    mBgdModel.release();
    mFgdModel.release();
    mInputImage = cast(image);
    for (auto & mask : mMasks) {
        mask = cv::Mat::zeros(sizeImage, CV_8UC1);
    }
}

QImage GCModule::get()
{
    static const int step = 255 / COUNT_CLASSES;
    cv::Mat result = cv::Mat::zeros(sizeImage, CV_8UC1);
    for (int i = 0; i < COUNT_CLASSES; ++i)
    {
        cv::Mat mask = mMasks[i] == cv::GC_FGD |
                       mMasks[i] == cv::GC_PR_FGD;
        result.setTo(255 - i * step, mask);
    }
    return cast(result);
}


QImage GCModule::setRects(const QVector<QRect> &rects)
{
    qDebug() << "GCModule::setRects(" << rects << ");";
    for (auto & rect : rects)
    {
        cv::Mat mask;
        cv::grabCut(mInputImage, mask, cast(rect), mBgdModel,
                    mFgdModel, 5, cv::GC_INIT_WITH_RECT);
        mMasks[mIdx] |= mask;
    }

    for (int i = mIdx - 1; i >= 0; --i)
    {
        cv::Mat mask =
                mMasks[i] == cv::GC_FGD |
                mMasks[i] == cv::GC_PR_FGD;
        mMasks[mIdx].setTo(cv::GC_BGD, mask);
    }

    return getCurrentImage();
}

void GCModule::nextClass()
{
    if (mIdx == COUNT_CLASSES - 1)
    {
        mMasks[mIdx].setTo(cv::GC_FGD);
        for (int i = mIdx; i >= 0; --i)
        {
            cv::Mat mask =
                    mMasks[i] == cv::GC_FGD |
                    mMasks[i] == cv::GC_PR_FGD;
            mMasks[mIdx].setTo(cv::GC_BGD, mask);
        }
        mIdx = 0;
        mInputImage.release();
    }
    else mIdx++;
}


void GCModule::prevClass()
{
    mMasks[mIdx] = cv::Mat::zeros(sizeImage, CV_8UC1);
    mIdx = mIdx != 0 ? mIdx : mIdx + 1;
}

bool GCModule::ok()
{
    return !mInputImage.empty();
}
