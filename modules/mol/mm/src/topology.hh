#ifndef OST_MM_TOPOLOGY_HH
#define OST_MM_TOPOLOGY_HH

#include <vector>
#include <map>
#include <set>

#include <boost/shared_ptr.hpp>

#include <ost/message.hh>
#include <ost/mol/atom_handle.hh>
#include <ost/mol/residue_handle.hh>
#include <ost/mol/entity_handle.hh>
#include <ost/mol/mm/index.hh>
#include <ost/mol/mm/buildingblock.hh>
#include <ost/mol/mm/block_modifiers.hh>
#include <ost/mol/mm/forcefield.hh>
#include <ost/mol/mm/mm_settings.hh>
#include <ost/mol/mm/index.hh>
#include <ost/mol/mm/mm_interaction.hh>
#include <ost/mol/xcs_editor.hh>
#include <ost/mol/bond_handle.hh>
#include <ost/mol/residue_prop.hh>

#include <time.h>



namespace ost { namespace mol{ namespace mm{

class Topology;
typedef boost::shared_ptr<Topology> TopologyPtr;


class Topology{

public:

  Topology(const ost::mol::EntityHandle& ent, const std::vector<Real>& masses);

  static TopologyPtr Load(const String& filename);

  void Save(const String& filename);

  uint AddHarmonicBond(uint index_one,
                       uint index_two, 
                       Real bond_length,
                       Real force_constant);

  uint AddHarmonicAngle(uint index_one,
                        uint index_two,
                        uint index_three,
                        Real angle,
                        Real force_constant);

  uint AddUreyBradleyAngle(uint index_one,
                           uint index_two,
                           uint index_three,
                           Real angle,
                           Real angle_force_constant,
                           Real bond_length,
                           Real bond_force_constant);

  uint AddPeriodicDihedral(uint index_one,
                           uint index_two,
                           uint index_three,
                           uint index_four,
                           int multiplicity,
                           Real phase,
                           Real force_constant);

  uint AddPeriodicImproper(uint index_one,
                           uint index_two,
                           uint index_three,
                           uint index_four,
                           int multiplicity,
                           Real phase,
                           Real force_constant);

  uint AddHarmonicImproper(uint index_one,
                           uint index_two,
                           uint index_three,
                           uint index_four,
                           Real angle,
                           Real force_constant);

  uint AddCMap(uint index_one,
               uint index_two,
               uint index_three,
               uint index_four,
               uint index_five,
               int dimension,
               std::vector<Real> values);

  uint AddLJPair(uint index_one, 
                 uint index_two,
                 Real sigma,
                 Real epsilon);


  uint AddDistanceConstraint(uint index_one, 
                             uint index_two,
                             Real distance);

  uint AddExclusion(uint index_one,
                    uint index_two);

  void AddPositionConstraint(uint index);

  void ResetPositionConstraints() { position_constraints_.clear(); }

  void ResetExclusions() { exclusions_.clear(); }

  uint AddHarmonicPositionRestraint(uint index, const geom::Vec3& ref_position, Real k, 
                                    Real x_scale = 1.0, Real y_scale = 1.0, Real z_scale = 1.0);

  uint AddHarmonicDistanceRestraint(uint index_one, uint index_two, 
                                    Real length, Real force_constant);

  //Single atom parameters are expected to be set at once...
  void SetSigmas(const std::vector<Real>& sigmas);

  void SetSigma(uint index, Real sigma);

  void SetEpsilons(const std::vector<Real>& epsilons);

  void SetEpsilon(uint index, Real epsilon);

  void SetGBSARadii(const std::vector<Real>& gbsa_radii);

  void SetGBSARadius(uint index, Real radius);

  void SetOBCScalings(const std::vector<Real>& obc_scaling);

  void SetOBCScaling(uint index, Real scaling);

  void SetCharges(const std::vector<Real>& charges);

  void SetCharge(uint index, Real charge);

  void SetMasses(const std::vector<Real>& masses);

  void SetMass(uint index, Real mass);

  void SetFudgeQQ(Real fudge) { fudge_qq_ = fudge; }

  void SetFudgeLJ(Real fudge) { fudge_lj_ = fudge;} 


  void GetHarmonicBondParameters(uint index, uint& index_one, uint& index_two,
                                 Real& bond_length, Real& force_constant) const;

