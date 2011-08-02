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
#include "editor_base.hh"
#include <ost/invalid_handle.hh>

#include "impl/entity_impl.hh"
#include "impl/chain_impl.hh"
#include "impl/residue_impl.hh"
#include "impl/atom_impl.hh"
#include "mol.hh"

/*
  Author: Marco Biasini
 */
namespace ost { namespace  mol {

EditorBase::EditorBase(const EntityHandle& ent, EditMode mode):
  ent_(ent), mode_(mode)
{
}

ChainHandle EditorBase::InsertChain(const String& chain_name)
{
  return ent_.Impl()->InsertChain(chain_name);
}

ResidueHandle EditorBase::AppendResidue(ChainHandle chain, const ResidueKey& k)
{
  CheckHandleValidity(chain);  
  return ResidueHandle(chain.Impl()->AppendResidue(k));
}


ResidueHandle EditorBase::AppendResidue(ChainHandle chain, const ResidueKey& k, 
                                        const ResNum& num)
{
  CheckHandleValidity(chain);
  return ResidueHandle(chain.Impl()->AppendResidue(k, num));
}

ResidueHandle EditorBase::InsertResidueBefore(ChainHandle chain, int index, 
                                              const ResNum& num,
                                              const ResidueKey& k)
{
  CheckHandleValidity(chain);  
  return ResidueHandle(chain.Impl()->InsertResidueBefore(index, num, k));
}

ResidueHandle EditorBase::InsertResidueAfter(ChainHandle chain, int index,
                                             const ResNum& num,
                                             const ResidueKey& k)
{
  CheckHandleValidity(chain);
  return ResidueHandle(chain.Impl()->InsertResidueAfter(index, num, k));  
}

void EditorBase::RenameResidue(ResidueHandle res, const String& new_name)
{
  CheckHandleValidity(res);
  res.Impl()->SetKey(new_name);
}

void EditorBase::RenameChain(ChainHandle chain, const String& new_name)
{
  CheckHandleValidity(chain); 
  ent_.Impl()->RenameChain(chain.Impl(), new_name);
}

void EditorBase::SetChainType(ChainHandle chain, const ChainType type)
{
  CheckHandleValidity(chain);
  chain.Impl()->SetType(type);
}

void EditorBase::SetChainDescription(ChainHandle chain, const String desc)
{
  CheckHandleValidity(chain);
  chain.Impl()->SetDescription(desc);
}

AtomHandle EditorBase::InsertAtom(ResidueHandle res, const String& name,
                                  const geom::Vec3& pos, const String& ele,
                                  Real occupancy, Real b_factor, 
                                  bool is_hetatm)
{
  CheckHandleValidity(res);
  ent_.Impl()->MarkTraceDirty();  
  AtomHandle atom(res.Impl()->InsertAtom(name, pos, ele));
  atom.SetBFactor(b_factor);
  atom.SetHetAtom(is_hetatm);
  atom.SetOccupancy(occupancy);
  return atom;
}

AtomHandle EditorBase::InsertAltAtom(ResidueHandle res, const String& name,
                                     const String& alt_group,
                                     const geom::Vec3& pos,
                                     const String& ele) 
{
  CheckHandleValidity(res);
  ent_.Impl()->MarkTraceDirty();
  AtomHandle atom(res.Impl()->InsertAltAtom(name, alt_group, pos, ele));
  this->UpdateTrace();
  return atom;
}

void EditorBase::AddAltAtomPos(const String& group,
                               const AtomHandle& atom,
                               const geom::Vec3& position) 
{
  CheckHandleValidity(atom);
  atom.GetResidue().Impl()->AddAltAtomPos(group, atom.Impl(), position);
}

void EditorBase::DeleteChain(const ChainHandle& chain) 
{
  CheckHandleValidity(chain);
  ent_.Impl()->DeleteChain(chain.Impl());
}

void EditorBase::DeleteAtom(const AtomHandle& atom_handle) 
{
  CheckHandleValidity(atom_handle);
  atom_handle.GetResidue().Impl()->DeleteAtom(atom_handle.Impl());
}

void EditorBase::DeleteResidue(const ResidueHandle& residue_handle) 
{
  CheckHandleValidity(residue_handle);
  residue_handle.GetChain().Impl()->DeleteResidue(residue_handle.Impl());
}

void EditorBase::ReorderResidues(const ChainHandle& chain)
{
  CheckHandleValidity(chain);
  chain.Impl()->ReorderResidues();
}

void EditorBase::ReorderAllResidues()
{
  ent_.Impl()->ReorderAllResidues();
}

void EditorBase::RenameAtom(AtomHandle atom, const String& new_name)
{
  CheckHandleValidity(atom);
  atom.Impl()->Name()=new_name;
}

BondHandle EditorBase::Connect(const AtomHandle& first,
                                 const AtomHandle& second) {
  CheckHandleValidity(first);
  CheckHandleValidity(second);
  return this->Connect(first,second,0.0, 0.0, 0.0, 1);
}

BondHandle EditorBase::Connect(const AtomHandle& first,
                               const AtomHandle& second,
                               unsigned char bond_order) {
  CheckHandleValidity(first);
  CheckHandleValidity(second);
  return this->Connect(first,second,0.0, 0.0, 0.0, bond_order);
}

BondHandle EditorBase::Connect(const AtomHandle& first,
                               const AtomHandle& second,
                               Real len, Real theta, Real phi) {
  CheckHandleValidity(first);
  CheckHandleValidity(second);
  return this->Connect(first,second,0.0, 0.0, 0.0, 1);
}

BondHandle EditorBase::Connect(const AtomHandle& first,
                                 const AtomHandle& second,
                                 Real len, Real theta, Real phi,
                                 unsigned char bond_order) {
  CheckHandleValidity(first);
  CheckHandleValidity(second);  
  ent_.Impl()->MarkTraceDirty();  
  impl::ConnectorImplP bp = ent_.Impl()->Connect(first.Impl(), second.Impl(),
                                                 len, theta, phi, bond_order);
  this->UpdateTrace();                                                 
  return bp ? BondHandle(bp) : BondHandle();
}

TorsionHandle EditorBase::AddTorsion(const String& name, const AtomHandle& a1,
                                       const AtomHandle& a2, 
                                       const AtomHandle& a3,
                                       const AtomHandle& a4) 
{
  CheckHandleValidity(a1);
  CheckHandleValidity(a2);
  CheckHandleValidity(a3);
  CheckHandleValidity(a4);
  return TorsionHandle(ent_.Impl()->AddTorsion(name, a1.Impl(), a2.Impl(),
                                               a3.Impl(), a4.Impl()));
}


void EditorBase::UpdateTrace()
{
  if (mode_==UNBUFFERED_EDIT) {
    ent_.Impl()->TraceDirectionality();
  }
}

}} // ns
