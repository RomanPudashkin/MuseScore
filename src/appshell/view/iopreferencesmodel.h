//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2021 MuseScore BVBA and others
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//=============================================================================
#ifndef MU_APPSHELL_IOPREFERENCESMODEL_H
#define MU_APPSHELL_IOPREFERENCESMODEL_H

#include <QObject>

#include "modularity/ioc.h"
#include "audio/iaudioconfiguration.h"

namespace mu::appshell {
class IOPreferencesModel : public QObject
{
    Q_OBJECT

    INJECT(appshell, audio::IAudioConfiguration, audioConfiguration)

    Q_PROPERTY(bool isPulseAudioUsed READ isPulseAudioUsed NOTIFY usedAudioSystemChanged)

    Q_PROPERTY(bool isAlsaAudioUsed READ isAlsaAudioUsed NOTIFY usedAudioSystemChanged)
    Q_PROPERTY(QVariant alsaAudioConfiguration READ alsaAudioConfiguration WRITE setAlsaAudioConfiguration NOTIFY alsaAudioConfigurationChanged)

    Q_PROPERTY(bool isJaskAudioServerUsed READ isJaskAudioServerUsed NOTIFY usedAudioSystemChanged)
    Q_PROPERTY(QVariant jackAudioServerConfiguration READ jackAudioServerConfiguration WRITE setJackAudioServerConfiguration NOTIFY jackAudioServerConfigurationChanged)

public:
    explicit IOPreferencesModel(QObject* parent = nullptr);

    bool isPulseAudioUsed() const;

    bool isAlsaAudioUsed() const;
    QVariant alsaAudioConfiguration() const;

    bool isJaskAudioServerUsed() const;
    QVariant jackAudioServerConfiguration() const;

    Q_INVOKABLE bool isPulseAudioAvailable() const;
    Q_INVOKABLE bool isAlsaAudioAvailable() const;
    Q_INVOKABLE bool isJackAudioServerAvailable() const;

    Q_INVOKABLE void setUsedAudioSystem(const QString& audioSystemName);
    Q_INVOKABLE void restartAudioAndMidiDevices();

public slots:
    void setAlsaAudioConfiguration(const QVariant& configuration);
    void setJackAudioServerConfiguration(const QVariant& configuration);

signals:
    void usedAudioSystemChanged();

    void alsaAudioConfigurationChanged(const QVariant& configuration);
    void jackAudioServerConfigurationChanged(const QVariant& configuration);

private:
    bool isAudioSystemAvailable(audio::AudioSystemType type) const;
    bool isAudioSystemUsed(audio::AudioSystemType type) const;
    void setUsedAudioSystem(audio::AudioSystemType type);
};
}

#endif // MU_APPSHELL_IOPREFERENCESMODEL_H
