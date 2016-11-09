//! @file
//! @brief Dorothy module - Source file.
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
#include "dorothy.hpp"
#include "global.hpp"
#include "os.hpp"
#include <regex>
#include <fstream>
//============================================
//target
//ldflags 
//cppflags -std=c++11 
//include ./src/global.cpp ./src/os.cpp
//============================================
namespace ict { namespace dorothy {
//===========================================
#define LOG_DEBUG  3
#define LOG_INFO   2
#define LOG_NOTICE 1
//===========================================
typedef std::vector<std::string> namespace_t;
std::string encodeHtml(const std::string & input);
std::string replaceTokens(const std::string & input,const tokens_t & tokens_in);
//! Indeks plików.
class Files {
public:
  typedef unsigned int file_id_t;
  typedef unsigned int line_no_t;
  struct file_struct_t {Line * line;std::string path;};
  typedef std::map<file_id_t,file_struct_t> file_map_t;
  typedef std::set<file_id_t> file_set_t;
private:
  static file_map_t fileMap;
public:
  static Line * id2Line(file_id_t id);
  static std::string id2Path(file_id_t id);
  static file_id_t path2id(const std::string & base_in,const std::string & path_in);
  static file_id_t path2id(const std::string & path_in);
  static bool erase(file_id_t id);
  static void add(file_id_t id_in,Line * line_in,const std::string & base_in,const std::string & path_in);
  static void add(file_id_t id_in,Line * line_in,const std::string & path_in);
  static bool getLine(std::istream & input,line_no_t & lineCount,std::string & line);
  static void clear(){fileMap.clear();}
  static int dependences(const Options & options,std::ostream & output);
};
//! Indeks nazw.
class Names {
public:
  struct name_struct_t {Line * line;Files::file_id_t file_id;Files::line_no_t line_no;};
  typedef std::map<std::string,name_struct_t> name_map_t;
private:
  static name_map_t nameMap;
public:
  static Line * name2Line(const std::string & name);
  static Files::file_id_t name2id(const std::string & name);
  static Files::line_no_t name2no(const std::string & name);
  static bool erase(const std::string & name);
  static void add(const std::string & name,Line * line_in,Files::file_id_t file_id_in,Files::line_no_t line_no_in);
  static std::string fullName(const namespace_t & namesp,const std::string & name);
  static void clear(){nameMap.clear();}
};
//! Obiekt obsługujący jedną linię (bazowy).
class Line {
public:
  typedef std::shared_ptr<Line> ptr_t;
  typedef std::vector<ptr_t> children_list_t;
private:
  depth_t depth;
  Line * parent;
  children_list_t childrenList;
  bool addParent(Line * ptr);
protected:
  Files::file_id_t fileId;
  Files::line_no_t lineNo;
  std::string line;
  std::ostream & log(const std::string & level);
  std::ostream & debug(){return(log("info"));}
  std::ostream & info(){return(log("info"));}
  std::ostream & notice(){return(log("notice"));}
  std::ostream & warning(){return(log("warning"));}
  std::ostream & error(){return(log("error"));}
  bool testName(const std::string &name);
  Files::file_id_t getDepth() const {return(depth);}
  std::string getThisPath();
  bool addChild(ptr_t & ptr);
  void getChildren(line_list_t & lines);
  ptr_t factory(const std::string & line_in,Files::file_id_t fileId_in=-1,Files::line_no_t lineNo_in=-1);
  int parse(const Options & options,std::istream & input,Files::file_id_t fileId_in,Files::line_no_t & lineCount);
public:
  Line();
  Line(const std::string & line_in,Files::file_id_t fileId_in=-1,Files::line_no_t lineNo_in=-1);
  virtual int parseLine(const Options & options)=0;
  virtual int serialize(
    const Options & options,std::ostream & output,
    const namespace_t & namesp_in,const tokens_t & tokens_in,const line_list_t & lines_in,
    depth_t depth_in,depth_t comments_in
  )=0;
  int serializeLine(const Options & options,std::ostream & output);
  int serializeChildren(
    const Options & options,std::ostream & output,
    const namespace_t & namesp_in,const tokens_t & tokens_in,const line_list_t & lines_in,
    depth_t depth_in,depth_t comments_in
  );
  virtual int dependences(const Options & options,std::ostream & output,Files::file_set_t & files,bool last=false);
  std::size_t childrenCount(){return(childrenList.size());}
  virtual ~Line(){};
};
//! Obiekt obsługujący jeden plik.
class LineFile:public Line {
private:
  std::string path;
  static Files::file_id_t fileCount;
  Files::file_id_t linkedId;
  void contstruct();
public:
  LineFile(const std::string & path_in);
  LineFile(const std::string & base_in,const std::string & path_in);
  ~LineFile();
  int parseLine(const Options & options);
  int dependences(const Options & options,std::ostream & output,Files::file_set_t & files,bool last=false);
  int serialize(
    const Options & options,std::ostream & output,
    const namespace_t & namesp_in,const tokens_t & tokens_in,const line_list_t & lines_in,
    depth_t depth_in,depth_t comments_in
  );
};
//! Obiekt obsługujący linię z tokenami.
class LineTokens:public Line {
protected:
  tokens_t tokens;
  int loadTokens(const Options & options);
public:
  LineTokens(const std::string & line_in,Files::file_id_t fileId_in=-1,Files::line_no_t lineNo_in=-1):Line(line_in,fileId_in,lineNo_in){}
};
//!
//! Includes a file (identified by "path") into HTML file, 
//! text tokens ("token1","token2") replaces "$1$" and "$2$" strings in included file;  
//! Example: % include path token1 "token2" 
//!
class LineInclude:public LineTokens {
private:
  ptr_t includedFile;
public:
  LineInclude(const std::string & line_in,Files::file_id_t fileId_in=-1,Files::line_no_t lineNo_in=-1):LineTokens(line_in,fileId_in,lineNo_in){}
  int parseLine(const Options & options);
  int dependences(const Options & options,std::ostream & output,Files::file_set_t & files,bool last=false);
  int serialize(
    const Options & options,std::ostream & output,
    const namespace_t & namesp_in,const tokens_t & tokens_in,const line_list_t & lines_in,
    depth_t depth_in,depth_t comments_in
  );
};
//!
//! Defines namespace ("name") which is used for class names, 
//! id names and block names in nested elements 
//! (if name starts with "-" then root namespace is used);
//! Example:  % namespace name
//!
class LineNamespace:public LineTokens {
private:
  namespace_t namespace_names;
  bool root_namespace;
public:
  LineNamespace(const std::string & line_in,Files::file_id_t fileId_in=-1,Files::line_no_t lineNo_in=-1):root_namespace(false),LineTokens(line_in,fileId_in,lineNo_in){}
  int parseLine(const Options & options);
  int serialize(
    const Options & options,std::ostream & output,
    const namespace_t & namesp_in,const tokens_t & tokens_in,const line_list_t & lines_in,
    depth_t depth_in,depth_t comments_in
  );
};
//!
//! Defines a block (identified by "name") which is not inserted into HTML file until 'clone' directive is used 
//! (if name starts with "-" then it is defined in root namespace).
//! Example:  % define name
//!
class LineDefine:public LineTokens {
private:
  namespace_t defined_names;
  static depth_t defCount;
public:
  LineDefine(const std::string & line_in,Files::file_id_t fileId_in=-1,Files::line_no_t lineNo_in=-1):LineTokens(line_in,fileId_in,lineNo_in){
    defCount++;
  }
  ~LineDefine(){
    defCount--;
  }
  int parseLine(const Options & options);
  int serialize(
    const Options & options,std::ostream & output,
    const namespace_t & namesp_in,const tokens_t & tokens_in,const line_list_t & lines_in,
    depth_t depth_in,depth_t comments_in
  );
};
//! 
//! Clones a block (identified by "name") into HTML file, 
//! text tokens ("token1","token2") replaces "$1$" and "$2$" 
//! strings in defined block (if block name starts with "-" then root namespace is used);
//! Example: % clone name token1 "token2" - 
//!
class LineClone:public LineTokens {
public:
  LineClone(const std::string & line_in,Files::file_id_t fileId_in=-1,Files::line_no_t lineNo_in=-1):LineTokens(line_in,fileId_in,lineNo_in){}
  int parseLine(const Options & options);
  int serialize(
    const Options & options,std::ostream & output,
    const namespace_t & namesp_in,const tokens_t & tokens_in,const line_list_t & lines_in,
    depth_t depth_in,depth_t comments_in
  );
};
//!
//! Inserts text node ("any text") into HTML file (using HTML encoding);
//! Example: - any text
//!
class LineText:public Line {
private:
  std::string textLine;
public:
  LineText(const std::string & line_in,Files::file_id_t fileId_in=-1,Files::line_no_t lineNo_in=-1):Line(line_in,fileId_in,lineNo_in){}
  int parseLine(const Options & options);
  int serialize(
    const Options & options,std::ostream & output,
    const namespace_t & namesp_in,const tokens_t & tokens_in,const line_list_t & lines_in,
    depth_t depth_in,depth_t comments_in
  );
};
//!
//! Inserts text ("any text") directly into HTML (without changes);
//! Example: = any text
//!
class LineHtml:public Line {
private:
  std::string htmlLine;
public:
  LineHtml(const std::string & line_in,Files::file_id_t fileId_in=-1,Files::line_no_t lineNo_in=-1):Line(line_in,fileId_in,lineNo_in){}
  int parseLine(const Options & options);
  int serialize(
    const Options & options,std::ostream & output,
    const namespace_t & namesp_in,const tokens_t & tokens_in,const line_list_t & lines_in,
    depth_t depth_in,depth_t comments_in
  );
};
//!
//! Inserts comment node ("any text") into HTML file (without changes);
//! Example: / any text 
//!
class LineComment:public Line {
private:
  std::string commentLine;
public:
  LineComment(const std::string & line_in,Files::file_id_t fileId_in=-1,Files::line_no_t lineNo_in=-1):Line(line_in,fileId_in,lineNo_in){}
  int parseLine(const Options & options);
  int serialize(
    const Options & options,std::ostream & output,
    const namespace_t & namesp_in,const tokens_t & tokens_in,const line_list_t & lines_in,
    depth_t depth_in,depth_t comments_in
  );
};
//!
//! Comment (not inserted into HTML file);
//! Example: // any text 
//!
class LineMock:public Line {
public:
  LineMock(const std::string & line_in,Files::file_id_t fileId_in=-1,Files::line_no_t lineNo_in=-1):Line(line_in,fileId_in,lineNo_in){}
  int parseLine(const Options & options);
  int serialize(
    const Options & options,std::ostream & output,
    const namespace_t & namesp_in,const tokens_t & tokens_in,const line_list_t & lines_in,
    depth_t depth_in,depth_t comments_in
  );
};
//!
//! Inserts node ("tag") with class names ("class1 class2" - if name starts with "-" then root namespace is used), 
//! with id ("id"- if name starts with "-" then root namespace is used), 
//! with attributes (attr1="a1" attr2="a2") and with first text node ("any text") - into HTML (using HTML encoding).
//! Example: tag.class1.class2#id(attr1=a1,attr2="a2") any text
//!
class LineNode:public Line {
private:
  enum stage_t {
    stage_start=0,
    stage_tag,
    stage_class,
    stage_id,
    stage_attr_name,
    stage_attr_value,
    stage_space,
    stage_text
  };
  typedef std::vector<std::string> class_names_t;
  typedef std::map<std::string,std::string> attr_t;
  std::string tag;
  class_names_t class_names;
  std::string id;
  attr_t attr;
  std::string textLine;
  bool no_short;
  stage_t ctrlChar(stage_t stage, wchar_t c);
  int loadAll(const Options & options);
  void saveToken(stage_t stage,const std::string & token, std::string & attrName);
  std::string getAttr(const namespace_t & namesp_in,const tokens_t & tokens_in);
public:
  LineNode(const std::string & line_in,Files::file_id_t fileId_in=-1,Files::line_no_t lineNo_in=-1):no_short(false),Line(line_in,fileId_in,lineNo_in){}
  int parseLine(const Options & options);
  int serialize(
    const Options & options,std::ostream & output,
    const namespace_t & namesp_in,const tokens_t & tokens_in,const line_list_t & lines_in,
    depth_t depth_in,depth_t comments_in
  );
};
//===========================================
std::string encodeHtml(const std::string & input){
  std::string sinput(input);
  std::wstring winput;
  std::wstring woutput;
  std::string soutput;
  ict::global::transferUTF(sinput,winput);
  for (wchar_t c : winput){
    switch (c){
      case L'&':
        woutput+=L"&amp;";
        break;
      case L'<':
        woutput+=L"&lt;";
        break;
      case L'>':
        woutput+=L"&gt;";
        break;
      case L'"':
        woutput+=L"&quot;";
        break;
      case L'\'':
        woutput+=L"&apos;";
        break;
      default:
        woutput+=c;
        break;
    }
  }
  ict::global::transferUTF(woutput,soutput);
  return(soutput);
}
std::string replaceTokens(const std::string & input,const tokens_t & tokens_in){
  std::string sinput(input);
  std::wstring winput;
  std::wstring woutput;
  std::string soutput;
  std::wstring token;
  ict::global::transferUTF(sinput,winput);
  winput+=L'\0';
  bool t=false;
  for (wchar_t c : winput){
    if (t){
      switch(c){
        case L'0':case L'1':case L'2':case L'3':case L'4':
        case L'5':case L'6':case L'7':case L'8':case L'9':
          token+=c;
          break;
        default:
          if (token.size()){
            std::size_t k=0;
            try{
              k=std::stoull(token);
            }catch(...){
              k=0;
            }
            if (tokens_in.count(k)){
              std::string stoken(tokens_in.at(k));
              std::wstring wtoken;
              ict::global::transferUTF(stoken,wtoken);
              woutput+=wtoken;
            }
          } else {
            woutput+=L'$';
          }
          t=false;
          token.clear();
          if ((c)&&(c!=L'$')) woutput+=c;
          break;
      }
    } else {
      if (c==L'$') {
        t=true;
      } else if (c) {
        woutput+=c;
      }
    }
  }
  ict::global::transferUTF(woutput,soutput);
  return(soutput);
}
//===========================================
Files::file_map_t Files::fileMap;
Line * Files::id2Line(file_id_t id){
  if (fileMap.count(id)) return(fileMap.at(id).line);
  return(nullptr);
}
std::string Files::id2Path(file_id_t id){
  if (fileMap.count(id)) return(fileMap.at(id).path);
  return("");
}
Files::file_id_t Files::path2id(const std::string & base_in,const std::string & path_in){
  std::string path(ict::os::getRealPath(base_in,path_in));
  for (file_map_t::const_iterator it=fileMap.cbegin();it!=fileMap.cend();++it){
    if (it->second.path==path) return(it->first);
  }
  return(-1);
}
Files::file_id_t Files::path2id(const std::string & path_in){
  return(path2id("-",path_in));
}
bool Files::erase(file_id_t id){
  if (fileMap.count(id)) {
    fileMap.erase(id);
    return(true);
  }
  return(false);
}
void Files::add(file_id_t id_in,Line * line_in,const std::string & base_in,const std::string & path_in){
  fileMap[id_in].line=line_in;
  fileMap[id_in].path=ict::os::getRealPath(base_in,path_in);
}
void Files::add(file_id_t id_in,Line * line_in,const std::string & path_in){
  add(id_in,line_in,"-",path_in);
}
bool Files::getLine(std::istream & input,line_no_t & lineCount,std::string & soutput){
  std::string sinput;
  std::wstring winput;
  std::wstring woutput;
  soutput.clear();
  for (;input.good();) {
    std::getline(input,sinput);
    ict::global::transferUTF(sinput,winput);
    lineCount++;
    if (winput.back()==L'\\'){
      winput.resize(winput.size()-1);
      woutput+=winput;
      winput.clear();
    } else {
      woutput+=winput;
      for (wchar_t c : woutput) switch (c){
      case L' ':case L'\t':break;
      default:
        ict::global::transferUTF(woutput,soutput);
        return(true);
      }
      return(false);
    }
  }
  return(false);
}
int Files::dependences(const Options & options,std::ostream & output){
  int out;
  for (Files::file_map_t::const_iterator it=fileMap.cbegin();it!=fileMap.end();++it) if (it->second.line) {
    Files::file_set_t files;
    out=it->second.line->dependences(options,output,files);
    output<<std::endl;
    if (out) return(out);
  }
  return(0);
}

//===========================================
Names::name_map_t Names::nameMap;
Line * Names::name2Line(const std::string & name){
  if (nameMap.count(name)) return(nameMap.at(name).line);
  return(nullptr);
}
Files::file_id_t Names::name2id(const std::string & name){
  if (nameMap.count(name)) return(nameMap.at(name).file_id);
  return(-1);
}
Files::line_no_t Names::name2no(const std::string & name){
  if (nameMap.count(name)) return(nameMap.at(name).line_no);
  return(-1);
}
bool Names::erase(const std::string & name){
  if (nameMap.count(name)) {
    nameMap.erase(name);
    return(true);
  }
  return(false);
}
void Names::add(const std::string & name,Line * line_in,Files::file_id_t file_id_in,Files::line_no_t line_no_in){
  nameMap[name].line=line_in;
  nameMap[name].file_id=file_id_in;
  nameMap[name].line_no=line_no_in;
}
std::string Names::fullName(const namespace_t & namesp,const std::string & name){
  std::string out;
  std::string sname(name);
  std::wstring wname;
  ict::global::transferUTF(sname,wname);
  if (wname.front()==L'-') {
    sname.clear();
    wname=wname.substr(1);
    ict::global::transferUTF(wname,sname);
    return(sname);
  }
  for (const std::string & n : namesp){
    out+=n;
    out+='-';
  }
  out+=name;
  return(out);
}
//===========================================
Line::Line():parent(nullptr),fileId(-1),lineNo(-1),depth(0){
}
Line::Line(const std::string & line_in,Files::file_id_t fileId_in,Files::line_no_t lineNo_in):parent(nullptr),fileId(fileId_in),lineNo(lineNo_in),depth(0){
  std::string sline(line_in);
  std::wstring wline;
  ict::global::transferUTF(sline,wline);
  for (std::size_t i=0;i<wline.size();i++){
    switch (wline.at(i)){
      case L' ':case L'\t':break;
      default:
        depth=((fileId_in==-1)&&(lineNo_in==-1))?0:(i+1);
        wline=wline.substr(i);
        ict::global::transferUTF(wline,line);
        return;
    }
  }
}
bool Line::addParent(Line * ptr){
  if (!ptr) return(false);
  if (parent) return(false);
  parent=ptr;
  return(true);
}
std::ostream & Line::log(const std::string & level){
  Files::line_no_t l((lineNo==-1)?0:lineNo);
  depth_t d((depth==-1)?0:depth);
  std::cerr<<ict::os::getRelativePath(getThisPath());
  std::cerr<<":"<<l;
  std::cerr<<":"<<d;
  std::cerr<<": "<<level<<" : ";
  return(std::cerr);
}
bool Line::testName(const std::string & name){
  const static std::wstring name_item(L"([a-zA-Z0-9_$]+)");
  static std::wregex r(L"("+name_item+L"?(-"+name_item+L")+)|"+name_item,std::regex_constants::collate|std::regex_constants::nosubs);
  std::string sname(name);
  std::wstring wname;
  ict::global::transferUTF(sname,wname);
  return(std::regex_match(wname,r));
}
std::string Line::getThisPath(){
  std::string out;
  if (fileId==-1){
    out=ict::os::getCurrentDir();
  } else {
    out=Files::id2Path(fileId);
  }
  return(out);
}
bool Line::addChild(ptr_t & ptr){
  if (ptr){
    if ((getDepth())<(ptr->getDepth())){
      childrenList.push_back(ptr);
      return(ptr->addParent(this));
    } else {
      if (parent) return(parent->addChild(ptr));
    }
  }
  return(false);
}
void Line::getChildren(line_list_t & lines){
  lines.clear();
  lines.push_back(this);
  for (ptr_t & p:childrenList){
    if (p) lines.push_back(p.get());
  }
}
Line::ptr_t Line::factory(const std::string & line_in,Files::file_id_t fileId_in,Files::line_no_t lineNo_in){
  const static std::wstring space_o(L"[ \\t]*");
  const static std::wstring space_m(L"[ \\t]+");
  const static std::wstring percent(space_o+L"%"+space_o);
  const static std::wstring end(L".*");
  static std::wregex r_LineInclude(percent+L"include"+space_m+end,std::regex_constants::nosubs);
  static std::wregex r_LineNamespace(percent+L"namespace"+space_m+end,std::regex_constants::nosubs);
  static std::wregex r_LineClone(percent+L"clone"+space_m+end,std::regex_constants::nosubs);
  static std::wregex r_LineDefine(percent+L"define"+space_m+end,std::regex_constants::nosubs);
  static std::wregex r_LineText(space_o+L"-"+space_m+end,std::regex_constants::nosubs);
  static std::wregex r_LineHtml(space_o+L"="+space_m+end,std::regex_constants::nosubs);
  static std::wregex r_LineComment(space_o+L"/"+space_m+end,std::regex_constants::nosubs);
  static std::wregex r_LineMock(space_o+L"//"+space_m+end,std::regex_constants::nosubs);
  std::string sline(line_in);
  std::wstring wline;
  ict::global::transferUTF(sline,wline);
  #define LINE_FACTORY_REGEX(classname) \
  if (std::regex_match(wline,r_##classname)){ \
    ptr_t ptr(new classname(line_in,fileId_in,lineNo_in)); \
    return(ptr); \
  } else
  LINE_FACTORY_REGEX(LineInclude)
  LINE_FACTORY_REGEX(LineNamespace)
  LINE_FACTORY_REGEX(LineClone)
  LINE_FACTORY_REGEX(LineDefine)
  LINE_FACTORY_REGEX(LineText)
  LINE_FACTORY_REGEX(LineHtml)
  LINE_FACTORY_REGEX(LineComment)
  LINE_FACTORY_REGEX(LineMock)
  {
    ptr_t ptr(new LineNode(line_in,fileId_in,lineNo_in));
    return(ptr);
  }
}
int Line::parse(const Options & options,std::istream & input,Files::file_id_t fileId_in,Files::line_no_t & lineCount){
  std::string line_in;
  Files::line_no_t lineNo_in(lineCount);
  if (Files::getLine(input,lineCount,line_in)){
    ptr_t ptr(Line::factory(line_in,fileId_in,lineNo_in));
    int out=ptr->parseLine(options);
    if (out) return(out);
    if (!options.testMaxDepth(ptr->getDepth())){
      ptr->error()<<"Max depth reached ("<<options.getMaxDepth()<<") - change --max-depth param!"<<std::endl;
      return(__LINE__);
    }
    if(!addChild(ptr)){
      ptr->error()<<"Unable to add a child!"<<std::endl;
      return(__LINE__);
    }
    return(ptr->parse(options,input,fileId_in,lineCount));
  }
  return(0);
}
int Line::serializeLine(const Options & options,std::ostream & output){
  depth_t depth(0);
  unsigned int comments(0);
  namespace_t namesp;
  line_list_t lines;
  return(serialize(options,output,namesp,options.getTokens(),lines,depth,comments));
}
int Line::serializeChildren(
  const Options & options,std::ostream & output,
  const namespace_t & namesp_in,const tokens_t & tokens_in,const line_list_t & lines_in,
  depth_t depth_in,depth_t comments_in
){
  int out;
  if (!options.testMaxDepth(depth_in)){
    error()<<"Max depth reached ("<<options.getMaxDepth()<<") - change --max-depth param!"<<std::endl;
    return(__LINE__);
  }
  depth_in++;
  for (ptr_t & c : childrenList){
    out=c->serialize(options,output,namesp_in,tokens_in,lines_in,depth_in,comments_in);
    if (out) return(out);
  }
  return(0);
}
int Line::dependences(const Options & options,std::ostream & output,Files::file_set_t & files,bool last){
  int out;
  for (ptr_t & c : childrenList){
    out=c->dependences(options,output,files);
    if (out) return(out);
  }
  return(0);
}
//===========================================
Files::file_id_t LineFile::fileCount=0;
LineFile::LineFile(const std::string & path_in):path(ict::os::getRealPath(path_in)),Line(){
  fileId=fileCount;
  fileCount++;
}
LineFile::LineFile(const std::string & base_in,const std::string & path_in):path(ict::os::getRealPath(base_in,path_in)),Line(){
  fileId=fileCount;
  fileCount++;
}
LineFile::~LineFile(){
  fileCount--;
}
int LineFile::parseLine(const Options & options){
  if (!path.size()){
    error()<<"File path is missing !"<<std::endl;
    return(__LINE__);
  }
  if (path!="-") {
    if (!ict::os::isFileReadable(path)){
      static const std::string s_dorothy("dorothy");
      static std::regex r_dorothy(".*\\."+s_dorothy);
      if (std::regex_match(path,r_dorothy)) {
        error()<<"Unable to find file "<<ict::os::getRelativePath(path)<<" !"<<std::endl;
        return(__LINE__);
      }
      path+=".";
      path+=s_dorothy;
      if (!ict::os::isFileReadable(path)){
        error()<<"Unable to find file "<<ict::os::getRelativePath(path)<<" !"<<std::endl;
        return(__LINE__);
      }
    }
  }
  linkedId=Files::path2id(path);
  fileCount++;
  if (linkedId==-1){
    Files::add(fileId,this,path);
    linkedId=fileId;
  } else {
    return(0);
  }
  {
    int out;
    Files::line_no_t lineCount(1);
    if (options.getVerbose()>LOG_NOTICE) notice()<<"Parsing input "<<ict::os::getRelativePath(path)<<" (start)."<<std::endl;
    if (path=="-") {
      out=parse(options,std::cin,fileId,lineCount);
    } else {
      std::ifstream ifs(path,std::ifstream::in);
      out=parse(options,ifs,fileId,lineCount);
      ifs.close();
    }
    if (out) return(out);
    if (options.getVerbose()>LOG_NOTICE) notice()<<"Parsing input "<<ict::os::getRelativePath(path)<<" (stop)."<<std::endl;
  }
  line.clear();
  return(0);
}
int LineFile::serialize(
  const Options & options,std::ostream & output,
  const namespace_t & namesp_in,const tokens_t & tokens_in,const line_list_t & lines_in,
  depth_t depth_in,depth_t comments_in
){
  int out;
  Line * ptr=Files::id2Line(linkedId);
  if (ptr){
    out=ptr->serializeChildren(options,output,namesp_in,tokens_in,lines_in,depth_in,comments_in);
    if (out) return(out);
  } else {
    error()<<"Internal error ("<<__FILE__<<":"<<__LINE__<<")!"<<std::endl;
    return(14);
  }
  return(0);
}
int LineFile::dependences(const Options & options,std::ostream & output,Files::file_set_t & files,bool last){
  int out;
  if (last){
    if (!files.count(fileId)) output<<" "<<ict::os::getRelativePath(Files::id2Path(fileId));
  } else {
    output<<ict::os::getRelativePath(Files::id2Path(fileId))<<":";
    files.insert(fileId);
    out=Line::dependences(options,output,files);
    if (out) return(out);
  }
  return(0);
}
//===========================================
int LineTokens::loadTokens(const Options & options){
  const static std::wstring space_o(L"[ \\t]*");
  const static std::wstring space_m(L"[ \\t]+");
  const static std::wstring percent(space_o+L"%"+space_o);
  const static std::wstring directive(L"[a-z]+");
  const static std::wstring end(L"(.*)");
  static std::wregex r(percent+directive+space_m+end);
  std::wsmatch match;
  std::size_t k=0;
  wchar_t q(L'\0');
  wchar_t l(L'\0');
  std::wstring wtoken;
  bool empty=true;
  std::string sline(line);
  std::wstring wline;
  ict::global::transferUTF(sline,wline);
  wline+=L'\0';
  if ((std::regex_match(wline,match,r))&&(match.size()>1)) {
    wline=match[1];
  } else {
    error()<<"Internal parsing error !"<<std::endl;
    return(__LINE__);
  }
  for (wchar_t c : wline){
    if (l==L'\\'){//Poprzednio był znak ucieczki.
      switch(c){
        case L'n' :wtoken+=L'\n';break;
        case L'r' :wtoken+=L'\r';break;
        case L't' :wtoken+=L'\t';break;
        case L'v' :wtoken+=L'\v';break;
        case L'0' :wtoken+=L'\0';break;
        default:if (c) wtoken+=c;break;
      }
      empty=false;
    } else if (c==L'\\') {//Teraz jest znak ucieczki.
      empty=false;
    } else if (q==L'\0'){//Nie było cudzysłowa.
      if ((c==L'"')||(c==L'\'')) {//Zaczyna się cudzysłów.
        q=c;
        empty=false;
      } else if ((c==L' ')||(c==L'\t')||(!c)) {//Teraz jest biały znak.
        if (!empty){//Token nie jest pusty.
          std::string stoken;
          ict::global::transferUTF(wtoken,stoken);
          tokens[k++]=stoken;
          wtoken.clear();
          empty=true;
        }
      } else {//Nie jest to biały znak.
        wtoken+=c;
        empty=false;
      }
    } else {//Był cudzysłów.
      if (q==c) {//Kończy się cudzysłów.
        q=L'\0';
      } else {//Nie kończy się cudzysłów.
        if (c) wtoken+=c;
      }
      empty=false;
    }
    l=c;
  }
  if (options.getVerbose()>LOG_DEBUG) debug()<<"Parsed tokens: "<<tokens<<std::endl;
  return(0);
}
//===========================================
int LineInclude::parseLine(const Options & options){
  int out;
  out=loadTokens(options);
  if (out) return(out);
  if (!tokens.size()){
    error()<<"File path is missing in 'include' directive!"<<std::endl;
    return(__LINE__);
  }
  if (options.getVerbose()>LOG_NOTICE) notice()<<"Directive 'include' added: "<<tokens<<std::endl;
  includedFile.reset(new LineFile(getThisPath(),tokens.at(0)));
  if (includedFile){
    out=includedFile->parseLine(options);
    if (out) {
      info()<<"File included here."<<std::endl;
      return(out);
    }
  } else {
    error()<<"Internal error ("<<__FILE__<<":"<<__LINE__<<")!"<<std::endl;
    return(__LINE__);
  }
  line.clear();
  return(0);
}
int LineInclude::serialize(
  const Options & options,std::ostream & output,
  const namespace_t & namesp_in,const tokens_t & tokens_in,const line_list_t & lines_in,
  depth_t depth_in,depth_t comments_in
){
  int out;
  std::string space;
  std::string begin;
  std::string end;
  line_list_t lines;
  std::size_t c=0;
  bool debug((!options.getStrip())&&(options.getVerbose()>LOG_DEBUG));
  if (!options.getStrip()) space.append(depth_in?(depth_in-1):0,' ');
  if (debug){
    if (comments_in){
      begin="/* ";
      end=" */";
    } else {
      begin="<!-- ";
      end=" -->";
    }
    output<<space<<begin<<"Directive 'include' (start): "<<tokens<<end<<std::endl;
  }
  getChildren(lines);
  if (includedFile){
    c=includedFile->childrenCount();
    out=includedFile->serializeChildren(options,output,namesp_in,tokens,lines,depth_in,comments_in);
    if (out) {
      info()<<"File included here."<<std::endl;
      return(out);
    }
  } else {
    error()<<"Internal error ("<<__FILE__<<":"<<__LINE__<<")!"<<std::endl;
    return(__LINE__);
  }
  if (debug){
    output<<space<<begin<<"Directive 'include' (stop,"<<c<<"): "<<tokens<<end<<std::endl;
  }
  return(0);
}
int LineInclude::dependences(const Options & options,std::ostream & output,Files::file_set_t & files,bool last){
  if (includedFile) return(includedFile->dependences(options,output,files,true));
  return(0);
}
//===========================================
int LineNamespace::parseLine(const Options & options){
  int out;
  out=loadTokens(options);
  if (out) return(out);
  for (tokens_t::const_iterator it=tokens.cbegin();it!=tokens.end();++it){
    static std::wregex r(L"[a-zA-Z0-9_$]+",std::regex_constants::collate);
    std::wsmatch match;
    std::string stoken(it->second);
    std::wstring wtoken;
    ict::global::transferUTF(stoken,wtoken);
    if (wtoken.front()==L'-'){
      root_namespace=true;
      namespace_names.clear();
    }
    if (!testName(it->second)){
      error()<<"Wrong name format in 'namespace' directive: "<<it->second<<" !"<<std::endl;
      return(__LINE__);
    }
    if (std::regex_search(wtoken,match,r)){
      for (const std::wstring m : match){
        std::string sm;
        std::wstring wm(m);
        ict::global::transferUTF(wm,sm);
        namespace_names.push_back(sm);
      }
    }
  }
  if (options.getVerbose()>LOG_NOTICE) {
    if (root_namespace){
      notice()<<"Directive 'namespace' added (root): "<<namespace_names<<std::endl;
    } else {
      notice()<<"Directive 'namespace' added: "<<namespace_names<<std::endl;
    }
  }
  line.clear();
  return(0);
}
int LineNamespace::serialize(
  const Options & options,std::ostream & output,
  const namespace_t & namesp_in,const tokens_t & tokens_in,const line_list_t & lines_in,
  depth_t depth_in,depth_t comments_in
){
  int out;
  std::string space;
  std::string begin;
  std::string end;
  std::size_t c=childrenCount();
  namespace_t namesp_tmp(namesp_in);
  bool debug((!options.getStrip())&&(options.getVerbose()>LOG_DEBUG));
  if (!options.getStrip()) space.append(depth_in?(depth_in-1):0,' ');
  if (root_namespace) {
    namesp_tmp=namespace_names;
  } else {
    for (const std::string n : namespace_names){
      namesp_tmp.push_back(n);
    }
  }
  if (debug){
    if (comments_in){
      begin="/* ";
      end=" */";
    } else {
      begin="<!-- ";
      end=" -->";
    }
    output<<space<<begin<<"Directive 'namespace' (start): "<<namesp_tmp<<end<<std::endl;
  }
  out=serializeChildren(options,output,namesp_tmp,tokens_in,lines_in,depth_in,comments_in);
  if (out) return(out);
  if (debug){
    output<<space<<begin<<"Directive 'namespace' (stop,"<<c<<"): "<<namesp_tmp<<end<<std::endl;
  }
  return(0);
}
//===========================================
depth_t LineDefine::defCount=0;
int LineDefine::parseLine(const Options & options){
  int out;
  out=loadTokens(options);
  if (out) return(out);
  for (tokens_t::const_iterator it=tokens.cbegin();it!=tokens.end();++it){
    defined_names.push_back(it->second);
  }
  if (options.getVerbose()>LOG_NOTICE) notice()<<"Directive 'define' added: "<<defined_names<<std::endl;
  line.clear();
  return(0);
}
int LineDefine::serialize(
  const Options & options,std::ostream & output,
  const namespace_t & namesp_in,const tokens_t & tokens_in,const line_list_t & lines_in,
  depth_t depth_in,depth_t comments_in
){
  std::string space;
  std::string begin;
  std::string end;
  std::size_t c=childrenCount();
  bool debug((!options.getStrip())&&(options.getVerbose()>LOG_DEBUG));
  if (!options.getStrip()) space.append(depth_in?(depth_in-1):0,' ');
  if (debug){
    if (comments_in){
      begin="/* ";
      end=" */";
    } else {
      begin="<!-- ";
      end=" -->";
    }
    output<<space<<begin<<"Directive 'define' (start): "<<tokens_in<<end<<std::endl;
  }
  for (const std::string & n : defined_names){
    std::string fullName(Names::fullName(namesp_in,n));
    Line * ptr=Names::name2Line(fullName);
    if (ptr){
      if ((Names::name2id(fullName)!=fileId)||(Names::name2no(fullName)!=lineNo)){
        error()<<"Name "<<fullName<<" registred twice! First definition: "<<ict::os::getRelativePath(Files::id2Path(Names::name2id(fullName)))<<":"<<Names::name2no(fullName)<<std::endl;
        return(__LINE__);
      }
    } else {
      Names::add(fullName,this,fileId,lineNo);
      if (debug)  output<<space<<" "<<begin<<"Directive 'define' has registred new name: "<<fullName<<end<<std::endl;
    }
  }
  if (debug){
    output<<space<<begin<<"Directive 'define' (stop,"<<c<<"): "<<tokens_in<<end<<std::endl;
  }
  return(0);
}
//===========================================
int LineClone::parseLine(const Options & options){
  int out;
  out=loadTokens(options);
  if (out) return(out);
  if (!tokens.size()){
    error()<<"Name is missing in 'clone' directive!"<<std::endl;
    return(__LINE__);
  }
  if (!testName(tokens.at(0))){
    error()<<"Wrong name format in 'clone' directive: "<<tokens.at(0)<<" !"<<std::endl;
    return(__LINE__);
  }
  if (options.getVerbose()>LOG_NOTICE) notice()<<"Directive 'clone' added: "<<tokens<<std::endl;
  line.clear();
  return(0);
}
int LineClone::serialize(
  const Options & options,std::ostream & output,
  const namespace_t & namesp_in,const tokens_t & tokens_in,const line_list_t & lines_in,
  depth_t depth_in,depth_t comments_in
){
  int out;
  std::string space;
  std::string begin;
  std::string end;
  std::size_t c=childrenCount();
  line_list_t lines;
  static std::regex r("[0-9]+");
  bool debug((!options.getStrip())&&(options.getVerbose()>LOG_DEBUG));
  if (!options.getStrip()) space.append(depth_in?(depth_in-1):0,' ');
  if (debug){
    if (comments_in){
      begin="/* ";
      end=" */";
    } else {
      begin="<!-- ";
      end=" -->";
    }
    output<<space<<begin<<"Directive 'clone' (start): "<<tokens_in<<end<<std::endl;
  }
  getChildren(lines);
  if (std::regex_match(tokens.at(0),r)){
    std::size_t no=0;
    try{
      no=std::stoull(tokens.at(0));
    }catch(...){
      no=0;
    }
    if (no==0){
      for (Line * ptr : lines_in){
        if (ptr){
          out=ptr->serialize(options,output,namesp_in,tokens,lines,depth_in,comments_in);
          if (out) return(out);
        }
      }
    } else if (lines_in.size()>no){
      Line * ptr=lines_in.at(no);
      if (ptr){
        out=ptr->serialize(options,output,namesp_in,tokens,lines,depth_in,comments_in);
        if (out) return(out);
      }
    }
  } else {
    std::string fullName(Names::fullName(namesp_in,tokens.at(0)));
    Line * ptr=Names::name2Line(fullName);
    if (ptr){
       out=ptr->serializeChildren(options,output,namesp_in,tokens,lines,depth_in,comments_in);
       if (out) return(out);
    } else {
      error()<<"Name "<<fullName<<" not found!"<<std::endl;
      return(__LINE__);
    }
  }
  if (debug){
    output<<space<<begin<<"Directive 'clone' (stop,"<<c<<"): "<<tokens_in<<end<<std::endl;
  }
  return(0);
}
//===========================================
int LineText::parseLine(const Options & options){
  static std::wregex r(L"-[ \\t]*(.*)");
  std::wsmatch match;
  std::string sline(line);
  std::wstring wline;
  ict::global::transferUTF(sline,wline);
  if ((std::regex_match(wline,match,r))&&(match.size()>1)) {
    std::string stext;
    std::wstring wtext(match[1]);
    ict::global::transferUTF(wtext,stext);
    textLine=stext;
  } else {
    error()<<"Internal parsing error !"<<std::endl;
    return(__LINE__);
  }
  if (options.getVerbose()>LOG_NOTICE) notice()<<"Text added (size): "<<textLine.size()<<std::endl;
  line.clear();
  return(0);
}
int LineText::serialize(
  const Options & options,std::ostream & output,
  const namespace_t & namesp_in,const tokens_t & tokens_in,const line_list_t & lines_in,
  depth_t depth_in,depth_t comments_in
){
  int out;
  std::string space;
  if (!options.getStrip()) space.append(depth_in?(depth_in-1):0,' ');
  output<<space<<encodeHtml(replaceTokens(textLine,tokens_in))<<std::endl;
  out=serializeChildren(options,output,namesp_in,tokens_in,lines_in,depth_in,comments_in);
  if (out) return(out);
  return(0);
}
//===========================================
int LineHtml::parseLine(const Options & options){
  static std::wregex r(L"=[ \\t]*(.*)");
  std::wsmatch match;
  std::string sline(line);
  std::wstring wline;
  ict::global::transferUTF(sline,wline);
  if ((std::regex_match(wline,match,r))&&(match.size()>1)) {
    std::string shtml;
    std::wstring whtml(match[1]);
    ict::global::transferUTF(whtml,shtml);
    htmlLine=shtml;
  } else {
    error()<<"Internal parsing error !"<<std::endl;
    return(__LINE__);
  }
  if (options.getVerbose()>LOG_NOTICE) notice()<<"HTML added (size): "<<htmlLine.size()<<std::endl;
  line.clear();
  return(0);
}
int LineHtml::serialize(
  const Options & options,std::ostream & output,
  const namespace_t & namesp_in,const tokens_t & tokens_in,const line_list_t & lines_in,
  depth_t depth_in,depth_t comments_in
){
  int out;
  std::string space;
  if (!options.getStrip()) space.append(depth_in?(depth_in-1):0,' ');
  output<<space<<replaceTokens(htmlLine,tokens_in)<<std::endl;
  out=serializeChildren(options,output,namesp_in,tokens_in,lines_in,depth_in,comments_in);
  if (out) return(out);
  return(0);
}
//===========================================
int LineComment::parseLine(const Options & options){
  static std::wregex r(L"/[ \\t]*(.*)");
  std::wsmatch match;
  std::string sline(line);
  std::wstring wline;
  ict::global::transferUTF(sline,wline);
  if ((std::regex_match(wline,match,r))&&(match.size()>1)) {
    std::string scomment;
    std::wstring wcomment(match[1]);
    ict::global::transferUTF(wcomment,scomment);
    commentLine=scomment;
  } else {
    error()<<"Internal parsing error !"<<std::endl;
    return(__LINE__);
  }
  if (options.getVerbose()>LOG_NOTICE) notice()<<"Comment added (size): "<<commentLine.size()<<std::endl;
  line.clear();
  return(0);
}
int LineComment::serialize(
  const Options & options,std::ostream & output,
  const namespace_t & namesp_in,const tokens_t & tokens_in,const line_list_t & lines_in,
  depth_t depth_in,depth_t comments_in
){
  int out;
  std::string space;
  std::string begin;
  std::string end;
  std::size_t c=childrenCount();
  if (options.getStrip()) return(0);
  if (!options.getStrip()) space.append(depth_in?(depth_in-1):0,' ');
  if (comments_in){
    begin="/* ";
    end=" */";
  } else {
    begin="<!-- ";
    end=" -->";
  }
  output<<space<<begin<<replaceTokens(commentLine,tokens_in);
  if (c){
    output<<std::endl;
    out=serializeChildren(options,output,namesp_in,tokens_in,lines_in,depth_in,comments_in+1);
    if (out) return(out);
    output<<space;
  }
  output<<end<<std::endl;
  return(0);
}
//===========================================
int LineMock::parseLine(const Options & options){
  static std::wregex r(L"//[ \\t]*(.*)");
  std::wsmatch match;
  std::string sline(line);
  std::wstring wline;
  std::wstring commentLine;
  ict::global::transferUTF(sline,wline);
  if ((std::regex_match(wline,match,r))&&(match.size()>1)) {
    commentLine=match[1];
  } else {
    error()<<"Internal parsing error !"<<std::endl;
    return(__LINE__);
  }
  if (options.getVerbose()>LOG_NOTICE) notice()<<"Comment ignored (size): "<<commentLine.size()<<std::endl;
  line.clear();
  return(0);
}
int LineMock::serialize(
  const Options & options,std::ostream & output,
  const namespace_t & namesp_in,const tokens_t & tokens_in,const line_list_t & lines_in,
  depth_t depth_in,depth_t comments_in
){
  return(0);
}
//===========================================
LineNode::stage_t LineNode::ctrlChar(stage_t stage,wchar_t c){
  switch (c){
    case L'~':no_short=true;
    case L' ':case L'\t':case L'\0':return(stage_space);
    default:break;
  }
  switch (stage){
    case stage_tag:case stage_class: switch (c){
      case L'.': return(stage_class);
      case L'#': return(stage_id);
      case L'(': return(stage_attr_name);
      default:break;
    } break;
    case stage_id:switch (c){
      case L'(': return(stage_attr_name);
      default:break;
    } break;
    case stage_attr_name: switch (c){
      case L'=': return(stage_attr_value);
      case L',': return(stage_attr_name);
      case L')': return(stage_space);
      default:break;
    } break;
    case stage_attr_value: switch (c){
      case L',': return(stage_attr_name);
      case L')': return(stage_space);
      default:break;
    } break;
    default:break;
  }
  return(stage_start);
}
void LineNode::saveToken(stage_t stage,const std::string & token,std::string & attrName){
  switch(stage){
    case stage_tag:
      tag=token;
      attrName.clear();
      break;
    case stage_class:
      class_names.push_back(token);
      attrName.clear();
      break;
    case stage_id:
      id=token;
      attrName.clear();
      break;
    case stage_attr_name:
      attrName=token;
      if (attrName.size()) attr[attrName]="";
      break;
    case stage_attr_value:
      if (attrName.size()){
        attr[attrName]=token;
      }
      break;
    default:break;
  }
}
std::string LineNode::getAttr(const namespace_t & namesp_in,const tokens_t & tokens_in){
  std::string out;
  attr_t a(attr);
  for (const std::string & c : class_names){
    if (c.size()) {
      const static std::string class_str("class");
      if (a.count(class_str)) a[class_str]+=' ';
      a[class_str]+=Names::fullName(namesp_in,c);
    }
  }
  if (id.size()) a["id"]=Names::fullName(namesp_in,id);
  for (attr_t::const_iterator it=a.cbegin();it!=a.cend();++it){
    out+=' ';
    out+=encodeHtml(replaceTokens(it->first,tokens_in));
    if (it->second.size()){
      out+='=';
      out+='"';
      out+=encodeHtml(replaceTokens(it->second,tokens_in));
      out+='"';
    }
  }
  return(out);
}
int LineNode::loadAll(const Options & options){
  std::size_t k=0;
  wchar_t q(L'\0');
  wchar_t l(L'\0');
  std::wstring wtoken;
  std::string attrName;
  bool empty=true;
  stage_t stage_now(stage_start);
  stage_t stage_next(stage_start);
  std::string sline(line);
  std::wstring wline;
  std::wstring wtext;
  ict::global::transferUTF(sline,wline);
  wline+=L'\0';
  for (wchar_t c : wline){
    if (stage_now==stage_start){//Jeśli to początek
      switch(c){
        case L' ':case L'\t':case L'\0':
          break;
        default:
          stage_now=stage_tag;
          if (c) wtoken+=c;
          empty=false;
          break;
      }
    } else if (stage_now==stage_text){//Jeśli to już jest tekst
      if (c) wtext+=c;
    } else if (stage_now==stage_space){//Jeśli to jest przerwa pomiędzy definicją taga a tekstem.
      if ((c==L'~')&&(l!=L' ')&&(l!=L'\t')&&(!no_short)){
        no_short=true;
      } else switch(c){
        case L' ':case L'\t':case L'\0':
          break;
        default:
          stage_now=stage_text;
          wtext+=c;
          break;
      }
    } else if (l==L'\\'){//Poprzednio był znak ucieczki.
      switch(c){
        case L'\0':break;
        case L'n' :wtoken+=L'\n';break;
        case L'r' :wtoken+=L'\r';break;
        case L't' :wtoken+=L'\t';break;
        case L'v' :wtoken+=L'\v';break;
        case L'0' :wtoken+=L'\0';break;
        default:wtoken+=c;break;
      }
      empty=false;
    } else if (c=='\\') {//Teraz jest znak ucieczki.
      empty=false;
    } else if (q=='\0'){//Nie było cudzysłowa.
      if ((c=='"')||(c=='\'')) {//Zaczyna się cudzysłów.
        q=c;
        empty=false;
      } else if (stage_next=ctrlChar(stage_now,c)) {//Teraz jest znak sterujący.
        if (!empty){//Token nie jest pusty.
          std::string stoken;
          ict::global::transferUTF(wtoken,stoken);
          saveToken(stage_now,stoken,attrName);
          wtoken.clear();
          empty=true;
        }
        stage_now=stage_next;
      } else {//Nie jest to znak sterujący.
        if (c) wtoken+=c;
        empty=false;
      }
    } else {//Był cudzysłów.
      if (q==c) {//Kończy się cudzysłów.
        q=L'\0';
      } else {//Nie kończy się cudzysłów.
        if (c) wtoken+=c;
      }
      empty=false;
    }
    l=c;
  }
  ict::global::transferUTF(wtext,textLine);
  return(0);
}
int LineNode::parseLine(const Options & options){
  int out;
  out=loadAll(options);
  if (out) return(out);
  if (!tag.size()) tag=options.getDefaultTag();
  if (options.getVerbose()>LOG_NOTICE) 
    notice()<<"Tag added (tag name, class names, id, atrributes, text size): "<<tag<<", "<<class_names<<", "<<id<<", "<<attr<<", "<<textLine.size()<<std::endl;
  line.clear();
  return(0);
}
int LineNode::serialize(
  const Options & options,std::ostream & output,
  const namespace_t & namesp_in,const tokens_t & tokens_in,const line_list_t & lines_in,
  depth_t depth_in,depth_t comments_in
){
  int out;
  std::string space;
  std::size_t c=childrenCount();
  if (!options.getStrip()) space.append(depth_in?(depth_in-1):0,' ');
  if (textLine.size()||c||no_short){
    output<<space<<"<"<<encodeHtml(replaceTokens(tag,tokens_in))<<getAttr(namesp_in,tokens_in)<<">"<<encodeHtml(replaceTokens(textLine,tokens_in));
    if (c) output<<std::endl;
    out=serializeChildren(options,output,namesp_in,tokens_in,lines_in,depth_in,comments_in);
    if (out) return(out);
    if (c) output<<space;
    output<<"</"<<encodeHtml(replaceTokens(tag,tokens_in))<<">"<<std::endl;
  } else {
    output<<space<<"<"<<encodeHtml(replaceTokens(tag,tokens_in))<<getAttr(namesp_in,tokens_in)<<"/>"<<std::endl;
  }
  return(0);
}
//===========================================
depth_t Interface::counter=0;
Interface::Interface(bool strip_in,depth_t maxDepth_in,depth_t verbose_in,const tokens_t & tokens_in,const std::string & defaultTag_in):
  c(counter),options(strip_in,maxDepth_in,verbose_in,tokens_in,defaultTag_in){
    counter++;
  }
Interface::~Interface(){
  Files::clear();
  Names::clear();
  for (Line * f:files) if (f) delete(f);
  counter--;
}
void Interface::addInput(const std::string & input){
  if (c) return;
  files.emplace_back(new LineFile(input));
}
int Interface::parse(){
  int out;
  if (c) return(__LINE__);
  for (Line * f:files) if (f) {
    out=f->parseLine(options);
    if (out) return(out);
  }
  return(0);
}
int Interface::dependences(std::ostream & output){
  int out;
  if (c) return(__LINE__);
  for (Line * f:files) if (f) {
    Files::file_set_t files;
    out=f->dependences(options,output,files);
    output<<std::endl;
    if (out) return(out);
  }
  return(0);
}
int Interface::dependencesAll(std::ostream & output){
  if (c) return(__LINE__);
  return(Files::dependences(options,output));
}
int Interface::serialize(std::ostream & output){
  int out;
  if (c) return(__LINE__);
  for (Line * f:files) if (f) {
    out=f->serializeLine(options,output);
    if (out) return(out);
  }
  return(0);
}
//===========================================
}}
//===========================================