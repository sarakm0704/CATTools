#ifndef CATTools_Jet_H
#define CATTools_Jet_H

#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "CATTools/DataFormats/interface/Particle.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/JetCorrFactors.h"

#include <string>
#include <boost/array.hpp>

// Define typedefs for convenience
namespace cat {
  class Jet;
  typedef std::vector<Jet>              JetCollection;
  typedef edm::Ref<JetCollection>       JetRef;
  typedef edm::RefVector<JetCollection> JetRefVector;
}

namespace cat {

  // pfCombinedInclusiveSecondaryVertexV2BJetTags
  const std::string BTAG_CSVv2 = "pfCombinedInclusiveSecondaryVertexV2BJetTags";
  //                            2017// 
  const double WP_BTAG_CSVv2L = 0.5803;
  const double WP_BTAG_CSVv2M = 0.8838;
  const double WP_BTAG_CSVv2T = 0.9693;
  // pfCombinedCvsLJetTags
  const std::string CTAG_CvsL = "pfCombinedCvsLJetTags";
  //                           2017//
  const double WP_CTAG_CvsLL = -0.53;
  const double WP_CTAG_CvsLM =  0.07;
  const double WP_CTAG_CvsLT =  0.87;
  // pfCombinedCvsBJetTags
  const std::string CTAG_CvsB = "pfCombinedCvsBJetTags";
  //                           2017//
  const double WP_CTAG_CvsBL = -0.26;
  const double WP_CTAG_CvsBM = -0.10;
  const double WP_CTAG_CvsBT = -0.3; 
  // DeepCSV BvsAll
  const std::string BTAG_DeepCSV = "pfDeepCSVDiscriminatorsJetTags:BvsAll";
  const std::string BTAG_DeepCSVb = "pfDeepCSVJetTags:probb";
  const std::string BTAG_DeepCSVbb = "pfDeepCSVJetTags:probbb";
  //                              2017// 
  const double WP_BTAG_DeepCSVM = 0.4941;
  const double WP_BTAG_DeepCSVT = 0.8001;
  // DeepCSV CvsL
  const std::string CTAG_DeepCSVc = "pfDeepCSVJetTags:probc";
  const std::string CTAG_DeepCSVcc = "pfDeepCSVJetTags:probcc";
  const std::string CTAG_DeepCSVudsg = "pfDeepCSVJetTags:probudsg";
  const std::string CTAG_DeepCSV_CvsL = "pfDeepCSVDiscriminatorsJetTags:CvsL";
  //                                   2017
  const double WP_CTAG_DeepCSV_CvsLL = 0.05;
  const double WP_CTAG_DeepCSV_CvsLM = 0.15;
  const double WP_CTAG_DeepCSV_CvsLT = 0.8;
  // DeepCSV CvsB
  const std::string CTAG_DeepCSV_CvsB = "pfDeepCSVDiscriminatorsJetTags:CvsB";
  //                                   2017
  const double WP_CTAG_DeepCSV_CvsBL = 0.33;
  const double WP_CTAG_DeepCSV_CvsBM = 0.28;
  const double WP_CTAG_DeepCSV_CvsBT = 0.1;

  class Jet : public Particle{
  public:
    Jet();
    Jet(const reco::LeafCandidate & aJet);
    virtual ~Jet();

    bool LooseId() const { return looseJetID_; }// temp for backward comp
    bool TightId() const { return tightJetID_; }// temp for backward comp
    bool looseJetID() const { return looseJetID_; }
    bool tightJetID() const { return tightJetID_; }
    bool tightLepVetoJetID() const { return tightLepVetoJetID_; }

    float pileupJetId() const { return pileupJetId_; }
    float chargedEmEnergyFraction() const { return chargedEmEnergyFraction_; }

    // return the matched MC parton flavour (from the shower, used e.g. for b-tagging)
    int partonFlavour() const{ return partonFlavour_;}
    int hadronFlavour() const{ return hadronFlavour_;}
    // pdgId of the matched MC parton from hard scattering (i.e. the closest quark or gluon of status == 3)
    int partonPdgId() const{ return partonPdgId_;}
    const reco::GenJet * genJet() const { return genJetFwdRef_.get();}

    float bDiscriminator(const std::string &theLabel) const;
    const std::vector<std::pair<std::string, float> > & getPairDiscri() const {return pairDiscriVector_; }

