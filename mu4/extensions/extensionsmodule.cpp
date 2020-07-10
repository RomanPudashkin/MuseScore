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
#include "extensionsmodule.h"

#include <QQmlEngine>

#include "internal/extensionsconfiguration.h"
#include "internal/extensionscontroller.h"
#include "view/extensionlistmodel.h"

using namespace mu::extensions;

static ExtensionsConfiguration* m_extensionsConfiguration = new ExtensionsConfiguration();

static void extensions_init_qrc()
{
    Q_INIT_RESOURCE(extensions);
}

std::string ExtensionsModule::moduleName() const
{
    return "extensions";
}

void ExtensionsModule::registerExports()
{
    framework::ioc()->registerExport<IExtensionsConfiguration>(moduleName(), m_extensionsConfiguration);
    framework::ioc()->registerExport<IExtensionsController>(moduleName(), new ExtensionsController());
}

void ExtensionsModule::registerResources()
{
    extensions_init_qrc();
}

void ExtensionsModule::registerUiTypes()
{
    qmlRegisterType<ExtensionListModel>("MuseScore.Extensions", 1, 0, "ExtensionListModel");
}

void ExtensionsModule::onInit()
{
    m_extensionsConfiguration->init();
}
