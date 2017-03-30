//! @file
//! @brief Main module - Source file.
//! @author Mariusz Ornowski (mariusz.ornowski@ict-project.pl)
//! @version 1.0
//! @date 2016-2017
//! @copyright ICT-Project Mariusz Ornowski (ict-project.pl)
/* **************************************************************
Copyright (c) 2016-2017, ICT-Project Mariusz Ornowski (ict-project.pl)
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
//============================================
#include "global.hpp"
#include "main.hpp"
#include "os.hpp"
#include "dorothy.hpp"
#include <iostream>
#include <fstream>
#include <regex>
//============================================
namespace ict { namespace main {
//===========================================
enum mode_t {
  print_help=0,
  print_ver=1,
  all_dependences=10,
  direct_dependences=11,
  compile_strip=100,
  compile_no_strip=101
};
struct options_t {
  mode_t mode=compile_no_strip;
  ict::dorothy::depth_t verbose=0;
  ict::dorothy::depth_t maxDepth=10000;
  ict::dorothy::tokens_t tokens;
  std::vector<std::string> input;
  std::string output;
  //App out value.
  int out;
};
//===========================================
static int internal_error(int line){
  std::cerr<<"ERROR: Internal error ("<<__FILE__<<","<<line<<")!!!"<<std::endl;
  return(10000+line);
}
static int parse_options_add(options_t & options,const std::string & in){
  options.input.push_back(in);
  return(0);
}
static int parse_options_short(options_t & options,const std::string & in,unsigned int & token){
  static std::regex r_D("^D$");
  static std::regex r_d("^d$");
  static std::regex r_s("^s$");
  static std::regex r_v("^v$");
  static std::regex r_h("^h$");
  static std::regex r_0("^[0-9]+$");
  if (std::regex_match(in,r_D)){
    options.mode=all_dependences;
  } else if (std::regex_match(in,r_d)){
    options.mode=direct_dependences;
  } else if (std::regex_match(in,r_s)){
    options.mode=compile_strip;
  } else if (std::regex_match(in,r_v)){
    options.verbose++;
  } else if (std::regex_match(in,r_h)){
    options.mode=print_help;
  } else if (std::regex_match(in,r_0)){
    try {
      token=std::stoul(in);
    } catch (...){
      return(internal_error(__LINE__));
    }
  } else if (in.size()>1) {
    for (const auto & c : in){
      int out=0;
      std::string str;
      str+=c;
      out=parse_options_short(options,str,token);
      if (out) return(out);
    }
  } else {
    std::cerr<<"ERROR(-1): Unknown short option: -"<<in<<std::endl;
    return(-1);
  }
  return(0);
}
static int parse_options_long(options_t & options,const std::string & in1,const std::string & in2=""){
  static std::regex r_D("^dep-all$");
  static std::regex r_d("^dep$");
  static std::regex r_s("^strip$");
  static std::regex r_v("^vebose$");
  static std::regex r_h("^help$");
  static std::regex r_ver("^version$");
  static std::regex r_md("^max-depth");
  static std::regex r_0("^[0-9]+$");
  if (std::regex_match(in1,r_D)){
    options.mode=all_dependences;
  } else if (std::regex_match(in1,r_d)){
    options.mode=direct_dependences;
  } else if (std::regex_match(in1,r_s)){
    options.mode=compile_strip;
  } else if (std::regex_match(in1,r_v)){
    options.verbose++;
  } else if (std::regex_match(in1,r_h)){
    options.mode=print_help;
  } else if (std::regex_match(in1,r_ver)){
    options.mode=print_ver;
  } else if (std::regex_match(in1,r_md)){
    try {
      ict::dorothy::depth_t depth=std::stoul(in2);
      options.maxDepth=depth;
    } catch (...){
      return(internal_error(__LINE__));
    }
  } else if (std::regex_match(in1,r_0)){
    try {
      unsigned int token=std::stoul(in1);
      options.tokens[token]=in2;
    } catch (...){
      return(internal_error(__LINE__));
    }
  } else {
    std::cerr<<"ERROR(-2): Unknown long option: --"<<in1<<std::endl;
    return(-2);
  }
  return(0);
}
static int parse_options(options_t & options){
  bool first=true;
  int out=0;
  unsigned int token=0;
  for (const auto & arg : ict::global::arg){
    if (first){
      first=false;
    } else if (token){
      options.tokens[token]=arg;
      token=0;
    } else {
      static std::regex r0("^-$");
      static std::regex r1("^-([a-zA-Z0-9]+)$");
      static std::regex r2("^--([a-zA-Z0-9_]+)$");
      static std::regex r3("^--([a-zA-Z0-9_]+)=(.*)$");
      std::smatch match;
      if (std::regex_match(arg,match,r0)){
        out=parse_options_add(options,arg);
      } else if (std::regex_match(arg,match,r1)){
        if (match.size()<2) return(internal_error(__LINE__));
        out=parse_options_short(options,match[1],token);
      } else if (std::regex_match(arg,match,r2)){
        if (match.size()<2) return(internal_error(__LINE__));
        out=parse_options_long(options,match[1]);
      } else if (std::regex_match(arg,match,r3)){
        if (match.size()<2) return(internal_error(__LINE__));
        if (match.size()>2){
          out=parse_options_long(options,match[1],match[2]);
        } else {
          out=parse_options_long(options,match[1]);
        }
      } else {
        out=parse_options_add(options,arg);
      }
      if (out) return(out);
    }
  }
  if (options.input.size()){
    options.output=options.input.at(options.input.size()-1);
    options.input.resize(options.input.size()-1);
  }
  return(0);
}
static int test_options(options_t & options){
  if (!options.input.size()){
    std::cerr<<"ERROR(-3): Input is missing!"<<std::endl;
    return(-3);
  }
  if (!options.output.size()){
    std::cerr<<"ERROR(-4): Output is missing!"<<std::endl;
    return(-4);
  }
  for (const auto & path : options.input) if (path!="-") if (!ict::os::isFileReadable(path)){
    std::cerr<<"ERROR(-5): Can not read input "<<path<<" !"<<std::endl;
    return(-5);
  };
  if (options.verbose>0) {
    std::cerr<<"Mode: ";
    switch (options.mode){
      case print_help: std::cerr<<"Print help.";break;
      case print_ver: std::cerr<<"Print version.";break;
      case all_dependences: std::cerr<<"Get all dependences.";break;
      case direct_dependences: std::cerr<<"Get direct dependences.";break;
      case compile_strip: std::cerr<<"Compile (strip).";break;
      case compile_no_strip: std::cerr<<"Compile (no strip).";break;
      default:internal_error(__LINE__);break;
    }
    std::cerr<<std::endl;
    std::cerr<<"Input: ";
    for (const auto & str : options.input) std::cerr<<str<<" ";
    std::cerr<<std::endl;
    std::cerr<<"Output: "<<options.output<<std::endl;
    std::cerr<<"Working dir: "<<ict::os::getCurrentDir()<<std::endl;
    std::cerr<<"Verbose: "<<options.verbose<<std::endl;
    std::cerr<<"Tokens: "<<std::endl;
    std::cerr<<" $$ = $"<<std::endl;
    for (ict::dorothy::tokens_t::const_iterator it=options.tokens.cbegin();it!=options.tokens.cend();++it){
      std::cerr<<" $"<<it->first<<"$ = "<<it->second<<std::endl;
    }
  }
  return(0);
}
static int print_version(options_t & options){
  #ifdef GIT_VERSION
    std::cerr<<"Version: "<<GIT_VERSION<<std::endl;
  #endif
  return(0);
}
static int print_usage(options_t & options){
  if ((options.out<0)||(!options.mode)){
    std::cerr<<std::endl;
    std::cerr<<"Usage:"<<std::endl;
    std::cerr<<" "<<ict::global::arg[0]<<" -D input output"<<std::endl;
    std::cerr<<" "<<ict::global::arg[0]<<" -d input output"<<std::endl;
    std::cerr<<" "<<ict::global::arg[0]<<" options input output"<<std::endl;
    std::cerr<<" "<<ict::global::arg[0]<<" -s options input output"<<std::endl;
    std::cerr<<"Input: File path or '-' if standard input should be used (UTF8)."<<std::endl;
    std::cerr<<"Output: File path or '-' if standard output should be used (UTF8)."<<std::endl;
    std::cerr<<"Options:"<<std::endl;
    std::cerr<<" "<<"-D, --dep-all - Gets all dependences (list of file paths) for given input."<<std::endl;
    std::cerr<<" "<<"-d, --dep - Gets direct dependences (list of file paths) for given input."<<std::endl;
    std::cerr<<" "<<"-s, --strip - Strip output (no new lines are added and comments are ommited)."<<std::endl;
    std::cerr<<" "<<"-v, --verbose - Verbose error stream."<<std::endl;
    std::cerr<<" "<<"-<number> value, --<number>=value - Tokens used inside input (replaces $<number> in input)."<<std::endl;
    std::cerr<<" "<<"--max-depth=<value> - Changes max-depth value."<<std::endl;
    std::cerr<<" "<<"-h, --help - Prints this help."<<std::endl;
    std::cerr<<" "<<"--version - Prints version."<<std::endl;
    std::cerr<<"Examples:"<<std::endl;
    std::cerr<<" "<<ict::global::arg[0]<<" -D ./inputfile.dorothy - # Shows all dependences for ./inputfile.dorothy file."<<std::endl;
    std::cerr<<" "<<ict::global::arg[0]<<" -d ./inputfile.dorothy - # Shows direct dependences for ./inputfile.dorothy file."<<std::endl;
    std::cerr<<" "<<ict::global::arg[0]<<" --1=One --2=\"Two\" ./inputfile.dorothy ./outputfile.html"<<std::endl;
    std::cerr<<"  "<<" # Creates HTML for ./inputfile.dorothy file using given tokens and writes it in ./outputfile.html ."<<std::endl;
    std::cerr<<"Bug reports: "<<"mariusz.ornowski@ict-project.pl"<<std::endl;
    print_version(options);
    std::cerr<<std::endl;
  }
  return(options.mode?options.out:0);
}
static int do_job(options_t & options){
  int out;
  ict::dorothy::Interface interface(
    options.mode==compile_strip,
    options.maxDepth,
    options.verbose,
    options.tokens,
    "div"
  );
  try {
    for (const std::string & i : options.input) interface.addInput(i);
    out=interface.parse();
    if (out) return(out);
    if (options.output=="-"){
      switch (options.mode){
        case compile_strip:case compile_no_strip:
          out=interface.serialize(std::cout);
          break;
        case direct_dependences:
          out=interface.dependences(std::cout);
          break;
        case all_dependences:
          out=interface.dependencesAll(std::cout);
          break;
        default:break;
      }
    } else {
      std::ofstream ofs(options.output,std::ofstream::out);
      if(ofs.good()){
        switch (options.mode){
          case compile_strip:case compile_no_strip:
            out=interface.serialize(ofs);
            break;
          case direct_dependences:
            out=interface.dependences(ofs);
            break;
          case all_dependences:
            out=interface.dependencesAll(ofs);
            break;
          default:break;
        }
      } else {
        std::cerr<<"ERROR(-6): Can not write output "<<options.output<<" !"<<std::endl;
        return(-6);
      }
      ofs.close();
    }
    if (out) return(out);
  } catch (...){
    return(internal_error(__LINE__));
  }
  return(0);
}
static int dorothy(){
  options_t options;
  //return(ict::os::test());
  options.out=parse_options(options);
  if (options.mode==print_help) return(print_usage(options));
  if (options.mode==print_ver) return(print_version(options));
  if (options.out) return(print_usage(options));
  options.out=test_options(options);
  if (options.out) return(print_usage(options));
  options.out=do_job(options);
  if (options.out) return(print_usage(options));
  return(0);
}
//============================================
}}
//============================================
int main(int argc, char **argv){
  return(ict::global::main(argc,argv,ict::main::dorothy));
}
//===========================================