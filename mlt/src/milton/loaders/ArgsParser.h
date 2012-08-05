#pragma once

/* A simple generic file input parser.
 *
 * It assumes that the format of the file will be:
 *    PARAM_NAME1: value
 *	   PARAM_NAME2: value
 *
 * All arguments should be on a seperate line. All parameter names must be unique.
 *
 * The implementation is simple and naive - it has running time NxM where N
 * is the number of requested arguments and M is the number of lines in the file.
 * This should not be used for anything time consuming.
 *
 * Boolean arguments should have the value of either "true" or "false".
 *
 * Original Author: Nong Li (2007)
 */

#define ARGS_PARSER_ERROR_NOFILE -1
#define ARGS_PARSER_ERROR_INVALIDFILE -2
#define ARGS_PARSER_ERROR_NOMEMORY -3
#define ARGS_PARSER_ERROR_REQUIREDPARAMETER -4
#define ARGS_PARSER_ERROR_UNSPECIFIEDPARAMETER -5

#include <string>
#include <map>

namespace milton {

class ArgsParser {
   public:

      static void test();

      ArgsParser(void);
      ~ArgsParser(void);

      /* Returns 0 on success and an error if it failed.
       * There are a few #define errors at the top of this file.
       */
      int parse(const char* file);

      /* Sets the maximum length of the parameter name. Uses default if not specified.
      */
      void setMaxParameterNameLength(unsigned int length);

      /* Sets the maximum length of the parameter value. Uses default if not specified.
      */
      void setMaxParameterValueLength(unsigned int length);

      /* Sets whether the parser should ignore fields that have not been specified.
       * If ignore is set to true - parser will not report an error on parse.
       * If ignore is set to false - parser will continue to parse values it can but
       * error on parse.
       * Default value is false.
       */
      void ignoreUnspecifiedParams(bool ignore);

      /* Functions to configure the parser.  For all the functions below,
       * the first parameter is the exact (case-sensitive) name for the argument,
       * the second parameter is the value that will be assigned upon parsing and
       * the optional third parameter specifies whether that value is required
       * in the file.
       *
       */
      void addBoolArg(const char* argName, bool& value, bool required=false); 
      void addCharArg(const char* argName, char& value, bool required=false);
      void addIntArg(const char* argName, int& value, bool required=false);
      void addFloatArg(const char* argName, float& value, bool required=false);
      void addStringArg(const char* argName, std::string& value, bool required=false); 

   private:
      std::map<std::string, bool*> boolArgs;
      std::map<std::string, char*> charArgs;
      std::map<std::string, int*> intArgs;
      std::map<std::string, float*> floatArgs;
      std::map<std::string, std::string*> stringArgs;

      std::map<std::string, bool> requiredParams;

      unsigned int m_maxNameLength;
      unsigned int m_maxValueLength;
      char* m_stringBuffer;
      bool m_ignoreUnspecified;
      FILE* m_file;

      bool assignBool(const char* name);
      bool assignChar(const char* name);
      bool assignInt(const char* name);
      bool assignFloat(const char* name);
      bool assignString(const char* name);

      char* stripWhiteSpace(char* buffer);

      void error(const char* name, const char* value);

};

}

