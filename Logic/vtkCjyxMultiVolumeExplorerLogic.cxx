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

// Cjyx includes
#include <vtkCjyxColorLogic.h>
#include <vtkCjyxVolumesLogic.h>

// MultiVolumeExplorer includes
#include "vtkCjyxMultiVolumeExplorerLogic.h"

// DRML includes
#include <vtkDRMLScalarVolumeDisplayNode.h>
#include <vtkDRMLScalarVolumeNode.h>
#include <vtkDRMLMultiVolumeNode.h>
#include <vtkDRMLMultiVolumeDisplayNode.h>
#include <vtkDRMLMultiVolumeStorageNode.h>
#include <vtkDRMLVolumeArchetypeStorageNode.h>

// VTK includes
#include <vtkDoubleArray.h>
#include "vtkObjectFactory.h"
#include <vtkStringArray.h>
#include <vtkNew.h>

// ITK includes
#include <itkGDCMImageIO.h>
#include <itkGDCMSeriesFileNames.h>
#include <itkImage.h>
#include <itkImageSeriesReader.h>

// STD includes
#include <cassert>

// DCMTK includes
#include <dcmtk/dcmdata/dcmetinf.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcuid.h>
#include <dcmtk/dcmdata/dcdict.h>
#include <dcmtk/dcmdata/cmdlnarg.h>
#include <dcmtk/ofstd/ofconapp.h>
#include <dcmtk/ofstd/ofstd.h>
#include <dcmtk/ofstd/ofdatime.h>
#include <dcmtk/dcmdata/dcuid.h>         /* for dcmtk version name */
#include <dcmtk/dcmdata/dcdeftag.h>      /* for DCM_StudyInstanceUID */

// STD includes
#include <sys/types.h>
#include <errno.h>


//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkCjyxMultiVolumeExplorerLogic);

//----------------------------------------------------------------------------
vtkCjyxMultiVolumeExplorerLogic::vtkCjyxMultiVolumeExplorerLogic()
{
}

//----------------------------------------------------------------------------
vtkCjyxMultiVolumeExplorerLogic::~vtkCjyxMultiVolumeExplorerLogic()
{
}

//----------------------------------------------------------------------------
void vtkCjyxMultiVolumeExplorerLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkCjyxMultiVolumeExplorerLogic::InitializeEventListeners()
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkDRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkDRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkDRMLScene::EndBatchProcessEvent);
  this->SetAndObserveDRMLSceneEventsInternal(this->GetDRMLScene(), events.GetPointer());
}

