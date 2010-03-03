//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2009 by the OpenStructure authors
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
#ifndef OST_GUI_SCENE_WIN_RENDER_MODE_NODE_HH
#define OST_GUI_SCENE_WIN_RENDER_MODE_NODE_HH

#include <QObject>
#include <QVariant>
#include <QModelIndex>

#include <ost/mol/entity_view.hh>
#include <ost/mol/query_view_wrapper.hh>

#include <ost/gfx/entity.hh>
#include <ost/gfx/entity_fw.hh>
#include <ost/gfx/render_mode.hh>
#include <ost/gfx/scene_observer.hh>
#include <ost/gfx/gfx_object.hh>
#include <ost/gfx/gfx_object_fw.hh>

#include <ost/gui/module_config.hh>
#include <ost/gui/scene_win/gfx_scene_node.hh>

/*
  Author: Stefan Scheuber
 */

namespace ost { namespace gui {

class DLLEXPORT_OST_GUI RenderModeNode : public GfxSceneNode {
  Q_OBJECT
public:
  RenderModeNode(gfx::EntityP entity, gfx::RenderMode::Type render_mode, SceneNode* node_parent );

  virtual QVariant GetData(int column, int role);
  virtual bool SetData(int column, const QVariant& value, int role);
  virtual Qt::ItemFlags Flags(int column) const;
  virtual int GetColumnCount() const;

  gfx::RenderMode::Type GetRenderMode() const;

private:
  gfx::RenderMode::Type render_mode_;
};

}}

#endif
