/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.cjyx.org/copyright/copyright.txt for details.

  Program:   3D Cjyx
  Module:    $RCSfile: vtkDRMLNRRDStorageNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.8 $

=========================================================================auto=*/
///  vtkDRMLMultiVolumeStorageNode - DRML node for representing a
///  multi value volume storage
/// 

#ifndef __vtkDRMLMultiVolumeStorageNode_h
#define __vtkDRMLMultiVolumeStorageNode_h

// MultiVolumeExplorer includes
#include <vtkCjyxMultiVolumeExplorerModuleDRMLExport.h>

#include "vtkDRMLNRRDStorageNode.h"

/// \ingroup Cjyx_QtModules_MultiVolumeNode
class VTK_CJYX_MULTIVOLUMEEXPLORER_MODULE_DRML_EXPORT vtkDRMLMultiVolumeStorageNode : public vtkDRMLNRRDStorageNode
{
  public:

  static vtkDRMLMultiVolumeStorageNode *New();
  vtkTypeMacro(vtkDRMLMultiVolumeStorageNode,vtkDRMLNRRDStorageNode);

  virtual vtkDRMLNode* CreateNodeInstance() override;

  /// 
  /// Get node XML tag name (like Storage, Model)
  virtual const char* GetNodeTagName() override  {return "MultiVolumeStorage";};

  /// Return true if the node can be read in.
  virtual bool CanReadInReferenceNode(vtkDRMLNode *refNode) override;

protected:
  vtkDRMLMultiVolumeStorageNode();
  ~vtkDRMLMultiVolumeStorageNode();
  vtkDRMLMultiVolumeStorageNode(const vtkDRMLMultiVolumeStorageNode&);
  void operator=(const vtkDRMLMultiVolumeStorageNode&);

  /// Does the actual reading. Returns 1 on success, 0 otherwise.
  /// Returns 0 by default (read not supported).
  /// This implementation delegates most everything to the superclass
  /// but it has an early exit if the file to be read is not a
  /// MultiVolume, e.g. the file is a NRRD but not a MultiVolume NRRD.
  virtual int ReadDataInternal(vtkDRMLNode* refNode) override;
};

#endif
