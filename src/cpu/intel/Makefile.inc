# Note: from here on down, we are socket-centric. Socket choice determines what other cpu files are included. 
# Therefore:
# ONLY include Makefile.inc from socket directories!

subdirs-y += speedstep
subdirs-y += socket_mFCPGA478
subdirs-y += socket_PGA370

#socket_mPGA478
#socket_mPGA479M
#socket_mPGA603
#socket_mPGA604
#socket_mPGA604_533Mhz
#socket_mPGA604_800Mhz
