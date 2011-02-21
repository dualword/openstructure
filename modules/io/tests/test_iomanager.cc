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
#include <ost/io/io_manager.hh>
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
using boost::unit_test_framework::test_suite;


using namespace ost;
using namespace ost::io;

BOOST_AUTO_TEST_SUITE( io )


BOOST_AUTO_TEST_CASE(io_manager) 
{
  IOManager& iom = IOManager::Instance();
  BOOST_CHECK(iom.FindEntityImportHandler("model.pdb"));
  BOOST_CHECK(iom.FindEntityImportHandler("model.PDB"));
  BOOST_CHECK(iom.FindEntityImportHandler("model.ent"));
}

BOOST_AUTO_TEST_SUITE_END()