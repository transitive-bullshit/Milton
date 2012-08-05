# @auth Travis Fischer
# @acct tfischer
# @date March 2008
# @site http://www.cs.brown.edu/people/tfischer/make
# @version 1.0
#
#    This file contains unit tests for the Makefile subsystem, as well as 
# old/unused definitions and rules
ifndef __UNITTESTS_MK__
__UNITTESTS_MK__ := # Guard against multiple includes


include $(PROJECT_BASE_LIB)/defines.mk
include $(PROJECT_BASE_LIB)/targets.mk

VALID_ASSERTION_STR		:= "Valid Assertion Failed -- you should NOT see this message!"
INVALID_ASSERTION_STR	:= "Invalid Assertion Passed -- you should NOT see this message!"

TEST_NULL_ASSERT			 = $(call ASSERT,$(call NOT,$(1)),$(INVALID_ASSERTION_STR))
TEST_NULL_ASSERTEQ		 = $(call ASSERT,$(call NOTEQUAL,$(1),$(2)),$(INVALID_ASSERTION_STR))

# Test main routines used in unit tests: ASSERT, ASSERTEQ, EQUAL, NOTEQUAL
STATUS	:= $(call ASSERT,xyz,$(VALID_ASSERTION_STR))
STATUS	:= $(call ASSERTEQ,a,a,$(VALID_ASSERTION_STR))
STATUS	:= $(call ASSERTEQ,a b c,a b c,$(VALID_ASSERTION_STR))
STATUS	:= $(call ASSERT,$(call EQUAL,x y,x y),$(VALID_ASSERTION_STR))
STATUS	:= $(call ASSERTEQ,$(call EQUAL,x y,x y),x y,$(VALID_ASSERTION_STR))
STATUS	:= $(call ASSERT,$(call NOTEQUAL,a,b),$(VALID_ASSERTION_STR))

# The following tests should fail
STATUS	:= $(call TEST_NULL_ASSERT,)
STATUS	:= $(call TEST_NULL_ASSERTEQ,a b c,a b c d)
STATUS	:= $(call TEST_NULL_ASSERTEQ,a b c,a b d)


# Ex.  $(call ADD_UNIQUE_PREFIX,-I. .. -Ifoo bar,-I) -> -I. -I.. -Ifoo -Ibar
TEST		:= -I. .. -Ifoo bar
STATUS	:= $(call ASSERTEQ,$(call ADD_UNIQUE_PREFIX,$(TEST),-I),-I. -I.. -Ifoo -Ibar,$(VALID_ASSERTION_STR))

# Test MAP routines, along with logic routines
# --------------------------------------------
STATUS := $(call MAP,ASSERT,$(call MAP,EQUAL,2a 2b 2c,2a 2b 2c))
STATUS := $(call ASSERTEQ,$(call MAP,EQUAL,2a 2b 2c,3a 2b 3c), 2b ,$(VALID_ASSERTION_STR))

# Test PRECEDES routine
# ---------------------
STATUS	:= $(call ASSERTEQ,$(call PRECEDES,a,b,a b c d),true,$(VALID_ASSERTION_STR))
STATUS	:= $(call ASSERTEQ,$(call PRECEDES,b,a,a b c d),,$(VALID_ASSERTION_STR))
STATUS	:= $(call ASSERTEQ,$(call PRECEDES,a,b,c d e f),,$(VALID_ASSERTION_STR))
STATUS	:= $(call ASSERTEQ,$(call PRECEDES,a,b,a b a d),true,$(VALID_ASSERTION_STR))
STATUS	:= $(call ASSERTEQ,$(call PRECEDES,a,b,a b a b),true,$(VALID_ASSERTION_STR))

# Test REMOVE_AT_INDEX routine
# ----------------------------
STATUS	:= $(call ASSERTEQ,$(call REMOVE_AT_INDEX,1,a b c),b c,$(VALID_ASSERTION_STR))
STATUS	:= $(call ASSERTEQ,$(call REMOVE_AT_INDEX,2,a b c),a c,$(VALID_ASSERTION_STR))
STATUS	:= $(call ASSERTEQ,$(call REMOVE_AT_INDEX,3,a b c),a b,$(VALID_ASSERTION_STR))
STATUS	:= $(call ASSERTEQ,$(call REMOVE_AT_INDEX,4,a b c),a b,$(VALID_ASSERTION_STR))

# Test REMOVE_FIRST and REMOVE_LAST routines
# ------------------------------------------
STATUS	:= $(call ASSERTEQ,$(call REMOVE_FIRST,a,a b c),b c,$(VALID_ASSERTION_STR))
STATUS	:= $(call ASSERTEQ,$(call REMOVE_FIRST,b,a b c),a c,$(VALID_ASSERTION_STR))
STATUS	:= $(call ASSERTEQ,$(call REMOVE_FIRST,a,a b a),b a,$(VALID_ASSERTION_STR))
STATUS	:= $(call ASSERTEQ,$(call REMOVE_FIRST,a,a b A a),b A a,$(VALID_ASSERTION_STR))
STATUS	:= $(call ASSERTEQ,$(call REMOVE_LAST,a,a b c),b c,$(VALID_ASSERTION_STR))
STATUS	:= $(call ASSERTEQ,$(call REMOVE_LAST,b,a b c),a c,$(VALID_ASSERTION_STR))
STATUS	:= $(call ASSERTEQ,$(call REMOVE_LAST,a,a b a),a b,$(VALID_ASSERTION_STR))
STATUS	:= $(call ASSERTEQ,$(call REMOVE_LAST,a,a b A a),a b A,$(VALID_ASSERTION_STR))

