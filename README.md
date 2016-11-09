# Language
The *dorothy* is a HTML abstraction markup language for AngularJS apps (but might be also used in other cases).

# Inspiration
The dorothy language was inspired by 
[HAML](http://haml.info/) and 
[PUG (orginaly JADE)](https://pugjs.org/) languages.

# Compiler
The code (C++) in this repository provides *dorothy* compiler that 
compiles *dorothy* source into HTML.

# Basics
The *dorothy* language uses indentation to nest elements into each other 
(the same as HTML elements are nested). 
Every line includes one of the following directives:
 * `- any text` - inserts text node ("any text") into HTML file 
  (using HTML encoding);
 * `= any text` - inserts text ("any text") directly into HTML 
  (without changes);
 * `/ any text` - inserts comment node ("any text") into HTML file 
  (without changes);
 * `// any text` - comment (not inserted into HTML file);
 * `% define name` - defines a block (identified by "name") 
  which is not inserted into HTML file 
  until `% clone name` directive is used
  (if name starts with "-" then it is defined in root namespace);
 * `% clone name token1 "token2"` - clones a block (identified by "name") 
  into HTML file, text tokens ("token1","token2") replace "$1$" and "$2$" 
  strings in defined block
  (if block name starts with "-" then root namespace is used),
  if "name" is a number, then 'clone' subelements are used
  (0 - means all of them, 1 - first only, 2 - second, and so on);
 * `% namespace name` - defines namespace ("name") 
   which is used for class names,
   id names and block names in nested elements
   (if name starts with "-" then root namespace is used);
 * `% include path token1 "token2"` - includes a file (identified by "path") 
   into HTML file, text tokens ("token1","token2") replace "$1$" and "$2$" 
   strings in included file;
 * `tag.class1.class2#id(attr1=a1,attr2="a2") any text` - inserts node ("tag") 
   with class names ("class1 class2" - if name starts with "-" 
    then root namespace is used),
   with id ("id"- if name starts with "-" 
    then root namespace is used), 
   with attributes (attr1="a1" attr2="a2")
   and with first text node ("any text") - into HTML (using HTML encoding).

# Example
## Input files
### File *head.dorothy*
```
% namespace head
 head
  title $1
  meta(http-equiv="Content-Type",content="text/html; charset=UTF-8")
```
### File *body.dorothy*
```
% namespace body
 % define signature
  small.signature $1
 body#body
  h1.title $1
  p.-content
   % clone 1
```
### File *all.dorothy*
```
= <!doctype html>
% namespace all
 html
  / Begin
  // Header
  % include ./head "Page title"
  // Body
  % include ./body "Page header"
   span <span>Text content</span>
    - <span>Text content</span>
    = <span>Html content</span>
    % clone signature "User Name"
  / End
```
## Output files
### File *output.html*
```  html
<!doctype html>
 <html>
  <!-- Begin -->
    <head>
     <title>Page title</title>
     <meta content="text/html; charset=UTF-8" http-equiv="Content-Type"/>
    </head>
    <body id="all-body-body">
     <h1 class="all-body-title">Page header</h1>
     <p class="content">
      <span>&lt;span&gt;Text content&lt;/span&gt;
       &lt;span&gt;Text content&lt;/span&gt;
       <span>Html content</span>
        <small class="all-body-signature">User Name</small>
      </span>
     </p>
    </body>
  <!-- End -->
 </html>
```