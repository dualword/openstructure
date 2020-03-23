//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2020 by the OpenStructure authors
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 3.0 of the License, or (at your option)
// any later version.
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//------------------------------------------------------------------------------
#ifndef OST_SEQUENCE_VIEWER_ALIGN_PROPERTIES_PAINTER
#define OST_SEQUENCE_VIEWER_ALIGN_PROPERTIES_PAINTER

/*
  Author: Stefan Scheuber
 */


#include "painter.hh"
#include <QObject>

namespace ost { namespace gui {



class AlignPropertiesPainter : public Painter
{
  Q_OBJECT
public:
  AlignPropertiesPainter(QObject* parent = 0);
  void Paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index);
private:
  static QMap<QString,QColor> color_map_;
};

}}

#endif
