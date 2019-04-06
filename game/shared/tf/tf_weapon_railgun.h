//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: TF Sniper Rifle
//
//=============================================================================//
#ifndef TF_WEAPON_RAILGUN_H
#define TF_WEAPON_RAILGUN_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_gun.h"
#include "Sprite.h"

#if defined( CLIENT_DLL )
#define CTFRailgun C_TFRailgun
#define CRailgunDot C_RailgunDot
#endif

//=============================================================================
//
// Sniper Rifle Laser Dot class.
//


class CRailgunDot : public CBaseEntity
{
public:

	DECLARE_CLASS( CRailgunDot, CBaseEntity );
	DECLARE_NETWORKCLASS();
	DECLARE_DATADESC();

	// Creation/Destruction.
	CRailgunDot( void );
	~CRailgunDot( void );

	static CRailgunDot *Create( const Vector &origin, CBaseEntity *pOwner = NULL, bool bVisibleDot = true );
	void		ResetChargeTime( void ) { m_flChargeStartTime = gpGlobals->curtime; }

	// Attributes.
	int			ObjectCaps()							{ return (BaseClass::ObjectCaps() & ~FCAP_ACROSS_TRANSITION) | FCAP_DONT_SAVE; }

	// Targeting.
	void        Update( CBaseEntity *pTarget, const Vector &vecOrigin, const Vector &vecNormal );
	CBaseEntity	*GetTargetEntity( void )				{ return m_hTargetEnt; }

// Client specific.
#ifdef CLIENT_DLL

	// Rendering.
	virtual bool			IsTransparent( void )		{ return true; }
	virtual RenderGroup_t	GetRenderGroup( void )		{ return RENDER_GROUP_TRANSLUCENT_ENTITY; }
	virtual int				DrawModel( int flags );
	virtual bool			ShouldDraw( void );

	//
	virtual void			OnDataChanged( DataUpdateType_t updateType );

	CMaterialReference		m_hSpriteMaterial;

#endif

protected:

	Vector					m_vecSurfaceNormal;
	EHANDLE					m_hTargetEnt;

	CNetworkVar( float, m_flChargeStartTime );
};

//=============================================================================
//
// Sniper Rifle class.
//
class CTFRailgun: public CTFWeaponBaseGun
{
public:

	DECLARE_CLASS( CTFRailgun, CTFWeaponBaseGun );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CTFRailgun();
	~CTFRailgun();

	virtual int	GetWeaponID( void ) const			{ return TF_WEAPON_RAILGUN; }

	virtual void Spawn();
	virtual void Precache();
	void		 ResetTimers( void );

	virtual bool Reload( void );
	virtual bool CanHolster( void ) const;
	virtual bool Holster( CBaseCombatWeapon *pSwitchingTo );

	void		 HandleZooms( void );
	virtual void ItemPostFrame( void );
	virtual bool Lower( void );
	virtual float GetProjectileDamage( void );
	virtual int	GetDamageType() const;

	virtual void WeaponReset( void );

	virtual bool CanFireCriticalShot( bool bIsHeadshot = false );

#ifdef CLIENT_DLL
	float GetHUDDamagePerc( void );
#endif

	bool IsZoomed( void );

private:

	void CreateRailgunDot( void );
	void DestroyRailgunDot( void );
	void UpdateRailgunDot( void );

private:
	// Auto-rezooming handling
	void SetRezoom( bool bRezoom, float flDelay );

	void Zoom( void );
	void ZoomOutIn( void );
	void ZoomIn( void );
	void ZoomOut( void );
	void Fire( CTFPlayer *pPlayer );

private:

	CNetworkVar( float,	m_flChargedDamage );

#ifdef GAME_DLL
	CHandle<CRailgunDot>		m_hRailgunDot;
#endif

	// Handles rezooming after the post-fire unzoom
	float m_flUnzoomTime;
	float m_flRezoomTime;
	bool m_bRezoomAfterShot;

	CTFRailgun( const CTFRailgun & );
};

#endif // TF_WEAPON_RAILGUN_H
