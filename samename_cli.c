#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>


int samenamefunc(const char * name1, const char * name2) {
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
	if (name1==NULL || name2==NULL)
	{
		return 0;
	}

	size_t name1len = strlen(name1);
	size_t name2len = strlen(name2);

	if ((name1len == 0) || (name2len == 0))
	{
		return 0;
	}

	int minlen = (name2len < name1len) ? (int)name2len : (int)name1len;

	char* n = (char*)name1;
	char* m = (char*)name2;
    // flag for if we actually did a compare of valid chars
    // this lets us fix equal if we didn't do any compares
    // due to all chars being invalid/ignored
    int compared = 0;
    // keep track of both positions
    int n_i = 0;
    int m_i = 0;
    while(n_i < name1len && m_i < name2len)
	{
        printf("%c <=> %c\n", tolower(*n), tolower(*m));
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
            printf("Found unequal chars!\n");
            return 0;
		}
        n++; m++;
        n_i++; m_i++;
	}

    // all-invalid strings are not equal
    if (!compared) {
        printf("Never compared! Defaulting to not equal.\n");
        return 0;
    }

	// for (int k=0; k<minlen; k++)
	// {
	// 	if(tolower(*n++) != tolower(*m++))
	// 	{
    //         return 0;
	// 	}
	// }

    return 1;
}


int main( int argc, char *argv[] )  {
    // two arguments: name1, name2
    assert( argc==3 );
    printf("Comparing %s and %s\n", argv[1], argv[2]);
    if (samenamefunc(argv[1], argv[2])) {
        printf("Same!\n");
    } else {
        printf("Different.\n");
    }
}



// Compiled with: gcc samename_cli.c -o samename_cli
// gcc -shared -fPIC -lm samename.c -o libsamename.so
