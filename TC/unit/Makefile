CC ?= gcc

TARGETS = utc_ApplicationFW_bundle_create_func utc_ApplicationFW_bundle_free_func utc_ApplicationFW_bundle_add_func utc_ApplicationFW_bundle_del_func utc_ApplicationFW_bundle_get_val_func utc_ApplicationFW_bundle_get_count_func utc_ApplicationFW_bundle_dup_func utc_ApplicationFW_bundle_iterate_func utc_ApplicationFW_bundle_encode_func utc_ApplicationFW_bundle_decode_func

PKGS = bundle

LDFLAGS = `pkg-config --libs $(PKGS)`
LDFLAGS += $(TET_ROOT)/lib/tet3/tcm_s.o
LDFLAGS += -L$(TET_ROOT)/lib/tet3 -ltcm_s
LDFLAGS += -L$(TET_ROOT)/lib/tet3 -lapi_s

CFLAGS = -I. `pkg-config --cflags $(PKGS)`
CFLAGS += -I$(TET_ROOT)/inc/tet3
CFLAGS += -Wall

all: $(TARGETS)

$(TARGETS): %: %.c
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGETS)
