//! @file
//! @brief Dorothy module - header file.
//! @author Mariusz Ornowski (mariusz.ornowski@ict-project.pl)
//! @version 1.0
//! @date 2016
//! @copyright ICT-Project Mariusz Ornowski (ict-project.pl)
/* **************************************************************
Copyright (c) 2016, ICT-Project Mariusz Ornowski (ict-project.pl)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
3. Neither the name of the ICT-Project Mariusz Ornowski nor the names
of its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**************************************************************/
#ifndef _DOROTHY_HEADER
#define _DOROTHY_HEADER
//============================================
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
//============================================
namespace ict { namespace dorothy {
//===========================================
  class Line;
  typedef unsigned int depth_t;
  typedef std::map<unsigned int,std::string> tokens_t;
  typedef std::vector<Line*> line_list_t;

  class Options{
  private:
    bool strip;
    depth_t maxDepth;
    depth_t verbose;
    std::string defaultTag;
    tokens_t tokens;
  public:
    Options(bool strip_in,depth_t maxDepth_in,depth_t verbose_in,const tokens_t & tokens_in,const std::string & defaultTag_in="div"):
      strip(strip_in),maxDepth(maxDepth_in),verbose(verbose_in),tokens(tokens_in),defaultTag(defaultTag_in){}
    bool getStrip() const {return(strip);}
    depth_t getMaxDepth() const {return(maxDepth);}
    bool testMaxDepth(depth_t d) const {return(d<maxDepth);}
    depth_t getVerbose() const {return(verbose);}
    bool testVerbose(depth_t v) const {return(v<verbose);}
    const std::string & getDefaultTag() const {return(defaultTag);}
    const tokens_t & getTokens() const {return(tokens);}
  };

  class Interface{
  private:
    static depth_t counter;
    depth_t c;
    Options options;
    line_list_t files;
  public:
    Interface(bool strip_in,depth_t maxDepth_in,depth_t verbose_in,const tokens_t & tokens_in,const std::string & defaultTag_in="div");
    ~Interface();
    void addInput(const std::string & input);
    int parse();
    int dependences(std::ostream & output);
    int dependencesAll(std::ostream & output);
    int serialize(std::ostream & output);
  };
//===========================================
}}
//============================================
#endif