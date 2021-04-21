set (DOCK_LIBS
    kddockwidgets
)

set (DOCKWINDOW_SRC
    ${CMAKE_CURRENT_LIST_DIR}/docksetup.cpp
    ${CMAKE_CURRENT_LIST_DIR}/docksetup.h
    ${CMAKE_CURRENT_LIST_DIR}/docktypes.h
    ${CMAKE_CURRENT_LIST_DIR}/dockwindow.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dockwindow.h
    ${CMAKE_CURRENT_LIST_DIR}/dockpage.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dockpage.h
    ${CMAKE_CURRENT_LIST_DIR}/dockpanel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dockpanel.h
    ${CMAKE_CURRENT_LIST_DIR}/dockstatusbar.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dockstatusbar.h
    ${CMAKE_CURRENT_LIST_DIR}/docktoolbar.cpp
    ${CMAKE_CURRENT_LIST_DIR}/docktoolbar.h
    ${CMAKE_CURRENT_LIST_DIR}/dockcentral.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dockcentral.h
    ${CMAKE_CURRENT_LIST_DIR}/internal/dockbase.cpp
    ${CMAKE_CURRENT_LIST_DIR}/internal/dockbase.h
    ${CMAKE_CURRENT_LIST_DIR}/internal/dropindicators.cpp
    ${CMAKE_CURRENT_LIST_DIR}/internal/dropindicators.h
    ${CMAKE_CURRENT_LIST_DIR}/internal/indicatorswindow.cpp
    ${CMAKE_CURRENT_LIST_DIR}/internal/indicatorswindow.h
    ${CMAKE_CURRENT_LIST_DIR}/internal/toolbargripmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/internal/toolbargripmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/internal/dockseparator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/internal/dockseparator.h
)

