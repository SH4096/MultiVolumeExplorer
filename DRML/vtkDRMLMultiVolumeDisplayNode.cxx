/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women\"s Hospital (BWH) All Rights Reserved.

See COPYRIGHT.txt
or http://www.cjyx.org/copyright/copyright.txt for details.

Program:   3D Cjyx
Module:    $RCSfile: vtkDRMLMultiVolumeDisplayNode.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

// DRML includes
#include "vtkDRMLMultiVolumeDisplayNode.h"

// VTK includes
#include <vtkAlgorithmOutput.h>
#include <vtkImageAppendComponents.h>
#include <vtkImageData.h>
#include <vtkImageExtractComponents.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkImageThreshold.h>
#include <vtkObjectFactory.h>

// STD includes
#include <sstream>

//------------------------------------------------------------------------------
vtkDRMLNodeNewMacro(vtkDRMLMultiVolumeDisplayNode);

//----------------------------------------------------------------------------
vtkDRMLMultiVolumeDisplayNode::vtkDRMLMultiVolumeDisplayNode()
{
  // Strings
  this->FrameComponent = 0;
  this->ExtractComponent = vtkImageExtractComponents::New();
#if (VTK_MAJOR_VERSION <= 5)
  this->Threshold->SetInput( this->ExtractComponent->GetOutput());
  this->MapToWindowLevelColors->SetInput( this->ExtractComponent->GetOutput());
#else
  this->Threshold->SetInputConnection(this->ExtractComponent->GetOutputPort());
  this->MapToWindowLevelColors->SetInputConnection(this->ExtractComponent->GetOutputPort());
#endif

}

//----------------------------------------------------------------------------
vtkDRMLMultiVolumeDisplayNode::~vtkDRMLMultiVolumeDisplayNode()
{
  this->ExtractComponent->Delete();
}

//----------------------------------------------------------------------------
void vtkDRMLMultiVolumeDisplayNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);

  vtkIndent indent(nIndent);

  std::stringstream ss;
  ss << this->FrameComponent;
  of << indent << " FrameComponent=\"" << ss.str() << "\"";
}

//----------------------------------------------------------------------------
void vtkDRMLMultiVolumeDisplayNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();

  Superclass::ReadXMLAttributes(atts);

  const char* attName;
  const char* attValue;
  while (*atts != NULL) 
    {
    attName = *(atts++);
    attValue = *(atts++);
    if (!strcmp(attName, "FrameComponent")) 
      {
      std::stringstream ss;
      ss << attValue;
      ss >> this->FrameComponent;
      }
    }  
  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
// Copy the node\"s attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID
void vtkDRMLMultiVolumeDisplayNode::Copy(vtkDRMLNode *anode)
{
  int disabledModify = this->StartModify();

  Superclass::Copy(anode);
  vtkDRMLMultiVolumeDisplayNode *node = (vtkDRMLMultiVolumeDisplayNode *) anode;

  this->SetFrameComponent(node->FrameComponent);

  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkDRMLMultiVolumeDisplayNode::PrintSelf(ostream& os, vtkIndent indent)
{
  
  Superclass::PrintSelf(os,indent);

  os << indent << "Frame Component:   " << this->FrameComponent << "\n";

}

//----------------------------------------------------------------------------
#if (VTK_MAJOR_VERSION <= 5)
void vtkDRMLMultiVolumeDisplayNode
::SetInputToImageDataPipeline(vtkImageData *imageData)
{
  this->ExtractComponent->SetInput(imageData);
}
#else
void vtkDRMLMultiVolumeDisplayNode
::SetInputToImageDataPipeline(vtkAlgorithmOutput *imageDataConnection)
{
  this->ExtractComponent->SetInputConnection(imageDataConnection);
}
#endif

//----------------------------------------------------------------------------
#if (VTK_MAJOR_VERSION <= 5)
vtkImageData* vtkDRMLMultiVolumeDisplayNode::GetInputImageData()
{
  return vtkImageData::SafeDownCast(this->ExtractComponent->GetInput());
}
#else
vtkAlgorithmOutput* vtkDRMLMultiVolumeDisplayNode::GetInputImageDataConnection()
{
  return this->ExtractComponent->GetNumberOfInputConnections(0) ?
    this->ExtractComponent->GetInputConnection(0,0) : 0;
}
#endif

//----------------------------------------------------------------------------
#if (VTK_MAJOR_VERSION <= 5)
vtkImageData* vtkDRMLMultiVolumeDisplayNode::GetOutputImageData()
{
  return this->AppendComponents->GetOutput();
}
#else
vtkAlgorithmOutput* vtkDRMLMultiVolumeDisplayNode::GetOutputImageDataConnection()
{
  return this->AppendComponents->GetOutputPort();
}
#endif

//---------------------------------------------------------------------------
vtkImageData* vtkDRMLMultiVolumeDisplayNode::GetScalarImageData()
{
  return vtkImageData::SafeDownCast(this->ExtractComponent->GetOutput());
}

//----------------------------------------------------------------------------
void vtkDRMLMultiVolumeDisplayNode::UpdateImageDataPipeline()
{
  this->ExtractComponent->SetComponents(this->GetFrameComponent());
  this->Superclass::UpdateImageDataPipeline();
}
