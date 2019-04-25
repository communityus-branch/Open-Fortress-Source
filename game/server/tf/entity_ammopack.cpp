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
#include "entity_ammopack.h"

//=============================================================================
//
// CTF AmmoPack defines.
//

#define TF_AMMOPACK_PICKUP_SOUND	"AmmoPack.Touch"

LINK_ENTITY_TO_CLASS( item_ammopack_full, CAmmoPack );
LINK_ENTITY_TO_CLASS( item_ammopack_small, CAmmoPackSmall );
LINK_ENTITY_TO_CLASS( item_ammopack_medium, CAmmoPackMedium );

BEGIN_DATADESC( CAmmoPack )

// Inputs.
DEFINE_KEYFIELD( m_iszModel, FIELD_STRING, "model" ),
DEFINE_KEYFIELD( m_iszModelOLD, FIELD_STRING, "powerup_model" ),
DEFINE_KEYFIELD( m_iszPickupSound, FIELD_STRING, "pickup_sound" ),

END_DATADESC()

BEGIN_DATADESC( CAmmoPackSmall )

// Inputs.
DEFINE_KEYFIELD( m_iszModel, FIELD_STRING, "model" ),
DEFINE_KEYFIELD( m_iszModelOLD, FIELD_STRING, "powerup_model" ),
DEFINE_KEYFIELD( m_iszPickupSound, FIELD_STRING, "pickup_sound" ),

END_DATADESC()

BEGIN_DATADESC( CAmmoPackMedium )

// Inputs.
DEFINE_KEYFIELD( m_iszModel, FIELD_STRING, "model" ),
DEFINE_KEYFIELD( m_iszModelOLD, FIELD_STRING, "powerup_model" ),
DEFINE_KEYFIELD( m_iszPickupSound, FIELD_STRING, "pickup_sound" ),

END_DATADESC()

//=============================================================================
//
// CTF AmmoPack functions.
//

