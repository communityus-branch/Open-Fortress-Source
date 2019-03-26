//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: CTF AmmoPack.
//
//=============================================================================//

#include "cbase.h"
#include "tf_player.h"
#include "tf_weaponbase.h"
#include "tf_shareddefs.h"
#include "tf_weapon_builder.h"
#include "tf_gamerules.h"
#include "tf_team.h"
#include "engine/IEngineSound.h"
#include "entity_weapon_spawner.h"

#include "tier0/memdbgon.h"

#define TF_WEAPON_PICKUP_SOUND		"AmmoPack.Touch"

extern ConVar ofd_instagib;

//-----------------------------------------------------------------------------
// Purpose: Spawn function for the Weapon Spawner
//-----------------------------------------------------------------------------


BEGIN_DATADESC( CWeaponSpawner )

// Inputs.
	DEFINE_KEYFIELD( m_iszWeaponName, FIELD_STRING, "weaponname" ),
	DEFINE_KEYFIELD( m_iszWeaponModel, FIELD_STRING, "powerup_model" ),
//	DEFINE_KEYFIELD( m_bSpin, FIELD_BOOLEAN, "spin" ),

END_DATADESC()

LINK_ENTITY_TO_CLASS( dm_weapon_spawner, CWeaponSpawner );

void CWeaponSpawner::Spawn( void )
{
	m_nRenderFX = kRenderFxNone;
	if ( ofd_instagib.GetInt() <= 0 ) {
	Precache();
	SetModel( STRING(m_iszWeaponModel) );
	BaseClass::Spawn();
	}
}
//-----------------------------------------------------------------------------
// Purpose: Precache function for the ammopack
//-----------------------------------------------------------------------------
void CWeaponSpawner::Precache( void )
{
	PrecacheModel( STRING(m_iszWeaponModel) );
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
	
		
//		CTFWeaponBase *pSpawnedWeapon = GiveNamedItem( STRING(m_iszWeaponName) );
//		if ( pWeapon && pWeapon->GetWeaponID() != pSpawnedWeapon->GetWeaponID()  )
//		{
//			UTIL_Remove( pWeapon );
//		}
		bSuccess = true;
		CTFWeaponBase *pWeapon = (CTFWeaponBase *)pPlayer->GiveNamedItem( STRING(m_iszWeaponName) );
		
		for ( int iWeapon = 0; iWeapon < TF_WEAPON_COUNT; ++iWeapon )
		{		
			CTFWeaponBase *pCarriedWeapon = (CTFWeaponBase *)pPlayer->GetWeapon( iWeapon );
			if ( pCarriedWeapon == pWeapon ) 
			{
				
				
				int iMaxPrimary = pTFPlayer->GetPlayerClass()->GetData()->m_aAmmoMax[TF_AMMO_PRIMARY];
				if ( pPlayer->GiveAmmo( ceil(iMaxPrimary * 0.5), TF_AMMO_PRIMARY, true ) )
				{
					CSingleUserRecipientFilter filter( pPlayer );
					EmitSound( filter, entindex(), TF_WEAPON_PICKUP_SOUND );
					return bSuccess;
				}
				int iMaxSecondary = pTFPlayer->GetPlayerClass()->GetData()->m_aAmmoMax[TF_AMMO_SECONDARY];
				if (  pPlayer->GiveAmmo( ceil(iMaxSecondary * 0.5), TF_AMMO_SECONDARY, true ) )
				{
					CSingleUserRecipientFilter filter( pPlayer );
					EmitSound( filter, entindex(), TF_WEAPON_PICKUP_SOUND );
					return bSuccess;
				}
				bSuccess=false;
			}
		}
		// did we give them anything?
		if ( bSuccess )
		{
		
			CSingleUserRecipientFilter filter( pPlayer );
			EmitSound( filter, entindex(), TF_WEAPON_PICKUP_SOUND );

			pWeapon->GiveTo( pPlayer );
		}
	}
	return bSuccess;

}