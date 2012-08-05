#include "ArgsParser.h"
#include <common.h>
#include <string.h>

#define DEFAULT_MAX_NAMELENGTH 256
#define DEFAULT_MAX_VALUELENGTH 1024

// disable warning to use secure stdio.h functions
#ifdef MILTON_ARCH_WINDOWS
#  pragma warning(disable : 4996)
#endif

namespace milton {

void ArgsParser::test() {
   ArgsParser parser;

   bool boolValue1, boolValue2;
   float floatValue1, floatValue2;
   std::string string1, string2;

   int i;
   char c;

   parser.addCharArg("char", c);
   parser.addIntArg("int", i);

   parser.addBoolArg("bool3", boolValue1, true);

   parser.addBoolArg("bool1", boolValue1);
   parser.addBoolArg("bool2", boolValue2);

   parser.addFloatArg("float1", floatValue1);
   parser.addFloatArg("float2", floatValue2);

   parser.addStringArg("string1", string1);
   parser.addStringArg("string2", string2);

   int ret = parser.parse("c:\\data\\parserTestFile.txt");
   printf("%d\n", boolValue1);
   printf("%d\n", boolValue2);
   printf("%f\n", floatValue1);
   printf("%f\n", floatValue2);
   printf("%s\n", string1.c_str());
   printf("%s\n", string2.c_str());
   printf("%c\n", c);
   printf("%d\n", i);

   printf("Ret: %d\n", ret);
}

ArgsParser::ArgsParser(void) {
   m_maxNameLength = DEFAULT_MAX_NAMELENGTH;
   m_maxValueLength = DEFAULT_MAX_VALUELENGTH;
   m_ignoreUnspecified = false;
}

ArgsParser::~ArgsParser(void) {
}

int ArgsParser::parse(const char* filename) {
   m_file = NULL;
   char* buffer = NULL;

   if (NULL == (m_file = fopen(filename, "r"))) {
      return ARGS_PARSER_ERROR_NOFILE;
   }

   buffer = new char[m_maxNameLength];
   if (buffer == NULL) {
      return ARGS_PARSER_ERROR_NOMEMORY;
   }

   m_stringBuffer = new char[m_maxValueLength];
   if (m_stringBuffer == NULL) {
      delete[] buffer;
      return ARGS_PARSER_ERROR_NOMEMORY;
   }

   int error = 0;

   while (!feof(m_file)) {
      bool found = false;

      /* Extract the name of the parameter */
      if (fscanf(m_file, "%s:", buffer) == -1)
         break;
      char *buf = stripWhiteSpace(buffer);

      /* Strip the last : */
      if (buf[strlen(buf) - 1] == ':')
         buf[strlen(buf) - 1] = '\0';

      /* Try to match with all of the types */
      if (!found)
         found = assignBool(buf);
      if (!found)
         found = assignChar(buf);
      if (!found)
         found = assignInt(buf);
      if (!found)
         found = assignFloat(buf);
      if (!found)
         found = assignString(buf);

      /* If not found - check the error type and do some clean up */
      if (!found) {
         /* This is relatively low priority - only assign if no error was there before */
         if (!m_ignoreUnspecified && !error)
            error = ARGS_PARSER_ERROR_UNSPECIFIEDPARAMETER;

         /* Clean up trailing newlines */
         fgets(buf, DEFAULT_MAX_NAMELENGTH, m_file);
      }
      /* Take it out of the required buffer */
      else {
         std::map<std::string, bool>::iterator iter;
         iter = requiredParams.find(buf);
         if (iter != requiredParams.end()) {
            requiredParams[buf] = true;
         }
      }
   }

   /* Check for required */
   std::map<std::string, bool>::iterator iter = requiredParams.begin();
   for (; iter != requiredParams.end(); iter++) {
      if (iter->second == false) {
         error = ARGS_PARSER_ERROR_REQUIREDPARAMETER;
         break;
      }
   }

   fclose(m_file);
   delete[] buffer;
   delete[] m_stringBuffer;
   return error;
}

bool ArgsParser::assignBool(const char* name) {
   /* See if the map contains the name */
   std::map<std::string, bool*>::iterator iter;
   iter = boolArgs.find(name);
   if (iter == boolArgs.end())
      return false;

   /* Parse the file for the value */
   char buffer[DEFAULT_MAX_NAMELENGTH];
   char* buf;

   fgets(buffer, DEFAULT_MAX_NAMELENGTH, m_file);
   //  fscanf(m_file, "%s", buffer);
   buf = stripWhiteSpace(buffer);

   if (!strcmp(buf, "true")) {
      *(iter->second) = true;
   }
   else if (!strcmp(buf, "false")) {
      *(iter->second) = false;
   }
   else {
      error(name, buf);
   }

   return true;
}

bool ArgsParser::assignChar(const char* name) {
   /* See if the map contains the name */
   std::map<std::string, char*>::iterator iter;
   iter = charArgs.find(name);
   if (iter == charArgs.end())
      return false;

   /* Parse the file for the value */
   fgets(m_stringBuffer, DEFAULT_MAX_NAMELENGTH, m_file);
   char* buf = stripWhiteSpace(m_stringBuffer);
   *(iter->second) = buf[0];

   return true;
}

bool ArgsParser::assignInt(const char* name) {
   /* See if the map contains the name */
   std::map<std::string, int*>::iterator iter;
   iter = intArgs.find(name);
   if (iter == intArgs.end())
      return false;

   /* Parse the file for the value */
   int value;
   fscanf(m_file, "%d", &value);
   *(iter->second) = value;

   return true;
}

bool ArgsParser::assignFloat(const char* name) {
   /* See if the map contains the name */
   std::map<std::string, float*>::iterator iter;
   iter = floatArgs.find(name);
   if (iter == floatArgs.end())
      return false;

   /* Parse the file for the value */
   float value;
   fscanf(m_file, "%f", &value);
   *(iter->second) = value;

   return true;
}

bool ArgsParser::assignString(const char* name) {
   /* See if the map contains the name */
   std::map<std::string, std::string*>::iterator iter;
   iter = stringArgs.find(name);
   if (iter == stringArgs.end())
      return false;

   /* Parse the file for the value */
   fgets(m_stringBuffer, DEFAULT_MAX_NAMELENGTH, m_file);
   char* buf = stripWhiteSpace(m_stringBuffer);

   *(iter->second) = buf;

   return true;
}

char* ArgsParser::stripWhiteSpace(char* buffer) {
   while (*buffer == ' ' || *buffer == '\n' || 
          *buffer == '\t' || *buffer == '\r') {
      buffer++;
   }

   size_t len = strlen(buffer);

   while ( buffer[len - 1] == ' ' || buffer[len - 1] == '\n' || 
           buffer[len - 1] == '\t' || buffer[len - 1] == '\r') {
      buffer[len - 1] = '\0';
      len--;
   }
   return buffer;
}

void ArgsParser::error(const char*, const char*) {
}

void ArgsParser::addBoolArg(const char* argName, bool &value, bool required) {
   boolArgs[argName] = &value;
   if (required) {
      requiredParams[argName] = false;
   }
}
void ArgsParser::addCharArg(const char* argName, char &value, bool required) {
   charArgs[argName] = &value;
   if (required) {
      requiredParams[argName] = false;
   }
}
void ArgsParser::addIntArg(const char* argName, int &value, bool required) {
   intArgs[argName] = &value;
   if (required) {
      requiredParams[argName] = false;
   }
}
void ArgsParser::addFloatArg(const char *argName, float &value, bool required) {
   floatArgs[argName] = &value;
   if (required) {
      requiredParams[argName] = false;
   }
}
void ArgsParser::addStringArg(const char *argName, std::string &value, bool required) {
   stringArgs[argName] = &value;
   if (required) {
      requiredParams[argName] = false;
   }
}

void ArgsParser::setMaxParameterNameLength(unsigned int length) {
   m_maxNameLength = length;
}
void ArgsParser::setMaxParameterValueLength(unsigned int length) {
   m_maxValueLength = length;
}
void ArgsParser::ignoreUnspecifiedParams(bool ignore) {
   m_ignoreUnspecified = ignore;
}

}

