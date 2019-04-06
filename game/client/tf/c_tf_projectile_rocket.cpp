//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "c_tf_projectile_rocket.h"
#include "particles_new.h"
#include "tf_gamerules.h"
#include "tempent.h"
#include "iefx.h"
#include "dlight.h"
#include "c_te_legacytempents.h"

IMPLEMENT_NETWORKCLASS_ALIASED( TFProjectile_Rocket, DT_TFProjectile_Rocket )

extern ConVar of_muzzlelight;

BEGIN_NETWORK_TABLE( C_TFProjectile_Rocket, DT_TFProjectile_Rocket )
	RecvPropBool( RECVINFO( m_bCritical ) ),
END_NETWORK_TABLE()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_TFProjectile_Rocket::C_TFProjectile_Rocket( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_TFProjectile_Rocket::~C_TFProjectile_Rocket( void )
{
	ParticleProp()->StopEmission();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_TFProjectile_Rocket::OnDataChanged(DataUpdateType_t updateType)
{
	BaseClass::OnDataChanged(updateType);

	if ( updateType == DATA_UPDATE_CREATED )
	{
		CreateRocketTrails();		
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_TFProjectile_Rocket::CreateRocketTrails(void)
{
	if (IsDormant())
		return;

	if (enginetrace->GetPointContents(GetAbsOrigin()) & MASK_WATER)
	{
		ParticleProp()->Create("rockettrail_underwater", PATTACH_POINT_FOLLOW, "trail");
	}
	else
	{
		ParticleProp()->Create(GetTrailParticleName(), PATTACH_POINT_FOLLOW, "trail");
	}

	if (m_bCritical)
	{
		switch (GetTeamNumber())
		{
		case TF_TEAM_BLUE:
			ParticleProp()->Create("critical_rocket_blue", PATTACH_ABSORIGIN_FOLLOW);
			break;
		case TF_TEAM_RED:
			ParticleProp()->Create("critical_rocket_red", PATTACH_ABSORIGIN_FOLLOW);
			break;
		case TF_TEAM_MERCENARY:
			ParticleProp()->Create("critical_rocket_mercenary", PATTACH_ABSORIGIN_FOLLOW);
			break;
		default:
			break;
		}
	}
}

void C_TFProjectile_Rocket::CreateLightEffects(void)
{
	// Handle the dynamic light
	if (of_muzzlelight.GetBool())
	{
		AddEffects(EF_DIMLIGHT);

		dlight_t *dl;
		if (IsEffectActive(EF_DIMLIGHT))
		{
			dl = effects->CL_AllocDlight(LIGHT_INDEX_TE_DYNAMIC + index);
			dl->origin = GetAbsOrigin();
			dl->color.r = 255;
			dl->color.g = 100;
			dl->color.b = 10;
			switch (GetTeamNumber())
			{
			case TF_TEAM_RED:
				if (!m_bCritical) {
					dl->color.r = 255; dl->color.g = 100; dl->color.b = 10;
				}
				else {
					dl->color.r = 255; dl->color.g = 10; dl->color.b = 10;
				}
				break;

			case TF_TEAM_BLUE:
				if (!m_bCritical) {
					dl->color.r = 255; dl->color.g = 100; dl->color.b = 10;
				}
				else {
					dl->color.r = 10; dl->color.g = 10; dl->color.b = 255;
				}
				break;

			case TF_TEAM_MERCENARY:
				if (!m_bCritical) {
					dl->color.r = 255; dl->color.g = 100; dl->color.b = 10;
				}
				else {
					dl->color.r = 10; dl->color.g = 10; dl->color.b = 255;
				}
				break;
			}
			dl->die = gpGlobals->curtime + 0.01f;
			dl->radius = 340.f;
			dl->decay = 512.0f;

			tempents->RocketFlare(GetAbsOrigin());
		}
	}
}

