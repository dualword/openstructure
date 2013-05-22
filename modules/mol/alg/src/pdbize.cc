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
#include "pdbize.hh"
#include <ost/mol/residue_handle.hh>
#include <ost/mol/residue_view.hh>
#include <ost/mol/atom_handle.hh>
#include <ost/mol/atom_view.hh>
#include <ost/mol/transfer_connectivity.hh>
#include <ost/mol/chain_handle.hh>
#include <ost/mol/chain_view.hh>
#include <ost/mol/xcs_editor.hh>
#include <ost/seq/sequence_handle.hh>

namespace ost { namespace mol { namespace alg {

DLLEXPORT const char* POLYPEPTIDE_CHAIN_NAMES="ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz";
DLLEXPORT const char* LIGAND_CHAIN_NAME="_";
DLLEXPORT const char* WATER_CHAIN_NAME="-";


namespace {

bool copy_atoms(ResidueView src_res, ResidueHandle dst_res, XCSEditor& edi, 
                const geom::Mat4 transform) {
  bool needs_adjustment_ = false;
  for (AtomViewList::const_iterator 
       i = src_res.GetAtomList().begin(), 
       e = src_res.GetAtomList().end(); i != e; ++i) {

    AtomHandle new_atom = edi.InsertAtom(dst_res, i->GetName(), 
                                         geom::Vec3(transform*i->GetPos()),
                                         i->GetName(), i->GetOccupancy(), 
                                         i->GetBFactor(), i->IsHetAtom());
    geom::Vec3 pos = new_atom.GetPos();
    for (int j = 0; j<3; ++j) {
      needs_adjustment_|= pos[j]<=-1000 || pos[j]>=10000;
    }
  }
  return needs_adjustment_;
}

}

void PDBize::Add(EntityView asu, const geom::Mat4List& transforms, 
                 seq::SequenceList seqres)
{
  XCSEditor edi = ent_.EditXCS(BUFFERED_EDIT);

  ChainHandle ligand_chain;
  ChainHandle water_chain;
  int last_rnum = 0;
  for (geom::Mat4List::const_iterator i = transforms.begin(), 
       e = transforms.end(); i != e; ++i) {
    for (ChainViewList::const_iterator j = asu.GetChainList().begin(),
         e2 =asu.GetChainList().end(); j != e2; ++j) {
      ChainView chain = *j; 
      int chain_length = chain.GetResidueCount();
      if (chain_length < min_polymer_size_ && seqres.IsValid()) {
        seq::SequenceHandle s = seqres.FindSequence(chain.GetName());
        if (s.IsValid())
          chain_length = s.GetLength();
      }
      if (chain.IsPolymer() && chain_length >= min_polymer_size_) {
        if (*curr_chain_name_ == 0) {
          throw std::runtime_error("running out of chain names");
        }
        ChainHandle new_chain = edi.InsertChain(String(1, curr_chain_name_[0]));
        curr_chain_name_++;
        edi.SetChainDescription(new_chain, chain.GetDescription());
        edi.SetChainType(new_chain, chain.GetType());
        new_chain.SetStringProp("original_name", chain.GetName());
        if (chain.HasProp("pdb_auth_chain_name"))
          new_chain.SetStringProp("pdb_auth_chain_name",
                                  chain.GetStringProp("pdb_auth_chain_name"));
        
        for (ResidueViewList::const_iterator k = chain.GetResidueList().begin(),
             e3 = chain.GetResidueList().end(); k != e3; ++k) {
          ResidueHandle new_res = edi.AppendResidue(new_chain, k->GetName(),
                                                    k->GetNumber());
          dst_to_src_map_[new_res] = k->GetHandle();
          needs_adjustment_ |= copy_atoms(*k, new_res, edi, *i);
        }
        continue;
      }
      if (chain.GetType() == CHAINTYPE_WATER) {
        if (!water_chain) {
          water_chain = edi.InsertChain(WATER_CHAIN_NAME);
          edi.SetChainDescription(water_chain, chain.GetDescription());
          edi.SetChainType(water_chain, chain.GetType());
        }
        for (ResidueViewList::const_iterator k = chain.GetResidueList().begin(),
             e3 = chain.GetResidueList().end(); k != e3; ++k) {
          ResidueHandle new_res = edi.AppendResidue(water_chain, k->GetName());
          new_res.SetStringProp("type", StringFromChainType(chain.GetType()));
          new_res.SetStringProp("description", chain.GetDescription());
          dst_to_src_map_[new_res] = k->GetHandle();
          needs_adjustment_ |= copy_atoms(*k, new_res, edi, *i);
        }
        continue;
      }
      // deal with ligands...
      if (!ligand_chain) {
        ligand_chain = edi.InsertChain(LIGAND_CHAIN_NAME);
        last_rnum = 0;
      }
      char ins_code = chain.GetResidueCount()>1 ? 'A' : '\0';
     
      for (ResidueViewList::const_iterator k = chain.GetResidueList().begin(),
           e3 = chain.GetResidueList().end(); k != e3; ++k) {
        ResidueHandle new_res = edi.AppendResidue(ligand_chain, k->GetName(),
                                                  ResNum(last_rnum+1, ins_code));
        new_res.SetStringProp("type", StringFromChainType(chain.GetType()));
        new_res.SetStringProp("description", chain.GetDescription());
        new_res.SetStringProp("original_name", chain.GetName());
        if (chain.HasProp("pdb_auth_chain_name"))
          new_res.SetStringProp("pdb_auth_chain_name", 
                                chain.GetStringProp("pdb_auth_chain_name"));
        dst_to_src_map_[new_res] = k->GetHandle();
        ins_code += 1;
        needs_adjustment_ |= copy_atoms(*k, new_res, edi, *i);
      }
      last_rnum+=1;
    }
  }
}

EntityHandle PDBize::Finish(bool shift_to_fit) {

  if (needs_adjustment_ && shift_to_fit) {
    geom::Vec3 min_coord = ent_.GetBounds().GetMin();
    geom::Mat4 tf(1, 0, 0, -999 - min_coord[0],
                  0, 1, 0, -999 - min_coord[1],
                  0, 0, 1, -999 - min_coord[2],
                  0, 0, 0, 1);
    XCSEditor edi = ent_.EditXCS();
    edi.ApplyTransform(tf);
  }
  TransferConnectivity(ent_, dst_to_src_map_);
  return ent_;
}

}}}