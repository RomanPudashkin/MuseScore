/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-Studio-CLA-applies
 *
 * MuseScore Studio
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore Limited
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

#ifndef MU_APPSHELL_APPSHELLMODULE_H
#define MU_APPSHELL_APPSHELLMODULE_H

#include <memory>

#include "modularity/imodulesetup.h"

namespace mu::appshell {
class ApplicationActionController;
class ApplicationUiActions;
class AppShellConfiguration;
class SessionsManager;
#if QT_VERSION < QT_VERSION_CHECK(6, 9, 0) && defined(Q_OS_MAC)
class MacOSScrollingHook;
#endif
class AppShellModule : public muse::modularity::IModuleSetup
{
public:

    std::string moduleName() const override;

    void registerExports() override;
    void resolveImports() override;

    void registerResources() override;
    void registerUiTypes() override;

    void onPreInit(const muse::IApplication::RunMode& mode) override;
    void onInit(const muse::IApplication::RunMode& mode) override;
    void onAllInited(const muse::IApplication::RunMode& mode) override;
    void onDeinit() override;

private:
    std::shared_ptr<ApplicationActionController> m_applicationActionController;
    std::shared_ptr<ApplicationUiActions> m_applicationUiActions;
    std::shared_ptr<AppShellConfiguration> m_appShellConfiguration;
    std::shared_ptr<SessionsManager> m_sessionsManager;

#if QT_VERSION < QT_VERSION_CHECK(6, 9, 0) && defined(Q_OS_MAC)
    std::shared_ptr<MacOSScrollingHook> m_scrollingHook;
#endif
};
}

#endif // MU_APPSHELL_APPSHELLMODULE_H