  void GetHarmonicAngleParameters(uint index, uint& index_one, uint& index_two, uint& index_three,
                                  Real& angle, Real& force_constant) const;

  void GetUreyBradleyAngleParameters(uint index, uint& index_one, uint& index_two, uint& index_three,
                                     Real& angle, Real& angle_force_constant, Real& bond_length, Real& bond_force_constant) const;

  void GetPeriodicDihedralParameters(uint index, uint& index_one, uint& index_two, uint& index_three, uint& index_four,
                                     int& multiplicity, Real& phase, Real& force_constant) const;

  void GetPeriodicImproperParameters(uint index, uint& index_one, uint& index_two, uint& index_three, uint& index_four,
                                     int& multiplicity, Real& phase, Real& force_constant) const;

  void GetHarmonicImproperParameters(uint index, uint& index_one, uint& index_two, uint& index_three, uint& index_four,
                                     Real& angle, Real& force_constant) const;

  void GetCMapParameters(uint index, uint& index_one, uint& index_two, uint& index_three, uint& index_four, uint& index_five,
                         int& dimension, std::vector<Real>& map) const;

  void GetLJPairParameters(uint index, uint& index_one, uint& index_two,
                           Real& sigma, Real& epsilon) const;

  void GetDistanceConstraintParameters(uint index, uint& index_one, uint& index_two,
                                       Real& distance) const;

  void GetHarmonicPositionRestraintParameters(uint index, uint& atom_index, geom::Vec3& ref_position,
                                               Real& k, Real& x_scale, Real& y_scale, Real& z_scale) const;

  void GetHarmonicDistanceRestraintParameters(uint index, uint& atom_one, uint& atom_two, Real& length,
                                               Real& force_constant) const;

  void SetHarmonicBondParameters(uint index, const Real& bond_length, const Real& force_constant);

  void SetHarmonicAngleParameters(uint index, const Real& angle, const Real& force_constant);

  void SetUreyBradleyAngleParameters(uint index, const Real& angle, const Real& angle_force_constant, const Real& bond_length, const Real& bond_force_constant);

  void SetPeriodicDihedralParameters(uint index, const int& multiplicity, const Real& phase, const Real& force_constant);

  void SetPeriodicImproperParameters(uint index, const int& multiplicity, const Real& phase, const Real& force_constant);

  void SetHarmonicImproperParameters(uint index, const Real& angle, const Real& force_constant);

  void SetCMapParameters(uint index, const int& dimension, const std::vector<Real>& map);

  void SetLJPairParameters(uint index, const Real& sigma, const Real& epsilon);

  void SetDistanceConstraintParameters(uint index, const Real& distance);

  void SetHarmonicPositionRestraintParameters(uint index, const geom::Vec3& ref_position, Real k, 
                                               Real x_scale = 1.0, Real y_scale = 1.0, Real z_scale = 1.0);

  void SetHarmonicDistanceRestraintParameters(uint index, Real length, Real force_constant);

  ost::mol::EntityHandle GetEntity() const { return ent_; };

  uint GetAtomIndex(const ost::mol::AtomHandle& at) const;

  uint GetAtomIndex(uint residue_index, const String& atom_name) const;

  uint GetResidueIndex(const ost::mol::ResidueHandle& res) const;

  const std::vector<std::pair<Index<2>, std::vector<Real> > >& GetHarmonicBonds() const { return harmonic_bonds_; }
 
  const std::vector<std::pair<Index<3>, std::vector<Real> > >& GetHarmonicAngles() const { return harmonic_angles_; }
 
  const std::vector<std::pair<Index<3>, std::vector<Real> > >& GetUreyBradleyAngles() const { return urey_bradley_angles_; }
 
  const std::vector<std::pair<Index<4>, std::vector<Real> > >& GetPeriodicDihedrals() const { return periodic_dihedrals_; }
 
  const std::vector<std::pair<Index<4>, std::vector<Real> > >& GetPeriodicImpropers() const { return periodic_impropers_; }
 
  const std::vector<std::pair<Index<4>, std::vector<Real> > >& GetHarmonicImpropers() const { return harmonic_impropers_; }
 
  const std::vector<std::pair<Index<5>, std::vector<Real> > >& GetCMaps() const { return cmaps_; }
 
  const std::vector<std::pair<Index<2>, std::vector<Real> > >& GetLJPairs() const { return lj_pairs_; }
 
