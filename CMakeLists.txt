
cmake_minimum_required(VERSION 3.13.0)

#-----------------------------------------------------------------------------
set(MODULE_NAME MultiVolumeExplorer)
set(MODULE_TITLE ${MODULE_NAME})

string(TOUPPER ${MODULE_NAME} MODULE_NAME_UPPER)

#-----------------------------------------------------------------------------
add_subdirectory(DRML)
add_subdirectory(Logic)
add_subdirectory(Py)

#-----------------------------------------------------------------------------
set(MODULE_EXPORT_DIRECTIVE "Q_CJYX_QTMODULES_${MODULE_NAME_UPPER}_EXPORT")

# Additional includes - Current_{source,binary} and Cjyx_{Libs,Base} already included
set(MODULE_INCLUDE_DIRECTORIES
  ${CMAKE_CURRENT_SOURCE_DIR}/Logic
  ${CMAKE_CURRENT_BINARY_DIR}/Logic
  ${CMAKE_CURRENT_SOURCE_DIR}/DRML
  ${CMAKE_CURRENT_BINARY_DIR}/DRML
  )

set(MODULE_SRCS
  qCjyxMultiVolumeExplorerModule.cxx
  qCjyxMultiVolumeExplorerModule.h
  )

set(MODULE_MOC_SRCS
  qCjyxMultiVolumeExplorerModule.h
  )

set(MODULE_UI_SRCS
)

set(MODULE_TARGET_LIBRARIES
  vtkCjyxMultiVolumeExplorerModuleDRML
  vtkCjyxMultiVolumeExplorerModuleLogic
  )

set(MODULE_RESOURCES
  Resources/qCjyxMultiVolumeExplorerModule.qrc
)

#-----------------------------------------------------------------------------
cjyxMacroBuildQtModule(
  NAME ${MODULE_NAME}
  EXPORT_DIRECTIVE ${MODULE_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${MODULE_INCLUDE_DIRECTORIES}
  SRCS ${MODULE_SRCS}
  MOC_SRCS ${MODULE_MOC_SRCS}
  UI_SRCS ${MODULE_UI_SRCS}
  TARGET_LIBRARIES ${MODULE_TARGET_LIBRARIES}
  RESOURCES ${MODULE_RESOURCES}
  )

#-----------------------------------------------------------------------------
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
