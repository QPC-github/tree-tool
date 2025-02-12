// kmerIndex_make.cpp

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
*   Make a DNA k-mer index
*
*/


#undef NDEBUG
#include "../common.inc"

#include "../common.hpp"
using namespace Common_sp;
#include "seq.hpp"
using namespace Seq_sp;
#include "../version.inc"



namespace 
{
  
  

struct ThisApplication : Application
{
  ThisApplication ()
    : Application ("Make a DNA k-mer index.\n\
Index size = O(n), where n is the number of sequences.\
")  // ??
    {
      version = VERSION;
  	  addPositional ("kmer_index", "K-mer index file name");
  	  addPositional ("kmer_size", "K-mer size (1..14)");
    }


	
	void body () const final
  {
	  const string kmerFName =          getArg ("kmer_index");
	  const size_t kmer_size = (size_t) arg2uint ("kmer_size");
	  
    const KmerIndex kmi (kmerFName, kmer_size);
  }  
};


}  // namespace




int main (int argc, 
          const char* argv[])
{
  ThisApplication app;
  return app. run (argc, argv);
}



