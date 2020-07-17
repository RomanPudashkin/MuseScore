//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2020 MuseScore BVBA and others
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

#ifndef MU_INSPECTOR_SCORESETTINGSMODEL_H
#define MU_INSPECTOR_SCORESETTINGSMODEL_H

#include "view/abstractinspectormodel.h"

#include "modularity/ioc.h"
#include "actions/iactionsdispatcher.h"

namespace mu {
namespace scene {
namespace inspector {
class ScoreSettingsModel : public AbstractInspectorModel
{
    Q_OBJECT

    INJECT(inspector, actions::IActionsDispatcher, dispatcher)

    Q_PROPERTY(bool shouldShowInvisible READ shouldShowInvisible WRITE setShouldShowInvisible NOTIFY shouldShowInvisibleChanged)
    Q_PROPERTY(bool shouldShowUnprintable READ shouldShowUnprintable WRITE setShouldShowUnprintable NOTIFY shouldShowUnprintableChanged)
    Q_PROPERTY(bool shouldShowFrames READ shouldShowFrames WRITE setShouldShowFrames NOTIFY shouldShowFramesChanged)
    Q_PROPERTY(bool shouldShowPageMargins READ shouldShowPageMargins WRITE setShouldShowPageMargins NOTIFY shouldShowPageMarginsChanged)

public:
    explicit ScoreSettingsModel(QObject* parent, IElementRepositoryService* repository);

    void createProperties() override;
    void requestElements() override;
    void loadProperties() override;
    void resetProperties() override;

    bool hasAcceptableElements() const override;

    bool shouldShowInvisible() const;
    bool shouldShowUnprintable() const;
    bool shouldShowFrames() const;
    bool shouldShowPageMargins() const;

public slots:
    void setShouldShowInvisible(bool shouldShowInvisible);
    void setShouldShowUnprintable(bool shouldShowUnprintable);
    void setShouldShowFrames(bool shouldShowFrames);
    void setShouldShowPageMargins(bool shouldShowPageMargins);

signals:
    void shouldShowInvisibleChanged(bool shouldShowInvisible);
    void shouldShowUnprintableChanged(bool shouldShowUnprintable);
    void shouldShowFramesChanged(bool shouldShowFrames);
    void shouldShowPageMarginsChanged(bool shouldShowPageMargins);

private:
    bool m_shouldShowInvisible = false;
    bool m_shouldShowUnprintable = false;
    bool m_shouldShowFrames = false;
    bool m_shouldShowPageMargins = false;
};
}
}
}

#endif // MU_INSPECTOR_SCORESETTINGSMODEL_H
