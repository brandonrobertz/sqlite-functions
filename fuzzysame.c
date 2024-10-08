#if !defined(SQLITE_CORE)
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef SQLITE_CORE
#include "sqlite3ext.h"
SQLITE_EXTENSION_INIT1
#else
#include "sqlite3.h"
#endif


static void fuzzysamefunc(sqlite3_context *context, int argc, sqlite3_value **argv);


// helpers
int minimum(int a,int b,int c);

/*
  function to determine levenshtein distance
  levenshtein(src,dts,max_distance) => bool
*/
static void fuzzysamefunc(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
){
  
  if (argc < 3) {
    sqlite3_result_int(context, 0);
    return;
  }
  
  const char *s    = (const char *)sqlite3_value_text(argv[0]);
  const char *t    = (const char *)sqlite3_value_text(argv[1]);
  int max_distance = sqlite3_value_int(argv[2]);

  if (s==NULL || t==NULL)
  {
      sqlite3_result_int(context, 0);
      return;
  }
  
  //Step 1
  int n = strlen(s); 
  int m = strlen(t);
  if ((n == 0) || (m == 0)) {
    sqlite3_result_int(context, 0);
    return;
  }
  
  if (abs(n - m) > max_distance) {
    sqlite3_result_int(context, 0);
    return;
  }
  
  int *d = malloc((sizeof(int))*(m+1)*(n+1));
  m++;
  n++;
  
  //Step 2
  int k;
  for(k=0;k<n;k++) d[k]=k;
  for(k=0;k<m;k++) d[k*n]=k;
  
  //Step 3 and 4	
  int i, j;
  for(i=1; i<n; i++) {
    for(j=1; j<m; j++) {
      int cost;
      
      //Step 5
      //cost = s[i-1] != t[j-1];
      if (s[i-1] == t[j-1]) {
        cost = 0;
      }  
      else {
        cost = 1;
      }
        
      //Step 6			 
      d[j*n+i] = minimum(d[(j-1)*n+i]+1, d[j*n+i-1]+1, d[(j-1)*n+i-1]+cost);
    }
  }
  
  int distance = d[n*m-1];
  free(d);
  
  sqlite3_result_int(context, distance <= max_distance);
}


inline int minimum(int a, int b, int c) {
  int min = a;
  if (b < min) min=b;
  if (c < min) min=c;
  return min;
}

int sqlite3fuzzysameInit(sqlite3 *db)
{
  sqlite3_create_function(db, "fuzzysame", -1, SQLITE_UTF8, 0, &fuzzysamefunc, 0, 0);
  return 0;
}

#if !SQLITE_CORE
int sqlite3_extension_init(
  sqlite3 *db, 
  char **pzErrMsg,
  const sqlite3_api_routines *pApi
){
  SQLITE_EXTENSION_INIT2(pApi)
  return sqlite3fuzzysameInit(db);
}
#endif

#endif

// Compiled with: gcc -shared -fPIC -lm fuzzysame.c -o libfuzzysame.so
