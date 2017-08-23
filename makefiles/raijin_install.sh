#!/bin/sh

installpath='/short/public/rcb547/apps/rjmcmcmt'
cd ..

mkdir -p $installpath
mkdir -p $installpath/bin

cp -pru docs $installpath/docs
cp -pru examples $installpath/examples
cp -pru matlab $installpath/matlab
cp -pru bin/raijin $installpath/bin/raijin

chmod -R go+rx $installpath/bin/raijin/*.exe


