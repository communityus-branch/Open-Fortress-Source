//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: CTF AmmoPack.
//
//=============================================================================//
#ifndef TF_POWERUP_H
#define TF_POWERUP_H

#ifdef _WIN32
#pragma once
#endif

#include "items.h"

enum powerupsize_t
{
	POWERUP_SMALL,
	POWERUP_MEDIUM,
	POWERUP_FULL,

	POWERUP_SIZES,
};

extern float PackRatios[POWERUP_SIZES];

//=============================================================================
//
// CTF Powerup class.
//

class CTFPowerup : public CItem
{
public:
	DECLARE_CLASS( CTFPowerup, CItem );

	CTFPowerup();

	void			Spawn( void );
	CBaseEntity*	Respawn( void );
	void			Materialize( void );
	virtual bool	ValidTouch( CBasePlayer *pPlayer );
	virtual bool	MyTouch( CBasePlayer *pPlayer );

	bool			IsDisabled( void );
	void			SetDisabled( bool bDisabled );

	virtual float	GetRespawnDelay( void ) { return g_pGameRules->FlItemRespawnTime( this ); }
	float fl_RespawnTime=-1;

	// Input handlers
	void			InputEnable( inputdata_t &inputdata );
	void			InputDisable( inputdata_t &inputdata );
	void			InputToggle( inputdata_t &inputdata );

	virtual powerupsize_t	GetPowerupSize( void ) { return POWERUP_FULL; }
	string_t m_iszSpawnSound=MAKE_STRING( "Item.Materialize" );

private:
	bool			m_bDisabled;
	bool			m_bRespawning;

	DECLARE_DATADESC();
};

#endif // TF_POWERUP_H


