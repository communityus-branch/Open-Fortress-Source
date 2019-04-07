#include "cbase.h"
#include "ofd_trigger_jump.h"
#include "tf_gamerules.h"
#include "world.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_DATADESC( COFDTriggerJump )
	DEFINE_KEYFIELD( m_szTarget, FIELD_STRING, "target" ),
	DEFINE_KEYFIELD( m_iSound, FIELD_INTEGER, "do_bounce_sound" ),
	DEFINE_KEYFIELD( m_flApexBoost, FIELD_FLOAT, "apex_height_boost" ),
	DEFINE_KEYFIELD( m_iSoftLanding, FIELD_INTEGER, "soft_landing" ),

	DEFINE_OUTPUT( m_OnJump, "OnJump" ), 
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( COFDTriggerJump, DT_OFDTriggerJump )
	SendPropVector( SENDINFO( m_vecTarget ), 0, SPROP_COORD ),
	SendPropFloat( SENDINFO( m_flApexBoost ), 0, SPROP_NOSCALE ),
	SendPropInt( SENDINFO( m_iSound ), 3, SPROP_UNSIGNED ),
END_SEND_TABLE()
 
LINK_ENTITY_TO_CLASS( ofd_trigger_jump, COFDTriggerJump );


COFDTriggerJump::COFDTriggerJump( void )
{
	m_pTarget = NULL;
	m_vecTarget = vec3_origin;
	m_iSoftLanding = 0;
}

void COFDTriggerJump::Spawn( void )
{

	Precache();
	BaseClass::Spawn();
	InitTrigger();

	m_pTarget = gEntList.FindEntityByName(0, m_szTarget);

	if ( !m_pTarget )
	{
		Warning("Tried creating ofd_trigger_jump but couldn't find target, will fail to launch.\n");
		UTIL_Remove( this );
		return;
	}

	// temp
	m_vecTarget = m_pTarget->GetAbsOrigin();

	//SetTouch( &COFDTriggerJump::OnTouched );
	SetNextThink( TICK_NEVER_THINK );

	//m_bClientSidePredicted = true;
	SetTransmitState( FL_EDICT_PVSCHECK );
}

void COFDTriggerJump::Precache( void )
{
	PrecacheScriptSound( "JumpPadSound" );
}
