//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// TF Nail
//
//=============================================================================
#include "cbase.h"
#include "tf_projectile_nail.h"

#ifdef CLIENT_DLL
#include "c_basetempentity.h"
#include "c_te_legacytempents.h"
#include "c_te_effect_dispatch.h"
#include "input.h"
#include "c_tf_player.h"
#include "cliententitylist.h"
#endif

//=============================================================================
//
// TF Syringe Projectile functions (Server specific).
//
#define SYRINGE_MODEL				"models/weapons/w_models/w_syringe_proj.mdl"
#define SYRINGE_DISPATCH_EFFECT		"ClientProjectile_Syringe"
#define SYRINGE_GRAVITY	0.3f

LINK_ENTITY_TO_CLASS( tf_projectile_syringe, CTFProjectile_Syringe );
PRECACHE_REGISTER( tf_projectile_syringe );

short g_sModelIndexSyringe;
void PrecacheSyringe(void *pUser)
{
	g_sModelIndexSyringe = modelinfo->GetModelIndex( SYRINGE_MODEL );
}

PRECACHE_REGISTER_FN(PrecacheSyringe);

CTFProjectile_Syringe::CTFProjectile_Syringe()
{
}

CTFProjectile_Syringe::~CTFProjectile_Syringe()
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFProjectile_Syringe *CTFProjectile_Syringe::Create( const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, CBaseEntity *pScorer, bool bCritical )
{
	return static_cast<CTFProjectile_Syringe*>( CTFBaseProjectile::Create( "tf_projectile_syringe", vecOrigin, vecAngles, pOwner, CTFProjectile_Syringe::GetInitialVelocity(), g_sModelIndexSyringe, SYRINGE_DISPATCH_EFFECT, pScorer, bCritical ) );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
const char *CTFProjectile_Syringe::GetProjectileModelName( void )
{
	return SYRINGE_MODEL;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
float CTFProjectile_Syringe::GetGravity( void )
{
	return SYRINGE_GRAVITY;
}

#ifdef CLIENT_DLL

//-----------------------------------------------------------------------------
// Purpose: 
// Output : const char
//-----------------------------------------------------------------------------
const char *GetSyringeTrailParticleName( int iTeamNumber, bool bCritical )
{
	if ( iTeamNumber == TF_TEAM_BLUE )
	{
		return ( bCritical ? "nailtrails_medic_blue_crit" : "nailtrails_medic_blue" );
	}
	else if ( iTeamNumber == TF_TEAM_RED )
	{
		return ( bCritical ? "nailtrails_medic_red_crit" : "nailtrails_medic_red" );
	}
	else 
	{
		return ( bCritical ? "nailtrails_medic_mercenary_crit" : "nailtrails_medic_mercenary" );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void ClientsideProjectileSyringeCallback( const CEffectData &data )
{
	// Get the syringe and add it to the client entity list, so we can attach a particle system to it.
	C_TFPlayer *pPlayer = dynamic_cast<C_TFPlayer*>( ClientEntityList().GetBaseEntityFromHandle( data.m_hEntity ) );
	if ( pPlayer )
	{
		C_LocalTempEntity *pSyringe = ClientsideProjectileCallback( data, SYRINGE_GRAVITY );
		if ( pSyringe )
		{
			if ( pPlayer->GetTeamNumber() == TF_TEAM_RED ) pSyringe->m_nSkin = 0;
			else if  (pPlayer->GetTeamNumber() == TF_TEAM_BLUE ) pSyringe->m_nSkin = 1;
			else pSyringe->m_nSkin = 2;
			bool bCritical = ( ( data.m_nDamageType & DMG_CRITICAL ) != 0 );

			
			pPlayer->m_Shared.UpdateParticleColor( pSyringe->AddParticleEffect( GetSyringeTrailParticleName( pPlayer->GetTeamNumber(), bCritical ) ) );
			pSyringe->AddEffects( EF_NOSHADOW );
			pSyringe->flags |= FTENT_USEFASTCOLLISIONS;
		}
	}
}

DECLARE_CLIENT_EFFECT( SYRINGE_DISPATCH_EFFECT, ClientsideProjectileSyringeCallback );

#endif

//=============================================================================
//
// TF Nailgun Projectile functions (Server specific).
//
#define NAILGUN_NAIL_MODEL				"models/weapons/w_models/w_nail.mdl"
#define NAILGUN_NAIL_DISPATCH_EFFECT	"ClientProjectile_Nail"
#define NAILGUN_NAIL_GRAVITY	0.3f

LINK_ENTITY_TO_CLASS(tf_projectile_nail, CTFProjectile_Nail);
PRECACHE_REGISTER(tf_projectile_nail);

short g_sModelIndexNail;
void PrecacheNail(void *pUser)
{
	g_sModelIndexNail = modelinfo->GetModelIndex(NAILGUN_NAIL_MODEL);
}

PRECACHE_REGISTER_FN(PrecacheNail);

CTFProjectile_Nail::CTFProjectile_Nail()
{
}

CTFProjectile_Nail::~CTFProjectile_Nail()
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFProjectile_Nail *CTFProjectile_Nail::Create(const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, CBaseEntity *pScorer, bool bCritical)
{
	return static_cast<CTFProjectile_Nail*>(CTFBaseProjectile::Create("tf_projectile_nail", vecOrigin, vecAngles, pOwner, CTFProjectile_Nail::GetInitialVelocity(), g_sModelIndexNail, NAILGUN_NAIL_DISPATCH_EFFECT, pScorer, bCritical));
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
const char *CTFProjectile_Nail::GetProjectileModelName(void)
{
	return NAILGUN_NAIL_MODEL;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
float CTFProjectile_Nail::GetGravity(void)
{
	return NAILGUN_NAIL_GRAVITY;
}

#ifdef CLIENT_DLL
//-----------------------------------------------------------------------------
// Purpose: 
// Output : const char
//-----------------------------------------------------------------------------
const char *GetNailTrailParticleName(int iTeamNumber, bool bCritical)
{
	if (iTeamNumber == TF_TEAM_BLUE)
	{
		return (bCritical ? "nailtrails_scout_blue_crit" : "nailtrails_scout_blue");
	}
	else if (iTeamNumber == TF_TEAM_RED)
	{
		return (bCritical ? "nailtrails_scout_red_crit" : "nailtrails_scout_red");
	}
	else 
	{
		return (bCritical ? "nailtrails_scout_mercenary_crit" : "nailtrails_scout_mercenary");
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void ClientsideProjectileNailCallback(const CEffectData &data)
{
	C_TFPlayer *pPlayer = dynamic_cast<C_TFPlayer*>(ClientEntityList().GetBaseEntityFromHandle(data.m_hEntity));
	if (pPlayer)
	{
		C_LocalTempEntity *pNail = ClientsideProjectileCallback(data, NAILGUN_NAIL_GRAVITY);
		if (pNail)
		{
			switch (pPlayer->GetTeamNumber())
			{
			case TF_TEAM_RED:
				pNail->m_nSkin = 0;
				break;
			case TF_TEAM_BLUE:
				pNail->m_nSkin = 1;
				break;
			case TF_TEAM_MERCENARY:
				pNail->m_nSkin = 2;
				break;
			}
			bool bCritical = ((data.m_nDamageType & DMG_CRITICAL) != 0);
			pPlayer->m_Shared.UpdateParticleColor( pNail->AddParticleEffect(GetNailTrailParticleName(pPlayer->GetTeamNumber(), bCritical)) );
			pNail->AddEffects(EF_NOSHADOW);
			pNail->flags |= FTENT_USEFASTCOLLISIONS;
		}
	}
}

DECLARE_CLIENT_EFFECT(NAILGUN_NAIL_DISPATCH_EFFECT, ClientsideProjectileNailCallback);

#endif