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
#ifndef OST_SEQ_SEQUENCE_LIST_IMPL_FW_HH
#define OST_SEQ_SEQUENCE_LIST_IMPL_FW_HH

/*
  Author: Marco Biasini
 */

#include <vector>
#include <boost/shared_ptr.hpp>
#include <ost/seq/impl/sequence_impl_fw.hh>
namespace ost { namespace seq { namespace impl {

/// \internal
class SequenceListImpl;

/// \internal
typedef boost::shared_ptr<SequenceListImpl> SequenceListImplPtr;

/// \internal
typedef std::vector<SequenceImplPtr> SequenceListImplVector;

/// \internal
typedef SequenceListImplVector::iterator SequenceImplListIterator;

/// \internal
typedef SequenceListImplVector::const_iterator SequenceImplListConstIterator;
}}}

#endif
