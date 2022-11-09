/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women\"s Hospital (BWH) All Rights Reserved.

See COPYRIGHT.txt
or http://www.cjyx.org/copyright/copyright.txt for details.

Program:   3D Cjyx
Module:    $RCSfile: vtkDRMLMultiVolumeNode.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

// VTK includes
#include <vtkCommand.h>
#include <vtkDoubleArray.h>
#include <vtkObjectFactory.h>

// DRML includes
#include <vtkDRMLMultiVolumeDisplayNode.h>
#include <vtkDRMLVolumeNode.h>
#include <vtkDRMLMultiVolumeStorageNode.h>

// CropModuleDRML includes
#include <vtkDRMLMultiVolumeNode.h>

// STD includes
#include <algorithm>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

//----------------------------------------------------------------------------
vtkDRMLNodeNewMacro(vtkDRMLMultiVolumeNode);

//----------------------------------------------------------------------------
vtkDRMLMultiVolumeNode::vtkDRMLMultiVolumeNode()
{
  this->HideFromEditors = 0;
}

//----------------------------------------------------------------------------
vtkDRMLMultiVolumeNode::~vtkDRMLMultiVolumeNode()
{
}

//----------------------------------------------------------------------------
vtkDoubleArray* vtkDRMLMultiVolumeNode::GetLabelArray()
{
  return this->LabelArray;
}

//----------------------------------------------------------------------------
std::string vtkDRMLMultiVolumeNode::GetLabelName()
{
  return this->LabelName;
}

//----------------------------------------------------------------------------
void vtkDRMLMultiVolumeNode::SetLabelName(const std::string& name)
{
  this->LabelName = name;
}

//----------------------------------------------------------------------------
void vtkDRMLMultiVolumeNode::SetLabelArray(vtkDoubleArray* arr)
{
  if (!this->LabelArray)
    {
    this->LabelArray = vtkSmartPointer<vtkDoubleArray>::New();
    this->LabelArray->Allocate(arr->GetNumberOfTuples());
    this->LabelArray->SetNumberOfTuples(arr->GetNumberOfTuples());
    this->LabelArray->SetNumberOfComponents(1);
    }
  for (int i=0; i<arr->GetNumberOfTuples(); ++i)
    {
    this->LabelArray->SetComponent(i, 0, arr->GetComponent(i, 0));
    }
}

//----------------------------------------------------------------------------
void vtkDRMLMultiVolumeNode::ReadXMLAttributes(const char** atts)
{
  this->Superclass::ReadXMLAttributes(atts);

  const char* attName;
  const char* attValue;
  while (*atts != NULL)
    {
    attName = *(atts++);
    attValue = *(atts++);
    if (!strcmp(attName, "LabelArray"))
      {
      std::vector<double> labels;
      char* str = (char*)attValue;
      char* pch = strtok(str, " ");
      while(pch)
        {
        labels.push_back(atof(pch));
        pch = strtok(NULL," ");
        }
      std::cout << "Number of elements found: " << labels.size() << std::endl;
      if (!this->LabelArray)
        {
        this->LabelArray = vtkSmartPointer<vtkDoubleArray>::New();
        }
      this->LabelArray->SetNumberOfTuples(labels.size());
      this->LabelArray->SetNumberOfComponents(1);
      for (unsigned int i = 0; i < labels.size(); ++i)
        {
        std::cout << "Setting " << i << " to " << labels[i] << std::endl;
        this->LabelArray->SetComponent(i, 0, labels[i]);
        }
      continue;
      }
    if (!strcmp(attName, "LabelName"))
      {
      this->LabelName = attValue;
      continue;
      }
    }
  this->WriteXML(std::cout,1);
}

//----------------------------------------------------------------------------
void vtkDRMLMultiVolumeNode::WriteXML(ostream& of, int nIndent)
{
  this->Superclass::WriteXML(of, nIndent);

  vtkIndent indent(nIndent);
  if (this->LabelArray)
    {
    int nItems = this->LabelArray->GetNumberOfTuples();
    of << indent << " LabelArray=\"";
    for (int i=0; i < nItems; ++i)
      {
      of << indent << this->LabelArray->GetComponent(i, 0) << indent;
      }
    of << indent << "\"";
    }
  of << indent << " LabelName=\"" << this->LabelName << "\"";
}

//----------------------------------------------------------------------------
void vtkDRMLMultiVolumeNode::Copy(vtkDRMLNode *anode)
{
  vtkDRMLMultiVolumeNode *multiVolumeNode = vtkDRMLMultiVolumeNode::SafeDownCast(anode);
  if (!multiVolumeNode)
    {
    return;
    }

  this->Superclass::Copy(anode);

  if (multiVolumeNode->LabelArray)
    {
    vtkDoubleArray *arr = multiVolumeNode->LabelArray;
    if (arr)
      {
      if (!this->LabelArray)
        {
        this->LabelArray = vtkSmartPointer<vtkDoubleArray>::New();
        }
      this->LabelArray->SetNumberOfTuples(arr->GetNumberOfTuples());
      this->LabelArray->SetNumberOfComponents(1);
      for (int i = 0; i < arr->GetNumberOfTuples(); ++i)
        {
        this->LabelArray->SetComponent(i, 0, arr->GetComponent(i, 0));
        }
      }
    }
  this->LabelName = multiVolumeNode->LabelName;
}

//----------------------------------------------------------------------------
void vtkDRMLMultiVolumeNode::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  if (this->LabelArray)
    {
    os << "LabelArray: ";
    for (int i = 0; i < this->LabelArray->GetNumberOfTuples(); ++i)
      {
      os << this->LabelArray->GetComponent(i, 0) << " ";
      }
    os << std::endl;
    }
  os << "LabelName: " << this->LabelName << std::endl;
}

vtkDRMLMultiVolumeDisplayNode* vtkDRMLMultiVolumeNode::GetMultiVolumeDisplayNode()
{
  return vtkDRMLMultiVolumeDisplayNode::SafeDownCast(this->GetDisplayNode());
}

vtkDRMLStorageNode* vtkDRMLMultiVolumeNode::CreateDefaultStorageNode()
{
  return vtkDRMLMultiVolumeStorageNode::New();
}

void vtkDRMLMultiVolumeNode::CreateDefaultDisplayNodes()
{
  vtkDRMLMultiVolumeDisplayNode *displayNode = 
    vtkDRMLMultiVolumeDisplayNode::SafeDownCast(this->GetDisplayNode());
  if(displayNode == NULL)
  {
    displayNode = vtkDRMLMultiVolumeDisplayNode::New();
    if(this->GetScene())
    {
      displayNode->SetScene(this->GetScene());
      this->GetScene()->AddNode(displayNode);
      displayNode->SetDefaultColorMap();
      displayNode->Delete();

      this->SetAndObserveDisplayNodeID(displayNode->GetID());
      std::cout << "Display node set and observed" << std::endl;
    }
  }
}

void vtkDRMLMultiVolumeNode::SetNumberOfFrames(int nf)
{
  this->NumberOfFrames = nf;
}
