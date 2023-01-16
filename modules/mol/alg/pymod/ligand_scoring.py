import os

import numpy as np
import networkx

from ost import mol
from ost.mol.alg import chain_mapping


class LigandScorer:
    """ Helper class to access the various small molecule ligand (non polymer)
    scores available from ost.mol.alg.

    .. note ::
      Extra requirements:

      - Python modules `numpy` and `networkx` must be available
        (e.g. use ``pip install numpy networkx``)

    Mostly expects cleaned up structures (you can use the
    :class:`~ost.mol.alg.scoring.Scorer` outputs for that).

    :param model: Model structure - a deep copy is available as :attr:`model`.
                  No additional processing (ie. Molck), checks,
                  stereochemistry checks or sanitization is performed on the
                  input.
    :type model: :class:`ost.mol.EntityHandle`/:class:`ost.mol.EntityView`
    :param target: Target structure - a deep copy is available as :attr:`target`.
                  No additional processing (ie. Molck), checks or sanitization
                  is performed on the input.
    :type target: :class:`ost.mol.EntityHandle`/:class:`ost.mol.EntityView`
    :param model_ligands: Model ligands, as a list of
                  :class:`ost.mol.ResidueHandle` belonging to the model
                  entity. Can be instantiated with either a :class:list of
                  :class:`ost.mol.ResidueHandle`/:class:`ost.mol.ResidueView`
                  or of :class:`ost.mol.EntityHandle`/:class:`ost.mol.EntityView`.
                  If `None`, ligands will be extracted from the `model` entity,
                  from chains with :class:`~ost.mol.ChainType`
                  `CHAINTYPE_NON_POLY` (this is normally set properly in
                  entities loaded from mmCIF).
    :type model_ligands: :class:`list`
    :param target_ligands: Target ligands, as a list of
                  :class:`ost.mol.ResidueHandle` belonging to the target
                  entity. Can be instanciated either a :class:list of
                  :class:`ost.mol.ResidueHandle`/:class:`ost.mol.ResidueView`
                  or of :class:`ost.mol.EntityHandle`/:class:`ost.mol.EntityView`
                  containing a single residue each. If `None`, ligands will be
                  extracted from the `target` entity, from chains with
                  :class:`~ost.mol.ChainType` `CHAINTYPE_NON_POLY` (this is
                  normally set properly in entities loaded from mmCIF).
    :type target_ligands: :class:`list`
    :param resnum_alignments: Whether alignments between chemically equivalent
                              chains in *model* and *target* can be computed
                              based on residue numbers. This can be assumed in
                              benchmarking setups such as CAMEO/CASP.
    :type resnum_alignments: :class:`bool`
    :param chain_mapper: a chain mapper initialized for the target structure.
                         If None (default), a chain mapper will be initialized
                         lazily as required.
    :type chain_mapper:  :class:`ost.mol.alg.chain_mapping.ChainMapper`

    """
    def __init__(self, model, target, model_ligands=None, target_ligands=None,
                 resnum_alignments=False, chain_mapper=None):

        if isinstance(model, mol.EntityView):
            self.model = mol.CreateEntityFromView(model, False)
        elif isinstance(model, mol.EntityHandle):
            self.model = model.Copy()
        else:
            raise RuntimeError("model must be of type EntityView/EntityHandle")

        if isinstance(target, mol.EntityView):
            self.target = mol.CreateEntityFromView(target, False)
        elif isinstance(target, mol.EntityHandle):
            self.target = target.Copy()
        else:
            raise RuntimeError("model must be of type EntityView/EntityHandle")

        # Extract ligands from target
        if target_ligands is None:
            self.target_ligands = self._extract_ligands(self.target)
        else:
            self.target_ligands = self._prepare_ligands(self.target, target, target_ligands)

        # Extract ligands from model
        if model_ligands is None:
            self.model_ligands = self._extract_ligands(self.model)
        else:
            self.model_ligands = self._prepare_ligands(self.model, model, model_ligands)

        self._chain_mapper = chain_mapper
        self.resnum_alignments = resnum_alignments

        # lazily computed scores
        self._lddt_pli = None
        self._rmsd = None
        self._lddt_bs = None

    @property
    def chain_mapper(self):
        """ Chain mapper object for given :attr:`target`

        :type: :class:`ost.mol.alg.chain_mapping.ChainMapper`
        """
        if self._chain_mapper is None:
            self._chain_mapper = chain_mapping.ChainMapper(self.target,
                                                           n_max_naive=1e9,
                                                           resnum_alignments=self.resnum_alignments)
        return self._chain_mapper

    @staticmethod
    def _extract_ligands(entity):
        """Extracts ligands from entity. Returns a list of residues.

        Assumes that ligands are contained in one or more chain with chain type
        `mol.ChainType.CHAINTYPE_NON_POLY`. This is typically the case
        for entities loaded from mmCIF (tested with mmCIF files from the PDB
        and SWISS-MODEL), but it will most likely not work for most entities
        loaded from PDB files.

        As a deviation from the mmCIF semantics, we allow a chain, set as
        `CHAINTYPE_NON_POLY`, to contain more than one ligand. This function
        performs basic checks to ensure that the residues in this chain are
        not forming polymer bonds (ie peptide/nucleotide ligands) and will
        raise a RuntimeError if this assumption is broken.

        Note: This will not extract ligands based on the HET record in the old
        PDB style, as this is not a reliable indicator and depends on how the
        entity was loaded.

        :param entity: the entity to extract ligands from
        :type entity: :class:`~ost.mol.EntityHandle`
        :rtype: :class:`list` of :class:`~ost.mol.ResidueHandle`

        """
        extracted_ligands = []
        for chain in entity.chains:
            if chain.chain_type == mol.ChainType.CHAINTYPE_NON_POLY:
                for residue in chain.residues:
                    if mol.InSequence(residue, residue.next):
                        raise RuntimeError("Connected residues in non polymer "
                                           "chain %s" % (chain.name))
                    extracted_ligands.append(residue)
        return extracted_ligands

    @staticmethod
    def _prepare_ligands(new_entity, old_entity, ligands):
        """Prepare the ligands given into a list of ResidueHandles which are
        part of the copied entity, suitable for the model_ligands and
        target_ligands properties.

        This function takes a list of ligands as (Entity|Residue)(Handle|View).
        Entities can contain multiple ligands, which will be considered as
        separate ligands.

        Ligands which are part of the entity are simply fetched in the new
        copied entity. Otherwise, they are copied over to the copied entity.

        If
        Copy ligands into the new copied entity, if needed.



        and prepares the list of ligands to be returned as a list of
        ResidueHandles which are part of the copied entity, and suitable for
        model_ligands and target_ligands.

        Multiple ligands can be supplied at once in an entity.
        """
        extracted_ligands = []

        next_chain_num = 1
        new_editor = None

        def _copy_residue(handle):
            """ Copy the residue handle into the new chain.
            Return the new residue handle."""
            nonlocal next_chain_num, new_editor

            # Does a residue with the same name already exist?
            already_exists = new_entity.FindResidue(handle.chain.name,
                                                    handle.number).IsValid()
            if already_exists:
                msg = "A residue number %s already exists in chain %s" %(
                    handle.number, handle.chain.name)
                raise RuntimeError(msg)

            # Instanciate the editor
            if new_editor is None:
                new_editor = new_entity.EditXCS()

            # Get or create the chain
            new_chain = new_entity.FindChain(handle.chain.name)
            if not new_chain.IsValid():
                new_chain = new_editor.InsertChain(handle.chain.name)
            # Add the residue with its original residue number
            new_res = new_editor.AppendResidue(new_chain, handle, deep=True)
            new_res.SetIsLigand(True)
            return new_res

        def _process_ligand_residue(res):
            """Copy or fetch the residue. Return the residue handle."""
            if res.entity.handle == old_entity.handle:
                # Residue is already in copied entity. We only need to grab it
                new_res = new_entity.FindResidue(res.chain.name, res.number)
            else:
                # Residue is not part of the entity, need to copy it first
                new_res = _copy_residue(res.handle)
            return new_res

        for ligand in ligands:
            if isinstance(ligand, mol.EntityHandle) or isinstance(ligand, mol.EntityView):
                for residue in ligand.residues:
                    new_residue = _process_ligand_residue(residue)
                    extracted_ligands.append(new_residue)
            elif isinstance(ligand, mol.ResidueHandle) or isinstance(ligand, mol.ResidueView):
                new_residue = _process_ligand_residue(ligand)
                extracted_ligands.append(new_residue)
            else:
                raise RuntimeError("Ligands should be given as Entity or Residue")

        if new_editor is not None:
            new_editor.UpdateICS()
        return extracted_ligands

    def _get_binding_sites(self, ligand, topn=100000):
        """Find representations of the binding site of *ligand* in the model.

        Ignore other ligands and waters that may be in proximity.

        :param ligand: Defines the binding site to identify.
        :type ligand: :class:`~ost.mol.ResidueHandle`
        """
        if ligand.hash_code not in self._binding_sites:

            # create view of reference binding site
            ref_residues_hashes = set()  # helper to keep track of added residues
            for ligand_at in ligand.atoms:
                close_atoms = self.target.FindWithin(ligand_at.GetPos(), self.radius)
                for close_at in close_atoms:
                    # Skip other ligands and waters.
                    # This assumes that .IsLigand() is properly set on the entity's
                    # residues.
                    ref_res = close_at.GetResidue()
                    if not (ref_res.is_ligand or
                            ref_res.chem_type == mol.ChemType.WATERS):
                        h = ref_res.handle.GetHashCode()
                        if h not in ref_residues_hashes:
                            ref_residues_hashes.add(h)

            # reason for doing that separately is to guarantee same ordering of
            # residues as in underlying entity. (Reorder by ResNum seems only
            # available on ChainHandles)
            ref_bs = self.target.CreateEmptyView()
            for ch in self.target.chains:
                for r in ch.residues:
                    if r.handle.GetHashCode() in ref_residues_hashes:
                        ref_bs.AddResidue(r, mol.ViewAddFlag.INCLUDE_ALL)

            # Find the representations
            self._binding_sites[ligand.hash_code] = self.chain_mapper.GetRepr(
                ref_bs, self.model, inclusion_radius=self.lddt_bs_radius,
                topn=topn)
        return self._binding_sites[ligand.hash_code]

    def _compute_rmsd(self):
        """"""
        # Create the matrix
        self._rmsd_matrix = np.empty((len(self.target_ligands),
                                      len(self.model_ligands)), dtype=float)
        for trg_ligand in self.target_ligands:
            LogDebug("Compute RMSD for target ligand %s" % trg_ligand)

            for binding_site in self._get_binding_sites(trg_ligand):
                import ipdb; ipdb.set_trace()

                for mdl_ligand in self.model_ligands:
                    LogDebug("Compute RMSD for model ligand %s" % mdl_ligand)

                    # Get symmetry graphs
                    model_graph = model_ligand.spyrmsd_mol.to_graph()
                    target_graph = target_ligand.struct_spyrmsd_mol.to_graph()
                    pass


def ResidueToGraph(residue):
    """Return a NetworkX graph representation of the residue."""
    nxg = networkx.Graph()
    nxg.add_nodes_from([a.name for a in residue.atoms], element=[a.element for a in residue.atoms])
    # This will list all edges twice - once for every atom of the pair.
    # But as of NetworkX 3.0 adding the same edge twice has no effect so we're good.
    nxg.add_edges_from([(b.first.name, b.second.name) for a in residue.atoms for b in a.GetBondList()])
    return nxg


__all__ = ["LigandScorer", "ResidueToGraph"]