  const std::vector<std::pair<Index<2>, std::vector<Real> > >& GetDistanceConstraints() const { return distance_constraints_; }

  const std::vector<std::pair<Index<1>, std::vector<Real> > >&GetHarmonicPositionRestraints() const { return harmonic_position_restraints_; }
 
  const std::vector<std::pair<Index<2>, std::vector<Real> > >&GetHarmonicDistanceRestraints() const{ return harmonic_distance_restraints_; }

  const std::vector<Index<2> >& GetExclusions() const { return exclusions_; }
 
  const std::vector<uint>& GetPositionConstraints() const { return position_constraints_; }

  std::vector<Real> GetSigmas() const { return sigmas_; }

  std::vector<Real> GetEpsilons() const { return epsilons_; }
 
  std::vector<Real> GetGBSARadii() const { return gbsa_radii_; }

  std::vector<Real> GetOBCScalings() const { return obc_scaling_; }
 
  std::vector<Real> GetCharges() const { return charges_; }

  Real GetCharge(uint index) const;

  Real GetMass(uint index) const;

  Real GetSigma(uint index) const;

  Real GetEpsilon(uint index) const;

  Real GetGBSARadius(uint index) const;

  Real GetOBCScaling(uint index) const;

  std::vector<Real> GetMasses() const { return atom_masses_; }

  Real GetFudgeQQ() const { return fudge_qq_; }

  Real GetFudgeLJ() const { return fudge_lj_; }

  std::vector<uint> GetHarmonicBondIndices(uint index_one,
                                            uint index_two) const;

  std::vector<uint> GetHarmonicAngleIndices(uint index_one,
                                             uint index_two,
                                             uint index_three) const;

  std::vector<uint> GetUreyBradleyAngleIndices(uint index_one,
                                                uint index_two,
                                                uint index_three) const;

  std::vector<uint> GetPeriodicDihedralIndices(uint index_one,
                                                uint index_two,
                                                uint index_three,
                                                uint index_four) const;

  std::vector<uint> GetPeriodicImproperIndices(uint index_one,
                                                uint index_two,
                                                uint index_three,
                                                uint index_four) const;

  std::vector<uint> GetHarmonicImproperIndices(uint index_one,
                                                uint index_two,
                                                uint index_three,
                                                uint index_four) const;

  std::vector<uint> GetCMapIndices(uint index_one,
                                    uint index_two,
                                    uint index_three,
                                    uint index_four,
                                    uint index_five) const;

  int GetLJPairIndex(uint index_one,
                     uint index_two) const;

  int GetDistanceConstraintIndex(uint index_one,
                                 uint index_two) const;



  std::vector<uint> GetHarmonicDistanceRestraintIndices(uint index_one,
                                                        uint index_two) const;


  std::vector<uint> GetHarmonicBondIndices(uint atom_index) const;

  std::vector<uint> GetHarmonicAngleIndices(uint atom_index) const;

  std::vector<uint> GetUreyBradleyAngleIndices(uint atom_index) const;

  std::vector<uint> GetPeriodicDihedralIndices(uint atom_index) const;

  std::vector<uint> GetPeriodicImproperIndices(uint atom_index) const;

  std::vector<uint> GetHarmonicImproperIndices(uint atom_index) const;

  std::vector<uint> GetCMapIndices(uint atom_index) const;

  std::vector<uint> GetLJPairIndices(uint atom_index) const;

  std::vector<uint> GetDistanceConstraintIndices(uint atom_index) const;

  std::vector<uint> GetHarmonicPositionRestraintIndices(uint atom_index) const;

  std::vector<uint> GetHarmonicDistanceRestraintIndices(uint atom_index) const;

  uint GetNumAtoms() { return num_atoms_; }

  uint GetNumResidues() { return num_residues_; }

  uint GetNumHarmonicBonds() { return harmonic_bonds_.size(); }

  uint GetNumHarmonicAngles() { return harmonic_angles_.size(); }

  uint GetNumUreyBradleyAngles() { return urey_bradley_angles_.size(); }

  uint GetNumPeriodicDihedrals() { return periodic_dihedrals_.size(); }

  uint GetNumPeriodicImpropers() { return periodic_impropers_.size(); }

  uint GetNumHarmonicImpropers() { return harmonic_impropers_.size(); }

