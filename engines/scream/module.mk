MODULE := engines/scream

MODULE_OBJS = \
	scream.o \
	console.o \
	metaengine.o \
	resources.o \
	script.o \
	settings.o \
	cursor.o \
	font.o \
	menu.o \
	hints.o \
	world.o \
	track.o \
	dialogue.o \
	choice.o \
	item.o \
	journal.o \
	node.o \
	object.o \
	zoom.o

# This module can be built as a plugin
ifeq ($(ENABLE_SCREAM), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk

# Detection objects
DETECT_OBJS += $(MODULE)/detection.o
