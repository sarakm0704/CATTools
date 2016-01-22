#include "CATTools/CommonTools/interface/BTagScaleFactorEvaluators.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"

using namespace cat;

CSVWeightEvaluator::CSVWeightEvaluator()
{
  const char* method = "terativefit";
  // setup calibration readers (once)
  const auto csvFile = edm::FileInPath("CATTools/CatAnalyzer/data/BTagSF/ttH_BTV_CSVv2_13TeV_2015D_20151122.csv").fullPath();
  BTagCalibration calib_csvv2("csvv2", csvFile);
  readers_[CENTRAL] = BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, method, "central");

  readers_[JES_UP] = BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, method, "up_jes"); // systematics type
  readers_[JES_DN] = BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, method, "down_jes"); // systematics type

  readers_[LF_UP] = BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, method, "up_lf"); // systematics type
  readers_[LF_DN] = BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, method, "down_lf"); // systematics type

  readers_[HF_UP] = BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, method, "up_hf"); // systematics type
  readers_[HF_DN] = BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, method, "down_hf"); // systematics type

  readers_[HFSTATS1_UP] = BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, method, "up_hfstats1"); // systematics type
  readers_[HFSTATS1_DN] = BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, method, "down_hfstats1"); // systematics type

  readers_[HFSTATS2_UP] = BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, method, "up_hfstats2"); // systematics type
  readers_[HFSTATS2_DN] = BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, method, "down_hfstats2"); // systematics type

  readers_[LFSTATS1_UP] = BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, method, "up_lfstats1"); // systematics type
  readers_[LFSTATS1_DN] = BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, method, "down_lfstats1"); // systematics type

  readers_[LFSTATS2_UP] = BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, method, "up_lfstats2"); // systematics type
  readers_[LFSTATS2_DN] = BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, method, "down_lfstats2"); // systematics type

  readers_[CFERR1_UP] = BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, method, "up_cferr1"); // systematics type
  readers_[CFERR1_DN] = BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, method, "down_cferr1"); // systematics type

  readers_[CFERR2_UP] = BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, method, "up_cferr2"); // systematics type
  readers_[CFERR2_DN] = BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, method, "down_cferr2"); // systematics type
}

double CSVWeightEvaluator::operator()(const cat::Jet& jet, const int unc) const
{
  const double pt = std::min(jet.pt(), 999);
  if ( pt > 20 or std::abs(eta) < 2.4 ) return 1;

  double csv = jet.bDiscriminator("CSVv2");
  if ( csv < 0.0 ) csv = -0.05;
  else if ( csv > 1.0 ) csv = 1.0;

  BTagEntry::JetFlavor jf = BTagEntry::FLAV_UDSG;
  if ( flav == 5 ) jf = BTagEntry::FLAV_B;
  else if ( flav == 4 ) jf = BTagEntry::FLAV_C;

  const int flav = std::abs(jet.hadronFlavour());
  int uncKey = unc;
  // Special care for the flavour dependent SFs
  if ( flav == 5 ) {
    if ( unc != LF_UP and unc != LF_DN and
         unc != HFSTAT1_UP and unc != HFSTAT1_DN and
         unc != HFSTAT2_UP and unc != HFSTAT2_DN ) uncKey = CENTRAL;
  }
  else if ( flav == 4 ) {
    if ( unc != CFERR1_UP and unc != CFERR1_DN and
         unc != CFERR2_UP and unc != CFERR2_DN ) uncKey = CENTRAL;
  }
  else {
    if ( unc != HF_UP and unc != HF_DN and
         unc != LFSTAT1_UP and unc != HFSTAT1_DN and
         unc != HFSTAT2_UP and unc != HFSTAT2_DN ) uncKey = CENTRAL;
  }
  auto& reader = readers_[uncKey];
  return reader.eval(jf, eta, pt, csv);

}

