// xml.hpp

/*===========================================================================
*
*                            PUBLIC DOMAIN NOTICE                          
*               National Center for Biotechnology Information
*                                                                          
*  This software/database is a "United States Government Work" under the   
*  terms of the United States Copyright Act.  It was written as part of    
*  the author's official duties as a United States Government employee and 
*  thus cannot be copyrighted.  This software/database is freely available 
*  to the public for use. The National Library of Medicine and the U.S.    
*  Government have not placed any restriction on its use or reproduction.  
*                                                                          
*  Although all reasonable efforts have been taken to ensure the accuracy  
*  and reliability of the software and data, the NLM and the U.S.          
*  Government do not and cannot warrant the performance or results that    
*  may be obtained by using this software or data. The NLM and the U.S.    
*  Government disclaim all warranties, express or implied, including       
*  warranties of performance, merchantability or fitness for any particular
*  purpose.                                                                
*                                                                          
*  Please cite the author in any work or product based on this material.   
*
* ===========================================================================
*
* Author: Vyacheslav Brover
*
* File Description:
*   XML analysis utilities
*
*/


#ifndef XML_HPP_78429
#define XML_HPP_78429

#include "../common.hpp"
#include "../tsv/tsv.hpp"
using namespace Common_sp;




namespace Xml_sp
{



// Analysis

struct FlatTable : Root
// name: file name
{
  OFStream f;
  size_t keys {0};
  VectorPtr<Token> row;
  Token id;
  
  
  FlatTable (const string &fileName,
             size_t keys_arg,
             size_t column_max)
    : f (fileName)
    , keys (keys_arg)
    , row (column_max)
    { row [0] = & id; 
      id. type = Token::eInteger;
    }
  void qc () const override;
    
    
  void write (size_t xmlNum);
    // Update: row[]
};



struct Schema : Root
{
  // Tree
  const Schema* parent {nullptr};
  map<string/*name*/,const Schema*> name2schema;
    // !nullptr
    // 1-1
  bool multiple {false};
  Set<Token::Type> types;
  bool tokensStored {false};
  Set<Token> tokens;
    // !tokensStored => empty()
  size_t len_max {0};
  size_t rows {1};
  
  unique_ptr<const FlatTable> flatTable;
    // get() => multiple
  size_t column {no_index};
    // Valid if flatTable.get()
    // no_index <=> types.empty()
  
  
  Schema (const Schema* parent_arg,
          bool tokensStored_arg)
    : parent (parent_arg)
    , tokensStored (tokensStored_arg)
    {}
  static Schema* readSchema (const string &fName,
                             string &name);
    // Input: fName: created by xml2schema
    // Output: name
private:
  Schema (const Schema* parent_arg,
          const StringVector &vec,
          size_t &start,
          size_t depth,
          string &name);
public:
 ~Schema ()
    { for (auto& it : name2schema)
        delete it. second;
    }
  void qc () const override;
  void saveText (ostream &os) const override;
  
  
  const string& schema2name (const Schema* sch) const
    { for (const auto& it : name2schema)
        if (it. second == sch)
          return it. first;
      throw logic_error ("Schema::schema2name()");
    }
  void merge (Schema& other);

  // SQL      
  string getColumnName (const Schema* rootTable) const
    { if (this == rootTable)
        return string ();
      if (! parent)
        return string ();
      string s (parent->getColumnName (rootTable));
      if (! s. empty ())
        s += "_";
      return s + parent->schema2name (this);
    }
  void printTableDdl (ostream &os,
                      const Schema* curTable) const;
private:
  void printColumnDdl (ostream &os,
                       const Schema* curTable) const;
public:
  
  // Output: flatTable, column
  void setFlatTables (const string &dirName,
                      const Schema* curTable);
private:
  void setFlatColumns (const Schema* curTable,
                       size_t &column_max);
public:
};



struct Data : Named
{
  // Tree
  const Data* parent {nullptr};
  VectorOwn<Data> children;
    // May be empty()

  const bool attribute;
  bool colonInName {false};
  Token token;
    // May be empty()
private:
  bool isEnd {false};
  bool xmlText {false};
  mutable size_t columnTags {0};
public:
  

  Data (TokenInput &tin,
        VectorOwn<Data> &markupDeclarations);
private:
  Data (Data* parent_arg,
        TokenInput &ti)
    : parent (parent_arg)
    , attribute (false)
    { readInput (ti); }
  Data (Data* parent_arg,
        bool attribute_arg,
        bool colonInName_arg,
        string &&attr,
        Token &&value)
    : Named (move (attr))
    , parent (parent_arg)
    , attribute (attribute_arg)
    , colonInName (colonInName_arg)
    , token (move (value))
    {}
  Data (const Data &) = default;
  void clear () override;
  void readInput (TokenInput &ti);
    // <tag attribute1="value1" attribute2="value2" ... />
    // <tag attribute1="value1" attribute2="value2" ... > Data1 Data2 ... </tag>
    // <tag attribute1="value1" attribute2="value2" ... > XmlText </tag>
    // </tag>
    // <!-- comment -->
    // <? ProcessingInstruction ?>
  static bool readColonName (TokenInput &ti,
                             string &name);
    // Update: name
public:
  static Data* load (const string &fName,
                     VectorOwn<Data> &markupDeclarations);
  void qc () const override;
  void saveXml (Xml::File &f) const override;
  void saveText (ostream &os) const override;
    // attribute => skip
  
  
  size_t getDepth () const
    { if (parent)
        return parent->getDepth () + 1;
      return 0;
    }
  size_t getNodes () const
    { size_t n = 1;
      for (const Data* child : children)
        n += child->getNodes ();
      return n;
    }
  size_t getLeaves () const
    { if (children. empty ())
        return 1;
      size_t n = 0;
      for (const Data* child : children)
        n += child->getLeaves ();
      return n;
    }
  bool contains (const string &what,
                 bool equalName,
                 bool tokenSubstr,
                 bool tokenWord) const;
  bool find (VectorPtr<Data> &path,
             const string &what,
             bool equalName,
             bool tokenSubstr,
             bool tokenWord) const;
    // DFS
    // Update: path: reverse path to *Data containing <what>
    //               valid if Return
    // Invokes: contains()
  const Data* name2child (const string &name_arg) const
    { for (const Data* child : children)
        if (child->name == name_arg)
          return child;
      return nullptr;
    }
  TextTable unify (const Data& query,
                   const string &variableTagName) const;
private:
  StringVector tagName2texts (const string &tagName) const;
    // Output: columnTags
  bool unify_ (const Data& query,
               const string &variableTagName,
               size_t columnTags_root,
               map<string,StringVector> &tag2values,
               TextTable &tt) const;
    // Update: tag2values, tt (append)
public:
  Schema* createSchema (bool storeTokens) const;
    // Return: new
  void writeFiles (size_t xmlNum,
                   const Schema* sch,
                   FlatTable* flatTable) const;
};




}  



#endif
