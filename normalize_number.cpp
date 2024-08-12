#include <iostream>
#include <string>
#include <cctype>
#include <cstdlib>  // For std::strtol
#include <sqlite3ext.h>
#include <limits>
#include <stdexcept>

SQLITE_EXTENSION_INIT1

/**
  Get a string argument from SQLite. It assumes you already know it's there.
  You can check that with argc() or trust SQLite to do that. */
const char *getSQLiteString(
    sqlite3_context *ctx,
    sqlite3_value *arg,
    const std::string& func,
    const std::string& name) {
  const char *value = (const char *) sqlite3_value_text(arg);
  if (value) {
    return value;
  } else {
    sqlite3_result_error(ctx, (func + "(): missing " + name).data(), -1);
    return NULL;
  }
}

sqlite3_int64 convertToInt64(const std::string& input) {
    // Clean the input string
    std::string cleaned;
    for (char ch : input) {
        if (ch != ',' && ch != '+') {
            cleaned += ch;
        }
    }

    // Determine the suffix and numeric part
    double number = 0;
    char suffix = '\0';
    
    size_t len = cleaned.length();
    size_t pos = len;

    // Find the suffix (if any)
    if (pos > 0 && (cleaned[pos - 1] == 'M' || cleaned[pos - 1] == 'B' || cleaned[pos - 1] == 'K')) {
        suffix = cleaned[pos - 1];
        cleaned = cleaned.substr(0, pos - 1); // Remove the suffix
    }

    // Convert the numeric part to a double
    try {
        number = std::stod(cleaned);
    } catch (const std::invalid_argument& e) {
        throw std::invalid_argument("Invalid numeric part in input string.");
    } catch (const std::out_of_range& e) {
        throw std::out_of_range("Numeric part out of range.");
    }

    // Apply the suffix multiplier
    if (suffix == 'M') {
        number *= 1e6; // Million
    } else if (suffix == 'B') {
        number *= 1e9; // Billion
    } else if (suffix == 'K') {
        number *= 1e3; // Thousand
    }

    // Convert the result to sqlite3_int64
    if (number > std::numeric_limits<sqlite3_int64>::max() || number < std::numeric_limits<sqlite3_int64>::min()) {
        throw std::out_of_range("Converted number is out of range for sqlite3_int64.");
    }

    return static_cast<sqlite3_int64>(number);
}


extern "C" {

  /**
   * @brief Normalize a number in a human friendly condensed format and return a full int.
   */
  void normalize_number(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
      const char *subject = getSQLiteString(ctx, argv[0], "match", "subject");
      if (!subject) return;
      try {
        sqlite3_result_int64(ctx, convertToInt64(subject));
      } catch (const std::invalid_argument& e) {
        sqlite3_result_error(ctx, e.what(), -1);
      } catch (const std::out_of_range& e) {
        sqlite3_result_error(ctx, e.what(), -1);
      }
      return;
  }

  int sqlite3_extension_init(sqlite3 *db, char **err, const sqlite3_api_routines *api) {
    SQLITE_EXTENSION_INIT2(api)
    // Regular Expressions
    sqlite3_create_function(db, "normalize_number",  1, SQLITE_UTF8 | SQLITE_DETERMINISTIC, NULL, normalize_number, NULL, NULL);
    return 0;
  }
}
