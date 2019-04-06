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

//#define TF_WEAPON_PICKUP_SOUND		"AmmoPack.Touch"

extern ConVar ofd_instagib;

//-----------------------------------------------------------------------------
// Purpose: Spawn function for the Weapon Spawner
//-----------------------------------------------------------------------------


BEGIN_DATADESC(CWeaponSpawner)

// Inputs.
DEFINE_KEYFIELD(m_iszWeaponName, FIELD_STRING, "weaponname"),
DEFINE_KEYFIELD(m_iszWeaponModel, FIELD_STRING, "powerup_model"),
DEFINE_KEYFIELD(m_iszPickupSound, FIELD_STRING, "pickup_sound"),
DEFINE_KEYFIELD( m_bDisableSpin, FIELD_BOOLEAN, "disable_spin"),
DEFINE_KEYFIELD(m_bDisableShowOutline, FIELD_BOOLEAN, "disable_glow"),

END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CWeaponSpawner, DT_WeaponSpawner )
	SendPropBool( SENDINFO( m_bDisableSpin ) ),
	SendPropBool( SENDINFO( m_bDisableShowOutline ) ),
	SendPropBool( SENDINFO( m_bRespawning ) ),
END_SEND_TABLE()

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
	PrecacheScriptSound( STRING( m_iszPickupSound) );
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
					EmitSound( filter, entindex(), STRING(m_iszPickupSound) );
					m_nRenderFX = kRenderFxDistort;
					return bSuccess;
				}
				int iMaxSecondary = pTFPlayer->GetPlayerClass()->GetData()->m_aAmmoMax[TF_AMMO_SECONDARY];
				if (  pPlayer->GiveAmmo( ceil(iMaxSecondary * 0.5), TF_AMMO_SECONDARY, true ) )
				{
					CSingleUserRecipientFilter filter( pPlayer );
					EmitSound( filter, entindex(), STRING(m_iszPickupSound) );
					m_nRenderFX = kRenderFxDistort;
					return bSuccess;
				}
				bSuccess=false;
			}
		}
		// did we give them anything?
		if ( bSuccess )
		{
		
			CSingleUserRecipientFilter filter( pPlayer );
			EmitSound( filter, entindex(), STRING(m_iszPickupSound) );

			pWeapon->GiveTo( pPlayer );
			m_nRenderFX = kRenderFxDistort;
		}
	}
	return bSuccess;

}