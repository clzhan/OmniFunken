#ifndef AUDIOOUTFACTORY_H
#define AUDIOOUTFACTORY_H

#include "audioout_abstract.h"


class AudioOutFactory
{
protected:
    AudioOutFactory() {}
    ~AudioOutFactory() {}

public:
    static AudioOutAbstract *createAudioOut(const QString &key);
    static void registerAudioOut(AudioOutAbstract* audioOut);
};

#endif // AUDIOOUTFACTORY_H
