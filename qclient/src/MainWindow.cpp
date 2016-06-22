//
// Created by Nikita Somenkov on 15/06/16.
//

#include "MainWindow.h"
#include "SettingDialog.h"

#include <QApplication>

#include <QLayout>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QPushButton>
#include <QShortcut>

MainWindow::MainWindow() : mCurrectClass(0)
{
    toolBarInit();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    {
        mainLayout->addWidget(mImageViewer = new ImageViewer(mRects, mPaths));

        QHBoxLayout *buttonLayout = new QHBoxLayout;
        {
            QShortcut *shortcutApply = new QShortcut(Qt::Key_Space, this);
            QShortcut *shortcutNext = new QShortcut(Qt::Key_Enter, this);
            QShortcut *shortcutPrev = new QShortcut(Qt::Key_Enter + Qt::CTRL, this);
            QShortcut *shortcutUndo = new QShortcut(QKeySequence::Undo, this);

            QWidget* spacer = new QWidget;
            spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

            buttonLayout->addWidget(mApplyButton = new QPushButton("Apply"));
            buttonLayout->addWidget(spacer);
            buttonLayout->addWidget(mPrevButton = new QPushButton("Prev"));
            buttonLayout->addWidget(mNextButton = new QPushButton("Next"));

            connect(mNextButton, &QPushButton::clicked, this, &MainWindow::next);
            connect(mPrevButton, &QPushButton::clicked, this, &MainWindow::prev);
            connect(mApplyButton, &QPushButton::clicked, this, &MainWindow::apply);
            connect(shortcutNext, &QShortcut::activated, this, &MainWindow::next);
            connect(shortcutPrev, &QShortcut::activated, this, &MainWindow::prev);
            connect(shortcutApply, &QShortcut::activated, this, &MainWindow::apply);
            connect(shortcutUndo, &QShortcut::activated, this, &MainWindow::undo);
        }

        mainLayout->addLayout(buttonLayout);

        QSize imageSize = { sizeImage.width, sizeImage.height };
        mImageViewer->resize(imageSize);
        mImageViewer->setFixedSize(imageSize);
        mImageViewer->setImage(QImage(":/noImage.png"));
    }

    QWidget *mainWidget = new QWidget;
    mainWidget->setLayout(mainLayout);
    this->setCentralWidget(mainWidget);

    this->setContextMenuPolicy(Qt::NoContextMenu);
    this->statusBar()->showMessage("Connect to server");
    this->setWindowIcon(QIcon(":/app_icon-1024.png"));

    mLoader.setHost("geeknikcloud.hldns.ru");
    mLoader.setPort(31415);
}


void MainWindow::toolBarInit()
{
    mToolBar = addToolBar("Main Toolbar");
    {
        QAction *connectionSettingAction = new QAction(QIcon(":/icons/setting.png"), "Connection Setting", this);
        connectionSettingAction->setShortcuts(QKeySequence::Preferences);
        connect(connectionSettingAction, &QAction::triggered, this, &MainWindow::connectionSetting);
        mToolBar->addAction(connectionSettingAction);

        QAction *selectToolRectAction = new QAction(QIcon(":/icons/select_tool.png"), "Select Rect", this);
        selectToolRectAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
        connect(selectToolRectAction, &QAction::triggered, this, &MainWindow::selectRectTool);
        mToolBar->addAction(selectToolRectAction);

        QAction *selectToolCorrectionFAction = new QAction(QIcon(":/icons/correction.png"),
                                                           "Correction forenground", this);
        selectToolCorrectionFAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));
        connect(selectToolCorrectionFAction, &QAction::triggered, this, &MainWindow::selectCorrectionFTool);
        mToolBar->addAction(selectToolCorrectionFAction);

        QAction *selectToolCorrectionBAction = new QAction(QIcon(":/icons/correction_bg.png"),
                                                           "Correction background", this);
        selectToolCorrectionBAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));
        connect(selectToolCorrectionBAction, &QAction::triggered, this, &MainWindow::selectCorrectionBTool);
        mToolBar->addAction(selectToolCorrectionBAction);

        QWidget* spacer = new QWidget;
        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        mToolBar->addWidget(spacer);

        mToolBar->addAction(QIcon(":/icons/qt.png"), tr("About &Qt"), qApp, &QApplication::aboutQt);
    }
    mToolBar->setMovable(false);
    mToolBar->setHidden(false);
}

