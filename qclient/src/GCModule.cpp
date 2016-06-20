//
// Created by Nikita Somenkov on 16/06/16.
//

#include "GCModule.h"

cv::Mat GCModule::cast(const QImage &qImage)
{
    void * data = const_cast<uchar*>(qImage.bits());
    return cv::Mat(qImage.height(), qImage.width(), CV_8UC3, data);
}

QImage GCModule::cast(const cv::Mat &cvMat)
{
    return QImage(cvMat.data, cvMat.cols, cvMat.rows, QImage::Format_RGB888);
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

