
SET(KIT_PYTHON_SCRIPTS
  qCjyxMultiVolumeExplorerModuleHelper.py
  qCjyxMultiVolumeExplorerModuleWidget.py
  qCjyxMultiVolumeExplorerCharts.py
  )

SET(KIT_PYTHON_RESOURCES
  )

#-----------------------------------------------------------------------------
ctkMacroCompilePythonScript(
  TARGET_NAME qCjyxMultiVolumeExplorerModuleWidget
  SCRIPTS "${KIT_PYTHON_SCRIPTS}"
  RESOURCES "${KIT_PYTHON_RESOURCES}"
  DESTINATION_DIR ${CMAKE_BINARY_DIR}/${Cjyx_QTLOADABLEMODULES_PYTHON_LIB_DIR}
  INSTALL_DIR ${Cjyx_INSTALL_QTLOADABLEMODULES_PYTHON_LIB_DIR}
  NO_INSTALL_SUBDIR
  )
