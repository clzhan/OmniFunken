#include <QCoreApplication>
#include <QCommandLineParser>
#include <QHostInfo>
#include <QSettings>

#include "audiooutfactory.h"
#include "daemon.h"
#include "player.h"
#include "rtspserver.h"
#include "zeroconf_dns_sd.h"

#include <unistd.h>
#include <sys/stat.h>


int main(int argc, char *argv[])
{
    //qInstallMsgHandler(logOutput);
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("OmniFunken");
    QCoreApplication::setApplicationVersion("0.0.1");

    // command line options
    QString defaultName("OmniFunken@"); defaultName.append(QHostInfo::localHostName());
    QCommandLineParser parser;
    parser.setApplicationDescription("OmniFunken aims to be a general purpose media renderer.");
    parser.addVersionOption();
    parser.addHelpOption();
    QCommandLineOption verboseOption(QStringList() << "v" << "verbose", "Set verbose mode.");
    parser.addOption(verboseOption);
    QCommandLineOption nameOption(QStringList() << "n" << "name", "Set propagated name.", "name", defaultName);
    parser.addOption(nameOption);
    QCommandLineOption portOption(QStringList() << "p" << "port", "Set RTSP port.", "port", "5002");
    parser.addOption(portOption);
    QCommandLineOption latencyOption(QStringList() << "l" << "latency", "Set latency in milliseconds.");
    parser.addOption(latencyOption);
    QCommandLineOption daemonOption(QStringList() << "d" << "daemon", "Start as daemon.");
    parser.addOption(daemonOption);
    parser.process(a);

    if (parser.isSet(daemonOption)) {
        daemon_init();
    }


    // settings
    QSettings settings("/etc/omnifunken.conf", QSettings::IniFormat);

    RtspServer  *rtspServer = new RtspServer();
    RtpBuffer   *rtpBuffer = new RtpBuffer();
    RtpReceiver *rtpReceiver = new RtpReceiver(rtpBuffer);
    AudioOutAbstract *audioOut = AudioOutFactory::createAudioOut("ao");
    Player      *player = new Player(rtpBuffer, audioOut);

    QObject::connect(rtspServer, SIGNAL(announce(RtspMessage::Announcement)), rtpReceiver, SLOT(announce(RtspMessage::Announcement)));
    QObject::connect(rtspServer, SIGNAL(senderSocketAvailable(RtpReceiver::PayloadType, quint16)), rtpReceiver, SLOT(setSenderSocket(RtpReceiver::PayloadType, quint16)));
    QObject::connect(rtspServer, SIGNAL(receiverSocketRequired(RtpReceiver::PayloadType, quint16*)), rtpReceiver, SLOT(bindSocket(RtpReceiver::PayloadType, quint16*)));
    QObject::connect(rtspServer, SIGNAL(record(quint16)), rtpBuffer, SLOT(flush(quint16)));
    QObject::connect(rtspServer, SIGNAL(flush(quint16)), rtpBuffer, SLOT(flush(quint16)));
    QObject::connect(rtspServer, SIGNAL(teardown()), rtpReceiver, SLOT(teardown()));
    QObject::connect(rtspServer, SIGNAL(teardown()), rtpBuffer, SLOT(teardown()));
    QObject::connect(rtspServer, &RtspServer::teardown, player, &Player::teardown);
    QObject::connect(rtspServer, &RtspServer::volume, audioOut, &AudioOutAbstract::setVolume);

    rtspServer->listen(QHostAddress::AnyIPv4, parser.value(portOption).toInt());

    ZeroconfDnsSd *dnsSd = new ZeroconfDnsSd();
    dnsSd->registerService(parser.value(nameOption).toLatin1(), parser.value(portOption).toInt());

    return a.exec();
}
