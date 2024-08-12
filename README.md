# SQLite3 Functions

This is a collection of SQLite extensions that provide various functionality that I've needed in my work.

 - `fuzzysame.c` - `fuzzysame(name1,name2,max_distance)` a function that compares two names and returns true when they are above below distance score (0-1)
 - `samename.cpp` -  `samename(name1, name2)` a function that returns true if two names are determined to be the same despite different formats (e.g., "last, first mid" vs. "first mid last").
 - `sqlite3-regexp.cpp` - provides the following libboost regex functions: `regexp_match(column, regex)`, `regexp_search(column, regex)`, and `regexp_replace(column, regex, format_string)`

To build them use `make`.

To use them in SQLite3 via the CLI you can do the following:

    sqlite3 my.db
    .load ./libfuzzysame
    .load ./libsamename
    .load ./libregexp

Or if you're within Datasette you can do:

    datasette --load-extension ./libfuzzysame.so \
              --load-extension ./libsamename.so \
              --load-extension ./libregexp.so

Similarly for sqlite-utils:

    sqlite-utils --load-extension=./libregexp.so

From there you should be able to run queries using these functions:

    select id, name, email from users where regexp_match(email, ".*@.*.google.com$");
