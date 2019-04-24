//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#ifndef TF_WEAPON_BOTTLE_H
#define TF_WEAPON_BOTTLE_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_melee.h"

#ifdef CLIENT_DLL
#define CTFBottle C_TFBottle
#define CTFSword C_TFSword
#endif

//=============================================================================
//
// Bottle class.
//
class CTFBottle : public CTFWeaponBaseMelee
{
public:

	DECLARE_CLASS( CTFBottle, CTFWeaponBaseMelee );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CTFBottle();
	virtual int			GetWeaponID( void ) const			{ return TF_WEAPON_BOTTLE; }

	virtual void		Smack( void );
	virtual void		WeaponReset( void );
	virtual bool		DefaultDeploy( char *szViewModel, char *szWeaponModel, int iActivity, char *szAnimExt );

	void				SwitchBodyGroups( void );

private:

	CTFBottle( const CTFBottle & ) {}
	CNetworkVar( bool,	m_bBroken  );
};

class CTFSword : public CTFBottle
{
public:

	DECLARE_CLASS( CTFSword, CTFBottle );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CTFSword() {}
	virtual int			GetWeaponID( void ) const			{ return TF_WEAPON_SWORD; }

private:

	CTFSword( const CTFSword & ) {}
};


#endif // TF_WEAPON_BOTTLE_H
