#ifndef OFCORE_H
#define OFCORE_H

#include "global.h"
#include <QCommandLineParser>
#include <QSettings>

#define ofCore Core::instance()

class AudioOutAbstract;
class DeviceControlAbstract;

class Core : public QObject
{
    Q_OBJECT

public:
    // This should only be used to acccess the signals, so it could
    // theoretically return an QObject *. For source compatibility
    // it returns a Core.
    static Core *instance();

    void parseCommandLine(QCommandLineParser &parser);

    // Provide settings provided by configuration file
    QSettings *settings();

    AudioOutAbstract        *audioOut();
    DeviceControlAbstract   *deviceControl();

public slots:
    void shutdown();

private:
    Core();
    ~Core();

    bool powerOnDevice(uint time = UINT_MAX);
    bool powerOnDevice2(uint time = UINT_MAX);

    QString m_audioOutName;
    QString m_audioDeviceName;
    AudioOutAbstract *m_audioOut;
    DeviceControlAbstract *m_deviceControl;
};

#endif // OFCORE_H