  uint GetNumCMaps() { return cmaps_.size(); }

  uint GetNumLJPairs() { return lj_pairs_.size(); }

  uint GetNumDistanceConstraints() { return distance_constraints_.size(); }

  uint GetNumPositionConstraints() { return position_constraints_.size(); }

  uint GetNumHarmonicPositionRestraints() { return harmonic_position_restraints_.size(); }

  uint GetNumHarmonicDistanceRestraints() { return harmonic_distance_restraints_.size();}

  uint GetNumExclusions() { return exclusions_.size(); }

  void Merge(TopologyPtr p);

  void SetEntityPositions(const geom::Vec3List& positions);

  template <typename DS>
  void Serialize(DS& ds){
    
    ds & num_atoms_;
    ds & num_residues_;

    uint num_chains = 0;
    uint num_residues = 0;
    uint num_atoms = 0;
    uint num_bonded_atoms = 0;
    Real x_pos = 0.0;
    Real y_pos = 0.0;
    Real z_pos = 0.0;
    Real bfac = 0.0;
    Real occ = 0.0;
    bool is_hetatm = false;
    String chain_name = "X";
    String res_name = "XXX";
    int resnum_num = 0;
    char resnum_code = '\0';
    String atom_name = "X";
    String atom_element = "X";
    uint atom_index = 0;
    uint num_items = 0;
    Index<2> actual_index;

    if(ds.IsSource()){
      ost::mol::EntityHandle ent = ost::mol::CreateEntity();
      ost::mol::XCSEditor ed = ent.EditXCS();
      ds & num_chains;
      for(uint i = 0; i < num_chains; ++i){
        ds & chain_name;
        ds & num_residues;
        ost::mol::ChainHandle chain = ed.InsertChain(chain_name);
        for(uint j = 0; j < num_residues; ++j){
          ds & res_name;
          ds & resnum_num;
          ds & resnum_code;
          ds & num_atoms;
          ost::mol::ResNum num(resnum_num,resnum_code);
          ost::mol::ResidueHandle res = ed.AppendResidue(chain,res_name,num);
          for(uint k = 0; k < num_atoms; ++k){
            ds & atom_name;
            ds & atom_element;
            ds & x_pos;
            ds & y_pos;
            ds & z_pos;
            ds & bfac;
            ds & occ;
            ds & is_hetatm;
            geom::Vec3 pos(x_pos,y_pos,z_pos);
            ed.InsertAtom(res,atom_name,pos,atom_element,occ,bfac,is_hetatm);
          }
        }
      }
      ent_ = ent;
      atom_list_ = ent_.GetAtomList();
      res_list_ = ent_.GetResidueList();
      for(uint i = 0; i < atom_list_.size(); ++i){
        ds & num_bonded_atoms;
        for(uint j = 0; j < num_bonded_atoms; ++j){
          ds & atom_index;
          ed.Connect(atom_list_[i],atom_list_[atom_index]);
        }
      }
      InitMappers();
    }
    else{
      num_chains = ent_.GetChainCount();
      ds & num_chains;
      ost::mol::ChainHandleList chain_list = ent_.GetChainList();
      for(ost::mol::ChainHandleList::iterator i = chain_list.begin();
          i != chain_list.end(); ++i){
        chain_name = i->GetName();
        num_residues = i->GetResidueCount();
        ds & chain_name;
        ds & num_residues;
        ost::mol::ResidueHandleList res_list = i->GetResidueList();
        for(ost::mol::ResidueHandleList::iterator j = res_list.begin();
            j != res_list.end(); ++j){
          res_name = j->GetKey();
          resnum_num = j->GetNumber().GetNum();
          resnum_code = j->GetNumber().GetInsCode(); 
          num_atoms = j->GetAtomCount();
          ds & res_name;
          ds & resnum_num;
          ds & resnum_code;
          ds & num_atoms;
          ost::mol::AtomHandleList atom_list = j->GetAtomList();
          for(ost::mol::AtomHandleList::iterator k = atom_list.begin();
              k != atom_list.end(); ++k){
            atom_name = k->GetName();
            atom_element = k->GetElement();
            geom::Vec3 pos = k->GetPos();
            bfac = k->GetBFactor();
            occ = k->GetOccupancy();
            is_hetatm = k->IsHetAtom();
            ds & atom_name;
            ds & atom_element;
            ds & pos[0];
            ds & pos[1];
            ds & pos[2];
            ds & bfac;
            ds & occ;
            ds & is_hetatm;
          }
        }
      }
      ost::mol::AtomHandleList bonded_atoms;
      for(ost::mol::AtomHandleList::iterator i = atom_list_.begin();
          i != atom_list_.end(); ++i){
        bonded_atoms = i->GetBondPartners();
        num_bonded_atoms = bonded_atoms.size();
        ds & num_bonded_atoms;
        for(ost::mol::AtomHandleList::iterator j = bonded_atoms.begin();
            j != bonded_atoms.end(); ++j){
          atom_index = this->GetAtomIndex(*j);
          ds & atom_index;
        }
      }
    }

    ds & fudge_qq_;
    ds & fudge_lj_;

    if(ds.IsSource()){
      ds & num_items;
      atom_masses_ = std::vector<Real>(num_items);
      ds & num_items;
      sigmas_ = std::vector<Real>(num_items);
      ds & num_items;
      epsilons_ = std::vector<Real>(num_items);
      ds & num_items;
      gbsa_radii_ = std::vector<Real>(num_items);
      ds & num_items;
      obc_scaling_ = std::vector<Real>(num_items);
      ds & num_items;
      charges_ = std::vector<Real>(num_items);
      ds & num_items;
      position_constraints_ = std::vector<uint>(num_items);
    }
    else{
      num_items = atom_masses_.size();
      ds & num_items;
      num_items = sigmas_.size();
      ds & num_items;
      num_items = epsilons_.size();
      ds & num_items;
      num_items = gbsa_radii_.size();
      ds & num_items;
      num_items = obc_scaling_.size();
      ds & num_items;
      num_items = charges_.size();
      ds & num_items;
      num_items = position_constraints_.size();
      ds & num_items;
    }

    for(std::vector<Real>::iterator i = atom_masses_.begin();
        i != atom_masses_.end(); ++i){
      ds & *i;
    }

    for(std::vector<Real>::iterator i = sigmas_.begin();
        i != sigmas_.end(); ++i){
      ds & *i;
    }

    for(std::vector<Real>::iterator i = epsilons_.begin();
        i != epsilons_.end(); ++i){
      ds & *i;
    }

    for(std::vector<Real>::iterator i = gbsa_radii_.begin();
        i != gbsa_radii_.end(); ++i){
      ds & *i;
    }

    for(std::vector<Real>::iterator i = obc_scaling_.begin();
        i != obc_scaling_.end(); ++i){
      ds & *i;
    }

    for(std::vector<Real>::iterator i = charges_.begin();
        i != charges_.end(); ++i){
      ds & *i;
    }

    for(std::vector<uint>::iterator i = position_constraints_.begin();
        i != position_constraints_.end(); ++i){
      ds & *i;
    }

    if(ds.IsSource()){
      ds & num_items;
      for(uint i = 0; i < num_items; ++i){
        harmonic_bonds_.push_back(std::make_pair(Index<2>(),std::vector<Real>(2)));
      }

      ds & num_items;
      for(uint i = 0; i < num_items; ++i){
        harmonic_angles_.push_back(std::make_pair(Index<3>(),std::vector<Real>(2)));
      }

      ds & num_items;
      for(uint i = 0; i < num_items; ++i){
        urey_bradley_angles_.push_back(std::make_pair(Index<3>(),std::vector<Real>(4)));
      }

      ds & num_items;
      for(uint i = 0; i < num_items; ++i){
        periodic_dihedrals_.push_back(std::make_pair(Index<4>(),std::vector<Real>(3)));
      }

      ds & num_items;
      for(uint i = 0; i < num_items; ++i){
        periodic_impropers_.push_back(std::make_pair(Index<4>(),std::vector<Real>(3)));
      }

      ds & num_items;
      for(uint i = 0; i < num_items; ++i){
        harmonic_impropers_.push_back(std::make_pair(Index<4>(),std::vector<Real>(2)));
      }

      ds & num_items;
      for(uint i = 0; i < num_items; ++i){
        uint num_values = 0;
        ds & num_values;
        cmaps_.push_back(std::make_pair(Index<5>(),std::vector<Real>(num_values)));
      }

      ds & num_items;
      for(uint i = 0; i < num_items; ++i){
        lj_pairs_.push_back(std::make_pair(Index<2>(),std::vector<Real>(2)));
      } 

      ds & num_items;
      for(uint i = 0; i < num_items; ++i){
        distance_constraints_.push_back(std::make_pair(Index<2>(),std::vector<Real>(1)));
      }

      ds & num_items;
      exclusions_ = std::vector<Index<2> >(num_items);

      ds & num_items;
      for(uint i = 0; i < num_items; ++i){
        harmonic_position_restraints_.push_back(std::make_pair(Index<1>(),std::vector<Real>(7)));
      }

      ds & num_items;
      for(uint i = 0; i < num_items; ++i){
        harmonic_distance_restraints_.push_back(std::make_pair(Index<2>(),std::vector<Real>(2)));
      }
    }
    else{
      num_items = harmonic_bonds_.size();
      ds & num_items;
      num_items = harmonic_angles_.size();
      ds & num_items;
      num_items = urey_bradley_angles_.size();
      ds & num_items;
      num_items = periodic_dihedrals_.size();
      ds & num_items;
      num_items = periodic_impropers_.size();
      ds & num_items;
      num_items = harmonic_impropers_.size();
      ds & num_items;
      num_items = cmaps_.size();
      ds & num_items;
      for(uint i = 0; i < cmaps_.size(); ++i){
        num_items = cmaps_[i].second.size();
        ds & num_items;
      }
      num_items = lj_pairs_.size();
      ds & num_items;
      num_items = distance_constraints_.size();
      ds & num_items;
      num_items = exclusions_.size();
      ds & num_items;
      num_items = harmonic_position_restraints_.size();
      ds & num_items;
      num_items = harmonic_distance_restraints_.size();
      ds & num_items;
    }

    for(std::vector<std::pair<Index<2>,std::vector<Real> > >::iterator i = harmonic_bonds_.begin();
        i != harmonic_bonds_.end(); ++i){
      ds & i->first;
      ds & i->second[0];
      ds & i->second[1];
    }

    for(std::vector<std::pair<Index<3>,std::vector<Real> > >::iterator i = harmonic_angles_.begin();
        i != harmonic_angles_.end(); ++i){
      ds & i->first;
      ds & i->second[0];
      ds & i->second[1];
    }

    for(std::vector<std::pair<Index<3>,std::vector<Real> > >::iterator i = urey_bradley_angles_.begin();
        i != urey_bradley_angles_.end(); ++i){
      ds & i->first;
      ds & i->second[0];
      ds & i->second[1];
      ds & i->second[2];
      ds & i->second[3];
    }

    for(std::vector<std::pair<Index<4>,std::vector<Real> > >::iterator i = periodic_dihedrals_.begin();
        i != periodic_dihedrals_.end(); ++i){
      ds & i->first;
      ds & i->second[0];
      ds & i->second[1];
      ds & i->second[2];
    }

    for(std::vector<std::pair<Index<4>,std::vector<Real> > >::iterator i = periodic_impropers_.begin();
        i != periodic_impropers_.end(); ++i){
      ds & i->first;
      ds & i->second[0];
      ds & i->second[1];
      ds & i->second[2];
    }

    for(std::vector<std::pair<Index<4>,std::vector<Real> > >::iterator i = harmonic_impropers_.begin();
        i != harmonic_impropers_.end(); ++i){
      ds & i->first;
      ds & i->second[0];
      ds & i->second[1];
    }

    for(std::vector<std::pair<Index<5>,std::vector<Real> > >::iterator i = cmaps_.begin();
        i != cmaps_.end(); ++i){
      ds & i->first;
      for(std::vector<Real>::iterator j = i->second.begin();
          j != i->second.end(); ++j){
        ds & (*j);
      }
    }

    for(std::vector<std::pair<Index<2>,std::vector<Real> > >::iterator i = lj_pairs_.begin();
        i != lj_pairs_.end(); ++i){
      ds & i->first;
      ds & i->second[0];
      ds & i->second[1];
    }

    for(std::vector<std::pair<Index<2>,std::vector<Real> > >::iterator i = distance_constraints_.begin();
        i != distance_constraints_.end(); ++i){
      ds & i->first;
      ds & i->second[0];
    }

    for(std::vector<Index<2> >::iterator i = exclusions_.begin();
        i != exclusions_.end(); ++i){
      ds & (*i);
    }

    for(std::vector<std::pair<Index<1>,std::vector<Real> > >::iterator i = harmonic_position_restraints_.begin();
        i != harmonic_position_restraints_.end(); ++i){
      ds & i->first;
      ds & i->second[0];
      ds & i->second[1];
      ds & i->second[2];
      ds & i->second[3];
      ds & i->second[4];
      ds & i->second[5];
      ds & i->second[6];
    }

    for(std::vector<std::pair<Index<2>,std::vector<Real> > >::iterator i = harmonic_distance_restraints_.begin();
        i != harmonic_distance_restraints_.end(); ++i){
      ds & i->first;
      ds & i->second[0];
      ds & i->second[1];
    }

    if(ds.IsSource()){
      ds & num_items;
      for(uint i = 0; i < num_items; ++i){
        ds & actual_index;
        added_lj_pairs_.insert(actual_index);
      }

      ds & num_items;
      for(uint i = 0; i < num_items; ++i){
        ds & actual_index;
        added_distance_constraints_.insert(actual_index);
      }

      ds & num_items;
      for(uint i = 0; i < num_items; ++i){
        ds & actual_index;
        added_exclusions_.insert(actual_index);
      }
    }
    else{
      num_items = added_lj_pairs_.size();
      ds & num_items;
      for(std::set<Index<2> >::iterator i = added_lj_pairs_.begin(); 
          i != added_lj_pairs_.end(); ++i){
        actual_index = *i;
        ds & actual_index;
      }
      num_items = added_distance_constraints_.size();
      ds & num_items;
      for(std::set<Index<2> >::iterator i = added_distance_constraints_.begin(); 
          i != added_distance_constraints_.end(); ++i){
        actual_index = *i;
        ds & actual_index;
      }
      num_items = added_exclusions_.size();
      ds & num_items;
      for(std::set<Index<2> >::iterator i = added_exclusions_.begin(); 
          i != added_exclusions_.end(); ++i){
        actual_index = *i;
        ds & actual_index;
      }
    }
  }

private:

