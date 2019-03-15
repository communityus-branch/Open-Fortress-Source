//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: CTF AmmoPack.
//
//=============================================================================//
#ifndef ENTITY_WEAPON_SPAWNER_H
#define ENTITY_WEAPON_SPAWNER_H

#ifdef _WIN32
#pragma once
#endif

#include "tf_powerup.h"

//=============================================================================
//
// CTF WeaponSpawner class.
//

class CWeaponSpawner : public CTFPowerup
{
public:
	DECLARE_CLASS( CWeaponSpawner, CTFPowerup );
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();
	
	CWeaponSpawner();

	void	Spawn( void );
	void	Precache( void );
	virtual bool KeyValue( const char *szKeyName, const char *szValue );
	bool	MyTouch( CBasePlayer *pPlayer );

	powerupsize_t	GetPowerupSize( void ) { return POWERUP_FULL; }
	virtual const char *GetPowerupModel( void ) { return "models/items/ammopack_large.mdl"; }
};

#endif // ENTITY_WEAPON_SPAWNER_H


