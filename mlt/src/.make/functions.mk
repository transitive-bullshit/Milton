# @auth Travis Fischer
# @acct tfischer
# @date March 2008
# @site http://www.cs.brown.edu/people/tfischer/make
# @version 1.0
#
#    This file contains function definitions used by the Makefile subsytem.
ifndef __FUNCTIONS_MK__
__FUNCTIONS_MK__ := # Guard against multiple includes

include $(PROJECT_BASE_LIB)/common.mk

# The following public functions are defined in functions.mk:
# -----------------------------------------------------------
# functions.mk:
#   $(call ISNULL,c1)
#   $(call NOT,c1)
#   $(call ISNOTNULL,c1)
#   $(call EQUAL,c1,c2)
#   $(call NOTEQUAL,c1,c2)
#   $(call XOR,c1,c2)
#   $(call NOR,c1,c2)
#   $(call ASSERT,condition[,message])
#   $(call ASSERTEQ,c1,c2[,message])
#   $(call STORE,variable,value)
#   $(call ADD_UNIQUE_PREFIX,list,prefix)
#   $(call ESCAPE,str)
#   $(call CONVERT_TO_LOWERCASE,string)
#   $(call CONVERT_TO_UPPERCASE,string)
#   $(call MAKE_RECURSIVE,dirs[,flags])
#   $(call EXISTS,file)
#   $(call DOES_NOT_EXIST,file)
#   $(call DIR_EXISTS,dir)
#   $(call DIR_DOES_NOT_EXIST,dir)
#   $(call ENSURE_DIR_EXISTS,dir)
#   $(call GET_DIRS)
#   $(call GET_PROJECT_SUBDIRS)
#   $(call RECURSIVE_MAKE,project,target[,dependencies])
#   $(call TARGET_IS_UP_TO_DATE,target[,dir])
#   $(call SHOW_MESSAGE,"Message...","Prefix")
#   $(call WARNING,"Warning Message...")
#   $(call ERROR,"Error Message...")
#   $(call BFS,function,toInclude[,toExclude])
#   $(call EXTRACT_MK_INCLUDES,file)
#   $(call EXTRACT_INCLUDES,file)
#   $(call PRECEDES,c1,c2,list)
#   $(call INDEX_OF,item,list)
#   $(call LAST_INDEXOF,item,list)
#   $(call REMOVE_AT_INDEX,index,list)
#   $(call REMOVE_FIRST,item,list)
#   $(call REMOVE_LAST,item,list)
#   $(call REMOVE_ALL,item,list)
#   $(VA_ARGS_CONCAT)
#   $(VA_ARGS)
#   $(VA_ARGS_COUNT)
#   $(VA_ARGS_INDICES)
#   $(GET_ARGX)


# Boolean Logic Operators
# -----------------------
# Note:  The empty string is referred to as 'null' in the following definitions.
# These definitions should work fine with the builtin $(if), $(or), and $(and) 
# because they treat the empty string as the only false truth value as well.

# $(call ISNULL,c1)
#    Tests whether or not 'c1' is null
#
# @returns 'true' if 'c1' is null or null otherwise
ISNULL	= $(findstring $(1)true,$(or $(1),$(1)true))

# $(call NOT,c1)
#    Negates the logical value of 'c1'
#
# @returns 'true' if 'c1' is null or null otherwise
NOT		= $(call ISNULL,$(1))

# $(call ISNOTNULL,c1)
#    Tests whether or not 'c1' is not null
# 
# Note: this function is really not necessary but is included for completeness
# @returns true if 'c1' is non-null or null otherwise
ISNOTNULL= $(if $(1),true,)

# $(call EQUAL,c1,c2)
#    Tests 'c1' for equality with 'c2'
# 
# Note: if 'c1' and 'c2' are both null, 'true' will be returned 
#    (a non-null value because technically, 'c1' equals 'c2')
# @returns 'c1' if 'c1' exactly equals 'c2' or null otherwise
EQUAL		= $(or $(strip 													\
			  	  $(and $(findstring $(words $1),$(words $2)),		\
				     $(findstring $1,$2),$(findstring $2,$1))),		\
				  $(and $(call ISNULL,$1),$(call ISNULL,$2)))

# $(call NOTEQUAL,c1,c2)
#    Tests 'c1' for inequality with 'c2'
# 
# @returns non-null if 'c1' does not equal 'c2' or null otherwise
NOTEQUAL	= $(call EQUAL,$(1)success,$(or $(call EQUAL,$(1),$(2)),$(1)success))