# Test INDEX_OF and LAST_INDEX_OF routines
# ----------------------------------------
STATUS	:= $(call ASSERTEQ,$(call INDEX_OF,a,a b c),1,$(VALID_ASSERTION_STR))
STATUS	:= $(call ASSERTEQ,$(call INDEX_OF,b,a b c),2,$(VALID_ASSERTION_STR))
STATUS	:= $(call ASSERTEQ,$(call INDEX_OF,c,a b c),3,$(VALID_ASSERTION_STR))
STATUS	:= $(call ASSERTEQ,$(call INDEX_OF,c,a b c a b c),3,$(VALID_ASSERTION_STR))
STATUS	:= $(call ASSERTEQ,$(call INDEX_OF,d,a b c),,$(VALID_ASSERTION_STR))
STATUS	:= $(call ASSERTEQ,$(call LAST_INDEX_OF,c,a b c a b c),6,$(VALID_ASSERTION_STR))
STATUS	:= $(call ASSERTEQ,$(call LAST_INDEX_OF,d,a b c a b c),,$(VALID_ASSERTION_STR))
STATUS	:= $(call ASSERTEQ,$(call LAST_INDEX_OF,b,a b b b b a),5,$(VALID_ASSERTION_STR))

# Test CONTAINS function
# ----------------------
STATUS	:= $(call ASSERT,$(call CONTAINS,a,a b c),$(VALID_ASSERTION_STR))
STATUS	:= $(call ASSERT,$(call CONTAINS,abc,a b c abcd eabc abc,$(VALID_ASSERTION_STR)))
STATUS	:= $(call ASSERTEQ,$(call CONTAINS,a,aa a_ a%),,$(VALID_ASSERTION_STR))
STATUS	:= $(call ASSERTEQ,$(call CONTAINS,abc,a b c abcd eabc),,$(VALID_ASSERTION_STR))

# Test TARGET_IS_UP_TO_DATE function (these tests require previous setup)
# ----------------------------------
#$(call ASSERTEQ,$(call TARGET_IS_UP_TO_DATE,libdeform.so,.),)
#$(call ASSERTEQ,$(call TARGET_IS_UP_TO_DATE,.bin/dbg/objects/i686/Mesh.o,.),up-to-date)
# should result in 'error' because er_ero____ should not exist as a sane target
#$(call ASSERTEQ,$(call TARGET_IS_UP_TO_DATE,er_ero____,.),error)


# Assertions pertaining to valid input from the public interface
# --------------------------------------------------------------

# SUB_MODE should be empty in RELEASE mode
STATUS	:= $(call ASSERT,$(or $(call ISNULL,$(SUB_MODE)), \
					$(call NOTEQUAL,$(MODE),RELEASE)),$(VALID_ASSERTION_STR))

STATUS	:= $(call ASSERTEQ,$(NULL),,$(VALID_ASSERTION_STR))


# -------------------------------
# Old/Scrap/Unused Code Fragments
# -------------------------------

ifdef __DO_NOT_DEFINE_THIS__ # (should not be defined)

#$(eval H = $DD(foreach I,$DD(wordlist 1,$(NUMBERS_LEN),$DD(VA_ARGS_INDICES)),$DD(I)$DD(COMMA)))
#$(eval H = $DD1$D(COMMA)$DD2$D(COMMA)$DD3)
#$(eval H = $DD(foreach I,1,$(NUMBERS_LEN),$DD(NUMBERS),$$(DD)$DD(I)$DD(COMMA)))

#G = $$1$(COMMA)$$2$(COMMA)$$3
#$(eval H = $$(foreach I,$$(wordlist 1,3,$(NUMBERS)),$$(DD)$$(I)$$(COMMA)))
#$(eval H = $D(foreach I,$D(wordlist 1,3,$(NUMBERS)),$D(I)$D(COMMA)))

#$(eval H = $D(foreach I,$D(NUMBERS),$$(D)$D(OPEN_PAREN)$D(I)$D(END_PAREN)$D(COMMA)))
NO = 3
#$(eval H = $D(foreach I,$D(wordlist 1,$D(NO),$D(NUMBERS)),$$(D)$D(OPEN_PAREN)$D(I)$D(END_PAREN)$D(COMMA)))
#$(eval H = $DD(foreach I,$DD(wordlist 1,$DD(NO),$DD(NUMBERS)),$$(DD)$DD(OPEN_PAREN)$DD(I)$DD(END_PAREN)$DD(COMMA)))

#$(eval J = $D(or $D(eval NO=$DD(VA_ARGS_COUNT)),$D(eval K=$DH),$DK))


