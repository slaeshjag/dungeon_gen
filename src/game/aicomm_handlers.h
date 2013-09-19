#ifndef __AICOMM_HANDLERS_H__
#define	__AICOMM_HANDLERS_H__

static struct aicomm_struct(*aihandle[AICOMM_MSG_END])(struct aicomm_struct) = {
	[AICOMM_MSG_COLL] = aicomm_f_dummy,
	[AICOMM_MSG_SEND] = aicomm_f_dummy,
	[AICOMM_MSG_INVM] = aicomm_f_dummy,
	[AICOMM_MSG_NEXT] = aicomm_f_dummy,

	[AICOMM_MSG_FOLM] = aicomm_f_folm,
	[AICOMM_MSG_DIRU] = aicomm_f_diru,
	[AICOMM_MSG_SETP] = aicomm_f_setp,
	[AICOMM_MSG_GETP] = aicomm_f_getp,
	[AICOMM_MSG_KILL] = aicomm_f_kill,
	[AICOMM_MSG_SPWN] = aicomm_f_spwn,
	[AICOMM_MSG_GETF] = aicomm_f_getf,
	[AICOMM_MSG_CAMN] = aicomm_f_camn,
	[AICOMM_MSG_TPME] = aicomm_f_tpme,
};


#endif
