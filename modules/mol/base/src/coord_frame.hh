//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2011 by the OpenStructure authors
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
#ifndef OST_MOL_COORD_FRAME_HH
#define OST_MOL_COORD_FRAME_HH

/*
  Author: Marco Biasini
 */
#include <boost/shared_ptr.hpp>
#include <ost/geom/geom.hh>
#include <ost/mol/module_config.hh>

namespace ost { namespace mol {


typedef std::vector<geom::Vec3> CoordFrame;
typedef boost::shared_ptr<CoordFrame> CoordFramePtr;
typedef std::vector<CoordFramePtr> CoordFrameList;

}}

#endif
