MODULE := engines/scream

MODULE_OBJS = \
	scream.o \
	console.o \
	metaengine.o \
	script.o \
	script_thread.o \
	resources.o \
	settings.o \
	cursor.o \
	font.o \
	menu.o \
	hints.o \
	journal.o \
	gfx.o \
	image.o \
	camera.o \
	feedback.o \
	inventory.o \
	world/world.o \
	world/track.o \
	world/dialogue.o \
	world/choice.o \
	world/item.o \
	world/node.o \
	world/object.o \
	world/room.o \
	world/zoom.o \
	views/inventory_view.o \
	views/scene_view.o

# This module can be built as a plugin
ifeq ($(ENABLE_SCREAM), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk

# Detection objects
DETECT_OBJS += $(MODULE)/detection.o
