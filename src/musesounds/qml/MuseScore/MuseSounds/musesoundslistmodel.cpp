/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-Studio-CLA-applies
 *
 * MuseScore Studio
 * Music Composition & Notation
 *
 * Copyright (C) 2024 MuseScore Limited and others
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

#include "musesoundslistmodel.h"

using namespace mu::musesounds;

static const QVariantMap soundLibraryInfoToMap(const SoundLibraryInfo& soundLibraryInfo)
{
    QVariantMap map;
    map["code"] = soundLibraryInfo.code.toQString();
    map["title"] = soundLibraryInfo.title.toQString();
    map["subtitle"] = soundLibraryInfo.subtitle.toQString();
    map["thumbnail"] = QString::fromStdString(soundLibraryInfo.thumbnail.toString());
    map["uri"] = QString::fromStdString(soundLibraryInfo.uri.toString());

    return map;
}

static const QVariantList soundInfoListToVariantList(const SoundLibraryInfoList& soundLibraryInfoList)
{
    QVariantList list;

    for (const SoundLibraryInfo& soundLibraryInfo : soundLibraryInfoList) {
        list << soundLibraryInfoToMap(soundLibraryInfo);
    }

    return list;
}

MuseSoundsListModel::MuseSoundsListModel(QObject* parent)
    : QAbstractListModel(parent), muse::Contextable(muse::iocCtxForQmlObject(this))
{
}

void MuseSoundsListModel::load()
{
    if (!m_searchText.isEmpty()) {
        m_searchText.clear();
        emit searchTextChanged();
    }

    m_filteredCatalogs.clear();

    setSoundsCatalogs(repository()->soundsCatalogueList());
    repository()->soundsCatalogueListChanged().onNotify(this, [this](){
        setSoundsCatalogs(repository()->soundsCatalogueList());
    });
}

QString MuseSoundsListModel::searchText() const
{
    return m_searchText;
}

void MuseSoundsListModel::setSearchText(const QString& text)
{
    if (m_searchText == text) {
        return;
    }

    m_searchText = text;
    emit searchTextChanged();
    applyFilter();
}

QVariant MuseSoundsListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    const SoundCatalogueInfo& soundCategoryInfo = m_filteredCatalogs.at(index.row());

    switch (role) {
    case rCatalogueTitle:
        return soundCategoryInfo.title.toQString();
    case rCatalogueSoundLibraries:
        return soundInfoListToVariantList(soundCategoryInfo.soundLibraries);
    }

    return QVariant();
}

int MuseSoundsListModel::rowCount(const QModelIndex&) const
{
    return static_cast<int>(m_filteredCatalogs.size());
}

QHash<int, QByteArray> MuseSoundsListModel::roleNames() const
{
    return {
        { rCatalogueTitle, "catalogueTitle" },
        { rCatalogueSoundLibraries, "catalogueSoundsLibraries" }
    };
}

bool MuseSoundsListModel::isEmpty() const
{
    return m_soundsCatalogs.empty();
}

void MuseSoundsListModel::setSoundsCatalogs(const SoundCatalogueInfoList& soundsCatalogs)
{
    m_soundsCatalogs = soundsCatalogs;
    applyFilter();
}

static SoundLibraryInfoList filteredLibraries(const SoundLibraryInfoList& libs, const QString& search)
{
    SoundLibraryInfoList result;
    for (const SoundLibraryInfo& lib : libs) {
        if (lib.title.toQString().contains(search, Qt::CaseInsensitive)
            || lib.subtitle.toQString().contains(search, Qt::CaseInsensitive)) {
            result.emplace_back(lib);
        }
    }
    return result;
}

void MuseSoundsListModel::applyFilter()
{
    TRACEFUNC;

    beginResetModel();

    if (m_searchText.isEmpty()) {
        m_filteredCatalogs = m_soundsCatalogs;
        endResetModel();
        emit isEmptyChanged();
        return;
    }

    m_filteredCatalogs.clear();
    for (const SoundCatalogueInfo& catalogue : m_soundsCatalogs) {
        SoundLibraryInfoList libs = filteredLibraries(catalogue.soundLibraries, m_searchText);
        if (!libs.empty()) {
            m_filteredCatalogs.push_back({ catalogue.title, std::move(libs) });
        }
    }

    endResetModel();

    emit isEmptyChanged();
}
