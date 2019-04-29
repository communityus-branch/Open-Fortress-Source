//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: CTF AmmoPack.
//
//=============================================================================//
#include "cbase.h"
#include "items.h"
#include "tf_gamerules.h"
#include "tf_shareddefs.h"
#include "tf_player.h"
#include "tf_team.h"
#include "engine/IEngineSound.h"
#include "tf_powerup.h"

//=============================================================================
float PackRatios[POWERUP_SIZES] =
{
	0.2,	// SMALL
	0.5,	// MEDIUM
	1.0,	// FULL
};

//=============================================================================
//
// CTF Powerup tables.
//

BEGIN_DATADESC( CTFPowerup )

// Keyfields.
DEFINE_KEYFIELD( m_bDisabled, FIELD_BOOLEAN, "StartDisabled" ),
DEFINE_KEYFIELD( m_bHide, FIELD_BOOLEAN, "HiddenWhenRespawning" ),
DEFINE_KEYFIELD( fl_RespawnTime, FIELD_FLOAT, "respawntime" ),
DEFINE_KEYFIELD( m_iszSpawnSound, FIELD_STRING, "spawn_sound" ),
DEFINE_KEYFIELD( fl_RespawnDelay, FIELD_FLOAT, "respawndelay" ),

// Inputs.
DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),
DEFINE_INPUTFUNC( FIELD_VOID, "Toggle", InputToggle ),

// Outputs.

END_DATADESC();

//=============================================================================
//
// CTF Powerup functions.
//

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFPowerup::CTFPowerup()
{
	m_bDisabled = false;
	m_bRespawning = false;
	fl_RespawnTime = -1.0f;
	fl_RespawnDelay = -1.0f;

	UseClientSideAnimation();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFPowerup::Spawn( void )
{
	bInitialDelay = true;
	BaseClass::Precache();
	BaseClass::Spawn();

	BaseClass::SetOriginalSpawnOrigin( GetAbsOrigin() );
	BaseClass::SetOriginalSpawnAngles( GetAbsAngles() );

	VPhysicsDestroyObject();
	SetMoveType( MOVETYPE_NONE );
	SetSolidFlags( FSOLID_NOT_SOLID | FSOLID_TRIGGER );
	PrecacheScriptSound( STRING( m_iszSpawnSound ) );
	if ( m_bDisabled )
	{
		SetDisabled( true );
	}
	
	if ( GetRespawnDelay() )
	{
		Respawn();
	}
	else
	{
		bInitialDelay = false;
		m_bRespawning = false;
	}
	ResetSequence( LookupSequence("idle") );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CBaseEntity* CTFPowerup::Respawn( void )
{
	m_bRespawning = true;
	CBaseEntity *pReturn = BaseClass::Respawn();
	
	// Override the respawn time
	if ( bInitialDelay )
		SetNextThink( gpGlobals->curtime + GetRespawnDelay() );
	else
		SetNextThink( gpGlobals->curtime + fl_RespawnTime );
	return pReturn;
}

float CTFPowerup::GetRespawnDelay( void )
{
	return fl_RespawnDelay;
}
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFPowerup::Materialize( void )
{
	if ( !m_bDisabled )
	{
		// changing from invisible state to visible.
		EmitSound( STRING( m_iszSpawnSound ) );
		m_nRenderFX = kRenderFxNone;
		RemoveEffects( EF_NODRAW );
	}
	m_bRespawning = false;
	bInitialDelay = false;
	SetTouch( &CItem::ItemTouch );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFPowerup::ValidTouch( CBasePlayer *pPlayer )
{
	// Is the item enabled?
	if ( IsDisabled() )
	{
		return false;
	}

	// Only touch a live player.
	if ( !pPlayer || !pPlayer->IsPlayer() || !pPlayer->IsAlive() )
	{
		return false;
	}

	// Team number and does it match?
	int iTeam = GetTeamNumber();
	if ( iTeam && ( pPlayer->GetTeamNumber() != iTeam ) )
	{
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFPowerup::MyTouch( CBasePlayer *pPlayer )
{
	return false;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFPowerup::InputEnable( inputdata_t &inputdata )
{
	SetDisabled( false );
}
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFPowerup::InputDisable( inputdata_t &inputdata )
{
	SetDisabled( true );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CTFPowerup::IsDisabled( void )
{
	return m_bDisabled;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFPowerup::InputToggle( inputdata_t &inputdata )
{
	if ( m_bDisabled )
	{
		SetDisabled( false );
	}
	else
	{
		SetDisabled( true );
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFPowerup::SetDisabled( bool bDisabled )
{
	m_bDisabled = bDisabled;
	if ( bDisabled )
	{
		AddEffects( EF_NODRAW );
	}
	else
	{
		// only turn it back on if we're not in the middle of respawning
		if ( !m_bRespawning )
		{
            RemoveEffects( EF_NODRAW );
		}
	}
}


