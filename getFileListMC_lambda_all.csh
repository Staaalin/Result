#!/bin/csh

#input begin
set energy=27
set nRun=18
set particle=lambda

#run18List27
set outList=run${nRun}List${energy}_MC_${particle}_all.list
if(-e $outList)rm -r $outList

# find /star/embed/embedding/production_19GeV_2019/Lambda*20214002/*/*/*/*/*.MuDst.root > $outList # change this if particle is changed
find /star/embed/embedding/27GeV_production_2018/Lambda_*_20192604/*/*/*/*/*.MuDst.root /star/embed/embedding/27GeV_production_2018/LambdaExp_*_20192604/*/*/*/*/*.MuDst.root > $outList # flat pT

# sed -i 's/^/root:\/\/xrdstar.rcf.bnl.gov:1095\//' $outList