  Topology() { } //hidden constructor without parameters

  void InitMappers();

  ost::mol::EntityHandle ent_;
  ost::mol::AtomHandleList atom_list_;
  ost::mol::ResidueHandleList res_list_;

  uint num_atoms_;
  uint num_residues_;

  std::map<long,uint> atom_index_mapper_;
  std::map<long,uint> residue_index_mapper_;
  std::vector<std::map<String,uint> > atom_name_mapper_;

  //fudge parameters for lj 1,4 pairs
  Real fudge_qq_;
  Real fudge_lj_;

  //single atom parameters
  std::vector<Real> atom_masses_;
  std::vector<Real> sigmas_;
  std::vector<Real> epsilons_;
  std::vector<Real> gbsa_radii_;
  std::vector<Real> obc_scaling_;
  std::vector<Real> charges_;
  std::vector<uint> position_constraints_;

  //interactions with multiple atoms involved
  std::vector<std::pair<Index<2>,std::vector<Real> > > harmonic_bonds_;
  std::vector<std::pair<Index<3>,std::vector<Real> > > harmonic_angles_;
  std::vector<std::pair<Index<3>,std::vector<Real> > > urey_bradley_angles_;
  std::vector<std::pair<Index<4>,std::vector<Real> > > periodic_dihedrals_;
  std::vector<std::pair<Index<4>,std::vector<Real> > > periodic_impropers_;
  std::vector<std::pair<Index<4>,std::vector<Real> > > harmonic_impropers_;
  std::vector<std::pair<Index<5>,std::vector<Real> > > cmaps_;
  std::vector<std::pair<Index<2>,std::vector<Real> > > lj_pairs_;
  std::vector<std::pair<Index<2>,std::vector<Real> > > distance_constraints_;
  std::vector<Index<2> > exclusions_;
  std::vector<std::pair<Index<1>,std::vector<Real> > > harmonic_position_restraints_;
  std::vector<std::pair<Index<2>,std::vector<Real> > > harmonic_distance_restraints_;

  //the atoms of the interactions, that should be unique get tracked in here
  //note, that this is waste of memory, needs better implementation
  std::set<Index<2> > added_lj_pairs_;
  std::set<Index<2> > added_distance_constraints_;
  std::set<Index<2> > added_exclusions_;

};


}}} //ns

#endif
