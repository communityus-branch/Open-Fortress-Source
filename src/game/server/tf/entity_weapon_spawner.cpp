//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: CTF AmmoPack.
//
//=============================================================================//

#include "cbase.h"
#include "tf_gamerules.h"
#include "tf_shareddefs.h"
#include "tf_player.h"
#include "tf_team.h"
#include "engine/IEngineSound.h"
#include "entity_weapon_spawner.h"
#include "tf_weaponbase.h"
#include "basecombatcharacter.h"
#include "in_buttons.h"
#include "tf_fx.h"
#include "items.h"
#include "tf_viewmodel.h"

//=============================================================================
//
// CTF AmmoPack defines.
//

#define TF_WEAPON_PICKUP_SOUND		"AmmoPack.Touch"
#define TF_WEAPON_MODEL				"models/weapons/w_models/w_scattergun.mdl"
#define TF_WEAPON_ID				"tf_weapon_scattergun"

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
	SetModel( TF_WEAPON_MODEL );

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
/*
			CTFWeaponBase *pWeapon = (CTFWeaponBase *)GetWeapon( iWeapon );
			
			if ( pWeapon && pWeapon->GetWeaponID() != TF_WEAPON_ID )
			{
				Weapon_Detach( pWeapon );
				UTIL_Remove( pWeapon );
			}
			
			pWeapon = (CTFWeaponBase *)Weapon_OwnsThisID( TF_WEAPON_ID );		
			
			if ( pWeapon )
			{
				pWeapon->ChangeTeam( GetTeamNumber() );
				pWeapon->GiveDefaultAmmo();
	
				if ( m_bRegenerating == false )
				{
					pWeapon->WeaponReset();
				}
			}
			else
			{
				pWeapon = (CTFWeaponBase *)GiveNamedItem( pszWeaponName );

				if ( pWeapon )
				{
					pWeapon->DefaultTouch( this );
				}
			}

			if ( m_bRegenerating == false )
			{
				SetActiveWeapon( NULL );
				Weapon_Switch( Weapon_GetSlot( 0 ) );
				Weapon_SetLast( Weapon_GetSlot( 1 ) );
			}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// */
		}
	}
	return bSuccess;
}
