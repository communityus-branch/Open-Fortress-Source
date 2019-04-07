
#include "cbase.h"
#include "c_ofd_trigger_jump.h"


IMPLEMENT_CLIENTCLASS_DT( C_OFDTriggerJump, DT_OFDTriggerJump, COFDTriggerJump )
	RecvPropVector( RECVINFO( m_vecTarget ) ),
	RecvPropFloat( RECVINFO( m_flApexBoost ) ),
	RecvPropInt( RECVINFO( m_iSound ) ),
END_RECV_TABLE()


C_EntityClassList< C_OFDTriggerJump > g_TriggerJumpList;
template<> C_OFDTriggerJump *C_EntityClassList<C_OFDTriggerJump>::m_pClassList = NULL;


C_OFDTriggerJump::C_OFDTriggerJump()
{
	g_TriggerJumpList.Insert( this );
}

C_OFDTriggerJump::~C_OFDTriggerJump()
{
	g_TriggerJumpList.Remove( this );
}

void C_OFDTriggerJump::Spawn()
{
	//SetTouch( &C_OFDTriggerJump::OnTouched );
}