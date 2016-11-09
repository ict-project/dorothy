//! @file
//! @brief Global header file.
//! @author Mariusz Ornowski (mariusz.ornowski@ict-project.pl)
//! @version 1.0
//! @date 2012-2016
//! @copyright ICT-Project Mariusz Ornowski (ict-project.pl)
/* **************************************************************
Copyright (c) 2012-2016, ICT-Project Mariusz Ornowski (ict-project.pl)
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
#ifndef _GLOBAL_HEADER
#define _GLOBAL_HEADER
//============================================
#include <sys/types.h>
#include <string>
#include <locale>
#include <map>
#include <vector>
#include <set>
#include <mutex>
#include <memory>
#include <iostream>
//============================================
#define __WIDEN2(x) L ## x
#define __WIDEN(x) __WIDEN2(x)
#define __WFILE__ __WIDEN(__FILE__)
#define __WPRETTY_FUNCTION__ __WIDEN(__PRETTY_FUNCTION__)
//============================================
namespace ict { namespace global {
//===========================================
//! Typ funkcji podstawowej procesu.
typedef int (*main_fun_t)();
//! Lista argumentów funkcji main.
typedef std::vector<std::string> arg_t;
//! Lista zmiennych środowiskowych.
typedef std::map<std::string,std::string> env_t;
//===========================================
//! Przechowuje argumenty podane przy uruchomieniu programu.
extern arg_t arg;
//! Przechowuje zmienne środowiskowe podane przy uruchomieniu programu.
extern env_t env;
//! Identyfikator procesu.
extern std::string processId;
//! Czas uruchomienia procesu.
extern std::string processTime;
//! Nazwa procesu.
extern std::string processName;
//===========================================
template<class T>
class Operators {
public:
  //! 
  //! @brief Operator równości.
  //!
  //! @param [in] input  Obiekt, który ma być porównany.
  //! @return Prawda lub fałsz.
  //!
  bool operator == (const T & input) const {
    return(compare(input)==0);
  }
  //! 
  //! @brief Operator nierówności.
  //!
  //! @param [in] input  Obiekt, który ma być porównany.
  //! @return Prawda lub fałsz.
  //!
  bool operator != (const T & input) const {
    return(compare(input)!=0);
  }
  //! 
  //! @brief Operator mniejszy od.
  //!
  //! @param [in] input  Obiekt, który ma być porównany.
  //! @return Prawda lub fałsz.
  //!
  bool operator > (const T & input) const {
    return(compare(input)>0);
  }
  //! 
  //! @brief Operator większy od.
  //!
  //! @param [in] input  Obiekt, który ma być porównany.
  //! @return Prawda lub fałsz.
  //!
  bool operator < (const T & input) const {
    return(compare(input)<0);
  }
  //! 
  //! @brief Operator mniejszy lub równy od.
  //!
  //! @param [in] input  Obiekt, który ma być porównany.
  //! @return Prawda lub fałsz.
  //!
  bool operator >= (const T & input) const {
    return(compare(input)>=0);
  }
  //! 
  //! @brief Operator większy lub równy od.
  //!
  //! @param [in] input  Obiekt, który ma być porównany.
  //! @return Prawda lub fałsz.
  //!
  bool operator <= (const T & input) const {
    return(compare(input)<=0);
  }
  //! 
  //! @brief Porównuje obiekty.
  //!
  //! @param [in] input  Obiekt, który ma być porównany.
  //! @return Wartości:
  //!  @li -1 - obiekt mniejszy;
  //!  @li 0 - obiekt taki sam;
  //!  @li +1 - obiekt większy.
  //!
  virtual int compare(const T & input) const=0;
};
//===========================================
//! 
//! @brief Podaje nazwę pliku z podanej ścieżki do pliku.
//!
//! @param path Ścieżka do pliku (Linux).
//! @return Nazwa pliku.
//!
std::string getFileName(const std::string& path);
//! 
//! @brief Podaje losowy ciąg znaków ASCII (litery i cyfry).
//!
//! @param [in] len Długość wynikowego ciągu znaków.
//! @return Losowy ciąg znaków ASCII (litery i cyfry).
//!
std::string randomString(std::size_t len);
//! 
//! @brief Zamienia UTF string na wstring.
//!
//! @param input Wejściowy ciąg znaków (przetworzone znaki są usuwane ze zmiennej).
//! @param output Wyjściowy ciąg znaków (przetworzone znaki są dodawane do zmiennej).
//!
void transferUTF(std::string & input,std::wstring & output);
//! 
//! @brief Zamienia UTF wstring na string.
//!
//! @param input Wejściowy ciąg znaków (przetworzone znaki są usuwane ze zmiennej).
//! @param output Wyjściowy ciąg znaków (przetworzone znaki są dodawane do zmiennej).
//!
void transferUTF(std::wstring & input,std::string & output);
//! 
//! @brief Koduje strng do base64.
//!
//! @param input Wejściowy ciąg znaków.
//! @param output Wyjściowy ciąg znaków (nie jest czyszczony przed wstawieniem).
//! @param url Informacja, czy znaki '+' i '/' mają być zastąpione (odpowiednio) '-' i '_' (by zakodowany string mógł być użyty w URL).
//! @param padding Informacja, czy znaki uzupełnienia (padding - '=') mają być użyte.
//!
void base64Encode(const std::string & input,std::string & output,bool url=false,bool padding=true);
//! 
//! @brief Dekoduje strng z base64.
//!
//! @param input Wejściowy ciąg znaków.
//! @param output Wyjściowy ciąg znaków (nie jest czyszczony przed wstawieniem).
//!
void base64Decode(const std::string & input,std::string & output);
//! 
//! @brief Główna funkcja inicjująca proces. Powinna być wywołana bezpośrednio z funkcji ::main(int argc, char **argv).
//!
//! Przykład użycia:
//! @code
//! int main(int argc, char **argv){
//! 	return(ict::global::main(argc,argv,main_fun));
//! }
//! @endcode
//!
//! @param [in] argc Liczba parametrów z funkcji ::main(int argc, char **argv).
//! @param [in] argv Parametry z funkcji ::main(int argc, char **argv).
//! @param [in] next Podstawowa funkcja procesu.
//! @return Kod wyjściowy procesu, który zwróciła podstawowa funkcja procesu.
//!
int main(int argc, char **argv,main_fun_t next);
//===========================================
} }
//============================================
template <typename Key,typename T> std::ostream & operator << (
  std::ostream & s, 
  const std::map<Key,T> & v
){
  bool first=true;
  s<<"{";
  for (typename std::map<Key,T>::const_iterator it=v.begin(); it!=v.end();++it){
    if (first){
      first=false;
    } else {
      s<<",";
    }
    s<<it->first<<":"<<it->second;
  }
  s<<"}";
  return(s);
}
template <typename T> std::ostream & operator << (
  std::ostream & s, 
  const std::vector<T> & v
){
  bool first=true;
  s<<"[";
  for (typename std::vector<T>::const_iterator it=v.begin();it!=v.end();++it) {
    if (first){
      first=false;
    } else {
      s<<",";
    }
    s<<(*it);
  }
  s<<"]";
  return(s);
}
//============================================
#endif