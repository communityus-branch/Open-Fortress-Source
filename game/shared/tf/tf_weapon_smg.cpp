//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
//
//=============================================================================
#include "cbase.h"
#include "tf_weapon_smg.h"
#include "in_buttons.h"
#include "tf_weaponbase.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"

// Server specific.
#else
#include "tf_player.h"
#include "util.h"
#endif

//=============================================================================
//
// Weapon SMG tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( TFSMG, DT_WeaponSMG )

BEGIN_NETWORK_TABLE( CTFSMG, DT_WeaponSMG )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFSMG )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_smg, CTFSMG );
PRECACHE_WEAPON_REGISTER( tf_weapon_smg );

IMPLEMENT_NETWORKCLASS_ALIASED( TFSMG_Mercenary, DT_WeaponSMG_Mercenary )

BEGIN_NETWORK_TABLE( CTFSMG_Mercenary, DT_WeaponSMG_Mercenary )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFSMG_Mercenary )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_smg_mercenary, CTFSMG_Mercenary );
PRECACHE_WEAPON_REGISTER( tf_weapon_smg_mercenary );

IMPLEMENT_NETWORKCLASS_ALIASED( TFTommyGun, DT_WeaponTommyGun )

BEGIN_NETWORK_TABLE( CTFTommyGun, DT_WeaponTommyGun )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFTommyGun )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_tommygun, CTFTommyGun );
PRECACHE_WEAPON_REGISTER( tf_weapon_tommygun );

IMPLEMENT_NETWORKCLASS_ALIASED(TFAR, DT_WeaponAR)

BEGIN_NETWORK_TABLE(CTFAR, DT_WeaponAR)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CTFAR)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(tf_weapon_ar, CTFAR);
PRECACHE_WEAPON_REGISTER(tf_weapon_ar);

// Server specific.
#ifndef CLIENT_DLL
BEGIN_DATADESC( CTFSMG )
END_DATADESC()
#endif

//=============================================================================
//
// Weapon SMG functions.
//
CTFSMG::CTFSMG( void )
{
//	m_bReloadsAll = true;
}


