/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.cjyx.org/copyright/copyright.txt for details.

  Program:   3D Cjyx
  Module:    $RCSfile: vtkDRMLVolumeRenderingParametersNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/
// .NAME vtkDRMLVolumeRenderingParametersNode - DRML node for storing a slice through RAS space
// .SECTION Description
// This node stores the information about the currently selected volume
//
//

#ifndef __vtkDRMLMultiVolumeNode_h
#define __vtkDRMLMultiVolumeNode_h

// MultiVolumeExplorer includes
#include <vtkCjyxMultiVolumeExplorerModuleDRMLExport.h>

// DRML includes
#include <vtkDRML.h>
#include <vtkDRMLScene.h>
#include <vtkDRMLScalarVolumeNode.h>

// VTK includes
#include <vtkDoubleArray.h>
#include <vtkSmartPointer.h>

class vtkDRMLMultiVolumeDisplayNode;

/// \ingroup Cjyx_QtModules_MultiVolumeNode
class VTK_CJYX_MULTIVOLUMEEXPLORER_MODULE_DRML_EXPORT vtkDRMLMultiVolumeNode : public vtkDRMLScalarVolumeNode
{
  public:

  static vtkDRMLMultiVolumeNode *New();
  vtkTypeMacro(vtkDRMLMultiVolumeNode,vtkDRMLScalarVolumeNode);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  virtual vtkDRMLNode* CreateNodeInstance() override;

  /// Set node attributes
  virtual void ReadXMLAttributes( const char** atts) override;

  /// Write this node's information to a DRML file in XML format.
  virtual void WriteXML(ostream& of, int indent) override;

  /// Copy the node's attributes to this object
  virtual void Copy(vtkDRMLNode *node) override;

  /// Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() override {return "DRMLMultiVolume";};

  /// Update the stored reference to another node in the scene
  //virtual void UpdateReferenceID(const char *oldID, const char *newID);

  /// Updates this node if it depends on other nodes
  /// when the node is deleted in the scene
  //virtual void UpdateReferences();

  // Description:
  //virtual void UpdateScene(vtkDRMLScene *scene);

  //virtual void ProcessDRMLEvents ( vtkObject *caller, unsigned long event, void *callData);
  
  void SetLabelArray(vtkDoubleArray*);
  vtkDoubleArray* GetLabelArray();

  std::string GetLabelName();
  void SetLabelName(const std::string& n);

  virtual vtkDRMLStorageNode* CreateDefaultStorageNode() override;
  virtual void CreateDefaultDisplayNodes() override;
  
  vtkDRMLMultiVolumeDisplayNode* GetMultiVolumeDisplayNode();

  void SetNumberOfFrames(int val);
  vtkGetMacro(NumberOfFrames, int);

protected:
  vtkDRMLMultiVolumeNode();
  virtual ~vtkDRMLMultiVolumeNode();

  vtkDRMLMultiVolumeNode(const vtkDRMLMultiVolumeNode&);
  void operator=(const vtkDRMLMultiVolumeNode&);

  vtkSmartPointer<vtkDoubleArray> LabelArray;
  std::string LabelName;

  int NumberOfFrames;
};

#endif

