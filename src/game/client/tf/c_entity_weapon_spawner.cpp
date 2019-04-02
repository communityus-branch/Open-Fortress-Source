//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: Deathmatch weapon spawner
//
//=============================================================================//

#include "cbase.h"
#include "c_tf_player.h"
#include "view.h"
#include "tier0/memdbgon.h"
#include "glow_outline_effect.h"
#include "tf_gamerules.h"

//-----------------------------------------------------------------------------
// Purpose: Spawn function for the Weapon Spawner
//-----------------------------------------------------------------------------
class C_WeaponSpawner : public C_BaseAnimating
{
public:
	DECLARE_CLASS( C_WeaponSpawner, C_BaseAnimating );
	DECLARE_CLIENTCLASS();

	void	ClientThink(void);
	void	Spawn(void);

private:

	QAngle absAngle;
	~C_WeaponSpawner();

	private:

	CGlowObject		   *m_pGlowEffect;
	void	UpdateGlowEffect( void );
	void	DestroyGlowEffect(void);
	bool	m_bDisableSpin;
	bool	m_bDisableShowOutline;
};

// Inputs.
LINK_ENTITY_TO_CLASS( dm_weapon_spawner, C_WeaponSpawner );

IMPLEMENT_CLIENTCLASS_DT( C_WeaponSpawner, DT_WeaponSpawner, CWeaponSpawner )
RecvPropBool( RECVINFO( m_bDisableSpin ) ),
RecvPropBool( RECVINFO( m_bDisableShowOutline ) ),
END_RECV_TABLE()

//-----------------------------------------------------------------------------
// Purpose: Set initial angles 
//-----------------------------------------------------------------------------
void C_WeaponSpawner::Spawn(void)
{
	BaseClass::Spawn();
	absAngle = GetAbsAngles();

	UpdateGlowEffect();

	ClientThink();
}

//-----------------------------------------------------------------------------
// Purpose: Update angles every think
//-----------------------------------------------------------------------------
void C_WeaponSpawner::ClientThink( void )
{	
	if ( !m_bDisableSpin )
	{
		absAngle.y += 90 * gpGlobals->frametime;
		if (absAngle.y >= 360)
			absAngle.y -= 360;

		SetAbsAngles(absAngle);
	}
	
	UpdateGlowEffect();

	SetNextClientThink(CLIENT_THINK_ALWAYS);
}


//-----------------------------------------------------------------------------
// Purpose: Update glow effect
//-----------------------------------------------------------------------------
void C_WeaponSpawner::UpdateGlowEffect( void )
{
	DestroyGlowEffect();
	
	if ( !m_bDisableShowOutline )
		m_pGlowEffect = new CGlowObject(this, TFGameRules()->GetTeamGlowColor(GetLocalPlayerTeam()), 1.0, true, true);
}

void C_WeaponSpawner::DestroyGlowEffect(void)
{
	if ( m_pGlowEffect )
	{
		delete m_pGlowEffect;
		m_pGlowEffect = NULL;
	}
}

C_WeaponSpawner::~C_WeaponSpawner()
{
	delete m_pGlowEffect;
	m_pGlowEffect = NULL;
}