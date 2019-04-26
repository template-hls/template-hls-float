# If you get "fatal error: bits/c++config.h: No such file or directory"
# > sudo apt-get install gcc-4.8-multilib g++-4.8-multilib

NAME=dot_product
ifeq ($(NO_OPT),)
	NO_OPT=1
endif
ifeq ($(NO_INLINE),)
	NO_INLINE=1
endif
LEVEL = /home/legup/legup-4.0/examples
FRONT_END = clang++ -xc++
CFLAG = -std=c++11 -I../../../include -DTHLS_SYNTHESIS -DTHLS_FW_UINT_ON_MASKED_UINT -D__SYNTHESIS__=1
include $(LEVEL)/Makefile.common
