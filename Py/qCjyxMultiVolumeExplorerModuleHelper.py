from __main__ import cjyx
import vtk


class qCjyxMultiVolumeExplorerModuleHelper:

  @staticmethod
  def RGBtoHex(r, g, b):
    return f'#{r:02X}{g:02X}{b:02X}'

  @staticmethod
  def extractFrame(scalarVolumeNode, multiVolumeNode, frameId):
    # Extract frame from multiVolumeNode and put it into scalarVolumeNode
    # if no scalar volume given, create one
    if scalarVolumeNode is None:
      scalarVolumeNode = cjyx.vtkDRMLScalarVolumeNode()
      scalarVolumeNode.SetScene(cjyx.drmlScene)
      cjyx.drmlScene.AddNode(scalarVolumeNode)

    # Extract the image data
    mvImage = multiVolumeNode.GetImageData()
    extract = vtk.vtkImageExtractComponents()
    extract.SetInputData(mvImage)
    extract.SetComponents(frameId)
    extract.Update()

    ras2ijk = vtk.vtkMatrix4x4()
    ijk2ras = vtk.vtkMatrix4x4()
    multiVolumeNode.GetRASToIJKMatrix(ras2ijk)
    multiVolumeNode.GetIJKToRASMatrix(ijk2ras)
    scalarVolumeNode.SetRASToIJKMatrix(ras2ijk)
    scalarVolumeNode.SetIJKToRASMatrix(ijk2ras)

    scalarVolumeNode.SetAndObserveImageData(extract.GetOutput())

    displayNode = scalarVolumeNode.GetDisplayNode()
    if displayNode is None:
      displayNode = cjyx.drmlScene.CreateNodeByClass('vtkDRMLScalarVolumeDisplayNode')
      displayNode.SetReferenceCount(1)
      displayNode.SetScene(cjyx.drmlScene)
      cjyx.drmlScene.AddNode(displayNode)
      displayNode.SetDefaultColorMap()
      scalarVolumeNode.SetAndObserveDisplayNodeID(displayNode.GetID())
    return scalarVolumeNode

  @staticmethod
  def SetBgFgVolumes(bg, fg):
    appLogic = cjyx.app.applicationLogic()
    selectionNode = appLogic.GetSelectionNode()
    selectionNode.SetReferenceActiveVolumeID(bg)
    selectionNode.SetReferenceSecondaryVolumeID(fg)
    appLogic.PropagateVolumeSelection()

  @staticmethod
  def SetBgVolume(bg):
    appLogic = cjyx.app.applicationLogic()
    selectionNode = appLogic.GetSelectionNode()
    selectionNode.SetReferenceActiveVolumeID(bg)
    appLogic.PropagateVolumeSelection()

  @staticmethod
  def setupChartNodeViewLayout():
    layoutNodes = cjyx.drmlScene.GetNodesByClass('vtkDRMLLayoutNode')
    layoutNodes.SetReferenceCount(layoutNodes.GetReferenceCount()-1)
    layoutNodes.InitTraversal()
    layoutNode = layoutNodes.GetNextItemAsObject()
    layoutNode.SetViewArrangement(cjyx.vtkDRMLLayoutNode.CjyxLayoutConventionalQuantitativeView)

