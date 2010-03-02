//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2010 by the OpenStructure authors
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

/*
  Author: Marco Biasini
 */

#include <iostream>
#include <ost/profile.hh>
#include <ost/mol/mol.hh>
#include <ost/mol/impl/entity_impl.hh>
#include <ost/mol/impl/chain_impl.hh>
#include <ost/mol/impl/residue_impl.hh>
#include <ost/mol/impl/atom_impl.hh>
#include <ost/mol/impl/torsion_impl.hh>
#include "view_op.hh"

namespace ost { namespace mol {


namespace {

static String combining_not_allowed="Combining atoms from different entities "
                                    "into one view is not supported";
                                      
struct BondCmp
{
  bool operator()(const BondHandle& b1, const BondHandle& b2) const
  {
    return b1.GetHashCode()<b2.GetHashCode();
  }
};
  
}


typedef std::set<BondHandle, BondCmp> BondSet;
typedef std::map<BondHandle,bool, BondCmp> BondMap;
  
mol::EntityView Union(const mol::EntityView& ev1, const mol::EntityView& ev2) 
{
  Profile union_time("mol::Union");
  if (ev1.GetHandle()!=ev2.GetHandle()) {
    throw IntegrityError(combining_not_allowed);
  }
  BondSet bonds_to_add;
  mol::EntityView merge=ev1.Copy();
  mol::ChainViewList::const_iterator c_it=ev2.GetChainList().begin();
  for ( ; c_it!=ev2.GetChainList().end(); ++c_it) {
    mol::ChainView cv=*c_it;
    mol::ChainView cv2=merge.FindChain(cv.GetHandle());
    if (!cv2) {
      cv2=merge.AddChain(cv, mol::ViewAddFlag::INCLUDE_ALL);
    } else {
      mol::ResidueViewList::const_iterator r_it=cv.GetResidueList().begin();
      for (; r_it!=cv.GetResidueList().end(); ++r_it) {
        mol::ResidueView rv=*r_it;
        mol::ResidueView rv2=cv2.FindResidue(rv.GetHandle());
        if (!rv2) {
          rv2=cv2.AddResidue(rv, mol::ViewAddFlag::INCLUDE_ALL);
        } else {
          mol::AtomViewList::const_iterator a_it=rv.GetAtomList().begin();
          for (; a_it!=rv.GetAtomList().end(); ++a_it) {
            rv2.AddAtom(*a_it, mol::ViewAddFlag::CHECK_DUPLICATES);
          }
        }
      }
    }
  }
  // prepare for bond insertion
  BondHandleList bonds=ev2.GetBondList();
  for (BondHandleList::const_iterator i=bonds.begin(), 
       e=bonds.end(); i!=e; ++i) {
    bonds_to_add.insert(*i);    
  }  
  // substract the already existing bonds
  bonds=ev1.GetBondList();
  for (BondHandleList::const_iterator i=bonds.begin(), 
       e=bonds.end(); i!=e; ++i) {
    bonds_to_add.erase(*i);    
  }
  for (BondSet::const_iterator i=bonds_to_add.begin(), 
       e=bonds_to_add.end(); i!=e; ++i) {
    merge.AddBond(*i);
  }
  return merge;
}

mol::EntityView Difference(const mol::EntityView& ev1, 
                           const mol::EntityView& ev2) 
{
  Profile diff_time("mol::Difference");
  if (ev1.GetHandle()!=ev2.GetHandle()) {
    throw IntegrityError(combining_not_allowed);
  }  
  mol::EntityView diff=ev1.Copy();
  mol::ChainViewList::const_iterator c_it=ev2.GetChainList().begin();
  for ( ; c_it!=ev2.GetChainList().end(); ++c_it) {
    mol::ChainView cv=*c_it;
    mol::ChainView cv2=diff.FindChain(cv.GetHandle());
    if (!cv2) {
      continue;
    } else {
      mol::ResidueViewList::const_iterator r_it=cv.GetResidueList().begin();
      for (; r_it!=cv.GetResidueList().end(); ++r_it) {
        mol::ResidueView rv=*r_it;
        mol::ResidueView rv2=cv2.FindResidue(rv.GetHandle());
        if (!rv2) {
          continue;
        } else {
          mol::AtomViewList::const_iterator a_it=rv.GetAtomList().begin();
          for (; a_it!=rv.GetAtomList().end(); ++a_it) {
            if (mol::AtomView av=rv2.FindAtom((*a_it).GetHandle())) {
              rv2.RemoveAtom(av);
            }
          }
          if (rv2.GetAtomList().empty()) {
            cv2.RemoveResidue(rv2);
          }
        }
      }
      if (cv2.GetResidueList().empty()) {
        diff.RemoveChain(cv2);
      }
    }
  }
  BondHandleList bonds=ev2.GetBondList();
  for (BondHandleList::const_iterator i=bonds.begin(), 
       e=bonds.end(); i!=e; ++i) {
    diff.RemoveBond(*i);
  }
  return diff;
}
                                         

mol::EntityView Intersection(const mol::EntityView& ev1, 
                             const mol::EntityView& ev2) 
{
  Profile intersection_time("mol::Intersection");
  if (ev1.GetHandle()!=ev2.GetHandle()) {
    throw IntegrityError(combining_not_allowed);
  }
  mol::EntityView intersection=ev1.CreateEmptyView();
  BondMap bm;
  BondHandleList bl;
  BondHandleList bonds_to_add;
  for (AtomViewIter a=ev1.AtomsBegin(),e=ev1.AtomsEnd(); a!=e; ++a) {
    AtomView av=*a;
    AtomView av2=ev2.FindAtom(av.GetHandle());
    if (av2.IsValid()) {
      intersection.AddAtom(av.GetHandle());
      bl=av.GetBondList();
      for (BondHandleList::iterator i=bl.begin(), e2=bl.end(); i!=e2; ++i) {
        BondMap::iterator x=bm.find(*i);
        if (x==bm.end()) {
          bm.insert(std::make_pair(*i, true));
        }
      }  
      bl=av2.GetBondList();
      for (BondHandleList::iterator i=bl.begin(), e2=bl.end(); i!=e2; ++i) {
        BondMap::iterator x=bm.find(*i);
        if (x!=bm.end()) {
          if (x->second) {
            bonds_to_add.push_back(*i);
            x->second=false;
          }
        } else {
          bm.insert(std::make_pair(*i, true));
        }
      }
    }
  }
  for (BondHandleList::iterator i=bonds_to_add.begin(), 
       e=bonds_to_add.end(); i!=e; ++i) {
    intersection.AddBond(*i);
  }
  return intersection;
}


std::pair<EntityView, EntityView> CompareViews(const mol::EntityView& view1,
                                               const mol::EntityView& view2)
{
  return std::make_pair(Difference(view1, view2), Difference(view2, view1));
}

namespace {
 
bool belongs_to_same_ent(mol::AtomHandle a, mol::EntityView v)
{
  return v.GetHandle()==a.GetEntity();
}


bool belongs_to_same_ent(mol::AtomView a, mol::EntityView v)
{
 return a.GetHandle().GetEntity()==v.GetHandle();
}

mol::AtomHandle to_handle(mol::AtomHandle a)
{
  return a;
}

mol::AtomHandle to_handle(mol::AtomView v)
{
  return v.GetHandle();
}



template <typename T>
mol::EntityView assemble_view(const std::vector<T>& l, mol::EntityView v)
{
  typedef typename std::vector<T>::const_iterator Iter;
  for (Iter i=l.begin(), e=l.end(); i!=e; ++i) {
    if (!belongs_to_same_ent(*i, v))
      throw IntegrityError(combining_not_allowed);
    v.AddAtom(to_handle(*i), mol::ViewAddFlag::CHECK_DUPLICATES);
  }
  return v;
}

}

mol::EntityView CreateViewFromAtomList(const mol::AtomHandleList& atoms)
{
  if (atoms.empty()) {
    return mol::EntityView();
  }
  
  return assemble_view(atoms, atoms[0].GetEntity().CreateEmptyView());
}

mol::EntityView CreateViewFromAtomList(const mol::AtomViewList& atoms)
{
  if (atoms.empty()) {
    return mol::EntityView();
  }
  return assemble_view(atoms, atoms[0].GetEntity().GetHandle().CreateEmptyView());
}

namespace {

// replicates chain, residue, atom hierarchy and bonds. torsions are not handled 
// here.
class Replicator : public EntityVisitor {
public:
  Replicator(impl::EntityImplPtr ent, EntityView view, bool inc_excl_atoms): 
     ent_(ent), inc_excl_atoms_(inc_excl_atoms)
  {
    view.Apply(*this);
  } 
  virtual bool VisitChain(const ChainHandle& chain)
  {
    chain_=ent_->InsertChain(chain.GetName());
    return true;
  }
  
