#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Common/interface/Association.h"
#include "DataFormats/Common/interface/RefToPtr.h"

#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "CATTools/DataFormats/interface/GenJet.h"
#include "CATTools/DataFormats/interface/MCParticle.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
//#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "CommonTools/UtilAlgos/interface/StringCutObjectSelector.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"
//#include "FWCore/Utilities/interface/isFinite.h"

using namespace edm;
using namespace std;

namespace cat {

  class CATGenJetProducer : public edm::EDProducer {
  public:
    explicit CATGenJetProducer(const edm::ParameterSet & iConfig);
    virtual ~CATGenJetProducer() { }

    virtual void produce(edm::Event & iEvent, const edm::EventSetup & iSetup);
    
  private:
    edm::InputTag src_;
    
    const double pt_;
    const double eta_;

    std::vector<const reco::Candidate *> getAncestors(const reco::Candidate &c);
    bool hasBottom(const reco::Candidate &c);
    bool hasCharm(const reco::Candidate &c);
    bool decayFromBHadron(const reco::Candidate &c);
    bool decayFromCHadron(const reco::Candidate &c);
    const reco::Candidate* lastBHadron(const reco::Candidate &c);
    const reco::Candidate* lastCHadron(const reco::Candidate &c);

  };

} // namespace

cat::CATGenJetProducer::CATGenJetProducer(const edm::ParameterSet & iConfig) :
  src_(iConfig.getParameter<InputTag>( "src" )),
  pt_(iConfig.getParameter<double>("pt")),
  eta_(iConfig.getParameter<double>("eta"))
{
  produces<std::vector<cat::GenJet> >();
}

void 
cat::CATGenJetProducer::produce(edm::Event & iEvent, const edm::EventSetup & iSetup)
{
  Handle<View<reco::GenJet> > src;
  iEvent.getByLabel(src_, src);

  auto_ptr<vector<cat::GenJet> >  out(new vector<cat::GenJet>());

  for (View<reco::GenJet>::const_iterator it = src->begin(), ed = src->end(); it != ed; ++it) {
    unsigned int idx = it - src->begin();
    const reco::GenJet & aGenJet = src->at(idx);

    if ( aGenJet.pt() < pt_ || fabs(aGenJet.eta()) > eta_ ) continue;
 
    cat::GenJet aCatGenJet(aGenJet);
     
    bool isBHadron = false;
    bool isCHadron = false;
    cat::MCParticle BHad;
    cat::MCParticle CHad;
    std::vector <const reco::GenParticle*> mcparts = aGenJet.getGenConstituents();
    for (unsigned i = 0; i < mcparts.size (); i++) {
      const reco::GenParticle* mcpart = mcparts[i];
      const reco::Candidate* lastB = lastBHadron(*mcpart);
      if( lastB ) {
	isBHadron = true;
	cat::MCParticle tmp(*lastB); 
	BHad = tmp;
	break;
      }
    }

    for (unsigned i = 0; i < mcparts.size (); i++) {
      if( isBHadron ) break; //no need to loop over again, this is b-jet!
      const reco::GenParticle* mcpart = mcparts[i];
      const reco::Candidate* lastC = lastCHadron(*mcpart);
      if( lastC ) {
	isCHadron = true;
	cat::MCParticle tmp(*lastC);
	CHad = tmp;
	break;
      }
    }

    if( isBHadron ) aCatGenJet.setBHadron(BHad); //if B-Hadron matched, always assign B-Hadron
    if( isCHadron ) aCatGenJet.setCHadron(CHad); //if only no B-Hadron matched, assign C-Hadron
 
    out->push_back(aCatGenJet);

  }

  iEvent.put(out);
}

std::vector<const reco::Candidate *> cat::CATGenJetProducer::getAncestors(const reco::Candidate &c)
{
  vector<const reco::Candidate *> moms;
  if( c.numberOfMothers() == 1 ) {
    const reco::Candidate * dau = &c;
    const reco::Candidate * mom = c.mother();
    while ( dau->numberOfMothers() == 1) {
      moms.push_back( dau );
      dau = mom ;
      mom = dau->mother();
    }
  }
  return moms;
}

bool cat::CATGenJetProducer::hasBottom(const reco::Candidate &c)
{
  int code1;
  int code2;
  bool tmpHasBottom = false;
  code1 = (int)( ( abs(c.pdgId() ) / 100)%10 );
  code2 = (int)( ( abs(c.pdgId() ) /1000)%10 );
  if ( code1 == 5 || code2 == 5) tmpHasBottom = true;
  return tmpHasBottom;
}

bool cat::CATGenJetProducer::hasCharm(const reco::Candidate &c)
{
  int code1;
  int code2;
  bool tmpHasCharm = false;
  code1 = (int)( ( abs(c.pdgId() ) / 100)%10 );
  code2 = (int)( ( abs(c.pdgId() ) /1000)%10 );
  if ( code1 == 4 || code2 == 4) tmpHasCharm = true;
  return tmpHasCharm;
}

bool cat::CATGenJetProducer::decayFromBHadron(const reco::Candidate & c)
{
  bool isFromB = false;
  vector<const reco::Candidate *> allParents = getAncestors( c );
  for( vector<const reco::Candidate *>::const_iterator aParent = allParents.begin();
       aParent != allParents.end();
       aParent ++ )
    {
      if( hasBottom(**aParent) ) isFromB = true;
      /*
	cout << " particle Parent is " << (*aParent)->status()
	<< " type " << (*aParent)->pdgId()
	<< " pt= " << (*aParent)->pt()
	<< " isB = " << isFromB
	<< endl;
      */
    }
  return isFromB;
}

bool cat::CATGenJetProducer::decayFromCHadron(const reco::Candidate & c)
{
  bool isFromC = false;
  vector<const reco::Candidate *> allParents = getAncestors( c );
  for( vector<const reco::Candidate *>::const_iterator aParent = allParents.begin();
       aParent != allParents.end();
       aParent ++ )
    {
      if( hasCharm(**aParent) ) isFromC = true;
      /*
	cout << " particle Parent is " << (*aParent)->status()
	<< " type " << (*aParent)->pdgId()
	<< " pt=" << (*aParent)->pt()
	<< " isC = " << isFromC
	<< endl;
      */
    }
  return isFromC;
}


const reco::Candidate* cat::CATGenJetProducer::lastBHadron(const reco::Candidate & c)
{
  const reco::Candidate * out = 0;
  vector<const reco::Candidate *> allParents = getAncestors( c );
  for( vector<const reco::Candidate *>::const_iterator aParent = allParents.begin();
       aParent != allParents.end();
       aParent ++ )
    {
      if( hasBottom(**aParent) ) out = *aParent;
    }
  return out;
}

const reco::Candidate* cat::CATGenJetProducer::lastCHadron(const reco::Candidate & c)
{
  const reco::Candidate * out = 0;
  vector<const reco::Candidate *> allParents = getAncestors( c );
  for( vector<const reco::Candidate *>::const_iterator aParent = allParents.begin();
       aParent != allParents.end();
       aParent ++ )
    {
      if( hasCharm(**aParent) ) out = *aParent;
    }

  return out;
}


#include "FWCore/Framework/interface/MakerMacros.h"
using namespace cat;
DEFINE_FWK_MODULE(CATGenJetProducer);
