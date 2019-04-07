//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "tf_weapon_crowbar.h"
#include "decals.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
// Server specific.
#else
#include "tf_player.h"
#endif

//=============================================================================
//
// Weapon Crowbar tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( TFCrowbar, DT_TFWeaponCrowbar )

BEGIN_NETWORK_TABLE( CTFCrowbar, DT_TFWeaponCrowbar )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFCrowbar )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_crowbar, CTFCrowbar );
PRECACHE_WEAPON_REGISTER( tf_weapon_crowbar );

IMPLEMENT_NETWORKCLASS_ALIASED( TFUmbrella, DT_TFWeaponUmbrella )

BEGIN_NETWORK_TABLE( CTFUmbrella, DT_TFWeaponUmbrella )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFUmbrella )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_umbrella, CTFUmbrella );
PRECACHE_WEAPON_REGISTER( tf_weapon_umbrella );

//=============================================================================
//
// Weapon Crowbar functions.
//

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFCrowbar::CTFCrowbar()
{
}
CTFUmbrella::CTFUmbrella()
{
}
