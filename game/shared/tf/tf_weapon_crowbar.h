//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#ifndef TF_WEAPON_CROWBAR_H
#define TF_WEAPON_CROWBAR_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_melee.h"

#ifdef CLIENT_DLL
#define CTFCrowbar C_TFCrowbar
#define CTFUmbrella C_TFUmbrella
#endif

//=============================================================================
//
// Crowbar class.
//
class CTFCrowbar : public CTFWeaponBaseMelee
{
public:

	DECLARE_CLASS( CTFCrowbar, CTFWeaponBaseMelee );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CTFCrowbar();
	virtual int			GetWeaponID( void ) const			{ return TF_WEAPON_CROWBAR; }

private:

	CTFCrowbar( const CTFCrowbar & ) {}
};

class CTFUmbrella : public CTFWeaponBaseMelee
{
public:

	DECLARE_CLASS( CTFUmbrella, CTFWeaponBaseMelee );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CTFUmbrella();
	virtual int			GetWeaponID( void ) const			{ return TF_WEAPON_UMBRELLA; }

private:

	CTFUmbrella( const CTFUmbrella & ) {}
};

#endif // TF_WEAPON_CROWBAR_H
