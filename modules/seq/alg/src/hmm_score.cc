#include <ost/seq/alg/hmm_score.hh>
#include <numeric>

namespace{

void SetupSequence(const ost::seq::ProfileHandle& profile,
	               const ost::seq::AlignmentHandle& aln,
	               int s_idx, String& seq, int& offset) {

  ost::seq::ConstSequenceHandle s_handle = aln.GetSequence(s_idx);
  seq = s_handle.GetString();
  String s_gapless = s_handle.GetGaplessString();
  offset = s_handle.GetOffset();

  if(offset < 0) {
    throw ost::Error("aln must not contain sequences with negative offsets!");
  }

  // check whether sequences in aln match with profile sequences
  if(offset + s_gapless.size() > profile.size() ||
     profile.GetSequence().substr(offset, s_gapless.size()) != s_gapless) {
    std::stringstream ss;
    ss << "aln sequence: \"" << s_gapless << "\" (sequence offset: ";
    ss << offset << ") does not match sequence in profile: ";
    ss << profile.GetSequence();
    throw ost::Error(ss.str());
  }
}


Real InsertionTransitionScore(const ost::seq::ProfileHandle& prof_0, 
                              const ost::seq::ProfileHandle& prof_1,
                              int before_0, int before_1,
                              int after_0, int after_1) {

  // option 1:
  // s0 switched to insertion state after the beginning of the gap
  // and s1 stayed in match state
  ////////////////////////////////////////////////////////////////

  Real ins_score = 0.0;

  // s0
  ins_score += 
  std::log2(prof_0[before_0].GetTransProb(ost::seq::HMM_M2I)) * 0.6;
  int l = after_1 - before_1 - 1;
  ins_score += 
  (l - 1) * std::log2(prof_0[before_0].GetTransProb(ost::seq::HMM_I2I)) * 0.6;    
  ins_score += std::log2(prof_0[before_0].GetTransProb(ost::seq::HMM_I2M));

  // s1
  for(int i = before_1; i < after_1; ++i) {
    ins_score += std::log2(prof_1[i].GetTransProb(ost::seq::HMM_M2M));
  }

  // option 2:
  // s0 has one single M->M transition
  // and s1 switched into deletion state at the beginning of the gap
  //////////////////////////////////////////////////////////////////

  Real del_score = 0.0;

  // s0
  del_score += std::log2(prof_0[before_0].GetTransProb(ost::seq::HMM_M2M));

  // s1
  del_score += 
  std::log2(prof_1[before_1].GetTransProb(ost::seq::HMM_M2D)) * 0.6;
  for(int i = before_1 + 1; i < after_1 - 1; ++i ) {
  	del_score += std::log2(prof_1[i].GetTransProb(ost::seq::HMM_D2D)) * 0.6;
  }
  del_score += std::log2(prof_1[after_1-1].GetTransProb(ost::seq::HMM_D2M));

  return std::max(ins_score, del_score);
}

} // ns