# $(call XOR,c1,c2)
#    XORs the logical values of 'c1' and 'c2', true iff 'c1' and 'c2' have the 
# same truth value
#
# @returns 'true' if 'c1' XOR 'c2' non-null or null otherwise
XOR		= $(call EQUAL,$(call ISNULL,$(1)),$(call ISNULL,$(2)))

# $(call NOR,c1,c2)
#    NORs the values of 'c1' and 'c2', true iff 'c1' and 'c2' are both null
#
# @returns 'true' if 'c1' XOR 'c2' non-null or null otherwise
NOR		= $(and $(call NOT,$(1)),$(call NOT,$(2)))


# Assertion Routines built upon logical operators defined above
# -------------------------------------------------------------

# $(call ASSERT,condition[,message])
#    Asserts that 'condition' is not null (the empty string)
# 
# @returns 'condition' upon success or fails with a fatal error otherwise
ASSERT	= $(if $(1),$(1),$(error Assertion Failure: $(2)))

# $(call ASSERTEQ,c1,c2[,message])
#    Asserts that 'c1' exactly equals 'c2'
# 
# Note: ASSERTEQ should succeed if 'c1' and 'c2' are both null
# @returns 'c1' upon success or fails with a fatal error otherwise
ASSERTEQ	= $(call ASSERT,$(call EQUAL,$(1),$(2)),$(3))


# Miscellaneous useful utility functions
# --------------------------------------

# $(call STORE,variable,value)
#    Binds 'value' to a given 'variable' in an inline context. Used for 
# efficiency in places where 'value' needs to be referenced multiple times 
# but it may be expensive to compute.
#
# @returns 'value'
STORE = $(or $(eval $1 := $2),$($1))

# $(call ADD_UNIQUE_PREFIX,list,prefix)
#    Prefixes 'prefix' onto each item in 'list' which doesn't already start 
# with 'prefix'
#
# Ex.  $(call ADD_UNIQUE_PREFIX,-I. .. -Ifoo bar,-I) -> -I. -I.. -Ifoo -Ibar
#
# @returns a list where each element starts with 'prefix'
ADD_UNIQUE_PREFIX = $(foreach ARG,$(1),$(if $(ARG:$(2)%=),$(2)$(ARG),$(ARG)))

# $(call ESCAPE,str)
# 	  Escapes all forward slashes in the given string.
# (useful for escaping paths passed into regexps)
#
# @returns an escaped string
ESCAPE = $(subst /,\/,$(1))

# $(call CONVERT_TO_LOWERCASE,string)
#
# @returns a lowercase-only version of 'string'
CONVERT_TO_LOWERCASE	= $(shell echo "$(1)" | $(TR) 'A-Z' 'a-z')

# $(call CONVERT_TO_UPPERCASE,string)
#
# @returns an uppercase-only version of 'string'
CONVERT_TO_UPPERCASE	= $(shell echo "$(1)" | $(TR) 'a-z' 'A-Z')


# Utilities for working with Directories
# --------------------------------------

# $(call MAKE_RECURSIVE,dirs[,flags])
#    Recursively makes each directory in 'dirs', optionally adding 'flags' to 
# each invocation of $(MAKE)
#
# @returns a list of results from making each directory in 'dirs'
MAKE_RECURSIVE	= $(foreach DIR,$(1),$(MAKE) $(strip -C ($DIR) $(2)))

# $(call EXISTS,file)
#    Determines whether or not 'file' exists
#
# @returns "exists" if 'file' exists or null otherwise
EXISTS = $(shell [ -f $1 ] && echo "exists")

# $(call DOES_NOT_EXIST,file)
#    Determines whether a 'file' does not already exist
#
# @returns null if 'file' exists and a non-null value otherwise
DOES_NOT_EXIST = $(call ISNULL,$(call EXISTS,$1))

# $(call DIR_EXISTS,dir)
#    Determines whether or not 'dir' exists
#
# @returns "exists" if 'dir' exists or null otherwise
DIR_EXISTS = $(shell [ -d $1 ] && echo "exists")

# $(call DIR_DOES_NOT_EXIST,dir)
#    Determines whether 'dir' does not already exist
#
# @returns null if 'dir' exists and a non-null value otherwise
DIR_DOES_NOT_EXIST = $(call ISNULL,$(call DIR_EXISTS,$1))

# $(call ENSURE_DIR_EXISTS,dir)
#    Creates directory 'dir' and any necessary parent directories if they do not
# already exist.
#
# @returns void
ENSURE_DIR_EXISTS = $(CHK_DIR_EXISTS) $(1) || $(MKDIR) $(1)
ENSURE_DIR_EXISTS_SHELL = $(shell $(CHK_DIR_EXISTS) $(1) || $(MKDIR) $(1))

