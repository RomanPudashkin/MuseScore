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

namespace mu::appshell {
class IOPreferencesModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool usePulseAudio READ usePulseAudio WRITE setUsePulseAudio NOTIFY usePulseAudioChanged)

    Q_PROPERTY(bool useAlsaAudio READ useAlsaAudio WRITE setUseAlsaAudio NOTIFY useAlsaAudioChanged)
    Q_PROPERTY(QVariant alsaAudioConfiguration READ alsaAudioConfiguration WRITE setAlsaAudioConfiguration NOTIFY alsaAudioConfigurationChanged)

    Q_PROPERTY(bool usePortAudio READ usePortAudio WRITE setUsePortAudio NOTIFY usePortAudioChanged)
    Q_PROPERTY(QVariant portAudioConfiguration READ portAudioConfiguration WRITE setPortAudioConfiguration NOTIFY portAudioConfigurationChanged)

    Q_PROPERTY(bool useJaskAudioServer READ useJaskAudioServer WRITE setJackAudioServer NOTIFY useJackAudioServerChanged)
    Q_PROPERTY(QVariant jackAudioServerConfiguration READ jackAudioServerConfiguration WRITE setJackAudioServerConfiguration NOTIFY jackAudioServerConfigurationChanged)

public:
    explicit IOPreferencesModel(QObject* parent = nullptr);

    bool usePulseAudio() const;

    bool useAlsaAudio() const;
    QVariant alsaAudioConfiguration() const;

    bool usePortAudio() const;
    QVariant portAudioConfiguration() const;

    bool useJaskAudioServer() const;
    QVariant jackAudioServerConfiguration() const;

    Q_INVOKABLE bool isPulseAudioAvailable() const;
    Q_INVOKABLE bool isAlsaAudioAvailable() const;
    Q_INVOKABLE void restartAudioAndMidiDevices();

public slots:
    void setUsePulseAudio(bool value);
    void setUseAlsaAudio(bool value);
    void setAlsaAudioConfiguration(const QVariant& configuration);
    void setUsePortAudio(bool value);
    void setPortAudioConfiguration(const QVariant& configuration);
    void setJackAudioServer(bool value);
    void setJackAudioServerConfiguration(const QVariant& configuration);

signals:
    void usePulseAudioChanged(bool value);
    void useAlsaAudioChanged(bool value);
    void alsaAudioConfigurationChanged(const QVariant& configuration);
    void usePortAudioChanged(bool value);
    void portAudioConfigurationChanged(const QVariant& configuration);
    void useJackAudioServerChanged(bool value);
    void jackAudioServerConfigurationChanged(const QVariant& configuration);
};
}

#endif // MU_APPSHELL_IOPREFERENCESMODEL_H
