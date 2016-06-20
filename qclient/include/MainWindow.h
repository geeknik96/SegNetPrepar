//
// Created by Nikita Somenkov on 15/06/16.
//

#ifndef SOCCER_SEGMENTATION_MAINWINDOW_H
#define SOCCER_SEGMENTATION_MAINWINDOW_H

#include "ImageLoader.h"
#include "ImageViewer.h"
#include "GCModule.h"

#include <QMainWindow>

class ImageViewer;
class QPushButton;
class QToolBar;


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();

private slots:
    void connectionSetting();

    void selectRectTool();
    void selectCorrectionFTool();
    void selectCorrectionBTool();

    void nextImage();
    void nextClass();
    void sendToServer();
    void apply();
    void undo();
    void next();
    void prev();

private:
    void toolBarInit();
    void setConnectionIcon();
    void updateStatusTip();

    int mCurrectClass;
    GCModule mGrubCutter;
    ImageLoader mLoader;

    QImage mWorkImage;

    QVector<QRect> mRects;
    QVector<Path> mPaths;


    ImageViewer *mImageViewer;
    QToolBar *mToolBar;
    QPushButton *mNextButton;
    QPushButton *mPrevButton;
    QPushButton *mApplyButton;
};


#endif //SOCCER_SEGMENTATION_MAINWINDOW_H
