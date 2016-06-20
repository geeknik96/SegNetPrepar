//
// Created by Nikita Somenkov on 16/06/16.
//

#ifndef SOCCER_SEGMENTATION_IMAGEVIEWER_H
#define SOCCER_SEGMENTATION_IMAGEVIEWER_H

#include <QWidget>

typedef QPair<QVector<QPoint>, int> Path;

class ImageViewer : public QWidget {
    Q_OBJECT
public:
    ImageViewer(QVector<QRect> &rects, QVector<Path> &paths, QWidget *parent = nullptr);

    enum Mode { Rect, PathBG, PathFG, NotAction };

    Mode mode();
    void setMode(Mode mode);
    void setImage(const QImage &image);

protected:
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);

    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);

private:
    void drawShape();
    void drawLine();
    void drawRect(const QRect &rect);

private:
    QImage mOriginalImage;
    QImage mImage;

    QVector<QPoint> mPath;

    QVector<QRect> &mRects;
    QVector<Path> &mPaths;

    QPoint mOldPos, mCurPos;
    bool mMouseDown;
    int mMode;
};


#endif //SOCCER_SEGMENTATION_IMAGEVIEWER_H
