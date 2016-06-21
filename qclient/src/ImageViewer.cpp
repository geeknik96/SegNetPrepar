//
// Created by Nikita Somenkov on 16/06/16.
//

#include "ImageViewer.h"

#include <QPainter>
#include <QMouseEvent>

ImageViewer::ImageViewer(QVector<QRect> &rects, QVector<Path> &paths, QWidget *parent)
    : QWidget(parent), mRects(rects), mPaths(paths), mMouseDown(false), mMode(NotAction) { }

void ImageViewer::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;
    mMouseDown = true;
    mPath.clear();
    mOldPos = mCurPos = event->pos();
    mPath.push_back(mCurPos);
}

void ImageViewer::mouseMoveEvent(QMouseEvent *event)
{
    if (mMouseDown)
    {
        mOldPos = mCurPos;
        mCurPos = event->pos();
        mPath.push_back(mCurPos);
        drawShape();
    }
}

void ImageViewer::mouseReleaseEvent(QMouseEvent *event)
{
    drawShape();
    mOriginalImage = mImage;
    mMouseDown = false;

    switch (mMode)
    {
        case Rect:
            mRects.push_back(QRect(mPath.front(), mPath.back()).normalized());
            break;
        case PathBG:
            mPaths.push_back(qMakePair(mPath, PathBG));
            break;
        case PathFG:
            mPaths.push_back(qMakePair(mPath, PathFG));
            break;
        case NotAction:
        default:
            break;
    }
}

void ImageViewer::drawShape()
{
    switch (mMode)
        {
            case Rect:
                drawRect(QRect(mPath.front(), mPath.back()));
                break;
            case PathBG:
            case PathFG:
                drawLine();
                break;
            case NotAction:
            default:
                break;
        }
}

void ImageViewer::drawRect(const QRect &rect)
{
    mImage = mOriginalImage;
    QPainter painter(&mImage);
    painter.setPen(Qt::green);
    painter.drawRect(rect);
    update();
}

void ImageViewer::drawLine()
{
    QPainter painter(&mImage);
    if (mMode == PathFG) painter.setPen(Qt::red);
    if (mMode == PathBG) painter.setPen(Qt::blue);
    painter.drawLine(mOldPos, mCurPos);
    QRect rect(mOldPos, mCurPos);
    update(rect.normalized().adjusted(-5, -5, 5, 5));
}

void ImageViewer::resizeEvent(QResizeEvent *event)
{
    mImage = mImage.scaled(event->size());
    mOriginalImage = mOriginalImage.scaled(event->size());
    update();
}

void ImageViewer::setImage(const QImage &image)
{
    mImage = image.scaled(size());
    mOriginalImage = image.scaled(size());

    QPainter painter(&mImage);
    int i;
    switch (mMode)
    {
        case Rect:
            painter.drawRects(mRects);
            break;
        case PathBG:
        case PathFG:
            for (i = 0; i < mPaths.size(); i++)
                painter.drawLines(mPaths[i].first);
            break;
        case NotAction:
        default:
            break;
    }

    mOriginalImage = mImage;

    update();
}

void ImageViewer::setMode(ImageViewer::Mode mode)
{
    mMode = mode;
    QCursor cursor;
    switch (mode)
    {
        case Rect:
            cursor = QCursor(Qt::CrossCursor);
            break;
        case PathBG:
        case PathFG:
            cursor = QCursor(Qt::UpArrowCursor);
            break;
        case NotAction:
        default:
            cursor = QCursor(Qt::ArrowCursor);
            break;
    }
    setCursor(cursor);
}

void ImageViewer::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect, mImage, dirtyRect);
}


ImageViewer::Mode ImageViewer::mode()
{
    return Mode(mMode);
}