//---------------------------------------------------------------------------
void vtkCjyxMultiVolumeExplorerLogic::UpdateFromDRMLScene()
{
  assert(this->GetDRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkCjyxMultiVolumeExplorerLogic
::OnDRMLSceneNodeAdded(vtkDRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
void vtkCjyxMultiVolumeExplorerLogic
::OnDRMLSceneNodeRemoved(vtkDRMLNode* vtkNotUsed(node))
{
}

namespace {

ArchetypeVolumeNodeSet MultiVolumeNodeSetFactory(std::string& volumeName, vtkDRMLScene* scene, int options)
{
  ArchetypeVolumeNodeSet nodeSet(scene);

  // set up the scalar node's support nodes
  vtkNew<vtkDRMLMultiVolumeNode> multiVolumeNode;
  multiVolumeNode->SetName(volumeName.c_str());
  nodeSet.Scene->AddNode(multiVolumeNode.GetPointer());

  vtkNew<vtkDRMLMultiVolumeDisplayNode> mdisplayNode;
  nodeSet.Scene->AddNode(mdisplayNode.GetPointer());
  multiVolumeNode->SetAndObserveDisplayNodeID(mdisplayNode->GetID());

  vtkNew<vtkDRMLMultiVolumeStorageNode> storageNode;
  storageNode->SetCenterImage(options & vtkCjyxVolumesLogic::CenterImage);
  nodeSet.Scene->AddNode(storageNode.GetPointer());
  multiVolumeNode->SetAndObserveStorageNodeID(storageNode->GetID());

  nodeSet.StorageNode = storageNode.GetPointer();
  nodeSet.DisplayNode = mdisplayNode.GetPointer();
  nodeSet.Node = multiVolumeNode.GetPointer();

  return nodeSet;
}

};

//----------------------------------------------------------------------------
void vtkCjyxMultiVolumeExplorerLogic::RegisterArchetypeVolumeNodeSetFactory(vtkCjyxVolumesLogic* volumesLogic)
{
  if (volumesLogic)
    {
    volumesLogic->PreRegisterArchetypeVolumeNodeSetFactory(MultiVolumeNodeSetFactory);
    }
}

//----------------------------------------------------------------------------
void vtkCjyxMultiVolumeExplorerLogic::RegisterNodes()
{
  if(!this->GetDRMLScene())
    {
    return;
    }
  this->GetDRMLScene()->RegisterNodeClass(vtkNew<vtkDRMLMultiVolumeNode>().GetPointer());
  this->GetDRMLScene()->RegisterNodeClass(vtkNew<vtkDRMLMultiVolumeDisplayNode>().GetPointer());
  this->GetDRMLScene()->RegisterNodeClass(vtkNew<vtkDRMLMultiVolumeStorageNode>().GetPointer());
}

//----------------------------------------------------------------------------
int vtkCjyxMultiVolumeExplorerLogic
::ProcessDICOMSeries(std::string dir, std::string outputDir,
                     std::string dcmTag, vtkDoubleArray* tagValues)
{
  // this function takes on input the location of a directory that stores a single
  //  DICOM series and a tag used to separate individual subvolumes from that series.
  // It saves the individual subvolumes in the output directory ordered by alpha, and
  //  returns the number of subvolumes and the values of tags for each one.

  typedef itk::GDCMImageIO ImageIOType;
  typedef itk::GDCMSeriesFileNames InputNamesGeneratorType;
  typedef short PixelValueType;
  typedef itk::Image< PixelValueType, 3 > VolumeType;
  typedef itk::ImageSeriesReader< VolumeType > ReaderType;

  int i, j;

  std::cout << "Processing directory " << dir << std::endl;

  // each directory is handled dependent on the input series type
  ImageIOType::Pointer gdcmIO = ImageIOType::New();
  gdcmIO->LoadPrivateTagsOff();

  InputNamesGeneratorType::Pointer inputNames = InputNamesGeneratorType::New();
  inputNames->SetUseSeriesDetails(true);
  inputNames->SetDirectory(dir);

  itk::SerieUIDContainer seriesUIDs = inputNames->GetSeriesUIDs();
  int nSeriesUIDs = seriesUIDs.size();

  if(nSeriesUIDs != 1)
    {
    std::cerr << "Only one series is allowed!" << std::endl;
    return -1;
    }

  const ReaderType::FileNamesContainer & filenames =
      inputNames->GetFileNames(seriesUIDs[0]);
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetImageIO( gdcmIO );
  reader->SetFileNames( filenames );

  try
    {
    std::cout << "Splitting series.... updating reader." << std::endl;
    reader->Update();
    }
  catch (itk::ExceptionObject &excp)
    {
    std::cout << "Error encountered: exiting." << std::endl;
    std::cerr << "Exception thrown while reading the series" << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  ReaderType::DictionaryArrayRawPointer inputDict =
      reader->GetMetaDataDictionaryArray();
  int nSlices = inputDict->size();

  nSlices = filenames.size();
  //std::string sortTag = "0018|1060"; // DCE GE: trigger time
  std::string sortTag = dcmTag;
  std::string tagVal;
  std::map<int,ReaderType::FileNamesContainer> tagVal2fileList;

  for(j = 0; j < nSlices; ++j)
    {
    //std::cout << "\n\n\n\n\n Processing slice " << j << std::endl;

    itk::ExposeMetaData<std::string>(*(*inputDict)[j], sortTag, tagVal);
    //std::cout << "Tag value found: " << tagVal << "(" << tagVal2fileList.size() << ")" << " ";
    tagVal2fileList[atoi(tagVal.c_str())].push_back(filenames[j]);
    }

  // map items should be sorted by key
  tagValues->SetNumberOfComponents(1);
  tagValues->SetNumberOfTuples(tagVal2fileList.size());
  tagValues->Allocate(tagVal2fileList.size());

  i = 0;

  for(std::map<int,ReaderType::FileNamesContainer>::const_iterator
    it=tagVal2fileList.begin(); it!=tagVal2fileList.end(); ++it,++i)
    {

    std::ostringstream tagValStr;
    double tagVal = (*it).first;
    tagValStr  << tagVal;

    char fname[255];
    sprintf(fname, "%s/%08i.nrrd", outputDir.c_str(), i);
    std::string seriesFileName(fname);

    StoreVolumeNode((*it).second, seriesFileName);

    tagValues->SetComponent(i, 0, tagVal);
    }

  return tagVal2fileList.size();
}

//----------------------------------------------------------------------------
void vtkCjyxMultiVolumeExplorerLogic
::StoreVolumeNode(const std::vector<std::string>& filenames,
                  const std::string& seriesFileName)
{
  vtkDRMLVolumeArchetypeStorageNode* sNode =
    vtkDRMLVolumeArchetypeStorageNode::New();
  vtkDRMLScalarVolumeNode *vNode =
    vtkDRMLScalarVolumeNode::New();
  sNode->SetFileName(filenames[0].c_str());
  sNode->ResetFileNameList();
  for(std::vector<std::string>::const_iterator
    it=filenames.begin();it!=filenames.end();++it)
    sNode->AddFileName(it->c_str());
  sNode->SetSingleFile(0);
  sNode->ReadData(vNode);

  sNode->SetFileName(seriesFileName.c_str());
  sNode->SetWriteFileFormat("nrrd");
  sNode->SetURI(NULL);
  sNode->WriteData(vNode);
  sNode->Delete();
  vNode->Delete();
}
