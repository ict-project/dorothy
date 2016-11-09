//! @file
//! @brief OS module - Source file.
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
//============================================
#include "os.hpp"
#include "global.hpp"
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
//============================================
//target 
//ldflags 
//cppflags -std=c++11 
//include ./src/global.cpp
//============================================
namespace ict { namespace os {
//===========================================
std::string getCurrentDir(){
  std::string out;
  char *path=getcwd(NULL,0);
  if (path){
    out=path;
    free(path);
  }
  if (out.back()!='/') out+='/';
  return(out);
}
std::string getOnlyDir(const std::string & path){
  std::string out(path);
  std::size_t p;
  if (path=="-") return(getCurrentDir());
  p=path.rfind('/');
  if (p==std::string::npos) {
    out="/";
  } else {
    out=out.substr(0,p+1);
  }
  return(out);
}
bool isFileReadable(const std::string & base,const std::string & path){
  if (path=="-") return(true);
  return(access(getRealPath(base,path).c_str(),R_OK)==0);
}
bool isFileReadable(const std::string & path){
  return(isFileReadable("-",path));
}
std::string getRealPath(const std::string & path){
  std::string out(path);
  if (path=="-") return(getRealPath(path));
  {
    char *full_path=realpath(path.c_str(),NULL);
    if (full_path) {
      out=full_path;
      free(full_path);
    }
  }
  if ((path.back()=='/')&&(out.back()!='/')) out+='/';
  return(out);
}
std::string getRealPath(const std::string & base,const std::string & path){
  if (path=="-") return(getRealPath(path));
  if (base=="-") return(getRealPath(path));
  if (path.front()=='/') return(getRealPath(path));
  return(getRealPath(getOnlyDir(base)+path));
}
std::string getRelativePath(const std::string & base,const std::string & path){
  std::string out("./");
  std::size_t i=0;
  std::size_t k=0;
  std::string tmp_base((base=="-")?getCurrentDir():getRealPath(base));
  std::string tmp_path(getRealPath(path));
  if (path=="-") return(path);
  for (;(i<tmp_base.size())&&(i<tmp_path.size());i++){
    if (tmp_base.at(i)==tmp_path.at(i)) {
      if (tmp_base.at(i)=='/') k=i+1;
    } else {
      break;
    }
  }
  tmp_base=tmp_base.substr(k);
  tmp_path=tmp_path.substr(k);
  k=0;
  for (auto c : tmp_base) if (c=='/') k++;
  for (i=0;i<k;i++) out+="../";
  out+=tmp_path;
  return(out);
}
std::string getRelativePath(const std::string & path){
  return(getRelativePath("-",path));
}
int test(){
  #define PRINT_TEST(t) std::cout<<#t "="<<t<<std::endl;
  PRINT_TEST(getCurrentDir())
  PRINT_TEST(getOnlyDir(getCurrentDir()))
  PRINT_TEST(getRealPath("test.dorothy"))
  PRINT_TEST(getRealPath(getCurrentDir(),"test.dorothy"))
  PRINT_TEST(getRealPath(getRealPath("test.dorothy"),"test.dorothy"))
  PRINT_TEST(getRelativePath("test.dorothy"));
  PRINT_TEST(getRelativePath(getCurrentDir(),"test.dorothy"));
  PRINT_TEST(getRelativePath(getRealPath("test.dorothy"),"test.dorothy"));
  PRINT_TEST(getRelativePath("../dorothy/test.dorothy"));
  PRINT_TEST(getRelativePath(getCurrentDir(),"../dorothy/test.dorothy"));
  PRINT_TEST(getRelativePath(getRealPath("../dorothy/test.dorothy"),"../dorothy/test.dorothy"));
  PRINT_TEST(getRelativePath("./src/os.cpp","test.dorothy"));
  return(0);
}
//============================================
}}
//============================================