# $(call CLEAN_PATH,path)
#    Cleans the given path by removing extranneous markings.
#
# @returns a semantically equivalent (possibly cleaner) path
CLEAN_PATH	= $(subst //,/,$(subst ./,,$1))

# $(call GET_DIRS)
#
# @returns a comma separated list of subdirectories contained in the cwd
GET_DIRS = $(shell /bin/ls -p $1 | $(GREP) '/' | $(SED) 's|/| |' | $(TR) -d '\n')

# $(call GET_PROJECT_SUBDIRS)
#
# @returns a comma separated list of subdirectories contained in the cwd, all 
#    of which contain Makefiles
GET_PROJECT_SUBDIRS = $(strip $(foreach DIR,$(GET_DIRS),$(if $(shell [ -f $(DIR)/Makefile ] && echo "exists"),$(DIR))))

# $(call RECURSIVE_MAKE,project,[target,dependencies])
#    Defines a rule for 'target' which, when invoked, makes the 'all' target 
# of the given 'project'
# 
# Note: 'project' is the default value of 'target' if none is given.
#$(info RECURSIVE_MAKE: 1'$1' 2'$2' 3'$3')
#
# @returns text to be evaled to produce a new 'target' rule
define RECURSIVE_MAKE
$(if $2,$2,$1): $3
#	@echo "Recursively making: $1 $2 $2"
	+@$$(call SHOW_MESSAGE,,"Entering Project $(FR_COLOR_BLUE)$(notdir $1)$(END_COLOR)")
	+@$$(MAKE) -C $1 --no-print-directory;
	+@$$(call SHOW_MESSAGE,,"Leaving Project $(FR_COLOR_BLUE)$(notdir $1)$(END_COLOR)")
endef
#$$(info 1'$1' 2'$2' 3'$3' $(CURDIR))

ifndef STOP_RECURSION
# $(call TARGET_IS_UP_TO_DATE,target[,dir])
#    Checks if 'target' is up-to-date by silently invoking the Makefile 
# in the given 'dir'
#
# Note: 'dir' defaults to the current working directory if none is given
#
# @returns "up-to-date" if 'target' is up to date, "error" if attempting to 
# test the state of 'target' resulted in an error, or null otherwise
TARGET_IS_UP_TO_DATE = \
	$(and $(call DIR_EXISTS,$(if $2,$2,.)),$(or $(call NOT,$1),$(call EXISTS,$(if $2,$2,.)/$1)),\
   $(call STORE,UP,$(strip $(shell $(MAKE) -C $(if $2,$2,.) >& /dev/null --question $1 \
	-s --no-print-directory --silent STOP_RECURSION=true MODE=$(MODE); echo "$$?"))),\
   $(if $(call EQUAL,$(UP),0),up-to-date,$(if $(call EQUAL,$(UP),2),error)))

#$(or $(info UP='$(UP)'),
#TARGET_IS_UP_TO_DATE = $(or $(info TARGET_IS_UP_TO_DATE 1'$1' 2'$2' ret='$(call TARGET_IS_UP_TO_DATE2,$1,$2)'),
#	$(call TARGET_IS_UP_TO_DATE2,$1,$2))
#$(if $(call CONTAINS,up-to-date,$(UP)),up-to-date))))

#$(or $(info TARGET_IS_UP_TO_DATE 1'$1' 2'$2'),

# $(call GET_LIST_OF_TARGETS[,dir])
#    Obtains a list of user-defined targets defined in the Makefile in 'dir'
#
# Note: 'dir' defaults to the current working directory if none is given
#
# Summary of options passed to make
# -p -> --print-data-base
# -s -> --silent
# -f -> --file
# -r -> --no-builtin-rules
# -R -> --no-builtin-variables
#
#  @returns a whitespace-separated list of targets available (or null if an 
#  error occurred)
GET_LIST_OF_TARGETS = $(strip $(if $(call EXISTS,$(if $1,$1,.)/Makefile),\
	$(sort $(shell $(MAKE) $(if $1,-C $1) -psnrR \
	--no-print-directory STOP_RECURSION=true MODE=$(MODE) | \
	$(GREP) -o '^[^$(POUND_SIGN).][a-zA-Z0-9.]*:[^=]' | $(GREP) -v '$$2' | \
	$(SED) 's/:/ /g' | $(TR) -d '\n'))))
endif


# Logging Utility Functions
# -------------------------

RED			= 1
GREEN			= 2
YELLOW		= 3
BLUE			= 4
PURPLE		= 5
LIGHT_BLUE	= 6
LIGHT_TAN	= 7

COLORS			= RED GREEN YELLOW BLUE PURPLE LIGHT_BLUE LIGHT_TAN
BOLD_COLOR		= \033[1m
REVERSE_COLOR	= \033[7m
END_COLOR		= \033[0m
CECHO				= echo -e

define DEFINE_COLOR
   FR_COLOR_$1 = \033[3$$($1)m
   BR_COLOR_$1 = \033[4$$($1)m
endef

$(foreach COLOR,$(COLORS),$(eval $(call DEFINE_COLOR,$(COLOR))))

EMPHASIS	= $(shell $(CECHO) "$(BOLD_COLOR)$1$(END_COLOR)")

# $(call SHOW_MESSAGE,"Message...","Prefix")
#    Prints a very noticeable message to stdout.
# 
# @returns void
define SHOW_MESSAGE
   $(CECHO) $2 >  $(TEMP_FILE)
   $(CECHO) $1 >> $(TEMP_FILE)
   $(CAT) $(TEMP_FILE) | $(TR) -d '\n' > $(TEMP_FILE)2
   echo "" >> $(TEMP_FILE)2
   
   echo $2 >  $(TEMP_FILE)
   echo $1 >> $(TEMP_FILE)
   $(CAT) $(TEMP_FILE) | $(TR) -d '\n' > $(TEMP_FILE)3
   echo "" >> $(TEMP_FILE)3
   
# Generate a string of dashes to display before and after the message
   $(SED) -e 's/\\033\[[0-9]*m//g' -e 's/./-/g' $(TEMP_FILE)3 > $(TEMP_FILE)
   echo
   $(CAT) $(TEMP_FILE)		# ----------
   $(CAT) $(TEMP_FILE)2		# Message...
   $(CAT) $(TEMP_FILE)		# ----------
   echo
endef

# $(call WARNING,"Warning Message...")
#    Prints a very noticeable warning message to stdout.
#
# @returns void
WARNING	= $(call SHOW_MESSAGE,$(1),"$(FR_COLOR_RED)Warning:$(END_COLOR) ")

# $(call ERROR,"Error Message...")
#    Prints a very noticeable error message to stdout.
#
# @returns void
ERROR		= $(call SHOW_MESSAGE,$(1),"$(FR_COLOR_RED)Error:$(END_COLOR) ")

# Force shell to exit on error, preventing corruption in multi-line commands
EXIT_ON_ERROR	= set -e;


# Arithmetic
# ----------
BASE     	:= $(SPACE)0 1 2 3 4 5 6 7 8 9

# 1 2 3 4 ... 999
NUMBERS		:= $(shell echo \
	"$(foreach i,$(BASE),$(foreach j,$(BASE),$(addprefix $i$j,$(BASE))))" | \
   $(SED) -e 's/ 00/ /g' -e 's/ 0/ /g' -e 's/000 //')

NUMBERS_LEN := $(words $(NUMBERS))
#$(info $(NUMBERS))

ADD			= $(word $(2),$(wordlist $(1),$(NUMBERS_LEN),$(wordlist 2,$(NUMBERS_LEN),$(NUMBERS))))
INC			= $(call ADD,1,$(1))


# -----------------------
# Crazy VA_ARGS routines!
# -----------------------

# Escaping utilities at increasing depths (used heavily in the code below)
# These utilities defer makefile substitution until later evaluation.
D = $$
DD = $DD
DDD = $DDD
DDDD = $DDDD
DDDDD = $DDDDD
DDDDDD = $DDDDDD
DDDDDDD = $DDDDDDD
DDDDDDDD = $DDDDDDDD
DDDDDDDDD = $DDDDDDDDD
DDDDDDDDDD = $DDDDDDDDDD

# $(VA_ARGS_CONCAT)
#    Expands to a whitespace-separated list of arguments, where each argument 
# is a single word with any possible whitespace replaced by $(CONCAT_STR).
# (because individual arguments may contain whitespace themselves)
#
# Note: assumes caller is in the context of a function and should return an 
# empty string if that is not the case.
#
# @returns a whitespace-separated list of concatenated arguments to the 
# current function
$(eval VA_ARGS_CONCAT   = $D(filter-out $(SPACE),$(foreach ARG,$(NUMBERS)	\
   ,$D(subst $D(SPACE),$(CONCAT_STR),$D$(OPEN_PAREN)$(ARG)$(END_PAREN)))))

# $(VA_ARGS)
#    Expands to a whitespace-separated list of arguments. Because individual 
# arguments may contain whitespace themselves, the length of $(VA_ARGS) may 
# not equal the number of actual arguents. Use $(VA_ARGS_CONCAT) if this is 
# the case.
#
# Note: assumes caller is in the context of a function and should return an 
# empty string if that is not the case.
#
# @returns a whitespace-separated list of arguments to the current function
$(eval VA_ARGS          = $D(filter-out $(SPACE),$(foreach ARG,   \
   $(NUMBERS),$D$(OPEN_PAREN)$(ARG)$(END_PAREN))))

# $(VA_ARGS_COUNT)
#    Expands to an integer number describing the number of arguments passed 
# into the current function.
#
# Note: assumes caller is in the context of a function and should return zero 
# if that is not the case.
# 
# @returns an integer [0...$(NUMBERS_LEN)]
$(eval VA_ARGS_COUNT    = $D(words $D(VA_ARGS_CONCAT)))

# $(VA_ARGS_INDICES)
#    Expands to a whitespace-separated list of integer indices corresponding 
# to the arguments passed into the current function. Useful for referencing 
# all of the automatic temporary variables $1,$2,... within a foreach loop in 
# the context of a function.
#
# Note: assumes caller is in the context of a function and should return an 
# empty string if that is not the case.
#
# @returns a whitespace-separated list of indices corresponding to arguments 
# passed to the current function
$(eval VA_ARGS_INDICES  = $D(wordlist 1,$D(VA_ARGS_COUNT),$D(NUMBERS)))

# $(GET_ARGX)
#    Utility to reference the value of argument 'X' in the context of a 
# function when 'X' is not necessarily known beforehand (an item from invoking 
# VA_ARGS_INDICES, for instance).
#
# Note: assumes caller is in the context of a function and should return null 
# if that is not the case.
#
# @returns the value of the 'X'th argument passed to the current function
$(foreach NUM,$(NUMBERS),$(eval GET_ARG$(NUM) = $D$(OPEN_PAREN)$(NUM)$(END_PAREN)))
#	$D(subst $(CONCAT_STR),$(SPACE),$D(word $(NUM),$D(VA_ARGS_CONCAT)))))


# Scheme-like recursive list interpretation
FIRST	= $(word 1,$1) # also $(firstword $1)
SECOND	= $(word 2,$1)
NEXT	= $(SECOND)
THIRD	= $(word 3,$1)
FOURTH	= $(word 4,$1)
REST	= $(wordlist 2,$(words $1),$1)

# MAP : lambda(x) listof(x) -> listof(lambda(x))
# 	  Maps a one argument function over the given list
MAP1	= $(foreach item,$(2),$(call $(1),$(item)))

# MAP2 : lambda(x,y) listof(x) listof(y) -> listof(lambda(x,y))
# 	  Maps a two argument function over the given list
MAP2	= $(if $(and $(call ASSERTEQ,$(call ISNULL,$(call FIRST,$(2))),\
		  $(call ISNULL,$(call FIRST,$(3)))),$(call ISNULL,$(call FIST,$(2)))),\
				$(NULL),\
				$(call $(1),$(call FIRST,$(2)),$(call FIRST,$(3))) \
				$(call MAP2,$(1),$(call REST,$(2)),$(call REST,$(3))))


# MAPLIST : lambda(x y ...) listof(x) listof(y)... -> listof(lambda(x y ...))
#    Maps a 1-argument function over N lists, where the 1-argument function 
# should be expecting N arguments separated by whitespace within its first 
# argument.
#
# Note: the function over which you are mapping may not have normal functional 
#    syntax: it should be a one argument function which assumes that all args 
#    are distinct, whitespace-separated elements of that first argument. If 
#    this is not the functionality you are looking for, try the more generic 
#    'MAP' function instead.
#
# Note: it is assumed that N > 0
# Note: it is assumed that each of the input lists have the same length N.
# If they do not, then N will default to the first argument list's length.
#
# @returns an N-element result list
$(eval MAPLIST = $D(foreach SUBARGINDEX,\
   $D(wordlist 1,$D(words $D(GET_ARG2)),$D(NUMBERS)),$(strip\
   $D(call $D1,$D(foreach ARG,$D(wordlist 2,$D(NUMBERS_LEN),\
   $D(VA_ARGS_INDICES)),$D(word $D(SUBARGINDEX),$D(GET_ARG$D(ARG))))))))

# 'MAP' helper function (not of much use by itself)
$(eval ARGUMENTS = $D(shell echo '$D(foreach I,$D(wordlist 2,$D(NUMBERS_LEN),$D(NUMBERS)),$DD(word $DD(SUBARGINDEX),$$(D)$D(OPEN_PAREN)$D(I)$D(END_PAREN)))' | $(SED) 's/$D(END_PAREN) /$D(END_PAREN),/g'))
#$(eval ARGS = $D(shell echo '$D(foreach I,$D(wordlist 2,$D(NUMBERS_LEN),$D(NUMBERS)),$$(D)$D(OPEN_PAREN)$D(I)$D(END_PAREN))' | $(SED) 's/ /,/g'))

# MAP : lambda(x,y,...) listof(x) listof(y)... -> listof(lambda(x,y,...))
#    Maps an N-argument function over N lists
# 
# Note: it is assumed that N > 0
# Note: it is assumed that each of the input lists have the same length N.
# If they do not, then N will default to the first argument list's length.
#
# Ex. $(call MAP,EQUAL,SAME1 SAME2 foo,SAME1 SAME2 bar) -> 'SAME1 SAME2 '
# Ex. $(call MAP,ASSERTEQ,SAME1 SAME2 foo,SAME1 SAME2 bar) -> fatal error
#
# @returns an N-element result list
$(eval MAP = $D(foreach SUBARGINDEX,\
   $D(wordlist 1,$D(words $D2),$D(NUMBERS)),$D(strip\
   $D(call $D1,$(ARGUMENTS)))))


#PRINT = $(info 1'$1'  2'$2'  3'$3')
#STATUS := $(info $(call MAP,EQUAL,2a 2b 2c,3a 2b 3c))
#STATUS := $(call ASSERTEQ,$(call MAP,EQUAL,2a 2b 2c,3a 2b 3c), 2b )
#STATUS := $(call MAP,ASSERT,$(call MAP,EQUAL,2a 2b 2c,2a 2b 2c))


# String Utility Functions
# ------------------------

# $(call PRECEDES,c1,c2,list)
#    Determines whether or not the first appearance of 'c1' occurs before the 
# first appearance of 'c2' in 'list'
#
# @returns 'true' if 'c1' appears before 'c2' in 'list' or null otherwise
PRECEDES = $(and $(call STORE,PRECEDES_NUM,false),\
	$(foreach NUM,$(wordlist 1,$(words $3),$(NUMBERS)),\
		$(if $(and $(call EQUAL,$1,$(word $(NUM),$3)),$(call EQUAL,$(PRECEDES_NUM),false)),\
			$(call STORE,PRECEDES_NUM,true),\
			$(if $(and $(call EQUAL,$2,$(word $(NUM),$3)),$(call EQUAL,$(PRECEDES_NUM),false)),\
			$(call STORE,PRECEDES_NUM,false2)))),$(call EQUAL,true,$(PRECEDES_NUM)))

# $(call CONTAINS,c1,list)
#    Determines whether or not 'c1' appears exactly in 'list'
#
# @returns 'true' if 'c1' appears exactly in 'list' or null otherwise
CONTAINS = $(strip $(foreach ITEM,$2,$(call EQUAL,$1,$(ITEM))))

# $(call INDEX_OF,item,list)
#
# @returns the one-based index of the first occurrence of 'item in 'list'
INDEX_OF = $(and 																					\
$(or $(call STORE,IND_N,),$(foreach NUM,$(wordlist 1,$(words $2),$(NUMBERS)),	\
	$(if $(and $(call EQUAL,$1,$(word $(NUM),$2)),$(call EQUAL,$(IND_N),)),		\
	$(call STORE,IND_N,$(NUM))))),$(IND_N))

# $(call LAST_INDEXOF,item,list)
#
# @returns the one-based index of the last occurrence of 'item in 'list'
LAST_INDEX_OF = $(and 																			\
$(or $(call STORE,IND_N,),$(foreach NUM,$(wordlist 1,$(words $2),$(NUMBERS)),	\
	$(if $(call EQUAL,$1,$(word $(NUM),$2)),												\
	$(call STORE,IND_N,$(NUM))))),$(IND_N))

# $(call REMOVE_AT_INDEX,index,list)
#    Removes the item at the specified 'index' in 'list'.
# 
# Note: this function (and all GNU Make functions) assume indexing starts 
#    at one.
# Note: if 'index' equals zero, 'list' is returned as-is, and if 'index' is 
#    greater than the max index at the end of 'list', 'index' will be 
#    truncated and the last item in 'list' will be removed.
# See also: 'REMOVE_FIRST', 'REMOVE_LAST'
#
# @returns a modified copy of 'list' with at most one item removed
REMOVE_AT_INDEX = $(strip $(if $(call EQUAL,$1,0),$2,\
	$(if $(call EQUAL,$1,1),$(wordlist 2,$(words $2),$2), \
		$(if $(call EQUAL,$1,$(words $2)),$(firstword $2) \
			$(call REMOVE_AT_INDEX,$(words $(call STORE,N_REMOVE,\
			$(wordlist 2,$(words $2),$2))),$(N_REMOVE)),\
		$(call REMOVE_AT_INDEX,$(words $(wordlist 1,$1,$2)),$(wordlist 1,$1,$2)) \
		$(call REMOVE_AT_INDEX,1,$(wordlist $1,$(words $2),$2))))))

# $(call REMOVE_FIRST,item,list)
#    Removes the first occurrence of 'item' in 'list'.
# 
# Note: if 'item' does not appear in 'list, 'list' is returned as-is
# See also: 'REMOVE_AT_INDEX', 'REMOVE_LAST', 'REMOVE_ALL'
#
# @returns a modified copy of 'list' with at most one item removed
REMOVE_FIRST = $(call REMOVE_AT_INDEX,$(call INDEX_OF,$1,$2),$2)

# $(call REMOVE_LAST,item,list)
#    Removes the last occurrence of 'item' in 'list'.
# 
# Note: if 'item' does not appear in 'list, 'list' is returned as-is
# See also: 'REMOVE_AT_INDEX', 'REMOVE_FIRST', 'REMOVE_ALL'
#
# @returns a modified copy of 'list' with at most one item removed
REMOVE_LAST = $(call REMOVE_AT_INDEX,$(call LAST_INDEX_OF,$1,$2),$2)

# $(call REMOVE_ALL,item,list)
#    Removes all occurrences of 'item' in 'list'.
# 
# Note: if 'item' does not appear in 'list, 'list' is returned as-is
# See also: 'REMOVE_AT_INDEX', 'REMOVE_FIRST', 'REMOVE_LAST'
#
# @returns a modified copy of 'list' all occurrences of 'item' removed
REMOVE_ALL	= $(filter-out $1,$2)


# Helper routine for BFS which only calls the given function $1 if its argument
# $2 has not already been marked by appearing in list $3
BFS_MARK = $(if $(call CONTAINS,$2,$3),,$(call $1,$2))
#$(or $(info 1'$1' 2'$2' 3'$3'),...)

# $(call BFS,function,toInclude[,toExclude])
#    Recursively processes each item in the 'toInclude' list, calling the 
# given one-argument 'function' on each item. 'function' should return a new 
# member of 'toExclude' after processing its input. Items in 'toInclude' will 
# only be processed by 'function' if they are not present in the 'toExclude' 
# list.
#
# Note: the 'toExclude' list represents "marked" nodes in a traditional BFS, 
# and the 'toInclude' list represents a queue or nodes that have yet to be 
# processed.
#
# @returns a list of all processed items
BFS = $(sort $2 $(if $(call STORE,NEXT_BFS,$(foreach ARG,$2,$(call BFS_MARK,$1,$(ARG),$3))),$(call BFS,$1,$(NEXT_BFS),$2 $3)))

# $(call EXTRACT_MK_INCLUDES,file)
#    Returns a list of filenames included from 'file' using GNU Make syntax.
#
# Ex usage:
#    $(call BFS,EXTRACT_MK_INCLUDES,Makefile)
#
# Note: the last argument 'toExclude' to BFS was left off in this example 
# because no nodes need to be marked at the beginning of the search.
# @returns list of filenames
EXTRACT_MK_INCLUDES = $(shell $(GREP) '^[ 	]*include ' $1 | $(SED) 's/^[ 	]*include \([^\n]*\)/\1 /g' | $(TR) -d '\n')

#$(info $(call BFS,EXTRACT_INCLUDES,Makefile))

# $(call EXTRACT_INCLUDES,file)
#    Returns a list of filenames included from 'file' using C/C++ syntax.
#
# Ex usage:
#    $(call BFS,EXTRACT_INCLUDES,main.C)
#
# Note: the last argument 'toExclude' to BFS was left off in this example 
# because no nodes need to be marked at the beginning of the search.
# @returns list of filenames
EXTRACT_INCLUDES = $(if $(findstring $(QT_DIR),$1),,\
	$(shell $(GREP) '^[ 	]*$(POUND_SIGN)include ' $1 | $(GREP) -v '$(QT_DIR)' | $(SED) -e 's/^[ 	]*$(POUND_SIGN)include \$(OPEN_PAREN).*\$(END_PAREN)/\1 /' -e 's/[<>"]//g' | $(TR) -d '\n'))

#$(info $(call BFS,EXTRACT_INCLUDES,main.C))

GET_SUB_DIRS 			= $(strip $(foreach DIR,$(call GET_DIRS,$1),$(if $(strip $(foreach SUFFIX,C cpp cc c H h,$(wildcard $1/$(DIR)/*.$(SUFFIX)))),$1/$(DIR))))

#$(info first:'$(call GET_SUB_DIRS,.)')
#$(info SUBDIRS:'$(call BFS,GET_SUB_DIRS,.,..)')

EXTRACT_QT_OBJECTS 	= \
   $(strip $(shell $(GREP) -l 'Q_OBJECT' $1 | $(TR) '\n' ' '))

GET_QT_INCLUDES		= \
   $(shell /bin/ls /course/cs123/qt/include/Qt* | grep -v ':' | tr '\n' ' ')

DOXYFILE_PROPERTIES 	=				\
   PROJECT_NAME 						\
   OUTPUT_DIRECTORY 					\
   EXTRACT_ALL 						\
   TAB_SIZE 							\
   HTML_OUTPUT							\
   GENERATE_LATEX						\
   RECURSIVE							\
   EXCLUDE								\
   EXCLUDE_SYMLINKS					\
   SHOW_DIRECTORIES					\
   EXCLUDE_PATTERNS
#   ALIASES

DOXYFILE_VALUES	 	= 				\
   $(call CONVERT_TO_UPPERCASE,$(TARGET))	\
   $(OUT_DIR)$(BACKSLASH)/doc		\
   YES									\
   4										\
   \.\/									\
   NO										\
   $(if $(or $(call EQUAL,$(PROJECT_SRC_DIRS),ALL),$(call EQUAL,$(TARGET_TYPE),HIERARCHY)),YES,NO) \
   $(PROJECT_BASE_LIB) 				\
	YES									\
	YES									\
	*/.bin/*\t*/.svn/*\t*/.cvs/*\t*/.CVS/*\t*/.git/*\t*/.hg/*\t*/.snapshot/*

#	"cite=$(BACKSLASH)par$(SPACE)Referenced$(SPACE)Code:$(BACKSLASH)$(BACKSLASH)n$(SPACE)created=$(BACKSLASH)par$(SPACE)Created:$(BACKSLASH)$(BACKSLASH)n$(SPACE)edited=$(BACKSLASH)par$(SPACE)Last$(SPACE)modified:$(BACKSLASH)$(BACKSLASH)n$(SPACE)maintainer=$(BACKSLASH)par$(SPACE)Maintainer:$(BACKSLASH)$(BACKSLASH)n$(SPACE)units=$(BACKSLASH)par$(SPACE)Units:$(BACKSLASH)$(BACKSLASH)n"

DOXYFILE					= Doxyfile

# Create the template, surpressing Doxygen's usual output
# Overwrite specific properties with our own values
define CREATE_DOXYFILE
   $(shell doxygen -g $(DOXYFILE) > /dev/null)
   $(info $(SED) -i $(foreach INDEX,$(wordlist 1,$(words $(DOXYFILE_PROPERTIES)),$(NUMBERS)), 
      -e "s|\(^$(word $(INDEX),$(DOXYFILE_PROPERTIES)) *=\).*|\1 $(word $(INDEX),$(DOXYFILE_VALUES))|g") $(DOXYFILE))
   $(shell $(SED) -i $(foreach INDEX,$(wordlist 1,$(words $(DOXYFILE_VALUES)),$(NUMBERS)), 
      -e "s|\(^$(word $(INDEX),$(DOXYFILE_PROPERTIES)) *=\).*|\1 $(word $(INDEX),$(DOXYFILE_VALUES))|g") $(DOXYFILE))
endef

DOXY_OUTPUT_DIRECTORY = \
   $(shell $(GREP) "^OUTPUT_DIRECTORY" $(DOXYFILE) | $(SED) "s/^OUTPUT_DIRECTORY.*= \(.*$$\)/\1/")

# Ensure that we have the correct version of GNU Make
STATUS := $(if $(call NOTEQUAL,$(MAKE_VERSION),3.81), \
	$(info Unknown version of GNU Make detected; expected v3.81, found v$(MAKE_VERSION)))

endif # __FUNCTIONS_MK__

