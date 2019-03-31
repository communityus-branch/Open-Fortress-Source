//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: CTF AmmoPack.
//
//=============================================================================//

#include "cbase.h"
#include "tf_player.h"
#include "tf_team.h"
#include "engine/IEngineSound.h"
#include "entity_condpowerup.h"

#include "tier0/memdbgon.h"

//=============================================================================
//
// CTF AmmoPack defines.
//

//#define TF_WEAPON_PICKUP_SOUND		"AmmoPack.Touch"

//-----------------------------------------------------------------------------
// Purpose: Spawn function for the ammopack
//-----------------------------------------------------------------------------


BEGIN_DATADESC( CCondPowerup )

// Inputs.
DEFINE_KEYFIELD( m_bCondition, FIELD_INTEGER, "condID" ),
DEFINE_KEYFIELD( m_bCondDuration, FIELD_FLOAT, "duration" ),
DEFINE_KEYFIELD( m_iszPowerupModel, FIELD_STRING, "powerup_model" ),
DEFINE_KEYFIELD( m_iszPickupSound, FIELD_STRING, "pickup_sound" ),

END_DATADESC()

LINK_ENTITY_TO_CLASS( dm_powerup_spawner, CCondPowerup );

void CCondPowerup::Spawn( void )
{
	Precache();
	SetModel( STRING(m_iszPowerupModel) );

	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: Precache function for the ammopack
//-----------------------------------------------------------------------------
void CCondPowerup::Precache( void )
{
	PrecacheModel( STRING( m_iszPowerupModel ) );
	PrecacheScriptSound( STRING( m_iszPickupSound) );
}

//-----------------------------------------------------------------------------
// Purpose: MyTouch function for the ammopack
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

		// did we give them anything?
		if ( bSuccess )
		{
			pTFPlayer->m_Shared.AddCond( m_bCondition , m_bCondDuration );
			EmitSound( STRING(m_iszPickupSound) );
		}
	}
	return bSuccess;

}