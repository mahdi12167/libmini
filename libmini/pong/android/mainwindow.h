#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

// battery level
#include <QSystemDeviceInfo>

// gps location
#include <QGeoPositionInfo>
#include <QGeoPositionInfoSource>

QTM_USE_NAMESPACE

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:

    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

private:
    Ui::MainWindow *ui;

    QSystemDeviceInfo *deviceInfo_;
    QGeoPositionInfoSource* locationInfo_;

    void setupBattery();
    void setupGPS();

private slots:
    void setBatteryLevel(int);
    void setGPSLocation(QGeoPositionInfo geoPositionInfo);
    void timeout();
};

#endif // MAINWINDOW_H
