# @auth Travis Fischer
# @acct tfischer
# @date March 2008
# @site http://www.cs.brown.edu/people/tfischer/make
# @version 1.0
#
#    This file contains all of the targets (rules) used by the Makefile
# subsystem.  Note that dependency rules are automatically generated in the 
# same step as compilation, in the rule for .o files.
ifndef __TARGETS_MK__
__TARGETS_MK__ := # Guard against multiple includes


ifeq ($(TARGET_TYPE),HIERARCHY)

# Generate HIERARCHY targets explicitly
$(foreach PROJECT,$(SUB_PROJECTS),$(eval $(call ADD_PROJECT,$(PROJECT))))

AVAILABLE_TARGETS = all clean doc $(SUB_PROJECTS)

.PHONY: $(AVAILABLE_TARGETS)

else # normal compilation

# Core Compilation Targets
# ------------------------
all: $(TARGET) $(EXTRA_TARGETS)

# Let make know that the following targets will not create files corresponding 
# to their names (ex. the help target won't create a file called 'help')
.PHONY: help all run gdb tidy clean comments doc

LINK = $(LINKER) -o $1 $(BACKSLASH) \
$(NEWLINE)$(TAB)$(OFILES) $(BACKSLASH) \
$(NEWLINE)$(TAB)$(LINK_FLAGS) $(LIBRARIES)

CLEANLINK = $(call CLEAN_CMD,$(LINK))

ARCHIVE_DIR = $(OUT_DIR)/.archive

$(TARGET): $(DEP_MAKEFILE) $(OFILES)
ifneq ($(TARGET_TYPE),ARCHIVE)
# Linking
	@$(CECHO) "$(subst $(OBJ_DIR)/,,$(call LINK,$(call EMPHASIS,$@)))"
	@$(call CLEANLINK,$@)
