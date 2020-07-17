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

#include "inspectormodule.h"

#include "internal/inspectorconfiguration.h"
#include "internal/inspectoractioncontroller.h"
#include "internal/inspectoractions.h"

#include "modularity/ioc.h"
#include "actions/iactionsregister.h"

using namespace mu::scene::inspector;
using namespace mu::actions;

static void inspector_init_qrc()
{
    Q_INIT_RESOURCE(inspector);
}

static std::shared_ptr<InspectorActionController> m_inspectorActionController = std::make_shared<InspectorActionController>();

std::string InspectorModule::moduleName() const
{
    return "inspector";
}

void InspectorModule::registerExports()
{
    framework::ioc()->registerExport<IInspectorConfiguration>(moduleName(), new InspectorConfiguration());
}

void InspectorModule::resolveImports()
{
    auto ar = framework::ioc()->resolve<IActionsRegister>(moduleName());
    if (ar) {
        ar->reg(std::make_shared<InspectorActions>());
    }
}

void InspectorModule::registerResources()
{
    inspector_init_qrc();
}

void InspectorModule::onInit()
{
    m_inspectorActionController->init();
}

#include "view/abstractinspectormodel.h"
#include "view/inspectorlistmodel.h"
#include "view/score/internal/pagetypelistmodel.h"

#include "view/widgets/fretcanvas.h"
#include "view/widgets/gridcanvas.h"

#include "types/stemtypes.h"
#include "types/noteheadtypes.h"
#include "types/beamtypes.h"
#include "types/hairpintypes.h"
#include "types/ornamenttypes.h"
#include "types/dynamictypes.h"
#include "types/glissandotypes.h"
#include "types/fermatatypes.h"
#include "types/barlinetypes.h"
#include "types/markertypes.h"
#include "types/keysignaturetypes.h"
#include "types/accidentaltypes.h"
#include "types/fretdiagramtypes.h"
#include "types/pedaltypes.h"
#include "types/texttypes.h"
#include "types/crescendotypes.h"
#include "types/articulationtypes.h"
#include "types/ambitustypes.h"
#include "types/chordsymboltypes.h"
#include "types/scoreappearancetypes.h"
#include "types/bendtypes.h"
#include "types/tremolobartypes.h"

void InspectorModule::registerUiTypes()
{
    qmlRegisterType<InspectorListModel>("MuseScore.Inspector", 3, 3, "InspectorListModel");
    qmlRegisterType<PageTypeListModel>("MuseScore.Inspector", 3, 3, "PageTypeListModel");

    qmlRegisterUncreatableType<AbstractInspectorModel>("MuseScore.Inspector", 3, 3, "Inspector",
                                                       "Not creatable as it is an enum type");
    qmlRegisterUncreatableType<DirectionTypes>("MuseScore.Inspector", 3, 3, "DirectionTypes",
                                               "Not creatable as it is an enum type");
    qmlRegisterUncreatableType<NoteHeadTypes>("MuseScore.Inspector", 3, 3, "NoteHead",
                                              "Not creatable as it is an enum type");
    qmlRegisterUncreatableType<BeamTypes>("MuseScore.Inspector", 3, 3, "Beam", "Not creatable as it is an enum type");
    qmlRegisterUncreatableType<HairpinTypes>("MuseScore.Inspector", 3, 3, "Hairpin",
                                             "Not creatable as it is an enum type");
    qmlRegisterUncreatableType<OrnamentTypes>("MuseScore.Inspector", 3, 3, "OrnamentTypes",
                                              "Not creatable as it is an enum type");
    qmlRegisterUncreatableType<DynamicTypes>("MuseScore.Inspector", 3, 3, "Dynamic",
                                             "Not creatable as it is an enum type");
    qmlRegisterUncreatableType<GlissandoTypes>("MuseScore.Inspector", 3, 3, "Glissando",
                                               "Not creatable as it is an enum type");
    qmlRegisterUncreatableType<FermataTypes>("MuseScore.Inspector", 3, 3, "FermataTypes",
                                             "Not creatable as it is an enum type");
    qmlRegisterUncreatableType<BarlineTypes>("MuseScore.Inspector", 3, 3, "BarlineTypes",
                                             "Not creatable as it is an enum type");
    qmlRegisterUncreatableType<MarkerTypes>("MuseScore.Inspector", 3, 3, "MarkerTypes",
                                            "Not creatable as it is an enum type");
    qmlRegisterUncreatableType<KeySignatureTypes>("MuseScore.Inspector", 3, 3, "KeySignatureTypes",
                                                  "Not creatable as it is an enum type");
    qmlRegisterUncreatableType<AccidentalTypes>("MuseScore.Inspector", 3, 3, "AccidentalTypes",
                                                "Not creatable as it is an enum type");
    qmlRegisterUncreatableType<FretDiagramTypes>("MuseScore.Inspector", 3, 3, "FretDiagramTypes",
                                                 "Not creatable as it is an enum type");
    qmlRegisterUncreatableType<PedalTypes>("MuseScore.Inspector", 3, 3, "PedalTypes",
                                           "Not creatable as it is an enum type");
    qmlRegisterUncreatableType<TextTypes>("MuseScore.Inspector", 3, 3, "TextTypes",
                                          "Not creatable as it is an enum type");
    qmlRegisterUncreatableType<CrescendoTypes>("MuseScore.Inspector", 3, 3, "CrescendoTypes",
                                               "Not creatable as it is an enum type");
    qmlRegisterUncreatableType<ArticulationTypes>("MuseScore.Inspector", 3, 3, "ArticulationTypes",
                                                  "Not creatable as it is an enum type");
    qmlRegisterUncreatableType<AmbitusTypes>("MuseScore.Inspector", 3, 3, "AmbitusTypes",
                                             "Not creatable as it is an enum type");
    qmlRegisterUncreatableType<ChordSymbolTypes>("MuseScore.Inspector", 3, 3, "ChordSymbolTypes",
                                                 "Not creatable as it is an enum type");
    qmlRegisterType<Ms::FretCanvas>("MuseScore.Inspector", 3, 3, "FretCanvas");
    qmlRegisterUncreatableType<ScoreAppearanceTypes>("MuseScore.Inspector", 3, 3, "ScoreAppearanceTypes",
                                                     "Not creatable as it is an enum type");
    qmlRegisterType<Ms::GridCanvas>("MuseScore.Inspector", 3, 3, "GridCanvas");
    qmlRegisterUncreatableType<BendTypes>("MuseScore.Inspector", 3, 3, "BendTypes", "Not creatable as it is an enum type");
    qmlRegisterUncreatableType<TremoloBarTypes>("MuseScore.Inspector", 3, 3, "TremoloBarTypes", "Not creatable as it is an enum type");
}
