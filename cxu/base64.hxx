// YAL zeldan

#ifndef CXU_BASE64_HXX_
#define CXU_BASE64_HXX_

//*********************************************************************
//* C_Base64 - a simple base64 encoder and decoder.
//*
//*     Copyright (c) 1999, Bob Withers - bwit@pobox.com
//*
//* This code may be freely used for any purpose, either personal
//* or commercial, provided the authors copyright notice remains
//* intact.
//*********************************************************************


#include <string>

#include "defs.hxx"
#include "memory.hxx"

CXU_NS_BEGIN

class Base64
{
public:
    static std::string encode(const std::string & data);
    static std::string decode(const std::string & data);

    static std::string encode(const ConstSegment &);
    static void decode(const std::string &, Buffer &);
};




CXU_NS_END

#endif /*CXU_BASE64_HXX_*/

//.