  virtual bool VisitResidue(const ResidueHandle& res)
  {
    res_=chain_->AppendResidue(res.Impl());
    return true;
  }
  
  virtual bool VisitAtom(const AtomHandle& atom)
  {
    impl::AtomImplPtr dst_atom=res_->InsertAtom(atom.Impl());
    atom_map_.insert(std::make_pair(atom.GetHashCode(), dst_atom));
    return true;
  }
  
  impl::AtomImplPtr GetAtom(const AtomHandle& atom)
  {
    std::map<unsigned long, impl::AtomImplPtr>::iterator i;
    i=atom_map_.find(atom.GetHashCode());
    if (i!=atom_map_.end()) {
      return i->second;
    }
    return impl::AtomImplPtr();
  }
    
  virtual bool VisitBond(const BondHandle& bond)
  {
    impl::AtomImplPtr a1=this->GetAtom(bond.GetFirst());
    impl::AtomImplPtr a2=this->GetAtom(bond.GetSecond());
    if (a1 && a2) {
      ent_->Connect(a1, a2, 0, 0, 0, bond.GetBondOrder());
      return false;
    }
    if (!inc_excl_atoms_) {
      return false;
    }
    if (a1) {
      impl::AtomImplPtr dst_atom=res_->InsertAtom(bond.GetSecond().Impl());
      atom_map_.insert(std::make_pair(bond.GetSecond().GetHashCode(), dst_atom));
      ent_->Connect(a1, dst_atom, 0, 0, 0, bond.GetBondOrder());
    } else if (a2) {
      impl::AtomImplPtr dst_atom=res_->InsertAtom(bond.GetFirst().Impl());
      atom_map_.insert(std::make_pair(bond.GetFirst().GetHashCode(), dst_atom));
      ent_->Connect(dst_atom, a2, 0, 0, 0, bond.GetBondOrder());
    }    
    return false;
  }
private:
  impl::EntityImplPtr   ent_;
  impl::ChainImplPtr    chain_;
  impl::ResidueImplPtr  res_;
  impl::AtomImplPtr     atom_;
  std::map<unsigned long, impl::AtomImplPtr> atom_map_;
  bool inc_excl_atoms_;
}; 

class TorsionAdder : public EntityVisitor {
public:
  TorsionAdder(Replicator& repl, impl::EntityImplPtr ent, EntityView view):
    ent_(ent), repl_(repl)
  {
    view.Apply(*this);
  }
  
