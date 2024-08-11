#if !defined(SQLITE_CORE)
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#ifndef SQLITE_CORE
#include "sqlite3ext.h"
SQLITE_EXTENSION_INIT1
#else
#include "sqlite3.h"
#endif


static void samenamefunc(sqlite3_context *context, int argc, sqlite3_value **argv){
	// two arguments: name1, name2
	assert( argc==2 );

	// 1) get shortest of two names, this is maxlen
	// 2) init name1 and name2 char index pointers = 0
	// 3) loop over each character, while name1 & name2 pointers < maxlen :
	//   a) if one is a space or a period, increase that name char
	//   index, repeat loop with same other char
	//   b) lowercase each if necessary
	//
	// Lower case strings:
	//   for ( ; *p; ++p) *p = tolower(*p);

	// Convert names to char* and calculate min length
	const char* name1 = (const char*)sqlite3_value_text(argv[0]);
	const char* name2 = (const char*)sqlite3_value_text(argv[1]);
	if (name1==NULL || name2==NULL)
	{
		sqlite3_result_int(context, 0);
		return;
	}

	size_t name1len = strlen(name1);
	size_t name2len = strlen(name2);

	if ((name1len == 0) || (name2len == 0))
	{
		sqlite3_result_int(context, 0);
		return;
	}

	int minlen = (name2len < name1len) ? (int)name2len : (int)name1len;

	char* n = (char*)name1;
	char* m = (char*)name2;
    // default to equal
	int equal = 1;
    // flag for if we actually did a compare of valid chars
    // this lets us fix equal if we didn't do any compares
    // due to all chars being invalid/ignored
    int compared = 0;
    // keep track of both positions
    int n_i = 0;
    int m_i = 0;
    while(n_i < name1len && m_i < name2len)
	{
        if (tolower(*n) < 'a' || tolower(*n) > 'z')
        {
            n++; n_i++; continue;
        }
        if (tolower(*m) < 'a' || tolower(*m) > 'z')
        {
            m++; m_i++; continue;
        }
        compared = 1;
		if(tolower(*n) != tolower(*m))
		{
            equal = 0;
            break;
		}
        n++; m++;
        n_i++; m_i++;
	}

    // all-invalid strings are not equal
    if (!compared) equal = 0;

	// for (int k=0; k<minlen; k++)
	// {
	// 	if(tolower(*n++) != tolower(*m++))
	// 	{
	// 		equal = 0;
	// 		break;
	// 	}
	// }

	sqlite3_result_int(context, equal);

// wrong_number_args:
//   sqlite3_result_error(pCtx, "wrong number of arguments to function samename()", -1);

}


// static void samenamefunc(sqlite3_context *pCtx, int nVal, sqlite3_value **apVal){
//   int *aMatchinfo;				/* Return value of matchinfo() */
//   int nCol;					   /* Number of columns in the table */
//   int nPhrase;					/* Number of phrases in the query */
//   int iPhrase;					/* Current phrase */
//   double score = 0.0;			 /* Value to return */

//   assert( sizeof(int)==4 );

//   /* Check that the number of arguments passed to this function is correct.
//   ** If not, jump to wrong_number_args. Set aMatchinfo to point to the array
//   ** of unsigned integer values returned by FTS function matchinfo. Set
//   ** nPhrase to contain the number of reportable phrases in the users full-text
//   ** query, and nCol to the number of columns in the table.
//   */
//   if( nVal != 1 ) goto wrong_number_args;

//   aMatchinfo = (unsigned int *)sqlite3_value_blob(apVal[0]);
//   nPhrase = aMatchinfo[0];
//   nCol = aMatchinfo[1];
//   if( nVal!=(1+nCol) ) goto wrong_number_args;

//   /* Iterate through each phrase in the users query. */
//   for(iPhrase=0; iPhrase<nPhrase; iPhrase++){
//	 int iCol;					 /* Current column */

//	 /* Now iterate through each column in the users query. For each column,
//	 ** increment the relevancy score by:
//	 **
//	 **   (<hit count> / <global hit count>) * <column weight>
//	 **
//	 ** aPhraseinfo[] points to the start of the data for phrase iPhrase. So
//	 ** the hit count and global hit counts for each column are found in 
//	 ** aPhraseinfo[iCol*3] and aPhraseinfo[iCol*3+1], respectively.
//	 */
//	 int *aPhraseinfo = &aMatchinfo[2 + iPhrase*nCol*3];
//	 for(iCol=0; iCol<nCol; iCol++){
//	   int nHitCount = aPhraseinfo[3*iCol];
//	   int nGlobalHitCount = aPhraseinfo[3*iCol+1];
//	   double weight = sqlite3_value_double(apVal[iCol+1]);
//	   if( nHitCount>0 ){
//		 score += ((double)nHitCount / (double)nGlobalHitCount) * weight;
//	   }
//	 }
//   }

//   sqlite3_result_double(pCtx, score);
//   return;

//   /* Jump here if the wrong number of arguments are passed to this function */
// wrong_number_args:
//   sqlite3_result_error(pCtx, "wrong number of arguments to function samename()", -1);
// }

int sqlite3samenameInit(sqlite3 *db){
  sqlite3_create_function(db, "samename", -1, SQLITE_UTF8, 0, &samenamefunc, 0, 0);
  return 0;
}

#if !SQLITE_CORE
int sqlite3_extension_init(
  sqlite3 *db, 
  char **pzErrMsg,
  const sqlite3_api_routines *pApi
){
  SQLITE_EXTENSION_INIT2(pApi)
  return sqlite3samenameInit(db);
}
#endif

#endif


// Compiled with: gcc -shared -fPIC -lm samename.c -o libsamename.so
