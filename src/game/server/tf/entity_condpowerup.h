//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: CTF AmmoPack.
//
//=============================================================================//
#ifndef ENTITY_COND_POWERUP_H
#define ENTITY_COND_POWERUP_H

#ifdef _WIN32
#pragma once
#endif

#include "tf_powerup.h"

//=============================================================================
//
// CTF WeaponSpawner class.
//

class CCondPowerup : public CTFPowerup
{
public:
	DECLARE_CLASS( CCondPowerup, CTFPowerup );

	void	Spawn( void );
	void	Precache( void );
	bool	MyTouch( CBasePlayer *pPlayer );

	powerupsize_t	GetPowerupSize( void ) { return POWERUP_FULL; }
	int m_bCondition=5;
	float m_bCondDuration=5;
	string_t m_iszPowerupModel=MAKE_STRING( "models/pickups/pickup_powerup_uber.mdl" );
	DECLARE_DATADESC();
};

#endif // ENTITY_WEAPON_SPAWNER_H


