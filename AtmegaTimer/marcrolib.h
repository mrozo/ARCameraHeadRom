
/*
 * MarcroLib.h
 *
 * Created: 2014-11-01 19:18:10
 *  Author: m
 */ 

#ifndef _MACRO_LIB_H_
#define _MACRO_LIB_H_

#define CP_BITS(source,destination,mask) ((destination) & (~(mask))) | ((source)&(mask));

#define SET(byte,mask) (byte)|(mask)
#define CLR(byte,mask) (byte)&(~(mask))
#define TOGGLE(byte,mask) (byte)^(mask)

#define NUMBER2MASK(number) 0x1 << number

#endif