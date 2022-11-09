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

#ifndef __qCjyxMultiVolumeExplorerModule_h
#define __qCjyxMultiVolumeExplorerModule_h

// CjyxQt includes
#include <qCjyxLoadableModule.h>

#include "qCjyxMultiVolumeExplorerModuleExport.h"

class qCjyxMultiVolumeExplorerModulePrivate;

/// \ingroup Cjyx_QtModules_ExtensionTemplate
class Q_CJYX_QTMODULES_MULTIVOLUMEEXPLORER_EXPORT qCjyxMultiVolumeExplorerModule :
  public qCjyxLoadableModule
{
  Q_OBJECT
#ifdef Cjyx_HAVE_QT5
  Q_PLUGIN_METADATA(IID "org.cjyx.modules.loadable.qCjyxLoadableModule/1.0");
#endif
  Q_INTERFACES(qCjyxLoadableModule);

public:

  typedef qCjyxLoadableModule Superclass;
  explicit qCjyxMultiVolumeExplorerModule(QObject *parent=0);
  virtual ~qCjyxMultiVolumeExplorerModule();

  qCjyxGetTitleMacro(QTMODULE_TITLE);

  /// Help to use the module
  virtual QString helpText() const override;

  /// Return acknowledgements
  virtual QString acknowledgementText() const override;

  /// Return a custom icon for the module
  virtual QIcon icon() const override;

  /// Return the categories
  virtual QStringList categories() const override;

  /// Return the dependencies
  virtual QStringList dependencies() const override;

  /// Return the contributors
  virtual QStringList contributors() const override;

protected:

  /// Initialize the module. Register the volumes reader/writer
  virtual void setup() override;

  /// Create and return the widget representation associated to this module
  virtual qCjyxAbstractModuleRepresentation * createWidgetRepresentation() override;

  /// Create and return the logic associated to this module
  virtual vtkDRMLAbstractLogic* createLogic() override;

protected:
  QScopedPointer<qCjyxMultiVolumeExplorerModulePrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qCjyxMultiVolumeExplorerModule);
  Q_DISABLE_COPY(qCjyxMultiVolumeExplorerModule);

};

#endif
