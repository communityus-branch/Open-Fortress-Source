//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#ifndef TF_WEAPON_REVOLVER_H
#define TF_WEAPON_REVOLVER_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_gun.h"

// Client specific.
#ifdef CLIENT_DLL
#define CTFRevolver C_TFRevolver
#define CTFRevolver_Mercenary C_TFRevolver_Mercenary
#endif

//=============================================================================
//
// TF Weapon Revolver.
//
class CTFRevolver : public CTFWeaponBaseGun
{
public:

	DECLARE_CLASS( CTFRevolver, CTFWeaponBaseGun );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

// Server specific.
#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif

	CTFRevolver();// {}
//	~CTFRevolver() {}


	virtual int		GetWeaponID( void ) const			{ return TF_WEAPON_REVOLVER; }

private:

	CTFRevolver( const CTFRevolver & ) {}
};

// Mercenary specific version
class CTFRevolver_Mercenary : public CTFRevolver
{
public:
	DECLARE_CLASS( CTFRevolver_Mercenary, CTFRevolver );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	virtual int		GetWeaponID( void ) const			{ return TF_WEAPON_REVOLVER_MERCENARY; }
};

#endif // TF_WEAPON_REVOLVER_H
