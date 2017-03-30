//! @file
//! @brief Global module - Source file.
//! @author Mariusz Ornowski (mariusz.ornowski@ict-project.pl)
//! @version 1.0
//! @date 2012-2017
//! @copyright ICT-Project Mariusz Ornowski (ict-project.pl)
/* **************************************************************
Copyright (c) 2012-2017, ICT-Project Mariusz Ornowski (ict-project.pl)
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
#include <unistd.h>
#include <random>
#include <mutex>
//============================================
namespace ict { namespace global {
//===========================================
std::vector<std::string> arg;
std::map<std::string,std::string> env;
std::string processId;
std::string processTime;
std::string processName("bledna_nazwa");
//===========================================
std::string getFileName(const std::string& path) {
   char sep('/');
   size_t p=path.rfind(sep);
   if (p!=std::string::npos) {
      return(path.substr(p+1));
   }
   return(path);
}
template <typename I=int> I randomInt(I from,I to){
  static std::mutex mutex;
  std::unique_lock<std::mutex> lock(mutex);
  //static std::default_random_engine generator;
  static std::random_device rd;
  static std::mt19937 generator(rd());
  std::uniform_int_distribution<I> distribution(from,to);
  return(distribution(generator));
}
std::string randomString(std::size_t len){
  std::string out;
  static const char alphanum[]=
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";
  for (std::size_t i=0;i<len;++i) {
    out.append(1,alphanum[randomInt<std::size_t>(0,(sizeof(alphanum)-2))]);
  }
  return(out);
}
int main(int argc, char **argv,main_fun_t next){
  if (argc){
    //Zapisanie argumentów wejściowych funkcji main.
    for (int k=0;k<argc;k++){
      std::string item;
      if (argv[k]!=NULL) {
        item=argv[k];
      }
      arg.push_back(item);
    }
    //Nazwa aplikacji
    processName=getFileName(arg[0]);
  }
  processId=std::to_string(::getpid());
  processTime=std::to_string(::time(NULL));
  if (environ){
    for (int k=0;environ[k];k++){
      std::string e(environ[k]);
      std::size_t at(e.find('='));
      if (at==std::string::npos){
        env[e]="";
      } else {
        env[e.substr(0,at)]=e.substr(at+1);
      }
    }
  }
  return(next?next():-1);
}
//===========================================
} }
//===========================================