/*==============================================================================

  Program: 3D Cjyx

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.cjyx.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// Qt includes
#include <QFileInfo>
#include <QScopedPointer>

// Cjyx includes
#include <vtkCjyxVolumesLogic.h>

// MultiVolumeExplorer Logic includes
#include <vtkCjyxMultiVolumeExplorerLogic.h>

// MultiVolumeExplorer includes
#include "qCjyxMultiVolumeExplorerModule.h"

// CjyxQT includes
#include <qCjyxUtils.h>
#include <qCjyxModuleManager.h>
#include <qCjyxScriptedLoadableModuleWidget.h>
#include <qCjyxApplication.h>
#include <vtkCjyxConfigure.h>

//-----------------------------------------------------------------------------
#include <QtGlobal>
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
#include <QtPlugin>
Q_EXPORT_PLUGIN2(qCjyxMultiVolumeExplorerModule, qCjyxMultiVolumeExplorerModule);
#endif

//-----------------------------------------------------------------------------
/// \ingroup Cjyx_QtModules_ExtensionTemplate
class qCjyxMultiVolumeExplorerModulePrivate
{
public:
  qCjyxMultiVolumeExplorerModulePrivate();
};

//-----------------------------------------------------------------------------
// qCjyxMultiVolumeExplorerModulePrivate methods

//-----------------------------------------------------------------------------
qCjyxMultiVolumeExplorerModulePrivate::qCjyxMultiVolumeExplorerModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qCjyxMultiVolumeExplorerModule methods

//-----------------------------------------------------------------------------
qCjyxMultiVolumeExplorerModule::qCjyxMultiVolumeExplorerModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qCjyxMultiVolumeExplorerModulePrivate)
{
}

//-----------------------------------------------------------------------------
qCjyxMultiVolumeExplorerModule::~qCjyxMultiVolumeExplorerModule()
{
}

//-----------------------------------------------------------------------------
QString qCjyxMultiVolumeExplorerModule::helpText()const
{
  return "MultiVolumeExplorer module supports various operations related to examining multivolume nodes.";
}

//-----------------------------------------------------------------------------
QString qCjyxMultiVolumeExplorerModule::acknowledgementText()const
{
  return "Development of this module was supported in part by "
      "the following grants: P41EB015898, P41RR019703, R01CA111288 "
      "and U01CA151261.";
}

//-----------------------------------------------------------------------------
QIcon qCjyxMultiVolumeExplorerModule::icon()const
{
  return QIcon(":/Icons/MultiVolumeExplorer.png");
}

//-----------------------------------------------------------------------------
QStringList qCjyxMultiVolumeExplorerModule::categories() const
{
  return QStringList() << "MultiVolume Support";
}

//-----------------------------------------------------------------------------
QStringList qCjyxMultiVolumeExplorerModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
QStringList qCjyxMultiVolumeExplorerModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("Andrey Fedorov (SPL, BWH)");
  moduleContributors << QString("Jean-Christophe Fillion-Robin (Kitware)");
  moduleContributors << QString("Julien Finet (Kitware)");
  moduleContributors << QString("Steve Pieper (SPL, BWH)");
  moduleContributors << QString("Ron Kikinis (SPL, BWH)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
void qCjyxMultiVolumeExplorerModule::setup()
{
  this->Superclass::setup();

  // Register the IO module for loading MultiVolumes as a variant of nrrd files
  qCjyxAbstractCoreModule* volumes = qCjyxApplication::application()->moduleManager()->module("Volumes");
  if (volumes)
    {
    vtkCjyxVolumesLogic* volumesLogic 
      = dynamic_cast<vtkCjyxVolumesLogic*>(volumes->logic());
    vtkCjyxMultiVolumeExplorerLogic* logic
      = dynamic_cast<vtkCjyxMultiVolumeExplorerLogic*>(this->logic());
    if (volumesLogic && logic)
      {
      logic->RegisterArchetypeVolumeNodeSetFactory( volumesLogic );
      }
    }
}

//-----------------------------------------------------------------------------
qCjyxAbstractModuleRepresentation * qCjyxMultiVolumeExplorerModule::createWidgetRepresentation()
{
  QString pythonPath = qCjyxUtils::pathWithoutIntDir(
              QFileInfo(this->path()).path(), Cjyx_QTLOADABLEMODULES_LIB_DIR);

  QScopedPointer<qCjyxScriptedLoadableModuleWidget> widget(new qCjyxScriptedLoadableModuleWidget);
  QString classNameToLoad = "qCjyxMultiVolumeExplorerModuleWidget";
  bool ret = widget->setPythonSource(
        pythonPath + "/Python/" + classNameToLoad + ".py", classNameToLoad);
  if (!ret)
    {
    return 0;
    }
  return widget.take();
}

//-----------------------------------------------------------------------------
vtkDRMLAbstractLogic* qCjyxMultiVolumeExplorerModule::createLogic()
{
  return vtkCjyxMultiVolumeExplorerLogic::New();
}
