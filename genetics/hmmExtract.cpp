// hmmExtract.cpp

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
*   Extract HMMs from an HMM library
*
*/


#undef NDEBUG
#include "../common.inc"

#include "../common.hpp"
using namespace Common_sp;
#include "../version.inc"




namespace 
{


struct ThisApplication : Application
{
	ThisApplication ()
	  : Application ("Extract HMMs from an HMM library")
		{
      version = VERSION;
		  addPositional ("in", "Input HMM library");
		  addPositional ("target", "List of HMMs to be extracted");
		  addFlag ("acc", "Target identifier field is \"ACC\", otherwise \"NAME\"");
		  addPositional ("out", "Output HMM library");
		}



  void body () const final
	{
		const string inFName     = getArg ("in");
		const string targetFName = getArg ("target");
		const bool   acc         = getFlag ("acc");
		const string outFName    = getArg ("out");
				

    StringVector targets (targetFName, (size_t) 1000, true);
    targets. sort ();


    const string field (string (acc ? "ACC" : "NAME") + " ");

    OFStream ofs (outFName);
    LineInput li (inFName);
    string s;
    string id;
    Progress prog;
    while (li. nextLine ())
    {
   	  s += li. line + "\n";
   	  replace (li. line, '\t', ' ');
      if (isLeft (li. line, field))
      {
        ASSERT (id. empty ());
        id = li. line. substr (field. size ());
        trim (id);
      }
      if (isLeft (li. line, "//"))
      {
        prog ();
        if (targets. contains (id))
          ofs << s;
        s. clear ();
        id. clear ();
      }
    }
  }
};



}  // namespace




int main (int argc, 
          const char* argv[])
{
  ThisApplication app;
  return app. run (argc, argv);
}



