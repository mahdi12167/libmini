#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupBattery();
    setupGPS();
}

MainWindow::~MainWindow()
{
    delete deviceInfo_;

    locationInfo_->stopUpdates();
    delete locationInfo_;

    delete ui;
}

void MainWindow::setupBattery()
{
    // obtain the device info source
    deviceInfo_ = new QSystemDeviceInfo(this);

    // read battery level
    setBatteryLevel(deviceInfo_->batteryLevel());

    // pass changed battery levels to setBatteryLevel slot
    connect(deviceInfo_, SIGNAL(batteryLevelChanged(int)),
            this, SLOT(setBatteryLevel(int)));
}

void MainWindow::setupGPS()
{
    static const int minInterval = 1000; // ms

    // obtain the location data source
    locationInfo_ = QGeoPositionInfoSource::createDefaultSource(this);

    // select positioning method
    locationInfo_->setPreferredPositioningMethods(QGeoPositionInfoSource::AllPositioningMethods);

    // query update interval
    int interval = locationInfo_->minimumUpdateInterval();
    if (interval<minInterval) interval=minInterval;
    locationInfo_->setUpdateInterval(interval);

    // when the position has changed the setGPSLocation slot is called
    connect(locationInfo_, SIGNAL(positionUpdated(QGeoPositionInfo)),
            this, SLOT(setGPSLocation(QGeoPositionInfo)));

    // when the position could not be determined the timeout slot is called
    connect(locationInfo_, SIGNAL(updateTimeout()),
            this, SLOT(timeout()));

    // start listening for position updates
    locationInfo_->startUpdates();
}

void MainWindow::setBatteryLevel(int level)
{
    QString text=QString("Battery=%1%").arg(level);
    ui->batteryLabel->setText(text);
}

void MainWindow::setGPSLocation(QGeoPositionInfo geoPositionInfo)
{
    QString text="Location=unknown";

    if (geoPositionInfo.isValid())
    {
        // get the current location coordinates
        QGeoCoordinate geoCoordinate = geoPositionInfo.coordinate();

        // transform coordinates to lat/lon
        qreal latitude = geoCoordinate.latitude();
        qreal longitude = geoCoordinate.longitude();

        text=QString("Latitude=%1\nLongitude=%2")
                .arg(latitude,0,'g',8)
                .arg(longitude,0,'g',8);
    }

    ui->spotLabel->setText(text);
}

void MainWindow::timeout()
{
    setGPSLocation(locationInfo_->lastKnownPosition());
    locationInfo_->startUpdates();
}
