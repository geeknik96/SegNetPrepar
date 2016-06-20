//
// Created by Nikita Somenkov on 16/06/16.
//

#ifndef SOCCER_SEGMENTATION_CONNECTIONSETTINGWINDOW_H
#define SOCCER_SEGMENTATION_CONNECTIONSETTINGWINDOW_H

#include <QDialog>

class QLineEdit;
class ImageLoader;

class SettingDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingDialog(ImageLoader *imageLoader, QWidget *parent);

private:
    QLineEdit *mHost;
    QLineEdit *mPort;

    ImageLoader *mLoader;

private slots:
    void connect();
    void cancel();
};


#endif //SOCCER_SEGMENTATION_CONNECTIONSETTINGWINDOW_H
