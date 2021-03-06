#ifndef RTPRECEIVER_H
#define RTPRECEIVER_H

#include "alac.h"
#include "rtpbuffer.h"
#include "airtunes/airtunesconstants.h"
#include "rtsp/rtspmessage.h"

#include <openssl/aes.h>

#include <QObject>

class QElapsedTimer;
class QTimer;
class QUdpSocket;

class RtpReceiver : public QObject
{
    Q_OBJECT
public:
    explicit RtpReceiver(RtpBuffer *rtpBuffer, quint16 retryInterval = 25, QObject *parent = 0);

public slots:
    void announce(const RtspMessage::Announcement &announcement);
    void setSenderSocket(airtunes::PayloadType payloadType, quint16 port);
    void bindSocket(airtunes::PayloadType payloadType, quint16 *port);
    void teardown();

private slots:
    void readPendingDatagrams();
    void requestRetransmit();

private:
    void readHeader(const char* data, RtpHeader *header);
    void decrypt(const char *in, unsigned char *out, int length);

    void initAlac(const QByteArray &fmtp);

private:
    quint16     m_senderControlPort;

    QUdpSocket  *m_udpSocket;
    AES_KEY     m_aesKey;
    alac_file   *m_alac;
    RtspMessage::Announcement m_announcement;

    RtpBuffer   *m_rtpBuffer;

    // retry members
    QTimer  *m_retryTimer;
};

#endif // RTPRECEIVER_H
