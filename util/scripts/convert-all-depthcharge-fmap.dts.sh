#!/bin/sh
CROS_ROOT=~/cros
for i in auron bolt chell cosmos cyan daisy falco foster glados jecht lars link nyan nyan_big nyan_blaze oak panther peach_pit peppy purin rambi rush rush_ryu samus slippy smaug storm urara veyron_{brain,danger,emile,mickey,rialto,romy,speedy}; do
	util/scripts/dts-to-fmd.sh ${CROS_ROOT}/src/platform/depthcharge/board/$i/fmap.dts > src/mainboard/google/$i/chromeos.fmd
done
for i in kunimitsu sklrvp strago; do
	util/scripts/dts-to-fmd.sh ${CROS_ROOT}/src/platform/depthcharge/board/$i/fmap.dts > src/mainboard/intel/$i/chromeos.fmd
done
util/scripts/dts-to-fmd.sh ${CROS_ROOT}/src/platform/depthcharge/board/bayleybay/fmap.dts > src/mainboard/intel/bayleybay_fsp/chromeos.fmd
util/scripts/dts-to-fmd.sh ${CROS_ROOT}/src/platform/depthcharge/board/veyron_minnie/fmap.dts > src/mainboard/google/veyron/chromeos.fmd
