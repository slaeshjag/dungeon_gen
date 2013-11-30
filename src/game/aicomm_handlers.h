#ifndef __AICOMM_HANDLERS_H__
#define	__AICOMM_HANDLERS_H__

static struct aicomm_struct(*aihandle[AICOMM_MSG_END])(struct aicomm_struct) = {
	[AICOMM_MSG_COLL] = aicomm_f_dummy,
	[AICOMM_MSG_SEND] = aicomm_f_dummy,
	[AICOMM_MSG_INVM] = aicomm_f_dummy,
	[AICOMM_MSG_NEXT] = aicomm_f_dummy,
	[AICOMM_MSG_SILE] = aicomm_f_dummy,
	[AICOMM_MSG_STAT] = aicomm_f_dummy,

	[AICOMM_MSG_FOLM] = aicomm_f_folm,
	[AICOMM_MSG_DIRU] = aicomm_f_diru,
	[AICOMM_MSG_NSPR] = aicomm_f_nspr,
	[AICOMM_MSG_SETP] = aicomm_f_setp,
	[AICOMM_MSG_GETP] = aicomm_f_getp,
	[AICOMM_MSG_KILL] = aicomm_f_kill,
	[AICOMM_MSG_SPWN] = aicomm_f_spwn,
	[AICOMM_MSG_GETF] = aicomm_f_getf,
	[AICOMM_MSG_CAMN] = aicomm_f_camn,
	[AICOMM_MSG_TBOX] = aicomm_f_tbox,
	[AICOMM_MSG_TPME] = aicomm_f_tpme,
	[AICOMM_MSG_TXTE] = aicomm_f_txte,
	[AICOMM_MSG_PREL] = aicomm_f_prel,
	[AICOMM_MSG_UNLO] = aicomm_f_unlo,
};


#endif
