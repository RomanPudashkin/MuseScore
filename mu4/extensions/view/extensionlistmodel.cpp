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
#include "extensionlistmodel.h"

using namespace mu::extensions;

ExtensionListModel::ExtensionListModel(QObject* parent)
    : QAbstractListModel(parent)
{
    m_roles.insert(rName, "name");
    m_roles.insert(rVersion, "version");
    m_roles.insert(rFileSize, "fileSize");
    m_roles.insert(rStatus, "status");
}

QVariant ExtensionListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    Extension item = m_list[index.row()];

    switch (role) {
    case rName:
        return QVariant::fromValue(item.name);
    case rVersion:
        return QVariant::fromValue(item.version);
    case rFileSize:
        return QVariant::fromValue(item.fileSize);
    case rStatus:
        return QVariant();
    }

    return QVariant();
}

int ExtensionListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return m_list.count();
}

QHash<int, QByteArray> mu::extensions::ExtensionListModel::roleNames() const
{
    return m_roles;
}

void ExtensionListModel::load()
{
    ValCh<ExtensionList> extensions = extensionsController()->extensionList();

    beginResetModel();
    m_list = extensions.val;
    endResetModel();

    extensions.ch.onReceive(this, [this](const ExtensionList& extensions) {
        beginResetModel();
        m_list = extensions;
        endResetModel();
    });
}

void ExtensionListModel::updateList()
{
    extensionsController()->refreshExtensionList();
}
