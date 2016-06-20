//
// Created by Nikita Somenkov on 16/06/16.
//

#ifndef SOCCER_SEGMENTATION_CVGRABCUT_H
#define SOCCER_SEGMENTATION_CVGRABCUT_H

#define methods /* methods */
#define members /* members */

#include <array>

#include <QImage>
#include "opencv2/imgproc.hpp"

#include "SegInfo.h"

//provide grabCur
class GCModule {
public:
    enum CorrectionType {
        BackgroundSelect,
        ForengroundSelect
    };

    QImage setRects(const QVector<QRect> &rects);
    QImage setCorrectionsPoints(const QVector<QPoint> &masks, CorrectionType type);
    QImage getCurrentImage();

    void nextClass();
    void prevClass();
    void set(const QImage &image);
    bool ok();

    QImage get();

private methods:
    static cv::Mat cast(const QImage &qImage);
    static QImage  cast(const cv::Mat &cvMat);

    static cv::Rect cast(const QRect &qRect);
    static QRect cast(const cv::Rect &cvRect);

    static cv::Point cast(const QPoint &qPoint);
    static QPoint cast(const cv::Point &cvPoint);

private members:
    cv::Mat mInputImage;

    cv::Mat mBgdModel;
    cv::Mat mFgdModel;

    int mIdx;
    std::array<cv::Mat, COUNT_CLASSES> mMasks;
};


#endif //SOCCER_SEGMENTATION_CVGRABCUT_H
