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

/*
 * Author Niklaus Johner
 */
#include <stdexcept>
#include <ost/base.hh>
#include <ost/geom/vec3.hh>
#include <ost/base.hh>
#include <ost/geom/geom.hh>
#include <ost/mol/entity_view.hh>
#include <ost/mol/coord_group.hh>
#include "trajectory_analysis.hh"

namespace ost { namespace mol { namespace alg {

geom::Vec3List AnalyzeAtomPos(const CoordGroupHandle& traj, const AtomHandle& a1, unsigned int stride)
//This function extracts the position of an atom from a trajectory and returns it as a vector of geom::Vec3
{
  CheckHandleValidity(traj);
  geom::Vec3List pos;
  pos.reserve(ceil(traj.GetFrameCount()/float(stride)));
  int i1=a1.GetIndex();
  for (size_t i=0; i<traj.GetFrameCount(); i+=stride) {
    CoordFramePtr frame=traj.GetFrame(i);
    pos.push_back(frame->GetAtomPos(i1));
  }
  return pos;
}

std::vector<Real> AnalyzeDistanceBetwAtoms(const CoordGroupHandle& traj, const AtomHandle& a1, const AtomHandle& a2, 
                                  unsigned int stride)
//This function extracts the distance between two atoms from a trajectory and returns it as a vector
{
  CheckHandleValidity(traj);
  std::vector<Real> dist;
  dist.reserve(ceil(traj.GetFrameCount()/float(stride)));
  int i1=a1.GetIndex();
  int i2=a2.GetIndex();
  for (size_t i=0; i<traj.GetFrameCount(); i+=stride) {
    CoordFramePtr frame=traj.GetFrame(i);
    dist.push_back(frame->GetDistanceBetwAtoms(i1,i2));
  }
  return dist;
} 

std::vector<Real> AnalyzeAngle(const CoordGroupHandle& traj, const AtomHandle& a1, const AtomHandle& a2, 
                               const AtomHandle& a3, unsigned int stride)
//This function extracts the angle between three atoms from a trajectory and returns it as a vector
{
  CheckHandleValidity(traj);
  std::vector<Real> ang;
  ang.reserve(ceil(traj.GetFrameCount()/float(stride)));
  int i1=a1.GetIndex(),i2=a2.GetIndex(),i3=a3.GetIndex();
  for (size_t i=0; i<traj.GetFrameCount(); i+=stride) {
    CoordFramePtr frame=traj.GetFrame(i);
    ang.push_back(frame->GetAngle(i1,i2,i3));
  }
  return ang;
}

std::vector<Real> AnalyzeDihedralAngle(const CoordGroupHandle& traj, const AtomHandle& a1, const AtomHandle& a2, 
                                  const AtomHandle& a3, const AtomHandle& a4, unsigned int stride)
//This function extracts the dihedral angle between four atoms from a trajectory and returns it as a vector
{
  CheckHandleValidity(traj);
  std::vector<Real> ang;
  ang.reserve(ceil(traj.GetFrameCount()/float(stride)));
  int i1=a1.GetIndex(),i2=a2.GetIndex(),i3=a3.GetIndex(),i4=a4.GetIndex();
  for (size_t i=0; i<traj.GetFrameCount(); i+=stride) {
    CoordFramePtr frame=traj.GetFrame(i);
    ang.push_back(frame->GetDihedralAngle(i1,i2,i3,i4));
  }
  return ang;
}

geom::Vec3List AnalyzeCenterOfMassPos(const CoordGroupHandle& traj, const EntityView& sele,unsigned int stride)
//This function extracts the position of the CenterOfMass of a selection (entity view) from a trajectory 
//and returns it as a vector. 
  {
  CheckHandleValidity(traj);
  geom::Vec3List pos;
  pos.reserve(ceil(traj.GetFrameCount()/float(stride)));
  std::vector<unsigned long> indices;
  std::vector<Real> masses;
  GetIndicesAndMasses(sele, indices, masses);
  for (size_t i=0; i<traj.GetFrameCount(); i+=stride) {
    CoordFramePtr frame=traj.GetFrame(i);
    pos.push_back(frame->GetCenterOfMassPos(indices,masses));
  }
  return pos;
}

std::vector<Real> AnalyzeDistanceBetwCenterOfMass(const CoordGroupHandle& traj, const EntityView& sele1,
                                    const EntityView& sele2, unsigned int stride)
//This function extracts the distance between the CenterOfMass of two selection (entity views) from a trajectory 
//and returns it as a vector. 
  {
  CheckHandleValidity(traj);
  std::vector<Real> dist;
  dist.reserve(ceil(traj.GetFrameCount()/float(stride)));
  std::vector<unsigned long> indices1,indices2;
  std::vector<Real> masses1,masses2;
  GetIndicesAndMasses(sele1, indices1, masses1);
  GetIndicesAndMasses(sele2, indices2, masses2);
  for (size_t i=0; i<traj.GetFrameCount(); i+=stride) {
    CoordFramePtr frame=traj.GetFrame(i);
    dist.push_back(frame->GetDistanceBetwCenterOfMass(indices1,masses1,indices2,masses2));
  }
  return dist;
}

std::vector<Real> AnalyzeRMSD(const CoordGroupHandle& traj, const EntityView& reference_view,
                                    const EntityView& sele_view, unsigned int stride)
// This function extracts the rmsd between two entity views and returns it as a vector
// The views don't have to be from the same entity
// If you want to compare to frame i of the trajectory t, first use t.CopyFrame(i) for example:
// eh=io.LoadPDB(...),t=io.LoadCHARMMTraj(eh,...);Sele=eh.Select(...);t.CopyFrame(0);mol.alg.AnalyzeRMSD(t,Sele,Sele)
  {
  CheckHandleValidity(traj);
  int count_ref=reference_view.GetAtomCount();
  int count_sele=sele_view.GetAtomCount();
  if (count_ref!=count_sele){
    throw std::runtime_error("atom counts of the two views are not equal");
  }  
  std::vector<Real> rmsd;
  rmsd.reserve(ceil(traj.GetFrameCount()/float(stride)));
  std::vector<unsigned long> sele_indices;
  std::vector<geom::Vec3> ref_pos;
  GetIndices(reference_view, sele_indices);
  GetPositions(sele_view, ref_pos);
  for (size_t i=0; i<traj.GetFrameCount(); i+=stride) {
    CoordFramePtr frame=traj.GetFrame(i);
    rmsd.push_back(frame->GetRMSD(ref_pos,sele_indices));
  }
  return rmsd;
}
 
std::vector<Real>  AnalyzeMinDistance(const CoordGroupHandle& traj, const EntityView& view1,
                                      const EntityView& view2,unsigned int stride)
// This function extracts the minimal distance between two sets of atoms (view1 and view2) for
// each frame in a trajectory and returns it as a vector.
  {
  CheckHandleValidity(traj);
  if (view1.GetAtomCount()==0){
    throw std::runtime_error("first EntityView is empty");
  }
  if (view2.GetAtomCount()==0){
    throw std::runtime_error("second EntityView is empty");
  }  
  std::vector<Real> dist;
  dist.reserve(ceil(traj.GetFrameCount()/float(stride)));
  std::vector<unsigned long> indices1,indices2;
  GetIndices(view1, indices1);
  GetIndices(view2, indices2);
  for (size_t i=0; i<traj.GetFrameCount(); i+=stride) {
    CoordFramePtr frame=traj.GetFrame(i);
    dist.push_back(frame->GetMinDistance(indices1,indices2));
  }
    return dist;
}
  
std::vector<Real> AnalyzeMinDistanceBetwCenterOfMassAndView(const CoordGroupHandle& traj, const EntityView& view_cm,
                                            const EntityView& view_atoms,unsigned int stride)
  // This function extracts the minimal distance between a set of atoms (view_atoms) and the center of mass
  // of a second set of atoms (view_cm) for each frame in a trajectory and returns it as a vector.
  {
  CheckHandleValidity(traj);
  if (view_cm.GetAtomCount()==0){
    throw std::runtime_error("first EntityView is empty");
  }
  if (view_atoms.GetAtomCount()==0){
    throw std::runtime_error("second EntityView is empty");
  } 
  std::vector<Real> dist, masses_cm;
  dist.reserve(ceil(traj.GetFrameCount()/float(stride)));
  std::vector<unsigned long> indices_cm,indices_atoms;
  GetIndicesAndMasses(view_cm, indices_cm,masses_cm);
  GetIndices(view_atoms, indices_atoms);
  for (size_t i=0; i<traj.GetFrameCount(); i+=stride) {
    CoordFramePtr frame=traj.GetFrame(i);
    dist.push_back(frame->GetMinDistBetwCenterOfMassAndView(indices_cm, masses_cm, indices_atoms));
  }
  return dist;
  }

std::vector<Real> AnalyzeAromaticRingInteraction(const CoordGroupHandle& traj, const EntityView& view_ring1,
                                                   const EntityView& view_ring2,unsigned int stride)
  // This function is a crude analysis of aromatic ring interactions. For each frame in a trajectory, it calculates
  // the minimal distance between the atoms in one view and the center of mass of the other
  // and vice versa, and returns the minimum between these two minimal distances.
  // Concretely, if the two views are the heavy atoms of two rings, then it returns the minimal
  // center of mass - heavy atom distance betweent he two rings
  {
  CheckHandleValidity(traj);
  if (view_ring1.GetAtomCount()==0){
    throw std::runtime_error("first EntityView is empty");
  }
  if (view_ring2.GetAtomCount()==0){
    throw std::runtime_error("second EntityView is empty");
  } 
  std::vector<Real> dist, masses_ring1,masses_ring2;
  dist.reserve(ceil(traj.GetFrameCount()/float(stride)));
  std::vector<unsigned long> indices_ring1,indices_ring2;
  Real d1,d2;
  GetIndicesAndMasses(view_ring1, indices_ring1,masses_ring1);
  GetIndicesAndMasses(view_ring2, indices_ring2,masses_ring2);
  for (size_t i=0; i<traj.GetFrameCount(); i+=stride) {
    CoordFramePtr frame=traj.GetFrame(i);
    d1=frame->GetMinDistBetwCenterOfMassAndView(indices_ring1, masses_ring1, indices_ring2);
    d2=frame->GetMinDistBetwCenterOfMassAndView(indices_ring2, masses_ring2, indices_ring1);
    dist.push_back(std::min(d1,d2));
  }
  return dist;  
  }
}}} //ns