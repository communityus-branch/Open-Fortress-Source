//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: Deathmatch Powerup Spawner.
//
//=============================================================================//

#include "cbase.h"
#include "tf_player.h"
#include "tf_team.h"
#include "engine/IEngineSound.h"
#include "entity_condpowerup.h"

#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Spawn function for the powerupspawner
//-----------------------------------------------------------------------------


BEGIN_DATADESC( CCondPowerup )

// Inputs.
DEFINE_KEYFIELD( m_bCondition, FIELD_INTEGER, "condID" ),
DEFINE_KEYFIELD( m_bCondDuration, FIELD_FLOAT, "duration" ),
DEFINE_KEYFIELD( m_iszPowerupModel, FIELD_STRING, "model" ),
DEFINE_KEYFIELD( m_iszPowerupModelOLD, FIELD_STRING, "powerup_model" ),
DEFINE_KEYFIELD( m_iszPickupSound, FIELD_STRING, "pickup_sound" ),
DEFINE_KEYFIELD( m_bDisableShowOutline, FIELD_BOOLEAN, "disable_glow" ),

END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CCondPowerup, DT_CondPowerup )
SendPropBool( SENDINFO( m_bDisableShowOutline ) ),
SendPropBool( SENDINFO( m_bRespawning) ),
SendPropBool( SENDINFO( bInitialDelay ) ),
SendPropTime( SENDINFO( m_flRespawnTick ) ),
SendPropTime( SENDINFO( fl_RespawnTime ) ),
SendPropTime( SENDINFO( fl_RespawnDelay ) ),
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( dm_powerup_spawner, CCondPowerup );

void CCondPowerup::Spawn( void )
{
	Precache();
	if (m_iszPowerupModel==MAKE_STRING( "" )) SetModel( STRING(m_iszPowerupModelOLD)  );
	else SetModel( STRING(m_iszPowerupModel) );
	
	SetTransmitState( FL_EDICT_ALWAYS );

	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: Precache function for the powerupspawner
//-----------------------------------------------------------------------------
void CCondPowerup::Precache( void )
{
	if (m_iszPowerupModel==MAKE_STRING( "" )) PrecacheModel( STRING(m_iszPowerupModelOLD)  );
	else PrecacheModel( STRING(m_iszPowerupModel) );
	PrecacheScriptSound( STRING( m_iszPickupSound ) );
}

//-----------------------------------------------------------------------------
// Purpose: MyTouch function for the powerupspawner
//-----------------------------------------------------------------------------
bool CCondPowerup::MyTouch( CBasePlayer *pPlayer )
{
	bool bSuccess = false;
	if ( ValidTouch( pPlayer ) )
	{
		CTFPlayer *pTFPlayer = ToTFPlayer( pPlayer );
		if ( !pTFPlayer )
			return false;
		
		bSuccess = true;

		pTFPlayer->m_Shared.AddCond( m_bCondition , m_bCondDuration );
		EmitSound( STRING( m_iszPickupSound ) );
		m_nRenderFX = kRenderFxDistort;
	}
	return bSuccess;
}

CCondPowerup::CCondPowerup()
{
	m_flRespawnTick = 0.0f;
}

CBaseEntity* CCondPowerup::Respawn( void )
{
	CBaseEntity *ret = BaseClass::Respawn();
	m_nRenderFX = kRenderFxDistort;
	m_flRespawnTick = GetNextThink();
	return ret;
}