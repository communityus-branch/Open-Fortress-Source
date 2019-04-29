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
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();
	
	CCondPowerup();

	void	Spawn( void );
	virtual CBaseEntity* Respawn( void );
	void	Precache( void );
	bool	MyTouch( CBasePlayer *pPlayer );

	powerupsize_t	GetPowerupSize( void ) { return POWERUP_FULL; }
	int m_bCondition=5;
	float m_bCondDuration=5;
	string_t m_iszPowerupModel=MAKE_STRING( "" );
	string_t m_iszPowerupModelOLD=MAKE_STRING( "" );
	string_t m_iszPickupSound=MAKE_STRING( "AmmoPack.Touch" );

	CNetworkVar(bool, m_bDisableShowOutline);

	IMPLEMENT_NETWORK_VAR_FOR_DERIVED( m_bRespawning );
private:
	CNetworkVar( float, m_flRespawnTick );
};

#endif // ENTITY_WEAPON_SPAWNER_H


