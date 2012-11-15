from PyQt4.QtGui import *
from PyQt4.QtCore import *


__all__=('Table', )

class TableModel(QAbstractTableModel):
  def __init__(self, table, parent=None):
    QAbstractTableModel.__init__(self, parent)
    self.table=table
    
  def rowCount(self, index):
    return len(self.table.rows)

  def headerData(self, section, orientation, role):
    if role!=Qt.DisplayRole or orientation!=Qt.Horizontal:
      return QVariant()
    return self.table.col_names[section]
    
  def columnCount(self, index):
    return len(self.table.col_names)

  def sort(self, column, order):
    o='+'
    if order!=Qt.AscendingOrder:
      o='-'
    self.table.Sort(by=self.table.col_names[column], order=o)
    self.reset()
    
  def data(self, index, role):
    if not index.isValid() or role!=Qt.DisplayRole:
      return QVariant()
    row=self.table.rows[index.row()]
    return QVariant(row[index.column()])

class Table(QTableView):
  def __init__(self, table):
     QTableView.__init__(self)
     self._model=TableModel(table)
     self.setFrameShape(QFrame.NoFrame)    
     self.setAttribute(Qt.WA_MacSmallSize)
     self.setShowGrid(False)
     #self.horizontalHeader().setStretchLastSection(True)
     self.setContextMenuPolicy(Qt.CustomContextMenu)
     self.setSelectionBehavior(QAbstractItemView.SelectRows)
     self.setSizePolicy(QSizePolicy.MinimumExpanding, 
                        QSizePolicy.MinimumExpanding)
     self.setSortingEnabled(True)
     self.setModel(self._model)