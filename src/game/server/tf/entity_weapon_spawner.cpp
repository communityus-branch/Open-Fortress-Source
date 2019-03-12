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
#include "entity_weapon_spawner.h"

//=============================================================================
//
// CTF AmmoPack defines.
//

#define TF_WEAPON_PICKUP_SOUND		"AmmoPack.Touch"
#define TF_WEAPON_MODEL				"models/weapons/w_models/w_scattergun.mdl"
#define TF_WEAPON_ID				"tf_weapon_scattergun"

LINK_ENTITY_TO_CLASS( dm_weapon_spawner, CWeaponSpawner );

//=============================================================================
//
// CTF AmmoPack functions.
//

//-----------------------------------------------------------------------------
// Purpose: Spawn function for the ammopack
//-----------------------------------------------------------------------------
void CWeaponSpawner::Spawn( void )
{
	Precache();
	SetModel( GetPowerupModel() );

	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: Precache function for the ammopack
//-----------------------------------------------------------------------------
void CWeaponSpawner::Precache( void )
{
	PrecacheModel( GetPowerupModel() );
	PrecacheScriptSound( TF_WEAPON_PICKUP_SOUND );
}

//-----------------------------------------------------------------------------
// Purpose: MyTouch function for the ammopack
//-----------------------------------------------------------------------------
bool CWeaponSpawner::MyTouch( CBasePlayer *pPlayer )
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
			CSingleUserRecipientFilter filter( pPlayer );
			EmitSound( filter, entindex(), TF_WEAPON_PICKUP_SOUND );
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			char item_to_give[ 256 ];
			Q_strncpy( item_to_give, TF_WEAPON_ID , sizeof( item_to_give ) );
			Q_strlower( item_to_give );

			string_t iszItem = AllocPooledString( item_to_give );	// Make a copy of the classname
			pPlayer->GiveNamedItem( STRING(iszItem) );
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		}
	}
	return bSuccess;
}