$(info D    ='$D')
$(info DD   ='$DD')
$(info DDD  ='$DDD')
$(info DDDD ='$DDDD')

$(info )
$(info )
$(info )

#$(eval TEST  = $D(call PRINT,$(J)))

$(eval TEST  = $D(if $D(call EQUAL,1,$D(VA_ARGS_COUNT)),$D(call $D1),\
					$D(if $D(call EQUAL,2,$D(VA_ARGS_COUNT)),$D(call $D1,$D2),\
					$D(if $D(call EQUAL,3,$D(VA_ARGS_COUNT)),$D(call $D1,$D2,$D3),))))

#$(eval TEST  = $(foreach NUM,$(NUMBERS),$(D)$D(OPEN_PAREN)if $D(call EQUAL,$(NUM),$D(VA_ARGS_COUNT)),$D(call $D1,)
# can just have $1,$2,$3,$4,$5,... for each one!

#$(eval H = $D(foreach I,$D(wordlist 2,3,$D(NUMBERS)),$$(D)$D(OPEN_PAREN)$D(I)$D(END_PAREN)$D(COMMA)))
#$(eval H = $D(foreach I,$D(wordlist 2,3,$D(NUMBERS)),$DD(word $DD(SUBARGINDEX),$$(D)$D(OPEN_PAREN)$D(I)$D(END_PAREN))$D(COMMA)))
#$(eval TEST  = $D(call $D1,$(H)))

#$(eval ARGUMENTS = $D(foreach ARG,$D(wordlist 2,$(NUMBERS_LEN),$D(NUMBERS)),$DD(word $DD(SUBARGINDEX),$D(GET_ARG$D(ARG))$D(COMMA))))
#$(eval ARGUMENTS = $DD(filter-out $DD(COMMA),$D(foreach I,$D(wordlist 2,$D(NUMBERS_LEN),$D(NUMBERS)),$DD(word $DD(SUBARGINDEX),$$(D)$D(OPEN_PAREN)$D(I)$D(END_PAREN))$D(COMMA))))

# This version of ARGUMENTS works and is more elegant than the current MAP 
# solution, but it adds one extra space to each argument which is annoying
$(eval ARGUMENTS = $D(foreach I,$D(wordlist 2,$D(NUMBERS_LEN),$D(NUMBERS)),$DD(word $DD(SUBARGINDEX),$$(D)$D(OPEN_PAREN)$D(I)$D(END_PAREN))$D(COMMA)))

# $(join) args with list of commas?

$(eval CALL = $D(call $D1,$(H)))


$(info H='$(H)')
PRINT = $(info 1'$1'  2'$2'  3'$3')
STATUS := $(info $(call MAP,EQUAL,2a 2b 2c,3a 2b 3c))


#TEST = $(or $(info 1'$1' 2'$2'),$(if $(findstring $1,$2),,$(if $(call EQUAL,$1,b),d,)))


#$(foreach item,$(call MAKE_MAP,$(2)),$(call $(1),$(item)))

#(call MAP,$(MAKE),$(addsuffix -C ,$(DIRS)))


# Create a .depend file for each source file, containing all dependencies 
# for its object file (including headers).
#
# Add rules of the following form:
# 		.bin/objects/i386/DBG/main.o .bin/depend/main.depend : main.C main.H
# 
# Note: it is important to escape all slashes in the paths passed into sed
$(DEP_DIR)/%.$(DEP_SUFFIX) : %.$(SOURCE_SUFFIX)
	@$(CHK_DIR_EXISTS) $(DEP_DIR) || $(MKDIR) $(DEP_DIR);
	@$(COMPILER) -MM $(COMPILE_FLAGS) $(INCLUDE_PATH) $< -MT \
	'$(OBJ_DIR)/$(<:%.$(SOURCE_SUFFIX)=%.$(OBJ_SUFFIX)) $@ ' > $@;

#@$(COMPILER) -MM $(COMPILE_FLAGS) $(INCLUDE_PATH) $< -MT '$(OBJ_DIR)/$(<:%.$(SOURCE_SUFFIX)=%.$(OBJ_SUFFIX)) $@ ' \
	> $@;
#@set -e; $(COMPILER) -MM $(COMPILE_FLAGS) $(INCLUDE_PATH) $< \
#| $(SED) 's/\($*\)\.o[ :]*/$(OBJ_DIR_ESC)\/\1.$(OBJ_SUFFIX) $(call ESCAPE,$@) : /g' > $@;

@$(GREP) -l $($<:%.$(SOURCE_SUFFIX)=%.$(HEADER_SUFFIX)) $(SOURCES) $(HEADERS) >> $(DEP_TEMP) || touch $(DEP_TEMP);


# Additional rules to enforce dependence on deform library
# --------------------------------------------------------
ifeq ("$(shell [ -f $(LIB_DEFORM) ] && echo "exists")","")
all: $(LIB_DEFORM)

$(eval $(call RECURSIVE_MAKE,$(PROJECT_BASE_DIR)/deform,$(LIB_DEFORM)))

.PHONY: $(LIB_DEFORM)
endif


endif # __DO_NOT_DEFINE_THIS__

endif # __UNITTESTS_MK__

