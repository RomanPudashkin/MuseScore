/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-Studio-CLA-applies
 *
 * MuseScore Studio
 * Music Composition & Notation
 *
 * Copyright (C) 2026 MuseScore Limited and others
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "reviewscorequalitymodel.h"

#include <QTimer>

#include "cloud/cloudtypes.h"
#include "project/inotationproject.h"

using namespace mu::project;

static const muse::Uri NOTATION_PAGE_URI("musescore://notation");

ReviewScoreQualityModel::ReviewScoreQualityModel(QObject* parent)
    : QObject(parent), muse::Contextable(muse::iocCtxForQmlObject(this))
{
}

ReviewScoreQualityModel::Step ReviewScoreQualityModel::step() const
{
    return m_step;
}

void ReviewScoreQualityModel::submitGood()
{
    const std::optional<int> scoreId = currentScoreId();
    IF_ASSERT_FAILED(scoreId) {
        return;
    }

    convertFileToScoreService()->submitReview(*scoreId, ReviewRating::Good);

    emit closeRequested();
    openNotationPage();
}

void ReviewScoreQualityModel::submitBad()
{
    m_rating = ReviewRating::Bad;
    m_step = Step::Feedback;

    emit stepChanged();
}

void ReviewScoreQualityModel::submitFeedback(const QString& comment)
{
    const std::optional<int> scoreId = currentScoreId();
    IF_ASSERT_FAILED(scoreId) {
        return;
    }

    convertFileToScoreService()->submitReview(*scoreId, m_rating, comment);

    emit closeRequested();
    openNotationPage();
}

void ReviewScoreQualityModel::skipFeedback()
{
    emit closeRequested();
    openNotationPage();
}

std::optional<int> ReviewScoreQualityModel::currentScoreId() const
{
    INotationProjectPtr project = globalContext()->currentProject();
    if (!project) {
        return std::nullopt;
    }

    return static_cast<int>(muse::cloud::idFromCloudUrl(project->cloudInfo().sourceUrl).toUint64());
}

void ReviewScoreQualityModel::openNotationPage()
{
    QTimer::singleShot(0, this, [this]() {
        interactive()->open(NOTATION_PAGE_URI);
    });
}