//-----------------------------------------------------------------------------
// Purpose: Spawn function for the ammopack
//-----------------------------------------------------------------------------
void CAmmoPack::Spawn( void )
{
	Precache();
	if ( m_iszModel==MAKE_STRING( "" ) )
	{
		if ( m_iszModelOLD!=MAKE_STRING( "" ) )
			SetModel( STRING(m_iszModelOLD) );
		else
			SetModel( GetPowerupModel() );
	}
	else SetModel( STRING(m_iszModel) );
	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: Precache function for the ammopack
//-----------------------------------------------------------------------------
void CAmmoPack::Precache( void )
{
	if ( m_iszModel==MAKE_STRING( "" ) )
	{
		if ( m_iszModelOLD!=MAKE_STRING( "" ) )
			PrecacheModel( STRING(m_iszModelOLD) );
		else
			PrecacheModel( GetPowerupModel() );
	}
	else PrecacheModel( STRING(m_iszModel) );	
	PrecacheScriptSound( STRING(m_iszPickupSound) );
}

//-----------------------------------------------------------------------------
// Purpose: MyTouch function for the ammopack
//-----------------------------------------------------------------------------
bool CAmmoPack::MyTouch( CBasePlayer *pPlayer )
{
	bool bSuccess = false;

	if ( ValidTouch( pPlayer ) )
	{
		CTFPlayer *pTFPlayer = ToTFPlayer( pPlayer );
		if ( !pTFPlayer )
			return false;
		if ( pTFPlayer->RestockAmmo(PackRatios[GetPowerupSize()]) )
			bSuccess = true;
		int iMaxPrimary = pTFPlayer->GetPlayerClass()->GetData()->m_aAmmoMax[TF_AMMO_PRIMARY];
		if ( pPlayer->GiveAmmo( ceil(iMaxPrimary * PackRatios[GetPowerupSize()]), TF_AMMO_PRIMARY, true ) )
		{
			bSuccess = true;
		}

		int iMaxSecondary = pTFPlayer->GetPlayerClass()->GetData()->m_aAmmoMax[TF_AMMO_SECONDARY];
		if ( pPlayer->GiveAmmo( ceil(iMaxSecondary * PackRatios[GetPowerupSize()]), TF_AMMO_SECONDARY, true ) )
		{
			bSuccess = true;
		}

		int iMaxMetal = pTFPlayer->GetPlayerClass()->GetData()->m_aAmmoMax[TF_AMMO_METAL];
		if ( pPlayer->GiveAmmo( ceil(iMaxMetal * PackRatios[GetPowerupSize()]), TF_AMMO_METAL, true ) )
		{
			bSuccess = true;
		}

		///
		int iMaxShotgun = pTFPlayer->GetPlayerClass()->GetData()->m_aAmmoMax[TF_AMMO_SHOTGUN];
		if (pPlayer->GiveAmmo(ceil(iMaxShotgun * PackRatios[GetPowerupSize()]), TF_AMMO_SHOTGUN, true))
		{
			bSuccess = true;
		}

		int iMaxScatter = pTFPlayer->GetPlayerClass()->GetData()->m_aAmmoMax[TF_AMMO_SCATTERGUN];
		if (pPlayer->GiveAmmo(ceil(iMaxScatter * PackRatios[GetPowerupSize()]), TF_AMMO_SCATTERGUN, true))
		{
			bSuccess = true;
		}

		int iMaxSuperSG = pTFPlayer->GetPlayerClass()->GetData()->m_aAmmoMax[TF_AMMO_SUPERSHOTGUN];
		if (pPlayer->GiveAmmo(ceil(iMaxSuperSG * PackRatios[GetPowerupSize()]), TF_AMMO_SUPERSHOTGUN, true))
		{
			bSuccess = true;
		}

		int iMaxNails = pTFPlayer->GetPlayerClass()->GetData()->m_aAmmoMax[TF_AMMO_NAILS];
		if (pPlayer->GiveAmmo(ceil(iMaxNails * PackRatios[GetPowerupSize()]), TF_AMMO_NAILS, true))
		{
			bSuccess = true;
		}

		int iMaxPistol = pTFPlayer->GetPlayerClass()->GetData()->m_aAmmoMax[TF_AMMO_PISTOL];
		if (pPlayer->GiveAmmo(ceil(iMaxPistol * PackRatios[GetPowerupSize()]), TF_AMMO_PISTOL, true))
		{
			bSuccess = true;
		}

		int iMaxSMG = pTFPlayer->GetPlayerClass()->GetData()->m_aAmmoMax[TF_AMMO_SMG];
		if (pPlayer->GiveAmmo(ceil(iMaxSMG * PackRatios[GetPowerupSize()]), TF_AMMO_SMG, true))
		{
			bSuccess = true;
		}

		int iMaxSniper = pTFPlayer->GetPlayerClass()->GetData()->m_aAmmoMax[TF_AMMO_SNIPERRIFLE];
		if (pPlayer->GiveAmmo(ceil(iMaxSniper * PackRatios[GetPowerupSize()]), TF_AMMO_SNIPERRIFLE, true))
		{
			bSuccess = true;
		}

		int iMaxMini = pTFPlayer->GetPlayerClass()->GetData()->m_aAmmoMax[TF_AMMO_MINIGUN];
		if (pPlayer->GiveAmmo(ceil(iMaxMini * PackRatios[GetPowerupSize()]), TF_AMMO_MINIGUN, true))
		{
			bSuccess = true;
		}

		int iMaxFire = pTFPlayer->GetPlayerClass()->GetData()->m_aAmmoMax[TF_AMMO_FLAMETHROWER];
		if (pPlayer->GiveAmmo(ceil(iMaxFire * PackRatios[GetPowerupSize()]), TF_AMMO_FLAMETHROWER, true))
		{
			bSuccess = true;
		}

		int iMaxRevolver = pTFPlayer->GetPlayerClass()->GetData()->m_aAmmoMax[TF_AMMO_REVOLVER];
		if (pPlayer->GiveAmmo(ceil(iMaxRevolver * PackRatios[GetPowerupSize()]), TF_AMMO_REVOLVER, true))
		{
			bSuccess = true;
		}

		int iMaxGL = pTFPlayer->GetPlayerClass()->GetData()->m_aAmmoMax[TF_AMMO_GRENADELAUNCHER];
		if (pPlayer->GiveAmmo(ceil(iMaxGL * PackRatios[GetPowerupSize()]), TF_AMMO_GRENADELAUNCHER, true))
		{
			bSuccess = true;
		}

		int iMaxPipe = pTFPlayer->GetPlayerClass()->GetData()->m_aAmmoMax[TF_AMMO_PIPEBOMBLAUNCHER];
		if (pPlayer->GiveAmmo(ceil(iMaxPipe * PackRatios[GetPowerupSize()]), TF_AMMO_PIPEBOMBLAUNCHER, true))
		{
			bSuccess = true;
		}

		int iMaxSynringe = pTFPlayer->GetPlayerClass()->GetData()->m_aAmmoMax[TF_AMMO_SYNRINGES];
		if (pPlayer->GiveAmmo(ceil(iMaxSynringe * PackRatios[GetPowerupSize()]), TF_AMMO_SYNRINGES, true))
		{
			bSuccess = true;
		}

		int iMaxRPG = pTFPlayer->GetPlayerClass()->GetData()->m_aAmmoMax[TF_AMMO_ROCKETLAUNCHER];
		if (pPlayer->GiveAmmo(ceil(iMaxRPG * PackRatios[GetPowerupSize()]), TF_AMMO_ROCKETLAUNCHER, true))
		{
			bSuccess = true;
		}

		int iMaxRail = pTFPlayer->GetPlayerClass()->GetData()->m_aAmmoMax[TF_AMMO_RAILGUN];
		if (pPlayer->GiveAmmo(ceil(iMaxRail * PackRatios[GetPowerupSize()]), TF_AMMO_RAILGUN, true))
		{
			bSuccess = true;
		}
		///

		// did we give them anything?
		if ( bSuccess )
		{
			CSingleUserRecipientFilter filter( pPlayer );
			EmitSound( filter, entindex(), STRING(m_iszPickupSound) );
			AddEffects( EF_NODRAW );
		}
	}

	return bSuccess;
}


