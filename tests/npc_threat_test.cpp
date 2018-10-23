#include "catch/catch.hpp"

#include "common_types.h"
#include "player.h"
#include "npc.h"
#include "npc_class.h"
#include "game.h"
#include "map.h"
#include "monster.h"
#include "text_snippets.h"
#include "field.h"
#include "overmapbuffer.h"
#include "dialogue.h"
#include "faction.h"
#include "player.h"
#include "effect.h"
#include "calendar.h"
#include "coordinate_conversions.h"

#include <string>

const efftype_id effect_gave_quest_item( "gave_quest_item" );
const efftype_id effect_currently_busy( "currently_busy" );
const efftype_id effect_infection( "infection" );
const efftype_id effect_infected( "infected" );
static const trait_id trait_PROF_FED( "PROF_FED" );
static const trait_id trait_PROF_SWAT( "PROF_SWAT" );

npc &create_threat_npc( int x, int y, const std::string &id )
{
    const string_id<npc_template> test_threat( id );
    int model_id = g->m.place_npc( x, y, test_threat, true );
    g->load_npcs();

    npc *model_npc = g->find_npc( model_id );
    REQUIRE( model_npc != nullptr );

    return *model_npc;
}

TEST_CASE( "npc_threat_test" )
{
    tripoint test_origin( 15, 15, 0 );
    g->u.setpos( test_origin );

    g->faction_manager_ptr->create_if_needed();

    std::vector<npc> threats;
    npc &threat0 = create_threat_npc( 25, 25, "ranch_barber" );
    item knc0( "knife_combat" );
    threat0.wield( knc0 );
    threats.push_back( threat0 );
    npc &threat1 = create_threat_npc( 5, 5, "ranch_barber" );
    threat1.unwield();
    threats.push_back( threat1 );
    npc &threat2 = create_threat_npc( 25, 5, "old_guard_soldier" );
    item knc2( "knife_combat" );
    threat2.wield( knc2 );
    threats.push_back( threat2 );
    npc &threat3 = create_threat_npc( 5, 25, "apis" );
    threats.push_back( threat3 );
    npc &threat4 = create_threat_npc( 15, 25, "ranch_barber" );
    item knc4( "hammer_sledge" );
    threat4.wield( knc4 );
    threats.push_back( threat4 );
    npc &threat5 = create_threat_npc( 25, 15, "ranch_barber" );
    item knc5( "spear_pipe" );
    threat5.wield( knc5 );
    threats.push_back( threat5 );

    std::vector<monster> m_threats;
    monster zomb0 = monster( mtype_id( "mon_zombie" ) );
    m_threats.push_back( zomb0 );
    monster zomb1 = monster( mtype_id( "mon_zombie_brute" ) );
    m_threats.push_back( zomb1 );
    monster zomb2 = monster( mtype_id( "mon_zombie_hulk" ) );
    m_threats.push_back( zomb2 );
    monster bear = monster( mtype_id( "mon_bear" ) );
    m_threats.push_back( bear );
    monster black_rat = monster( mtype_id( "mon_black_rat" ) );
    m_threats.push_back( black_rat );
    monster moose = monster( mtype_id( "mon_moose" ) );
    m_threats.push_back( moose );

    size_t atr = 0;
    size_t dfr = 0;
    for( npc &attacker : threats ) {
        dfr = 0;
        atr += 1;
        for( npc &defender : threats ) {
            dfr += 1;
            if( dfr == atr ) {
                continue;
            }
            printf( "#%2ld:%12s attacks #%2ld:%12s with %s: expected damage\t%3.2f\n",
                    atr, attacker.myclass.c_str(),
                    dfr, defender.myclass.c_str(),
                    attacker.used_weapon().display_name().c_str(),
                    defender.approx_damage_per_attack( attacker ) );
        }
        for( monster &defender : m_threats ) {
            dfr += 1;
            printf( "#%2ld:%12s attacks #%2ld:%12s with %s: expected damage\t%3.2f\n",
                    atr, attacker.myclass.c_str(),
                    dfr, defender.name().c_str(),
                    attacker.used_weapon().display_name().c_str(),
                    defender.approx_damage_per_attack( attacker ) );
        }
    }
    for( monster &attacker : m_threats ) {
        dfr = 0;
        atr += 1;
        for( npc &defender : threats ) {
            dfr += 1;
            printf( "#%2ld:%12s attacks #%2ld:%12s: expected damage\t%3.2f\n",
                    atr, attacker.name().c_str(),
                    dfr, defender.myclass.c_str(),
                    defender.approx_damage_per_attack( attacker ) );
        }
        for( monster &defender : m_threats ) {
            dfr += 1;
            printf( "#%2ld:%12s attacks #%2ld:%12s: expected damage\t%3.2f\n",
                    atr, attacker.name().c_str(),
                    dfr, defender.name().c_str(),
                    defender.approx_damage_per_attack( attacker ) );
        }
    }
    printf( "\n\nHITS TO KILL\nattk  " );
    for( dfr = 0 ; dfr < threats.size(); dfr++ ) {
        printf( "#%2ld\t", 1 + dfr );
    }
    for( dfr = 0 ; dfr < m_threats.size(); dfr++ ) {
        printf( "#%2ld\t", 1 + threats.size() + dfr );
    }
    printf( "\n" );
    atr = 0;
    for( npc &attacker : threats ) {
        dfr = 0;
        atr += 1;
        printf( "%ld\t", atr );
        for( npc &defender : threats ) {
            dfr += 1;
            if( dfr == atr ) {
                printf( "\t" );
                continue;
            }
            printf( "%3d:%3d ", std::min( 999, attacker.hits_to_kill( defender ) ), std::min( 999,
                    defender.hits_to_kill( attacker ) ) );
        }
        for( monster &defender : m_threats ) {
            printf( "%3d:%3d ", std::min( 999, attacker.hits_to_kill( defender ) ), std::min( 999,
                    defender.hits_to_kill( attacker ) ) );
        }
        printf( "\n" );
    }
    for( monster &attacker : m_threats ) {
        dfr = 0;
        atr += 1;
        printf( "%ld\t", atr );
        for( npc &defender : threats ) {
            printf( "%3d:%3d ", std::min( 999, attacker.hits_to_kill( defender ) ), std::min( 999,
                    defender.hits_to_kill( attacker ) ) );
        }
        for( monster &defender : m_threats ) {
            dfr += 1;
            if( dfr == atr ) {
                printf( "\t" );
                continue;
            }
            printf( "%3d:%3d ", std::min( 999, attacker.hits_to_kill( defender ) ), std::min( 999,
                    defender.hits_to_kill( attacker ) ) );
        }
        printf( "\n" );
    }
    printf( "\n\nTURNS TO KILL\nattk  " );
    for( dfr = 0 ; dfr < threats.size(); dfr++ ) {
        printf( "#%2ld\t", 1 + dfr );
    }
    for( dfr = 0 ; dfr < m_threats.size(); dfr++ ) {
        printf( "#%2ld\t", 1 + threats.size() + dfr );
    }
    printf( "\n" );
    atr = 0;
    for( npc &attacker : threats ) {
        dfr = 0;
        atr += 1;
        printf( "%ld\t", atr );
        for( npc &defender : threats ) {
            dfr += 1;
            if( dfr == atr ) {
                printf( "\t" );
                continue;
            }
            printf( "%3d:%3d ", std::min( 999, attacker.turns_to_kill( defender ) ), std::min( 999,
                    defender.turns_to_kill( attacker ) ) );
        }
        for( monster &defender : m_threats ) {
            printf( "%3d:%3d ", std::min( 999, attacker.turns_to_kill( defender ) ), std::min( 999,
                    defender.turns_to_kill( attacker ) ) );
        }
        printf( "\n" );
    }
    for( monster &attacker : m_threats ) {
        dfr = 0;
        atr += 1;
        printf( "%ld\t", atr );
        for( npc &defender : threats ) {
            dfr += 1;
            printf( "%3d:%3d ", std::min( 999, attacker.turns_to_kill( defender ) ), std::min( 999,
                    defender.turns_to_kill( attacker ) ) );
        }
        for( monster &defender : m_threats ) {
            dfr += 1;
            if( dfr == atr ) {
                printf( "\t" );
                continue;
            }
            printf( "%3d:%3d ", std::min( 999, attacker.turns_to_kill( defender ) ), std::min( 999,
                    defender.turns_to_kill( attacker ) ) );
        }
        printf( "\n" );
    }
}