  virtual bool VisitResidue(const ResidueHandle& res)
  {
    impl::ResidueImplPtr src_res=res.Impl();
    int res_index=res.GetIndex();
    const impl::TorsionImplList& torsions=src_res->GetTorsionList();
    for (impl::TorsionImplList::const_iterator k=torsions.begin(), 
         e3=torsions.end(); k!=e3; ++k) {
      impl::TorsionImplP t=*k;
      // only add torsion if all involved atoms have a residue index 
      // smaller/equal to our residue index
      if (t->GetFourth()->GetResidue()->GetIndex()>res_index ||
          t->GetThird()->GetResidue()->GetIndex()>res_index ||
          t->GetSecond()->GetResidue()->GetIndex()>res_index  ||
          t->GetFirst()->GetResidue()->GetIndex()>res_index) {
        continue;      
      }
      impl::AtomImplPtr atom1=repl_.GetAtom(AtomHandle(t->GetFirst()));
      impl::AtomImplPtr atom2=repl_.GetAtom(AtomHandle(t->GetSecond()));      
      impl::AtomImplPtr atom3=repl_.GetAtom(AtomHandle(t->GetThird()));      
      impl::AtomImplPtr atom4=repl_.GetAtom(AtomHandle(t->GetFourth()));      
      if (atom1 && atom2 && atom3 && atom4) {
        
      }
      ent_->AddTorsion(t->GetName(), atom1, atom2, atom3, atom4);
    }
    return false;
  }
private:
  impl::EntityImplPtr ent_;
  Replicator& repl_;
};


}

EntityHandle CreateEntityFromView(const EntityView& view,
                                  bool include_exlusive_atoms)
{
  Profile create_time("mol::CreateEntityFromView");
  EntityHandle ent=CreateEntity();
  Replicator replicator(ent.Impl(), view, include_exlusive_atoms);
  TorsionAdder adder(replicator, ent.Impl(), view);
  return ent;
}

}} // ns
