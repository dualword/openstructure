import math
from ost import gfx
from PyQt4.QtGui import *
from PyQt4.QtCore import *
from PyQt4.QtOpenGL import *

TRANS_VAL = 20

class DokkGLCanvas(QGLWidget):
    
  def __init__(self, format, parent=None):
    QGLWidget.__init__(self, format, parent)
    self.last_pos_=QPoint()
    self.setAutoFillBackground(False)
    self.setAttribute(Qt.WA_KeyCompression,True)
    
  def initializeGL(self):
    gfx.Scene().InitGL()

  def paintGL(self):
    gfx.Scene().RenderGL()
  def paintEvent(self, event):
    gfx.Scene().RenderGL()
    painter=QPainter(self)
    self.RenderHUD(painter)

  def RenderHUD(self, painter):
    painter.setPen(QColor(100, 100, 100, 50))
    painter.setBrush(QColor(200, 200, 200, 50))
    painter.drawRect(QRect(QPoint(0, 0), QSize(self.width(), 25)))
    painter.setPen(QPen(QColor(255,255,255), Qt.SolidLine))
    painter.setFont(QFont("Verdana"))
    painter.drawText(QPoint(10, 20), "Test Message")
  def resizeGL(self, w, h):
    gfx.Scene().Resize(w, h)

  def mousePressEvent(self, event):
    self.last_point_=QPoint(event.x(), event.y())

  def mouseMoveEvent(self, event):
    delta=QPoint(event.x(), event.y())-self.last_point_
    self.last_point_=QPoint(event.x(), event.y())
    if delta.y()!=0:
      gfx.Scene().Apply(gfx.InputEvent(gfx.INPUT_DEVICE_MOUSE,
                                       gfx.INPUT_COMMAND_ROTX, delta.y()*0.5), 
                        False)
    if delta.x()!=0:
      gfx.Scene().Apply(gfx.InputEvent(gfx.INPUT_DEVICE_MOUSE,
                                       gfx.INPUT_COMMAND_ROTY, delta.x()*0.5), 
                        False)
    self.update()

  def wheelEvent(self, event):
    self.OnTransform(gfx.INPUT_COMMAND_TRANSZ,0,gfx.TRANSFORM_VIEW,
              0.1*(-event.delta()))


  def OnTransform(self,com, indx, trg, val):
    gfx.Scene().Apply(gfx.InputEvent(gfx.INPUT_DEVICE_MOUSE,
                                     com, indx,trg,val*0.5),False)
    self.update()
    
  def keyReleaseEvent(self, event):
    if event.key() == Qt.Key_Left or event.key() == Qt.Key_A:
      self.OnTransform(gfx.INPUT_COMMAND_TRANSX,0, gfx.TRANSFORM_VIEW, -TRANS_VAL)
      
    if event.key() == Qt.Key_Right or event.key() == Qt.Key_D:
      self.OnTransform(gfx.INPUT_COMMAND_TRANSX,0, gfx.TRANSFORM_VIEW, TRANS_VAL)
      
    if event.key() == Qt.Key_Down or event.key() == Qt.Key_S:
      self.OnTransform(gfx.INPUT_COMMAND_TRANSZ,0, gfx.TRANSFORM_VIEW, TRANS_VAL)
      
    if event.key() == Qt.Key_Up or event.key() == Qt.Key_W:
      self.OnTransform(gfx.INPUT_COMMAND_TRANSZ,0, gfx.TRANSFORM_VIEW, -TRANS_VAL)
      
    if event.key() == Qt.Key_Escape:
      QApplication.exit()
    
class DokkGLWin(gfx.GLWinBase):
    def _CreateFormat(self):
      fmt=QGLFormat()
      fmt.setAlpha(True)
      return fmt
    def __init__(self):
        gfx.GLWinBase.__init__(self)
        self.canvas_=DokkGLCanvas(self._CreateFormat())
    def DoRefresh(self):
      self.refresh_=True
      
    def Show(self, fullscreen):
      if fullscreen:
        self.canvas_.showFullScreen()
      else:
        self.canvas_.show()
    def SetStereo():
      pass