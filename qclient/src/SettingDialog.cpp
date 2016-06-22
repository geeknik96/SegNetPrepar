//
// Created by Nikita Somenkov on 16/06/16.
//

#include "SettingDialog.h"

#include <QMessageBox>
#include <QPushButton>
#include <QLineEdit>
#include <QLayout>
#include <QLabel>

#include "ImageLoader.h"

SettingDialog::SettingDialog(ImageLoader *imageLoader, QWidget *parent)
        : mLoader(imageLoader), QDialog(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    {
        QGridLayout *workLayout = new QGridLayout;
        {
            workLayout->addWidget(new QLabel(tr("Host address")), 0, 0);
            workLayout->addWidget(mHost = new QLineEdit(imageLoader->ip()), 0, 1);
            workLayout->addWidget(new QLabel(tr("Port")), 1, 0);
            workLayout->addWidget(mPort = new QLineEdit(QString::number(imageLoader->port())), 1, 1);
        }

        QHBoxLayout *buttonLayout = new QHBoxLayout;
        {
            QPushButton *connectButton = new QPushButton(tr("&Connect"));
            QPushButton *cancelButton = new QPushButton(tr("Ca&ncel"));

            buttonLayout->addWidget(connectButton);
            buttonLayout->addWidget(cancelButton);

            QObject::connect(connectButton, &QPushButton::clicked, this, &SettingDialog::connect);
            QObject::connect(cancelButton, &QPushButton::clicked, this, &SettingDialog::cancel);
        }

        mainLayout->addLayout(workLayout);
        mainLayout->addLayout(buttonLayout);
    }

    this->setLayout(mainLayout);
    this->setFixedSize(400, 150);
}

void SettingDialog::connect()
{
    mLoader->setHost(mHost->text());
    mLoader->setPort(mPort->text().toUShort());
    if (mLoader->connect()) {
        accept();
        return;
    }
    QMessageBox::critical(this, "Connection error", "Connection refused");
}

void SettingDialog::cancel()
{
    reject();
}
