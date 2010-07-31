#define OOM_ERR			7
#define DISK_FULL_ERR		10
#define UNRECOGNIZED_FIXUPP_FRAME_ERR		0x21
#define CANT_REACH_ERR		0x23
#define CREATESEM_ERR		0x38
#define SEG_ALREADY_RELEASED_ERR 0x42
#define FILE_EXP_ERR		0x76
#define CVP_CIRC_NO_CSUE_ERR	0xA3
#define IMPROBABLE_RES_ERR	0xB7


void _err_inbuf_abort(int);
void _error_undefined(struct SYMBOL_STRUCT *);
