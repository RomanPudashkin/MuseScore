#=============================================================================
#  MuseScore
#  Music Composition & Notation
#
#  Copyright (C) 2020 MuseScore BVBA and others
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License version 2.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#=============================================================================

set(MODULE inspector)

set(MODULE_QRC inspector.qrc)

set(MODULE_QML_IMPORT ${CMAKE_CURRENT_LIST_DIR}/qml)

set(MODULE_SRC
    ${CMAKE_CURRENT_LIST_DIR}/inspectormodule.cpp
    ${CMAKE_CURRENT_LIST_DIR}/inspectormodule.h
    ${CMAKE_CURRENT_LIST_DIR}/iinspectorconfiguration.h
    ${CMAKE_CURRENT_LIST_DIR}/iinspectorinteraction.h
    ${CMAKE_CURRENT_LIST_DIR}/types/accidentaltypes.h
    ${CMAKE_CURRENT_LIST_DIR}/types/ambitustypes.h
    ${CMAKE_CURRENT_LIST_DIR}/types/articulationtypes.h
    ${CMAKE_CURRENT_LIST_DIR}/types/barlinetypes.h
    ${CMAKE_CURRENT_LIST_DIR}/types/beamtypes.h
    ${CMAKE_CURRENT_LIST_DIR}/types/bendtypes.h
    ${CMAKE_CURRENT_LIST_DIR}/types/chordsymboltypes.h
    ${CMAKE_CURRENT_LIST_DIR}/types/crescendotypes.h
    ${CMAKE_CURRENT_LIST_DIR}/types/dynamictypes.h
    ${CMAKE_CURRENT_LIST_DIR}/types/fermatatypes.h
    ${CMAKE_CURRENT_LIST_DIR}/types/fretdiagramtypes.h
    ${CMAKE_CURRENT_LIST_DIR}/types/glissandotypes.h
    ${CMAKE_CURRENT_LIST_DIR}/types/hairpintypes.h
    ${CMAKE_CURRENT_LIST_DIR}/types/iconnames.h
    ${CMAKE_CURRENT_LIST_DIR}/types/keysignaturetypes.h
    ${CMAKE_CURRENT_LIST_DIR}/types/markertypes.h
    ${CMAKE_CURRENT_LIST_DIR}/types/noteheadtypes.h
    ${CMAKE_CURRENT_LIST_DIR}/types/ornamenttypes.h
    ${CMAKE_CURRENT_LIST_DIR}/types/pedaltypes.h
    ${CMAKE_CURRENT_LIST_DIR}/types/scoreappearancetypes.h
    ${CMAKE_CURRENT_LIST_DIR}/types/stemtypes.h
    ${CMAKE_CURRENT_LIST_DIR}/types/texttypes.h
    ${CMAKE_CURRENT_LIST_DIR}/types/tremolobartypes.h
    ${CMAKE_CURRENT_LIST_DIR}/view/abstractinspectormodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/abstractinspectormodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/abstractinspectorproxymodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/abstractinspectorproxymodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/inspectorlistmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/inspectorlistmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/propertyitem.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/propertyitem.h
    ${CMAKE_CURRENT_LIST_DIR}/view/general/generalsettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/general/generalsettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/general/appearance/appearancesettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/general/appearance/appearancesettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/general/playback/playbackproxymodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/general/playback/playbackproxymodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/general/playback/internal_models/arpeggioplaybackmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/general/playback/internal_models/arpeggioplaybackmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/general/playback/internal_models/breathplaybackmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/general/playback/internal_models/breathplaybackmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/general/playback/internal_models/dynamicplaybackmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/general/playback/internal_models/dynamicplaybackmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/general/playback/internal_models/fermataplaybackmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/general/playback/internal_models/fermataplaybackmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/general/playback/internal_models/glissandoplaybackmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/general/playback/internal_models/glissandoplaybackmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/general/playback/internal_models/hairpinplaybackmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/general/playback/internal_models/hairpinplaybackmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/general/playback/internal_models/noteplaybackmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/general/playback/internal_models/noteplaybackmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/score/scoreappearancesettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/score/scoreappearancesettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/score/scoredisplaysettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/score/scoredisplaysettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/score/internal/pagetypelistmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/score/internal/pagetypelistmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/text/textsettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/text/textsettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/widgets/fretcanvas.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/widgets/fretcanvas.h
    ${CMAKE_CURRENT_LIST_DIR}/view/widgets/gridcanvas.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/widgets/gridcanvas.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/notationsettingsproxymodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/notationsettingsproxymodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/accidentals/accidentalsettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/accidentals/accidentalsettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/ambituses/ambitussettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/ambituses/ambitussettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/articulations/articulationsettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/articulations/articulationsettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/barlines/barlinesettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/barlines/barlinesettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/bends/bendsettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/bends/bendsettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/brackets/bracesettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/brackets/bracesettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/brackets/bracketsettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/brackets/bracketsettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/chordsymbols/chordsymbolsettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/chordsymbols/chordsymbolsettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/clefs/clefsettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/clefs/clefsettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/crescendos/crescendosettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/crescendos/crescendosettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/fermatas/fermatasettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/fermatas/fermatasettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/frames/horizontalframesettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/frames/horizontalframesettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/frames/textframesettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/frames/textframesettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/frames/verticalframesettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/frames/verticalframesettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/fretdiagrams/fretdiagramsettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/fretdiagrams/fretdiagramsettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/glissandos/glissandosettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/glissandos/glissandosettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/hairpins/hairpinsettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/hairpins/hairpinsettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/images/imagesettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/images/imagesettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/jumps/jumpsettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/jumps/jumpsettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/keysignatures/keysignaturesettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/keysignatures/keysignaturesettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/markers/markersettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/markers/markersettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/ornaments/ornamentsettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/ornaments/ornamentsettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/pedals/pedalsettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/pedals/pedalsettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/sectionbreaks/sectionbreaksettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/sectionbreaks/sectionbreaksettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/spacers/spacersettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/spacers/spacersettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/staffs/staffsettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/staffs/staffsettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/stafftype/stafftypesettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/stafftype/stafftypesettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/tempos/temposettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/tempos/temposettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/timesignatures/timesignaturesettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/timesignatures/timesignaturesettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/tremolobars/tremolobarsettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/tremolobars/tremolobarsettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/notes/notesettingsproxymodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/notes/notesettingsproxymodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/notes/beams/beamsettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/notes/beams/beamsettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/notes/beams/internal/beammodelistmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/notes/beams/internal/beammodelistmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/notes/beams/internal/beammodesmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/notes/beams/internal/beammodesmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/notes/hooks/hooksettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/notes/hooks/hooksettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/notes/noteheads/noteheadsettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/notes/noteheads/noteheadsettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/notes/noteheads/noteheadtypesmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/notes/noteheads/noteheadtypesmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/notes/stems/stemsettingsmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/notation/notes/stems/stemsettingsmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/internal/elementrepositoryservice.cpp
    ${CMAKE_CURRENT_LIST_DIR}/internal/elementrepositoryservice.h
    ${CMAKE_CURRENT_LIST_DIR}/internal/ielementrepositoryservice.h
    ${CMAKE_CURRENT_LIST_DIR}/internal/inspectoractioncontroller.cpp
    ${CMAKE_CURRENT_LIST_DIR}/internal/inspectoractioncontroller.h
    ${CMAKE_CURRENT_LIST_DIR}/internal/inspectoractions.cpp
    ${CMAKE_CURRENT_LIST_DIR}/internal/inspectoractions.h
    ${CMAKE_CURRENT_LIST_DIR}/internal/inspectorconfiguration.cpp
    ${CMAKE_CURRENT_LIST_DIR}/internal/inspectorconfiguration.h
    )

include(${PROJECT_SOURCE_DIR}/build/module.cmake)