void MainWindow::selectRectTool()
{
    if (!mLoader.ok())
    {
        QMessageBox::critical(this, "Error", "First connect the server!");
        mImageViewer->setImage(QImage(":/noImage.png"));
        mImageViewer->setMode(ImageViewer::NotAction);
        return;
    }
    qDebug() << "select tool";
    mImageViewer->setMode(ImageViewer::Rect);
    updateStatusTip();
}

void MainWindow::selectCorrectionFTool()
{
    if (!mLoader.ok())
    {
        QMessageBox::critical(this, "Error", "First connect the server!");
        mImageViewer->setImage(QImage(":/noImage.png"));
        mImageViewer->setMode(ImageViewer::NotAction);
        return;
    }
    qDebug() << "setCorretioncPoint tool FG";
    mImageViewer->setMode(ImageViewer::PathFG);
    updateStatusTip();
}

void MainWindow::selectCorrectionBTool()
{
    if (!mLoader.ok())
    {
        QMessageBox::critical(this, "Error", "First connect the server!");
        mImageViewer->setImage(QImage(":/noImage.png"));
        mImageViewer->setMode(ImageViewer::NotAction);
        return;
    }
    qDebug() << "setCorretioncPoint tool BG";
    mImageViewer->setMode(ImageViewer::PathBG);
    updateStatusTip();
}

void MainWindow::connectionSetting()
{
    SettingDialog connectionSettingWindow(&mLoader, this);
    connectionSettingWindow.show();
    connectionSettingWindow.exec();

    setConnectionIcon();
    if (mLoader.ok()) {
        nextImage();
        selectRectTool();
    }
    else
        statusBar()->showMessage("Connect to server");
}

void MainWindow::setConnectionIcon()
{
    const char *path = mLoader.ok() ?
                       ":/icons/setting_connected.png" :
                       ":/icons/setting.png";
    mToolBar->actions().front()->setIcon(QIcon(path));
}


void MainWindow::nextImage()
{
    qDebug() << "nextImage";
    int status = mLoader.get(mWorkImage);
    switch (status) {
        case ImageLoader::DONE:
            mImageViewer->setImage(QImage(":/done.png"));
            mImageViewer->setMode(ImageViewer::NotAction);
            break;
        case ImageLoader::NETWORK_ERROR:
            mImageViewer->setImage(QImage(":/noImage.png"));
            mImageViewer->setMode(ImageViewer::NotAction);
            mToolBar->actions().front()->setIcon(QIcon(":/icons/setting.png"));
            break;
        case ImageLoader::SUCCESS:
            mImageViewer->setImage(mWorkImage);
            mGrubCutter.set(mWorkImage);
            break;
        default:
            break;
    }
}

void MainWindow::updateStatusTip()
{
    QString phrase;
    QString work_class = classes_color[mCurrectClass].first.c_str();
    switch (mImageViewer->mode())
    {
        case ImageViewer::Rect:
            phrase += "Select Rects for object class=\'" + work_class + '\'';
            phrase += " and click Apply";
            break;
        case ImageViewer::PathFG:
            phrase += "Draw path for forengroud pixels class=\'" + work_class + '\'';
            phrase += " and click Apply";
            break;
        case ImageViewer::PathBG:
            phrase += "Draw path for backgroud pixels class=\'" + work_class + '\'';
            phrase += " and click Apply";
            break;
        case ImageViewer::NotAction:
            phrase += "Select tool";
            break;
        default:
            break;
    }

    statusBar()->showMessage(phrase);
}


void MainWindow::undo()
{
    if (!mLoader.ok())
    {
        mImageViewer->setImage(QImage(":/noImage.png"));
        mImageViewer->setMode(ImageViewer::NotAction);
        return;
    }
    qDebug() << "undo";
    switch (mImageViewer->mode())
    {
        case ImageViewer::Rect:
            if(!mRects.empty())
                mRects.pop_back();
            break;
        case ImageViewer::PathBG:
            if(!mPaths.empty())
                mPaths.pop_back();
            break;
        default:
            break;
    }
    mImageViewer->setImage(mWorkImage);
    updateStatusTip();
}

