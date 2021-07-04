#include "rtapi.h"
#include "rtapi_ctype.h"
#include "rtapi_app.h"
#include "rtapi_string.h"
#include "rtapi_errno.h"
#include "rtapi_math64.h"
#include <rtapi_io.h>
#include "hal.h"
#include <halcmd_commands.h>

/* module information */
MODULE_AUTHOR("Skynet");
MODULE_DESCRIPTION("Kernel hal module sample to write or read : characters, strings, values");
MODULE_LICENSE("GPL");

/* Usage example :
 *
 * halcmd stop
 * halcmd loadrt threads name1=base-thread fp1=0 period1=25000 name2=servo-thread period2=1000000
 * halcmd loadrt port
 * halcmd addf write servo-thread
 * halcmd addf read servo-thread
 * halcmd sets signal 100
 * halcmd start
 *
 * To compile, see attached makefile eof.
 */

static int comp_idx;

typedef struct {
    hal_bit_t *bittest;
    char *chartest;
    int x;
} skynet_t;
skynet_t *port;

typedef struct {
    hal_float_t *Pin;
} float_data_t;

typedef struct {
    hal_bit_t *Pin;
} bit_data_t;

typedef struct {
    hal_float_t Pin;
} param_data_t;

typedef struct {
    hal_s32_t *Pin;
} s32_data_t;

typedef struct {
    hal_u32_t *Pin;
} u32_data_t;

typedef struct {
    hal_port_t *Pin;
} port_data_t;
port_data_t *P1,*P2;

static int comp_idx;		/* component ID */
static void writefunction();
static void readfunction();

int quit=0;

int rtapi_app_main(void) {

    int r = 0;
    comp_idx = hal_init("port");
    if(comp_idx < 0) return comp_idx;
    r = hal_export_funct("write", writefunction, &port,0,0,comp_idx);
    r = hal_export_funct("read", readfunction, &port,0,0,comp_idx);

    float_data_t *FL;
    FL = (float_data_t*)hal_malloc(sizeof(float_data_t));
    r+=hal_pin_float_new("FL",HAL_IN,&(FL->Pin),comp_idx);

    P1 = (port_data_t*)hal_malloc(sizeof(char[100]));
    r+=hal_pin_port_new("P1",HAL_OUT,&(P1->Pin),comp_idx);

    P2 = (port_data_t*)hal_malloc(sizeof(char[100]));
    r+=hal_pin_port_new("P2",HAL_IN,&(P2->Pin),comp_idx);

    hal_signal_new("signal",HAL_PORT);
    hal_link("P1","signal");
    hal_link("P2","signal");

    //do_sets_cmd("signal","100");

    if(r) {
        hal_exit(comp_idx);
    } else {
        hal_ready(comp_idx);
    }
    return 0;
}

void rtapi_app_exit(void) {
    hal_exit(comp_idx);
}

static void writefunction()
{
    if(!quit){
        char sent[6] = { 'a' , 'l', 'i', 'e' , 'n', 0};
        bool isdone=hal_port_write(*P1->Pin,sent,6);
        rtapi_print_msg(RTAPI_MSG_ERR,"port bytes are written:'%u'\n", isdone);

        unsigned int x= hal_port_writable(*P1->Pin);
        rtapi_print_msg(RTAPI_MSG_ERR,"port nr of bytes:'%u'\n", x);

        x= hal_port_buffer_size(*P1->Pin);
        rtapi_print_msg(RTAPI_MSG_ERR,"port buffersize :'%u'\n", x);
    }
}

static void readfunction()
{
    char dest[6] = {};
    bool ok= hal_port_read(*P2->Pin,dest,6);
    rtapi_print_msg(RTAPI_MSG_ERR,"received status 0-1 :'%d'\n", ok);
    rtapi_print_msg(RTAPI_MSG_ERR,"port dest data :'%s'\n", dest);
}

/* Makefile :
# usage : $ make all
# usgae : $ make clean

COMP=/home/user/Desktop/Linux-Embedded/linux-hal/hal-core/bin/halcompile

# include modinc
MODINC=$(shell $(COMP) --print-modinc)
ifeq (, $(MODINC))
  $(error Unable to get modinc path)
endif

include $(MODINC)

# File list to compile
obj-m += port.o
files-to-compile := \
    port.o \

cc-option = $(shell if $(CC) $(CFLAGS) $(1) -S -o /dev/null -xc /dev/null \
             > /dev/null 2>&1; then echo "$(1)"; else echo "$(2)"; fi ;)

module = $(patsubst %.o,%.so,$(obj-m))

EXTRA_CFLAGS := $(filter-out -Wframe-larger-than=%,$(EXTRA_CFLAGS))

$(module): $(files-to-compile)
    $(CC) -shared -o $@ $(files-to-compile) -Wl,-rpath,$(LIBDIR) -L$(LIBDIR) -llinuxcnchal -lrt



%.o: %.c
    $(CC) -o $@ $(EXTRA_CFLAGS) -Os -c $<

all: $(module)

clean:
    rm -f $(module)
    rm -f *.so *.ko *.o
    rm -f *.sym *.tmp *.ver
    rm -f *.mod.c .*.cmd
    rm -f modules.order Module.symvers
*/





