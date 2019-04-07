//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "tf_weapon_revolver.h"
#include "tf_fx_shared.h"

// Client specific.
#ifdef CLIENT_DLL 
#include "c_tf_player.h"
// Server specific.
#else
#include "tf_player.h"
#endif

//=============================================================================
//
// Weapon Revolver tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( TFRevolver, DT_WeaponRevolver )

BEGIN_NETWORK_TABLE( CTFRevolver, DT_WeaponRevolver )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFRevolver )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_revolver, CTFRevolver );
PRECACHE_WEAPON_REGISTER( tf_weapon_revolver );

IMPLEMENT_NETWORKCLASS_ALIASED( TFRevolver_Mercenary, DT_WeaponRevolver_Mercenary )

BEGIN_NETWORK_TABLE( CTFRevolver_Mercenary, DT_WeaponRevolver_Mercenary )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFRevolver_Mercenary )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_revolver_mercenary, CTFRevolver_Mercenary );
PRECACHE_WEAPON_REGISTER( tf_weapon_revolver_mercenary );

// Server specific.
#ifndef CLIENT_DLL
BEGIN_DATADESC( CTFRevolver )
END_DATADESC()
#endif

CTFRevolver::CTFRevolver()
{
//	m_bReloadsAll = true;
}

//=============================================================================
//
// Weapon Revolver functions.
//
/*
bool CTFRevolver::DefaultReload( int iClipSize1, int iClipSize2, int iActivity )
{
	m_bReloadsAll = true;
	// The the owning local player.
	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( !pPlayer )
		return false;
	return false;

//	return BaseClass::DefaultReload( iClipSize1, iClipSize2, iActivity );

}
*/