void MainWindow::apply()
{
    if (!mLoader.ok())
    {
        qDebug() << "!mLoader.ok()";
        mImageViewer->setImage(QImage(":/noImage.png"));
        mImageViewer->setMode(ImageViewer::NotAction);
        return;
    }
    if (!mGrubCutter.ok())
    {
        qDebug() << "!mGrubCutter.ok())";
        mImageViewer->setImage(QImage(":/error.png"));
        mImageViewer->setMode(ImageViewer::NotAction);
        return;
    }
    qDebug() << "setRects, sending to server";
    mGrubCutter.setRects(mRects);
    for (auto & path : mPaths)
    {
        const auto mode = path.second == ImageViewer::PathFG ?
                   GCModule::ForengroundSelect :
                   GCModule::BackgroundSelect;
        mGrubCutter.setCorrectionsPoints(path.first, mode);
    }
    mRects.clear();
    mPaths.clear();

    const QImage &image = mGrubCutter.getCurrentImage();
	qDebug() << image.size();
    if (image.isNull()) qDebug() << "image is null!!!";
    mImageViewer->setImage(image);

    statusBar()->showMessage("If ok: click Next elif choose correction tool");
}

void MainWindow::nextClass()
{
    qDebug() << "nextClass";
    if (!mLoader.ok())
    {
        qDebug() << "!mLoader.ok()";
        mImageViewer->setImage(QImage(":/noImage.png"));
        mImageViewer->setMode(ImageViewer::NotAction);
        return;
    }
    mRects.clear();
    mPaths.clear();
    if (mGrubCutter.ok())
    {
        qDebug() << "mGrubCutter.ok()";
        mGrubCutter.nextClass();
    }
    else {
        qDebug() << "!mGrubCutter.ok()";
        mImageViewer->setImage(QImage(":/error.png"));
        mImageViewer->setMode(ImageViewer::NotAction);
        return;
    }

    mImageViewer->setImage(mWorkImage);
}

void MainWindow::sendToServer()
{
    qDebug() << "nendToServer";
    if (!mLoader.ok())
    {
        qDebug() << "!mLoader.ok()";
        mImageViewer->setImage(QImage(":/noImage.png"));
        mImageViewer->setMode(ImageViewer::NotAction);
        return;
    }
    if (mGrubCutter.ok())   {
        qDebug() << "mGrubCutter.ok()";
        mLoader.load(mGrubCutter.get());
        mImageViewer->setImage(mGrubCutter.getCurrentImage());
    } else {
        qDebug() << "!mGrubCutter.ok()";
        mImageViewer->setImage(QImage(":/error.png"));
        mImageViewer->setMode(ImageViewer::NotAction);
        return;
    }

}

void MainWindow::next()
{
    if (!mLoader.ok())
    {
        qDebug() << "!mLoader.ok()";
        mImageViewer->setImage(QImage(":/noImage.png"));
        mImageViewer->setMode(ImageViewer::NotAction);
        return;
    }
    if (!mGrubCutter.ok())
    {
        qDebug() << "!mGrubCutter.ok()";
        return;
    }
    int next = mCurrectClass + 1;
    if (next == COUNT_CLASSES - 1) {
        sendToServer();
        nextImage();
        mCurrectClass = 0;
    } else {
        nextClass();
        mCurrectClass++;
    }
    selectRectTool();
    updateStatusTip();
}

void MainWindow::prev()
{
    if (!mLoader.ok())
    {
        mImageViewer->setImage(QImage(":/noImage.png"));
        mImageViewer->setMode(ImageViewer::NotAction);
        return;
    }
    int prev = mCurrectClass - 1;
    if (!mGrubCutter.ok())
    {
        return;
    }
    mGrubCutter.prevClass();
    if (prev > 0) {
        mRects.clear();
        mPaths.clear();
        mImageViewer->setImage(mWorkImage);
        mCurrectClass--;
    } else {
        mRects.clear();
        mPaths.clear();
        mImageViewer->setImage(mWorkImage);
    }
    selectRectTool();
    updateStatusTip();
}