    bool CSVv2L(){ return (bDiscriminator(BTAG_CSVv2) > WP_BTAG_CSVv2L);}
    bool CSVv2M(){ return (bDiscriminator(BTAG_CSVv2) > WP_BTAG_CSVv2M);}
    bool CSVv2T(){ return (bDiscriminator(BTAG_CSVv2) > WP_BTAG_CSVv2T);}

    enum BTAGCSV_CUT { BTAGCSV_LOOSE=0, BTAGCSV_MEDIUM, BTAGCSV_TIGHT };
    //float scaleFactorCSVv2(BTAGCSV_CUT op, int systDir) const;
    // op 0 = loose, 1 = medium, 2 = tight
    // sys 0 = central, 1 = up, -1 = down
    // flav 0 = b, 1 = c, 2 = light
    
    int bDiscriminatorPrint() const;
    // combinedSecondaryVertexBJetTags
    float vtxMass() const { return vtxMass_ ; }
    int vtxNtracks() const { return vtxNtracks_ ; }
    float vtx3DVal() const { return vtx3DVal_; }
    float vtx3DSig() const { return vtx3DSig_; }

    float shiftedEnDown() const {return  shiftedEnDown_;}
    float shiftedEnUp() const  {return  shiftedEnUp_;}
    float smearedRes(int direction=0, int era = 0) const; // 0, +1, -1 for smeared, smearedUp, smearedDown
    float smearedResUp(int era = 0) const { return smearedRes(+1, era); };
    float smearedResDown(int era = 0) const { return smearedRes(-1, era); };

    float qgLikelihood() const { return qgLikelihood_; }
    
    void setLooseJetID(bool id) { looseJetID_ = id; }
    void setTightJetID(bool id) { tightJetID_ = id; }
    void setTightLepVetoJetID(bool id) { tightLepVetoJetID_ = id; }

    void setPileupJetId(float f) { pileupJetId_ = f;}
    void setChargedEmEnergyFraction(float f) { chargedEmEnergyFraction_ = f; }

    void setVtxMass(float f) { vtxMass_ = f;}
    void setVtxNtracks(int f) { vtxNtracks_ = f;}
    void setVtx3DVal(float f) { vtx3DVal_ = f;}
    void setVtx3DSig(float f) { vtx3DSig_ = f;}

    void setPartonFlavour(int i) { partonFlavour_ = i; }
    void setHadronFlavour(int i) { hadronFlavour_ = i; }
    void setPartonPdgId(int i) { partonPdgId_ = i; }
    void setGenJetRef(const edm::FwdRef<reco::GenJetCollection> & gj){ genJetFwdRef_ = gj;}

    void setBDiscriminators(const std::vector<std::pair<std::string, float> > & ids) { pairDiscriVector_ = ids; }
    void addBDiscriminatorPair(const std::pair<std::string, float> & thePair) {pairDiscriVector_.push_back(thePair);}
    /* void addBDiscriminatorPair(float f) { pairDiscriVector_ = f;} */
    /* float bDiscriminator() const {return  pairDiscriVector_;} */

    void setShiftedEnDown(float f) { shiftedEnDown_ = f;}
    void setShiftedEnUp(float f) { shiftedEnUp_ = f;}

    void setJER(float fJER, float fJERUp, float fJERDown) {
      fJER_ = fJER; fJERUp_ = fJERUp; fJERDown_ = fJERDown;
    }

    void setQGLikelihood(float f) { qgLikelihood_ = f; }

  private:

    edm::FwdRef<reco::GenJetCollection>  genJetFwdRef_;

    bool looseJetID_;
    bool tightJetID_;
    bool tightLepVetoJetID_;

    float pileupJetId_;
    float chargedEmEnergyFraction_;

    /// b tagging discriminators
    std::vector<std::pair<std::string, float> >  pairDiscriVector_;
    //float pairDiscriVector_;
     /// b tagging information
    float vtxMass_;
    int vtxNtracks_;
    float vtx3DVal_;
    float vtx3DSig_;

    //parton flavour
    int partonFlavour_;
    int hadronFlavour_;
    int partonPdgId_;

    float shiftedEnDown_;
    float shiftedEnUp_;

    float fJER_, fJERUp_, fJERDown_;

    float qgLikelihood_;

  };
}

#endif
