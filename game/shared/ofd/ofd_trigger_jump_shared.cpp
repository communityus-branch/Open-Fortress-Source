
#include "cbase.h"
#include "movevars_shared.h"

#ifdef GAME_DLL

#include "ofd_trigger_jump.h"
#include "tf_player.h"

#else

#include "engine/ivdebugoverlay.h"
#include "c_ofd_trigger_jump.h"
#include "c_tf_player.h"
#include "prediction.h"

#define COFDTriggerJump C_OFDTriggerJump

#endif


void COFDTriggerJump::StartTouch( CBaseEntity *pOther )
{
	if( pOther->GetMoveType() != MOVETYPE_WALK && pOther->GetMoveType() != MOVETYPE_VPHYSICS && pOther->GetMoveType() != MOVETYPE_FLYGRAVITY && pOther->GetMoveType() != MOVETYPE_STEP )
		return;

	float flGravScale = 1.0f;

	if( pOther->GetMoveType() == MOVETYPE_FLYGRAVITY )
	{
		pOther->SetGravity( 1.0 ); // Just slam any projectiles to 1, not pretty but no big reason not to.
	}

	Vector startPos = pOther->GetAbsOrigin();
	Vector endPos = m_vecTarget;
	float flGravity = GetCurrentGravity() * flGravScale;
	trace_t tr;

	Vector vecMidPoint = startPos + ( endPos - startPos ) * 0.5;

	// Ensure the midpoint is > the Z of both start/end.
	vecMidPoint.z = MAX( vecMidPoint.z, startPos.z );
	vecMidPoint.z = MAX( vecMidPoint.z, endPos.z );

	UTIL_TraceHull( vecMidPoint, vecMidPoint + Vector( 0, 0, 128 + m_flApexBoost ), VEC_HULL_MIN, VEC_HULL_MAX, MASK_SOLID_BRUSHONLY, pOther, COLLISION_GROUP_NONE, &tr );
	vecMidPoint = tr.endpos + Vector( 0, 0, -6 );

	vecMidPoint.z = MAX( vecMidPoint.z, startPos.z );
	vecMidPoint.z = MAX( vecMidPoint.z, endPos.z );

	// How high should we travel to reach the apex
	float distance1 = ( vecMidPoint.z - startPos.z );
	float distance2 = ( vecMidPoint.z - endPos.z );

	// How long will it take to travel this distance
	float time1 = sqrt( distance1 / ( 0.5 * flGravity ) );
	float time2 = sqrt( distance2 / ( 0.5 * flGravity ) );
	if ( time1 < 0.1 )
		return;

	// how hard to launch to get there in time.
	Vector vecTargetVel = ( endPos - startPos ) / ( time1 + time2 );
	vecTargetVel.z = flGravity * time1;

	Vector vecFinal = vecTargetVel;

	pOther->SetGroundEntity( NULL );

	if ( vecFinal.IsValid() )
	{
		if( !pOther->IsCombatCharacter() && pOther->VPhysicsGetObject() )
		{
			pOther->VPhysicsGetObject()->SetVelocityInstantaneous( &vecFinal, NULL );
		}
		else
		{
			pOther->SetAbsVelocity( vecFinal );
		}
	}


#ifdef GAME_DLL
	m_OnJump.FireOutput( pOther, this );

	pOther->EmitSound( "JumpPadSound" );
#endif
}