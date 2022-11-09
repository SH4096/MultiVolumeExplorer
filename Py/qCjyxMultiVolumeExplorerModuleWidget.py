from __main__ import vtk, ctk, cjyx
import logging
import PythonQt
from qt import QVBoxLayout, QHBoxLayout, QGridLayout, QFormLayout, QSizePolicy, QDialog, QSize, QPoint
from qt import QWidget, QLabel, QPushButton, QCheckBox, QRadioButton, QSpinBox, QTimer, QButtonGroup, QGroupBox
from qCjyxMultiVolumeExplorerModuleHelper import qCjyxMultiVolumeExplorerModuleHelper as Helper
from qCjyxMultiVolumeExplorerCharts import LabeledImageChartView, MultiVolumeIntensityChartView


class qCjyxMultiVolumeExplorerSimplifiedModuleWidget:

  def __init__(self, parent=None):
    logging.debug("qCjyxMultiVolumeExplorerSimplifiedModuleWidget:init() called")
    if not parent or not hasattr(parent, "layout"):
      self.parent = cjyx.qDRMLWidget()
      self.parent.setLayout(QVBoxLayout())
    else:
      self.parent = parent

    self.layout = self.parent.layout()

    self._bgMultiVolumeNode = None
    self._fgMultiVolumeNode = None

    self.styleObserverTags = []
    self.sliceWidgetsPerStyle = {}

    self.chartPopupWindow = None
    self.chartPopupSize = QSize(600, 300)
    self.chartPopupPosition = QPoint(0,0)

  def hide(self):
    self.widget.hide()

  def show(self):
    self.widget.show()

  def setup(self):
    self.widget = QWidget()
    layout = QGridLayout()
    self.widget.setLayout(layout)
    self.layout.addWidget(self.widget)
    self.widget.show()
    self.layout = layout

    self.setupInputFrame()
    self.setupFrameControlFrame()
    self.setupAdditionalFrames()
    self.setupPlottingFrame()

    self.setFramesEnabled(False)

    self.timer = QTimer()
    self.timer.setInterval(50)

    self.setupConnections()

    # initialize slice observers (from DataProbe.py)
    # keep list of pairs: [observee,tag] so they can be removed easily
    self.styleObserverTags = []
    # keep a map of interactor styles to sliceWidgets so we can easily get sliceLogic
    self.sliceWidgetsPerStyle = {}
    self.refreshObservers()

  def setupInputFrame(self, parent=None):
    if not parent:
      parent = self.layout
    self.bgMultiVolumeSelector = cjyx.qDRMLNodeComboBox()
    self.bgMultiVolumeSelector.nodeTypes = ['vtkDRMLMultiVolumeNode']
    self.bgMultiVolumeSelector.setDRMLScene(cjyx.drmlScene)
    self.bgMultiVolumeSelector.addEnabled = 0
    self._bgMultiVolumeSelectorLabel = QLabel('Input multivolume')
    inputFrameWidget = QWidget()
    self.inputFrameLayout = QFormLayout()
    inputFrameWidget.setLayout(self.inputFrameLayout)
    self.inputFrameLayout.addRow(self._bgMultiVolumeSelectorLabel, self.bgMultiVolumeSelector)
    parent.addWidget(inputFrameWidget)

  def setupFrameControlFrame(self):
    # TODO: initialize the slider based on the contents of the labels array
    self.frameSlider = ctk.ctkSliderWidget()
    self.frameSlider.setSizePolicy(QSizePolicy.Ignored, QSizePolicy.Preferred)
    self.frameLabel = QLabel('Current frame number')
    self.playButton = QPushButton('Play')
    self.playButton.toolTip = 'Iterate over multivolume frames'
    self.playButton.checkable = True
    frameControlHBox = QHBoxLayout()
    frameControlHBox.addWidget(self.frameLabel)
    frameControlHBox.addWidget(self.frameSlider)
    frameControlHBox.addWidget(self.playButton)
    self.inputFrameLayout.addRow(frameControlHBox)

  def setupAdditionalFrames(self):
    pass

  def setupPlottingFrame(self, parent=None):
    if not parent:
      parent = self.layout
    self.plottingFrameWidget = QWidget()
    self.plottingFrameLayout = QGridLayout()
    self.plottingFrameWidget.setLayout(self.plottingFrameLayout)
    self._multiVolumeIntensityChart = MultiVolumeIntensityChartView()
    self.popupChartButton = QPushButton("Undock chart")
    self.popupChartButton.setCheckable(True)
    self.plottingFrameLayout.addWidget(self._multiVolumeIntensityChart.chartView)
    self.plottingFrameLayout.addWidget(self.popupChartButton)
    parent.addWidget(self.plottingFrameWidget)

  def setupConnections(self):
    self.parent.connect('drmlSceneChanged(vtkDRMLScene*)', self.onVCDRMLSceneChanged)
    self.bgMultiVolumeSelector.connect('currentNodeChanged(vtkDRMLNode*)', self.onBackgroundInputChanged)
    self.playButton.connect('toggled(bool)', self.onPlayButtonToggled)
    self.frameSlider.connect('valueChanged(double)', self.onSliderChanged)
    self.timer.connect('timeout()', self.goToNext)
    self.popupChartButton.connect('toggled(bool)', self.onDockChartViewToggled)

  def onDockChartViewToggled(self, checked):
    if checked:
      self.chartPopupWindow = QDialog()
      self.chartPopupWindow.setWindowFlags(PythonQt.QtCore.Qt.WindowStaysOnTopHint)
      layout = QGridLayout()
      self.chartPopupWindow.setLayout(layout)
      layout.addWidget(self._multiVolumeIntensityChart.chartView)
      layout.addWidget(self.popupChartButton)
      self.chartPopupWindow.finished.connect(self.dockChartView)
      self.chartPopupWindow.resize(self.chartPopupSize)
      self.chartPopupWindow.move(self.chartPopupPosition)
      self.chartPopupWindow.show()
      self.popupChartButton.setText("Dock chart")
      self._multiVolumeIntensityChart.chartView.show()
    else:
      self.chartPopupWindow.close()

  def dockChartView(self):
    self.chartPopupSize = self.chartPopupWindow.size
    self.chartPopupPosition = self.chartPopupWindow.pos
    self.plottingFrameLayout.addWidget(self._multiVolumeIntensityChart.chartView)
    self.plottingFrameLayout.addWidget(self.popupChartButton)
    self.popupChartButton.setText("Undock chart")
    self.popupChartButton.disconnect('toggled(bool)', self.onDockChartViewToggled)
    self.popupChartButton.checked = False
    self.popupChartButton.connect('toggled(bool)', self.onDockChartViewToggled)

  def onSliderChanged(self, frameId):
    if self._bgMultiVolumeNode is None:
      return
    newValue = int(frameId)
    self.setCurrentFrameNumber(newValue)

  def onVCDRMLSceneChanged(self, drmlScene):
    logging.debug("qCjyxMultiVolumeExplorerSimplifiedModuleWidget:onVCDRMLSceneChanged")
    self.bgMultiVolumeSelector.setDRMLScene(cjyx.drmlScene)
    self.onBackgroundInputChanged()

  def refreshGUIForNewBackgroundImage(self):
    self._multiVolumeIntensityChart.reset()
    self.setFramesEnabled(True)
    if self._fgMultiVolumeNode and self._bgMultiVolumeNode:
      Helper.SetBgFgVolumes(self._bgMultiVolumeNode.GetID(), self._fgMultiVolumeNode.GetID())
    else:
      Helper.SetBgVolume(self._bgMultiVolumeNode.GetID())
    self.refreshFrameSlider()
    self._multiVolumeIntensityChart.bgMultiVolumeNode = self._bgMultiVolumeNode
    self.refreshObservers()

  def getBackgroundMultiVolumeNode(self):
    return self.bgMultiVolumeSelector.currentNode()

  def onBackgroundInputChanged(self):
    self._bgMultiVolumeNode = self.getBackgroundMultiVolumeNode()

    if self._bgMultiVolumeNode is not None:
      self.refreshGUIForNewBackgroundImage()
    else:
      self.setFramesEnabled(False)

  def onPlayButtonToggled(self, checked):
    if self._bgMultiVolumeNode is None:
      return
    if checked:
      self.timer.start()
      self.playButton.text = 'Stop'
    else:
      self.timer.stop()
      self.playButton.text = 'Play'

  def processEvent(self, observee, event):
    # logging.debug("processing event %s" % event)
    if self._bgMultiVolumeNode is None:
      return

    # TODO: use a timer to delay calculation and compress events
    if event == 'LeaveEvent':
      # reset all the readouts
      # TODO: reset the label text
      return

    if observee not in self.sliceWidgetsPerStyle:
      return

    interactor = observee.GetInteractor()
    self.createChart(self.sliceWidgetsPerStyle[observee], interactor.GetEventPosition())

  def createChart(self, sliceWidget, position):
    self._multiVolumeIntensityChart.createChart(sliceWidget, position)

  def setCurrentFrameNumber(self, frameNumber):
    mvDisplayNode = self._bgMultiVolumeNode.GetDisplayNode()
    mvDisplayNode.SetFrameComponent(frameNumber)

  def setFramesEnabled(self, enabled):
    pass

  def refreshObservers(self):
    """ When the layout changes, drop the observers from
    all the old widgets and create new observers for the
    newly created widgets"""
    self.removeObservers()
    # get new slice nodes
    layoutManager = cjyx.app.layoutManager()
    sliceNodeCount = cjyx.drmlScene.GetNumberOfNodesByClass('vtkDRMLSliceNode')
    for nodeIndex in range(sliceNodeCount):
      # find the widget for each node in scene
      sliceNode = cjyx.drmlScene.GetNthNodeByClass(nodeIndex, 'vtkDRMLSliceNode')
      sliceWidget = layoutManager.sliceWidget(sliceNode.GetLayoutName())
      if sliceWidget:
        # add observers and keep track of tags
        style = sliceWidget.sliceView().interactorStyle()
        self.sliceWidgetsPerStyle[style] = sliceWidget
        events = ("MouseMoveEvent", "EnterEvent", "LeaveEvent")
        for event in events:
          tag = style.AddObserver(event, self.processEvent)
          self.styleObserverTags.append([style,tag])

  def removeObservers(self):
    for observee,tag in self.styleObserverTags:
      observee.RemoveObserver(tag)
    self.styleObserverTags = []
    self.sliceWidgetsPerStyle = {}

  def refreshFrameSlider(self):
    self.frameSlider.minimum = 0
    if not self._bgMultiVolumeNode:
      self.frameSlider.maximum = 0
      return
    nFrames = self._bgMultiVolumeNode.GetNumberOfFrames()
    self.frameSlider.maximum = nFrames - 1

  def goToNext(self):
    currentElement = self.frameSlider.value
    currentElement += 1
    if currentElement > self.frameSlider.maximum:
      currentElement = 0
    self.frameSlider.value = currentElement


