import FWCore.ParameterSet.Config as cms

## Energy smearing and scale correction
## https://twiki.cern.ch/twiki/bin/view/CMS/EGMSmearer

def enableElectronSmearing(process, runOnMC=True):
    process.load('EgammaAnalysis.ElectronTools.calibratedPatElectronsRun2_cfi')

    process.RandomNumberGeneratorService.calibratedPatElectrons = cms.PSet(
        initialSeed = cms.untracked.uint32(81),
        engineName = cms.untracked.string('TRandom3')
    )

    process.calibratedPatElectrons.isMC = runOnMC
    process.catElectrons.src = "calibratedPatElectrons"

    process.calibratedPatElectrons.correctionFile = "EgammaAnalysis/ElectronTools/data/ScalesSmearings/Moriond17_23Jan_ele"

    return process

def enablePhotonSmearing(process, runOnMC=True):
    process.load('EgammaAnalysis.ElectronTools.calibratedPatPhotonsRun2_cfi')

    process.RandomNumberGeneratorService.calibratedPatPhotons = cms.PSet(
        initialSeed = cms.untracked.uint32(81),
        engineName = cms.untracked.string('TRandom3')
    )

    process.calibratedPatPhotons.isMC = runOnMC
    process.catPhotons.src = "calibratedPatPhotons"

    process.calibratedPatPhotons.correctionFile = "EgammaAnalysis/ElectronTools/data/ScalesSmearings/Moriond17_74x_pho"

    return process

