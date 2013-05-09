"""
Some functions for analyzing trajectories

Author: Niklaus Johner
"""

import os
import ost

def GetFrameFromEntity(eh):
  """
  This function returns a CoordFrame from an EntityHandle
  Input:
    eh : EntityHandle
  """
  return ost.mol.CreateCoordFrame(eh.GetAtomPosList(ordered_by_index=True))
  
def GetDistanceBetwCenterOfMass(sele1,sele2):
  """
  This function calculates the distance between the centers of mass
  of sele1 and sele2, two selections from the same Entity.
  Input:
    sele1 : EntityView
    sele2 : EntityView
  """
  if not sele1.IsValid() and sele2.IsValid():
    print 'invalid view'
    return
  eh=sele1.GetHandle()
  if not eh==sele2.GetHandle():
    print 'The two views must be from the same entity'
    return
  f=GetFrameFromEntity(eh)
  return f.GetDistanceBetwCenterOfMass(sele1,sele2)

def GetMinDistanceBetweenViews(sele1,sele2):
  """
  This function calculates the minimal distance between
  sele1 and sele2, two selections from the same Entity.
  Input:
    sele1 : EntityView
    sele2 : EntityView
  """
  if not sele1.IsValid() and sele2.IsValid():
    print 'invalid view'
    return
  eh=sele1.GetHandle()
  if not eh==sele2.GetHandle():
    print 'The two views must be from the same entity'
    return
  f=GetFrameFromEntity(eh)
  return f.GetMinDistance(sele1,sele2)

def GetMinDistBetwCenterOfMassAndView(sele1,sele2):
  """
  This function calculates the minimal distance between sele2 and
  the center of mass of sele1, two selections from the same Entity.
  Input:
    sele1 : EntityView from which the center of mass is taken
    sele2 : EntityView
  """
  if not sele1.IsValid() and sele2.IsValid():
    print 'invalid view'
    return
  eh=sele1.GetHandle()
  if not eh==sele2.GetHandle():
    print 'The two views must be from the same entity'
    return
  f=GetFrameFromEntity(eh)
  return f.GetMinDistBetwCenterOfMassAndView(sele1,sele2)
  

def GetAlphaHelixContent(sele1):
  """
  This function calculates the content of alpha helix in a view.
  All residues in the view have to ordered and adjacent (no gaps allowed)
  Input:
    sele1 : EntityView
  """
  if not sele1.IsValid():
    print 'invalid view'
    return
  eh=sele1.GetHandle()
  f=GetFrameFromEntity(eh)
  return f.GetAlphaHelixContent(sele1)


def CalculateBestFitLine(sele1):
  """
  This function calculates the best fit line to the atoms in sele1.
  Input:
    sele1 : EntityView
  It returns a geom::Line3
  """
  if not sele1.IsValid():
    print 'invalid view'
    return
  eh=sele1.GetHandle()
  f=GetFrameFromEntity(eh)
  return f.GetODRLine(sele1)

def CalculateBestFitPlane(sele1):
  """
  This function calculates the best fit plane to the atoms in sele1.
  Input:
    sele1 : EntityView
  It returns a geom::Plane
  """
  if not sele1.IsValid():
    print 'invalid view'
    return
  eh=sele1.GetHandle()
  f=GetFrameFromEntity(eh)
  return f.GetODRPlane(sele1)

def CalculateHelixAxis(sele1):
  """
  This function calculates the best fit cylinder to the CA atoms in sele1,
  and returns its axis as a Line3.  residues should be ordered correctly
  in the EntityView.
  Input:
    sele1 : EntityView
  It returns a geom::Line3
  """
  if not sele1.IsValid():
    print 'invalid view'
    return
  eh=sele1.GetHandle()
  f=GetFrameFromEntity(eh)
  return f.FitCylinder(sele1)


def CalculateDistanceDifferenceMatrix(sele1,sele2):
  """
  This function calculates the pairwise distance differences between two EntityViews.
  The two EntityViews should have the same number of atoms
  It returns an NxN DistanceDifferenceMatrix M (where N is the number of atoms in sele1)
  where M[i,j]=(sele2.atoms[i].pos-sele2.atoms[j].pos)-(sele1.atoms[i].pos-sele1.atoms[j].pos)
  """
  try:import numpy as npy
  except ImportError:
    LogError("Function needs numpy, but I could not import it.")
    raise
  if not sele1.IsValid() and sele2.IsValid():
    print 'invalid view'
    return
  if not sele1.GetAtomCount()==sele2.GetAtomCount():
    print 'The two views must have the same number of atoms'
    return
  n_atoms=sele1.GetAtomCount()
  M=npy.zeros([n_atoms,n_atoms])
  for i,a1 in enumerate(sele1.atoms):
    for j,a2 in enumerate(sele2.atoms):
      if i>=j:continue
      d1=geom.Distance(a1.pos,a2.pos)
      d2=geom.Distance(sele2.atoms[i].pos,sele2.atoms[j].pos)
      M[i,j]=d2-d1
      M[j,i]=d2-d1
  return M