class qCjyxMultiVolumeExplorerModuleWidget(qCjyxMultiVolumeExplorerSimplifiedModuleWidget):

  def __init__(self, parent=None):
    qCjyxMultiVolumeExplorerSimplifiedModuleWidget.__init__(self, parent)

  def setupAdditionalFrames(self):
    self.setupPlotSettingsFrame()

  def setupInputFrame(self, parent=None):
    self.inputFrame = ctk.ctkCollapsibleButton()
    self.inputFrame.text = "Input"
    self.inputFrame.collapsed = 0
    inputFrameCollapsibleLayout = QFormLayout(self.inputFrame)
    qCjyxMultiVolumeExplorerSimplifiedModuleWidget.setupInputFrame(self, parent=inputFrameCollapsibleLayout)
    self.layout.addWidget(self.inputFrame)

    self.fgMultiVolumeSelector = cjyx.qDRMLNodeComboBox()
    self.fgMultiVolumeSelector.nodeTypes = ['vtkDRMLMultiVolumeNode']
    self.fgMultiVolumeSelector.setDRMLScene(cjyx.drmlScene)
    self.fgMultiVolumeSelector.addEnabled = 0
    self.fgMultiVolumeSelector.noneEnabled = 1
    self.fgMultiVolumeSelector.toolTip = "Secondary multivolume will be used for the secondary \
      plot in interactive charting. As an example, this can be used to overlay the \
      curve obtained by fitting a model to the data"
    self.inputFrameLayout.addRow(QLabel('Input secondary multivolume'), self.fgMultiVolumeSelector)

  def setupFrameControlFrame(self):
    qCjyxMultiVolumeExplorerSimplifiedModuleWidget.setupFrameControlFrame(self)

    self.frameCopySelector = cjyx.qDRMLNodeComboBox()
    self.frameCopySelector.setSizePolicy(QSizePolicy.Ignored, QSizePolicy.Preferred)
    self.frameCopySelector.nodeTypes = ['vtkDRMLScalarVolumeNode']
    self.frameCopySelector.setDRMLScene(cjyx.drmlScene)
    self.frameCopySelector.addEnabled = 1
    self.frameCopySelector.enabled = 0
    # do not show "children" of vtkDRMLScalarVolumeNode
    self.frameCopySelector.hideChildNodeTypes = ["vtkDRMLDiffusionWeightedVolumeNode",
                                                  "vtkDRMLDiffusionTensorVolumeNode",
                                                  "vtkDRMLVectorVolumeNode"]
    self.extractFrameCopy = False
    self.extractFrameCheckBox = QCheckBox('Enable copying while sliding')
    hbox = QHBoxLayout()
    hbox.addWidget(QLabel('Current frame copy'))
    hbox.addWidget(self.frameCopySelector)
    hbox.addWidget(self.extractFrameCheckBox)
    self.inputFrameLayout.addRow(hbox)

    self.currentFrameCopySelector = cjyx.qDRMLNodeComboBox()
    self.currentFrameCopySelector.setSizePolicy(QSizePolicy.Ignored, QSizePolicy.Preferred)
    self.currentFrameCopySelector.nodeTypes = ['vtkDRMLScalarVolumeNode']
    self.currentFrameCopySelector.setDRMLScene(cjyx.drmlScene)
    self.currentFrameCopySelector.addEnabled = 0
    self.currentFrameCopySelector.enabled = 0

    self.currentFrameCopyButton = QPushButton('Copy frame')
    self.currentFrameCopyButton.toolTip = 'Copy currently selected frame'

    hbox2 = QHBoxLayout()
    hbox2.addWidget(QLabel('Current frame click-to-copy'))
    hbox2.addWidget(self.currentFrameCopySelector)
    hbox2.addWidget(self.currentFrameCopyButton)
    self.inputFrameLayout.addRow(hbox2)

  def setupPlotSettingsFrame(self):
    self.plotSettingsFrame = ctk.ctkCollapsibleButton()
    self.plotSettingsFrame.text = "Plotting Settings"
    self.plotSettingsFrame.collapsed = 1
    plotSettingsFrameLayout = QFormLayout(self.plotSettingsFrame)
    self.layout.addWidget(self.plotSettingsFrame)

    # label map for probing
    self.labelMapSelector = cjyx.qDRMLNodeComboBox()
    self.labelMapSelector.nodeTypes = ['vtkDRMLLabelMapVolumeNode']
    self.labelMapSelector.toolTip = 'Label map to be probed'
    self.labelMapSelector.setDRMLScene(cjyx.drmlScene)
    self.labelMapSelector.addEnabled = 0
    self.chartButton = QPushButton('Chart')
    self.chartButton.setEnabled(False)

    hbox = QHBoxLayout()
    hbox.addWidget(QLabel('Probed label volume'))
    hbox.addWidget(self.labelMapSelector)
    hbox.addWidget(self.chartButton)
    plotSettingsFrameLayout.addRow(hbox)

    self.iCharting = QCheckBox('Interactive charting')
    self.iCharting.setChecked(True)
    plotSettingsFrameLayout.addRow(self.iCharting)

    self.iChartingMode = QButtonGroup()
    self.iChartingIntensity = QRadioButton('Signal intensity')
    self.iChartingIntensityFixedAxes = QRadioButton('Fixed range intensity')
    self.iChartingPercent = QRadioButton('Percentage change')
    self.iChartingIntensity.setChecked(1)
    self.iChartingMode.addButton(self.iChartingIntensity)
    self.iChartingMode.addButton(self.iChartingIntensityFixedAxes)
    self.iChartingMode.addButton(self.iChartingPercent)

    hbox = QHBoxLayout()
    self.plottingModeGroupBox = QGroupBox('Plotting mode:')
    plottingModeLayout = QVBoxLayout()
    self.plottingModeGroupBox.setLayout(plottingModeLayout)
    plottingModeLayout.addWidget(self.iChartingIntensity)
    plottingModeLayout.addWidget(self.iChartingIntensityFixedAxes)
    plottingModeLayout.addWidget(self.iChartingPercent)
    hbox.addWidget(self.plottingModeGroupBox)

    self.showLegendCheckBox = QCheckBox('Display legend')
    self.showLegendCheckBox.setChecked(0)
    self.xLogScaleCheckBox = QCheckBox('Use log scale for X axis')
    self.xLogScaleCheckBox.setChecked(0)
    self.yLogScaleCheckBox = QCheckBox('Use log scale for Y axis')
    self.yLogScaleCheckBox.setChecked(0)

    self.plotGeneralSettingsGroupBox = QGroupBox('General Plot options:')
    plotGeneralSettingsLayout = QVBoxLayout()
    self.plotGeneralSettingsGroupBox.setLayout(plotGeneralSettingsLayout)
    plotGeneralSettingsLayout.addWidget(self.showLegendCheckBox)
    plotGeneralSettingsLayout.addWidget(self.xLogScaleCheckBox)
    plotGeneralSettingsLayout.addWidget(self.yLogScaleCheckBox)
    hbox.addWidget(self.plotGeneralSettingsGroupBox)
    plotSettingsFrameLayout.addRow(hbox)

    self.nFramesBaselineCalculation = QSpinBox()
    self.nFramesBaselineCalculation.minimum = 1
    hbox = QHBoxLayout()
    hbox.addWidget(QLabel('Frame count(baseline calculation):'))
    hbox.addWidget(self.nFramesBaselineCalculation)
    plotSettingsFrameLayout.addRow(hbox)

  def setupPlottingFrame(self, parent=None):
    self.plotFrame = ctk.ctkCollapsibleButton()
    self.plotFrame.text = "Plotting"
    self.plotFrame.collapsed = 0
    plotFrameLayout = QGridLayout(self.plotFrame)
    self.layout.addWidget(self.plotFrame)
    qCjyxMultiVolumeExplorerSimplifiedModuleWidget.setupPlottingFrame(self, parent=plotFrameLayout)

  def onDockChartViewToggled(self, checked):
    qCjyxMultiVolumeExplorerSimplifiedModuleWidget.onDockChartViewToggled(self, checked)
    if checked:
      self.layout.removeWidget(self.plotFrame)
      self.plotFrame.hide()

  def dockChartView(self):
    qCjyxMultiVolumeExplorerSimplifiedModuleWidget.dockChartView(self)
    self.layout.addWidget(self.plotFrame)
    self.plotFrame.show()

  def setFramesEnabled(self, enabled):
    qCjyxMultiVolumeExplorerSimplifiedModuleWidget.setFramesEnabled(self, enabled)
    self.plotSettingsFrame.setEnabled(enabled)
    self.plotFrame.setEnabled(enabled)
    self.plotFrame.collapsed = 0 if enabled else 1

  def setupConnections(self):
    qCjyxMultiVolumeExplorerSimplifiedModuleWidget.setupConnections(self)
    self.labelMapSelector.connect('currentNodeChanged(vtkDRMLNode*)', self.onLabelNodeChanged)
    self.chartButton.connect('clicked()', self.onLabeledChartRequested)
    self.xLogScaleCheckBox.connect('stateChanged(int)', self.onXLogScaleRequested)
    self.yLogScaleCheckBox.connect('stateChanged(int)', self.onYLogScaleRequested)
    self.nFramesBaselineCalculation.valueChanged.connect(self.onFrameCountBaselineCalculationChanged)
    self.iChartingMode.buttonClicked.connect(self.onChartingModeChanged)
    self.showLegendCheckBox.connect('stateChanged(int)', self.onShowLegendChanged)
    self.fgMultiVolumeSelector.connect('currentNodeChanged(vtkDRMLNode*)', self.onForegroundInputChanged)
    self.extractFrameCheckBox.connect('stateChanged(int)', self.onExtractFrameChanged)
    self.frameCopySelector.connect('drmlSceneChanged(vtkDRMLScene*)', self.onVFDRMLSceneChanged)
    self.currentFrameCopyButton.connect('clicked()', self.onCopyButtonClicked)

  def onFrameCountBaselineCalculationChanged(self, value):
    self._multiVolumeIntensityChart.nFramesForBaselineCalculation = value

  def onChartingModeChanged(self, button):
    if button is self.iChartingIntensity:
      self._multiVolumeIntensityChart.activateSignalIntensityMode()
    elif button is self.iChartingIntensityFixedAxes:
      self._multiVolumeIntensityChart.activateFixedRangeIntensityMode()
    elif button is self.iChartingPercent:
      self._multiVolumeIntensityChart.activatePercentageChangeMode()

  def onShowLegendChanged(self, checked):
    self._multiVolumeIntensityChart.showLegend = True if checked == 2 else False

  def onXLogScaleRequested(self, checked):
    self._multiVolumeIntensityChart.showXLogScale = True if checked == 2 else False

  def onYLogScaleRequested(self, checked):
    self._multiVolumeIntensityChart.showYLogScale = True if checked == 2 else False

  def onLVDRMLSceneChanged(self, drmlScene):
    self.labelMapSelector.setDRMLScene(cjyx.drmlScene)

  def onVFDRMLSceneChanged(self, drmlScene):
    self.frameCopySelector.setDRMLScene(cjyx.drmlScene)

  def onLabelNodeChanged(self):
    labelNode = self.labelMapSelector.currentNode()
    self.chartButton.setEnabled(labelNode is not None and self._bgMultiVolumeNode is not None)

  def onForegroundInputChanged(self):
    logging.info("qCjyxMultiVolumeExplorerModuleWidget:ForegroundInputChanged")
    self._fgMultiVolumeNode = self.fgMultiVolumeSelector.currentNode()
    self._multiVolumeIntensityChart.fgMultiVolumeNode = self.fgMultiVolumeSelector.currentNode()
    self.refreshGUIForNewBackgroundImage()

  def onBackgroundInputChanged(self):
    qCjyxMultiVolumeExplorerSimplifiedModuleWidget.onBackgroundInputChanged(self)

    if self._bgMultiVolumeNode is not None:
      self.frameCopySelector.setCurrentNode(None)
      self.currentFrameCopySelector.setCurrentNode(None)

      self.nFramesBaselineCalculation.maximum = self._bgMultiVolumeNode.GetNumberOfFrames()
    self.onLabelNodeChanged()

  '''
  If extract button is checked, will copy the current frame to the
  selected volume node on each event from frame slider
  '''
  def onExtractFrameChanged(self, checked):
    if checked:
      self.extractFrameCopy = True
      self.onSliderChanged(self.frameSlider.value)
    else:
      self.extractFrameCopy = False

  def onSliderChanged(self, frameId):
    qCjyxMultiVolumeExplorerSimplifiedModuleWidget.onSliderChanged(self, frameId)
    frameId = int(frameId)

    if self.extractFrameCopy:
      frameVolume = self.frameCopySelector.currentNode()
      frameVolumeCopy = Helper.extractFrame(frameVolume, self._bgMultiVolumeNode, frameId)
      if not frameVolume:
        self.frameCopySelector.setCurrentNode(frameVolumeCopy)
      frameName = '%s frame %d' % (self._bgMultiVolumeNode.GetName(), frameId)
      frameVolumeCopy.SetName(frameName)

  def onCopyButtonClicked(self):
    if self._bgMultiVolumeNode is None:
      return

    frameId = int(self.frameSlider.value)

    frameName = '%s copied frame %d' % (self._bgMultiVolumeNode.GetName(), frameId)
    nodeVolume = cjyx.drmlScene.GetNodesByName(frameName).GetNumberOfItems()
    if (nodeVolume == 0):
      frameVolume = self.currentFrameCopySelector.addNode()
      frameVolumeCopy = Helper.extractFrame(frameVolume, self._bgMultiVolumeNode, frameId)
      frameVolumeCopy.SetName(frameName)
      self.currentFrameCopySelector.setCurrentNode(frameVolumeCopy)

  def onLabeledChartRequested(self):
    labelNode = self.labelMapSelector.currentNode()
    mvNode = self._bgMultiVolumeNode

    mvLabels = MultiVolumeIntensityChartView.getMultiVolumeLabels(self._bgMultiVolumeNode)

    chartViewNode = LabeledImageChartView(labelNode=labelNode,
                                           multiVolumeNode=mvNode,
                                           multiVolumeLabels=mvLabels,
                                           baselineFrames=self.nFramesBaselineCalculation,
                                           displayPercentageChange=self.iChartingPercent.checked)
    chartViewNode.requestChartCreation()

  def processEvent(self, observee, event):
    if not self.iCharting.checked:
      return
    qCjyxMultiVolumeExplorerSimplifiedModuleWidget.processEvent(self, observee, event)
