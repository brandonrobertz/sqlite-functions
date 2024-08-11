#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include <boost/regex.hpp>
#include <sqlite3ext.h>
SQLITE_EXTENSION_INIT1


extern "C" {

	static void samenamefunc(sqlite3_context *context, int argc, sqlite3_value **argv){
		// two arguments: name1, name2
		assert( argc==2 );

		// Convert names to char* and calculate min length
		const char* name1 = (const char*)sqlite3_value_text(argv[0]);
		const char* name2 = (const char*)sqlite3_value_text(argv[1]);
		if (name1==NULL || name2==NULL)
		{
			sqlite3_result_int(context, 0);
			return;
		}

		int name1len = (int)strlen(name1);
		int name2len = (int)strlen(name2);

		// blank strings not equal
		if ((name1len == 0) || (name2len == 0))
		{
			sqlite3_result_int(context, 0);
			return;
		}

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

		// int minlen = (name2len < name1len) ? (int)name2len : (int)name1len;
		// for (int k=0; k<minlen; k++)
		// {
		// 	if(tolower(*n++) != tolower(*m++))
		// 	{
		// 		equal = 0;
		// 		break;
		// 	}
		// }

		sqlite3_result_int(context, equal);
	}

	int sqlite3_extension_init(sqlite3 *db, char **err, const sqlite3_api_routines *api) {
		SQLITE_EXTENSION_INIT2(api)

			sqlite3_create_function(db, "samename", -1, SQLITE_UTF8 | SQLITE_DETERMINISTIC, NULL, &samenamefunc, NULL, NULL);
		return 0;
	}
}
// Compiled with: g++ -O3 -fPIC -std=c++11 -shared samename.cpp -o libsamename.so $(pkg-config --cflags sqlite3) -fPIC -W -Werror -Wno-unused-parameter

