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

#include "scoredisplaysettingsmodel.h"

using namespace mu::inspector;
using namespace mu::actions;

ScoreSettingsModel::ScoreSettingsModel(QObject* parent, IElementRepositoryService* repository)
    : AbstractInspectorModel(parent, repository)
{
    setSectionType(SECTION_SCORE_DISPLAY);
    setTitle(tr("Show"));
    createProperties();
}

void ScoreSettingsModel::createProperties()
{
}

void ScoreSettingsModel::requestElements()
{
    //!Note the model work only with the parent score, no need to request other child elements
}

bool ScoreSettingsModel::hasAcceptableElements() const
{
    if (parentScore()) {
        return true;
    }

    return false;
}

void ScoreSettingsModel::loadProperties()
{
    emit shouldShowInvisibleChanged(shouldShowInvisible());
    emit shouldShowUnprintableChanged(shouldShowUnprintable());
    emit shouldShowFramesChanged(shouldShowFrames());
    emit shouldShowPageMarginsChanged(shouldShowPageMargins());
}

void ScoreSettingsModel::resetProperties()
{
    m_shouldShowInvisible = false;
    m_shouldShowUnprintable = false;
    m_shouldShowFrames = false;
    m_shouldShowPageMargins = false;
}

bool ScoreSettingsModel::shouldShowInvisible() const
{
    return m_shouldShowInvisible;
}

bool ScoreSettingsModel::shouldShowUnprintable() const
{
    return m_shouldShowUnprintable;
}

bool ScoreSettingsModel::shouldShowFrames() const
{
    return m_shouldShowFrames;
}

bool ScoreSettingsModel::shouldShowPageMargins() const
{
    return m_shouldShowPageMargins;
}

void ScoreSettingsModel::setShouldShowInvisible(bool shouldShowInvisible)
{
    if (m_shouldShowInvisible == shouldShowInvisible) {
        return;
    }

    m_shouldShowInvisible = shouldShowInvisible;
    emit shouldShowInvisibleChanged(shouldShowInvisible);

    dispatcher()->dispatch("show-invisible", ActionData::make_arg1<bool>(shouldShowInvisible));
}

void ScoreSettingsModel::setShouldShowUnprintable(bool shouldShowUnprintable)
{
    if (m_shouldShowUnprintable == shouldShowUnprintable) {
        return;
    }

    m_shouldShowUnprintable = shouldShowUnprintable;
    emit shouldShowUnprintableChanged(shouldShowUnprintable);

    dispatcher()->dispatch("show-unprintable", ActionData::make_arg1<bool>(shouldShowUnprintable));
}

void ScoreSettingsModel::setShouldShowFrames(bool shouldShowFrames)
{
    if (m_shouldShowFrames == shouldShowFrames) {
        return;
    }

    m_shouldShowFrames = shouldShowFrames;
    emit shouldShowFramesChanged(shouldShowFrames);

    dispatcher()->dispatch("show-frames", ActionData::make_arg1<bool>(shouldShowFrames));
}

void ScoreSettingsModel::setShouldShowPageMargins(bool shouldShowPageMargins)
{
    if (m_shouldShowPageMargins == shouldShowPageMargins) {
        return;
    }

    m_shouldShowPageMargins = shouldShowPageMargins;
    emit shouldShowPageMarginsChanged(shouldShowPageMargins);

    dispatcher()->dispatch("show-pageborders", ActionData::make_arg1<bool>(shouldShowPageMargins));
}