namespace ost{ namespace seq{ namespace alg{

Real HMMScore(const ost::seq::ProfileHandle& prof_0, 
	          const ost::seq::ProfileHandle& prof_1,
	          const ost::seq::AlignmentHandle& aln,
	          int seq_0_idx, int seq_1_idx) {

  String s_0;
  int s_0_o;
  SetupSequence(prof_0, aln, seq_0_idx, s_0, s_0_o);

  String s_1;
  int s_1_o;
  SetupSequence(prof_1, aln, seq_1_idx, s_1, s_1_o);

  // hhblits uses the average background frequencies by default
  std::vector<Real> null_freq(20, 0.0);
  const Real* prof_0_null_freq = prof_0.GetNullModel().freqs_begin(); 
  const Real* prof_1_null_freq = prof_1.GetNullModel().freqs_begin(); 
  for(int i = 0; i < 20; ++i) {
    null_freq[i] = (prof_0_null_freq[i] + prof_1_null_freq[i]) / 2;
  }

  Real transition_score = 0.0;
  std::vector<Real> matching_scores(s_0.size(), 0.0); 
  int n_matches = 0;

  int col_0 = 0;
  int col_1 = 0;
  int state = 0; // 0: match
                 // 1: insertion s0
                 // 2: insertion s1
  Real offset = -0.03; // described in 2005 hmm-hmm comparison paper by soding

  // sum up column scores and all MM->MM transition scores
  for(uint idx = 0; idx < s_0.size(); ++idx) {
    if(s_0[idx] == '-' && s_1[idx] == '-') {
      throw Error("Cannot have gap at same location in both sequences!");
    }
    if(s_0[idx] != '-' && s_1[idx] != '-') {
      // score of matching hmm columns
      Real summed_col_score = 0.0;
      const Real* prof_0_freq = prof_0[col_0+s_0_o].freqs_begin();
      const Real* prof_1_freq = prof_1[col_1+s_1_o].freqs_begin();
      for (uint i = 0; i < 20; ++i) {
        summed_col_score += (prof_0_freq[i] * prof_1_freq[i] / null_freq[i]);
      }
      matching_scores[idx] = std::log2(summed_col_score);
      ++n_matches;

      if(state == 0 && col_0>0 && col_1>0){
        // MM->MM
        transition_score += 
        std::log2(prof_0[col_0+s_0_o-1].GetTransProb(ost::seq::HMM_M2M));
        transition_score += 
        std::log2(prof_1[col_1+s_1_o-1].GetTransProb(ost::seq::HMM_M2M));
      } 
      state = 0;
      ++col_0;
      ++col_1;
    } else if(s_0[idx] == '-') {
      state = 1;
      ++col_1;
    } else if(s_1[idx] == '-') {
      state = 2;
      ++col_0;
    }
  }

  Real correl_score = 0.0;
  for(int d = 1; d <= 4; ++d) {
    for(size_t l = 0; l < matching_scores.size() - d; ++l) {
      correl_score += matching_scores[l] * matching_scores[l+d];
    }
  }

  // Deal with transition probabilities from indels
  col_0 = 0;
  col_1 = 0;
  state = 0;

  int ins_0_before_col_0 = -1;
  int ins_0_before_col_1 = -1;
  int ins_1_before_col_0 = -1;
  int ins_1_before_col_1 = -1;

  for(uint idx = 0; idx < s_0.size(); ++idx) {

    if(s_0[idx] != '-' && s_1[idx] != '-') {
      if(state == 1 && ins_0_before_col_0 >= 0) {
        // an insertion in s0 ends here
        transition_score += InsertionTransitionScore(prof_0, prof_1,  
                                                     ins_0_before_col_0+s_0_o, 
                                                     ins_0_before_col_1+s_1_o, 
                                                     col_0+s_0_o, col_1+s_1_o);
      }
      if(state == 2 && ins_1_before_col_1 >= 0) {
        // an insertion in s0 ends here
        transition_score += InsertionTransitionScore(prof_1, prof_0, 
                                                     ins_1_before_col_1+s_1_o, 
                                                     ins_1_before_col_0+s_0_o, 
                                                     col_1+s_1_o, col_0+s_0_o);
      }
      ++col_0;
      ++col_1;
      state = 0;
    } else if(s_0[idx] == '-') {
      if(state == 0) {
        // start new insertion
        ins_0_before_col_0 = col_0-1;
        ins_0_before_col_1 = col_1-1;
      } 
      state = 1;
      ++col_1;
    } else if(s_1[idx] == '-') {
      if(state == 0) {
        // start new insertion
        ins_1_before_col_0 = col_0-1;
        ins_1_before_col_1 = col_1-1;
      } 
      state = 2;
      ++col_0;
    }
  }

  Real score = std::accumulate(matching_scores.begin(), 
                               matching_scores.end(), 0.0);

  score += n_matches * offset;
  score += 0.1 * correl_score;
  score += transition_score;

  return score;
}

}}} // ns