else
# Static archives
ifneq ($(PROJECT_LIBS),)
	@rm -rfd $(ARCHIVE_DIR)
	@$(MKDIR) $(ARCHIVE_DIR)
	(cd $(ARCHIVE_DIR) $(strip $(foreach LIB,$(PROJECT_LIBS),\
		$(foreach PATH,$(PROJECT_LIBPATH),\
			$(if $(call EXISTS,$(PATH)/lib$(LIB).a), \
				&& $(ARCHIVE) -x ../../$(PATH)/lib$(LIB).a)))))
	$(ARCHIVE) $(ARCHIVE_FLAGS) $@ \
	$(OFILES) $(ARCHIVE_DIR)/*
else
	$(ARCHIVE) $(ARCHIVE_FLAGS) $@ \
	$(OFILES)
endif
endif
	@-chmod +x $@
	@echo
	@$(CECHO) "Successfully built '$(BOLD_COLOR)$(FR_COLOR_GREEN)$(TARGET)$(END_COLOR)' in $(STR_MODE) mode."

# Compilation and dependency generation
#    Combining dependency generation and compilation into one step means that 
# we only have to compile each source file once instead of twice!
#    Create a .depend file for each source file, containing all dependencies 
# for its object file (including headers).
#
# Add rules of the following form:
# 		.bin/DBG/objects/i386/main.o .bin/DBG/depend/main.depend : main.C main.H
COMPILE = \
	$(COMPILER) -c $1 -o $2 $(BACKSLASH) \
$(NEWLINE)$(TAB)$($2_INCLUDE_PATH) $(BACKSLASH) \
$(NEWLINE)$(TAB)$($2_COMPILE_FLAGS) $($4_CFLAGS) $(BACKSLASH) \
$(NEWLINE)$(TAB)-MMD -MT '$2' -MF '$(DEP_DIR)/$3.$(DEP_SUFFIX)'

CLEANCOMPILE = $(call CLEAN_CMD,$(COMPILE))

$(OBJ_DIR)/%.$(OBJ_SUFFIX) : %.$(SOURCE_SUFFIX)
	@$(CECHO) "$(subst $(OBJ_DIR)/,,$(call COMPILE,$(call EMPHASIS,$<),$@,$*,$<))"
	@$(call CLEANCOMPILE,$<,$@,$*,$<)
	@sed -i 's/\\//g' $(DEP_DIR)/$*.$(DEP_SUFFIX) && cat $(DEP_DIR)/$*.$(DEP_SUFFIX) | tr -d '\n' > $(DEP_DIR)/$*.$(DEP_SUFFIX)

#@grep -v "$(QT_DIR)" $(DEP_DIR)/$*.$(DEP_SUFFIX) | tr -d '\n' > $(DEP_DIR)/$*.$(DEP_SUFFIX)2

#	$(COMPILER) -c $< -o $@ \
#	$(INCLUDE_PATH) \
#	$(COMPILE_FLAGS) \
#	-MMD -MT '$(DEP_DIR)/$*.$(DEP_SUFFIX)' -MF '$(DEP_DIR)/$*.$(DEP_SUFFIX)'

# Attempt to force recompilation of affected files when headers are modified
%.$(SOURCE_SUFFIX): %.$(HEADER_SUFFIX)

# Empty rule to prevent problems when headers are removed
%.$(HEADER_SUFFIX): ;

# Makefile dependencies -- force recompilation whenever a Makefile is changed 
# or a file that the main Makefile depends on is changed
$(DEP_MAKEFILE):: Makefile $(SETTINGS_FILE)
#	@echo "Remaking $@"
	@$(EXIT_ON_ERROR)																				\
	echo "# Add rules for object files to depend on main Makefile, including " \
		> $@;																							\
	echo "# any files included directly or indirectly by that Makefile"			\
		>> $@;																						\
	echo "$@ $(OFILES) : $(call BFS,EXTRACT_MK_INCLUDES,Makefile)" >> $@;

# Target whose purpose is to ensure that TARGET is remade if certain 
# commandline settings are changed (MODE, etc)
$(SETTINGS_FILE):
	@$(call ENSURE_ALL_DIR_EXISTS)
#	@echo "Remaking $@: $(SETTINGS)"
	@echo $(SETTINGS) > $@

ifndef STOP_RECURSION
ifeq ($(ONLYCLEAN),)
#$(or $(info cur_targets='$(CUR_TARGETS)'),true),
#$(info "TARGET is not up-to-date $(LIBPATH)/$(POSS)")

# Add dependency information for any libraries created locally
# ------------------------------------------------------------
STATUS := \
$(foreach LIBPATH,$(filter-out .,$(PROJECT_LIBPATH)),\
	$(and $(call STORE,CUR_TARGETS,$(call GET_LIST_OF_TARGETS,$(LIBPATH))),\
		$(foreach LIB,$(PROJECT_LIBS),\
			$(foreach POSS,$(call POSSIBLE_LIBS,$(LIB)), \
				$(if $(call CONTAINS,$(POSS),$(CUR_TARGETS)), \
					$(or $(call STORE,CUR_TARGETS,),\
					$(if $(call TARGET_IS_UP_TO_DATE,$(POSS),$(LIBPATH)),,\
						$(shell $(RM) -f $(LIBPATH)/$(POSS)) \
						$(eval $(call RECURSIVE_MAKE,$(LIBPATH),$(LIBPATH)/$(POSS))) \
						$(eval $$(TARGET): $(LIBPATH)/$(POSS)))))))))
endif # ONLYCLEAN
endif # STOP_RECURSION

ifeq ($(TARGET_TYPE),EXECUTABLE)

run: $(TARGET)
	./$(TARGET)

gdb: $(TARGET)
ifneq ($(MODE),DBG)
	+@$(call WARNING,"The '$@' target is only valid in DBG mode; compiling in DBG mode:")
	+@$(MAKE) $< MODE=DBG --no-print-directory
endif
	@echo
	$(GDB) -q $<

else  # main target is not EXECUTABLE

run gdb: $(TARGET)
	@$(call ERROR,"The '$@' target is not valid for $(TARGET_TYPE) targets")

endif # EXECUTABLE


# ----------
# QT-Support
# ----------

ifeq ($(USE_QT),true)

#$(info ADDING QT OBJECT: '$1')
define ADD_QT_OBJECT
$1_INCLUDE_PATH  = $(QT_INCLUDE_PATH)
$1_COMPILE_FLAGS = $(QT_COMPILE_FLAGS)

$(OBJ_DIR)/$1_INCLUDE_PATH  = $(QT_INCLUDE_PATH)
$(OBJ_DIR)/$1_COMPILE_FLAGS = $(QT_COMPILE_FLAGS)
endef
#$1: override INCLUDE_PATH	= $(QT_INCLUDE_PATH)
#$1: override COMPILE_FLAGS	= $(QT_COMPILE_FLAGS)

#$(info "ADDING MOC FILE: '$1' -> '$(call QT_CONVERT_SOURCE_TO_MOC,$1)'")
#$(info "   Dep: $(filter-out $(filter-out $(HEADERS),$($1_MOC_DEP)),$($1_MOC_DEP))")
define ADD_MOC_SOURCE
$(eval $1_MOC_DEP = $(strip								\
	$(foreach DEP,$(call BFS,EXTRACT_INCLUDES,$1),	\
		$(if $(call EXISTS,$(DEP)),$(DEP),				\
			$(foreach PATH,$(PROJECT_INCPATH),			\
				$(if $(call EXISTS,$(PATH)/$(DEP)),		\
					$(PATH)/$(DEP)))))))

$(call ADD_QT_OBJECT,$1)
$(eval $1_DEP = $(filter-out $(filter-out $(HEADERS),$($1_MOC_DEP)),$($1_MOC_DEP)))

$(call QT_CONVERT_SOURCE_TO_MOC,$1): $($1_DEP)
	$(QT_BIN)/moc $(QT_DEFINES) $(QT_INCLUDE_PATH) $(1:%.$(SOURCE_SUFFIX)=%.$(HEADER_SUFFIX)) -o $$@
endef

$(foreach SOURCE,$(QT_SOURCES),$(eval $(call ADD_MOC_SOURCE,$(SOURCE))))
$(foreach OBJECT,$(QT_OBJECT_FILES) $(QT_OBJECTS),$(eval $(call ADD_QT_OBJECT,$(OBJECT))))

.PHONY: QT_NOTICE

QT_NOTICE:
	+@$(call SHOW_MESSAGE,"Using Qt module(s): $(QT_REAL_USED_MODULES)",\
		"$(FR_COLOR_RED)Notice:$(END_COLOR) ")

endif # Qt-Support


# Utility Compilation Targets
# ---------------------------

# Replace C++ style comments with C style comments
comments:
	@echo "Replacing C++ style comments with C style comments"
	@sed -i 's/\/\/\(.*\)/\/*\1 *\//' $(SOURCES) $(HEADERS)

tidy:
	-$(RM) -rf $(OUT_DIR) $(TEMP_FILE)*

clean: tidy
	-$(RM)  -f $(TARGET)
	@echo

dummy: ;


# Load all dependency rules stored in the .depend files which were generated below
# Use -include to suppress errors if any of $(DFILES) are missing
ifeq ($(CLEAN),)
-include $(DFILES)
-include $(DEP_MAKEFILE)
endif

endif # HIERARCHY


# Targets common to all TARGET_TYPEs
# ----------------------------------

help::
	@$(CECHO) "$(FR_COLOR_RED)Usage:$(END_COLOR) make [MODE=$(STR_MODE_OPTIONS)] [target1 ...]"
	@echo "   * the default MODE is $(DEFAULT_MODE) if none is given"
	@echo "   * the default target is 'all'"
	@$(CECHO) "   * $($(TARGET_TYPE)_TYPE_DESCRIPTION)"
	@echo
	@echo "Available targets include:"
	@echo "   $(AVAILABLE_TARGETS)"

doc: $(DOXYFILE)
	@$(call ENSURE_DIR_EXISTS_SHELL,$(DOXY_OUTPUT_DIRECTORY))
	@$(CECHO) "$(FR_COLOR_RED)Building documentation:$(END_COLOR)"
	doxygen $<
	@$(CECHO) "$(FR_COLOR_RED)Done building documentation$(END_COLOR) at '$(FR_COLOR_BLUE)$(DOXY_OUTPUT_DIRECTORY)$(END_COLOR)'"

$(DOXYFILE):
	@$(CECHO) "Creating '$(FR_COLOR_RED)$(DOXYFILE)$(END_COLOR)' for your project"
	$(call CREATE_DOXYFILE)

$(DEP_MAKEFILE):: ;


endif # __TARGETS_MK__

