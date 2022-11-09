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

// .NAME vtkCjyxMultiVolumeExplorerLogic - cjyx logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkCjyxMultiVolumeExplorerLogic_h
#define __vtkCjyxMultiVolumeExplorerLogic_h

// Cjyx includes
#include <vtkCjyxModuleLogic.h>

// DRML includes

// STD includes
#include <cstdlib>

#include "vtkCjyxMultiVolumeExplorerModuleLogicExport.h"

class vtkDoubleArray;
class vtkStringArray;
class vtkDRMLScalarVolumeNode;
class vtkDRMLVolumeArchetypeStorageNode;
class vtkDRMLScalarVolumeDisplayNode;
class vtkDRMLMultiVolumeNode;
class vtkCjyxVolumesLogic;

/// \ingroup Cjyx_QtModules_ExtensionTemplate
class VTK_CJYX_MULTIVOLUMEEXPLORER_MODULE_LOGIC_EXPORT vtkCjyxMultiVolumeExplorerLogic :
  public vtkCjyxModuleLogic
{
public:

  static vtkCjyxMultiVolumeExplorerLogic *New();
  vtkTypeMacro(vtkCjyxMultiVolumeExplorerLogic, vtkCjyxModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /// Initialize listening to DRML events
  void InitializeEventListeners();
  int ProcessDICOMSeries(std::string inputDir, std::string outputDir,
                         std::string dcmTag, vtkDoubleArray*);

  /// Register the factory that the MultiVolume needs to manage nrrd
  /// file with the specified volumes logic
  void RegisterArchetypeVolumeNodeSetFactory(vtkCjyxVolumesLogic* volumesLogic);

protected:
  vtkCjyxMultiVolumeExplorerLogic();
  virtual ~vtkCjyxMultiVolumeExplorerLogic();

  /// Register DRML Node classes to Scene. Gets called automatically when the DRMLScene is attached to this logic class.
  virtual void RegisterNodes() override;

  virtual void UpdateFromDRMLScene() override;
  virtual void OnDRMLSceneNodeAdded(vtkDRMLNode* node) override;
  virtual void OnDRMLSceneNodeRemoved(vtkDRMLNode* node) override;
  void StoreVolumeNode(const std::vector<std::string>& filenames,
                       const std::string& seriesFileName);
private:

  vtkCjyxMultiVolumeExplorerLogic(const vtkCjyxMultiVolumeExplorerLogic&); // Not implemented
  void operator=(const vtkCjyxMultiVolumeExplorerLogic&);               // Not implemented

};

#endif
