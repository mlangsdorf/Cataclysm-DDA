#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "avatar.h"
#include "catch/catch.hpp"
#include "game.h"
#include "itype.h"
#include "map.h"
#include "map_helpers.h"
#include "map_iterator.h"
#include "veh_type.h"
#include "vehicle.h"
#include "vpart_range.h"
#include "bodypart.h"
#include "calendar.h"
#include "enums.h"
#include "game_constants.h"
#include "item.h"
#include "line.h"
#include "mapdata.h"
#include "map_helpers.h"
#include "monster.h"
#include "mtype.h"
#include "units.h"
#include "type_id.h"
#include "point.h"
#include "vpart_position.h"
#include "player_helpers.h"
#include "map_helpers.h"

const efftype_id effect_blind( "blind" );

static void clear_game_and_set_map( const int transit_x, bool use_ramp, bool up )
{
    // Set to turn 0 to prevent solars from producing power
    calendar::turn = 0;
    clear_creatures();
    clear_npcs();
    clear_vehicles();

    // Move player somewhere safe
    REQUIRE_FALSE( g->u.in_vehicle );
    g->u.setpos( tripoint_zero );
    // Blind the player to avoid needless drawing-related overhead
    //g->u.add_effect( effect_blind, 1_turns, num_bp, true );

    build_test_map( ter_id( "t_pavement" ) );
    if( use_ramp ) {
        const int upper_zlevel = up ? 1 : 0;
        const int lower_zlevel = up - 1;
        const int highx = transit_x + ( up ? 0 : 1 );
        const int lowx = transit_x + ( up ? 1 : 0 );
        // up   z1    ......  rdh  rDl
        //      z0            rUh  rul .................
        // down z0            rDl  rdh .................
        //      z-1   ......  rdl  rUh
        //                    57   58
        for( int y = 0; y < SEEY * MAPSIZE; y++ ) {
            const tripoint ramp_up_low = tripoint( lowx, y, lower_zlevel );
            const tripoint ramp_up_high = tripoint( highx, y, lower_zlevel );
            const tripoint ramp_down_low = tripoint( lowx, y, upper_zlevel );
            const tripoint ramp_down_high = tripoint( highx, y, upper_zlevel );
            g->m.ter_set( ramp_up_low, ter_id( "t_ramp_up_low" ) );
            g->m.ter_set( ramp_up_high, ter_id( "t_ramp_up_high" ) );
            g->m.ter_set( ramp_down_low, ter_id( "t_ramp_down_low" ) );
            g->m.ter_set( ramp_down_high, ter_id( "t_ramp_down_high" ) );
            for( int x = 0; x < transit_x; x++ ) {
                const tripoint road = tripoint( x, y, up ? upper_zlevel : lower_zlevel );
                g->m.ter_set( road, ter_id( "t_pavement" ) );
            }
        }
    }
    g->m.invalidate_map_cache( 0 );
    g->m.build_map_cache( 0, true );
}

// Algorithm goes as follows:
// Clear map
// Spawn a vehicle
// Drive it for a while, always moving it back to start point every turn to avoid it going off the bubble
static int ramp_transition( const vproto_id &veh_id, bool use_ramp, bool up )
{
    printf( "*** STARTING %s ****\n\n", use_ramp ? ( up ? "ramp up!" : "ramp down!" ) : "level" );
    constexpr int transition_x = 57;
    clear_game_and_set_map( transition_x, use_ramp, up );

    const tripoint map_starting_point( 60, 60, 0 );
    vehicle *veh_ptr = g->m.add_vehicle( veh_id, map_starting_point, 180, 1, 0 );

    REQUIRE( veh_ptr != nullptr );
    if( veh_ptr == nullptr ) {
        return 0;
    }
    monster *dmon_p = g->place_critter_at( mtype_id( "debug_mon" ), map_starting_point );
    monster &dmon = *dmon_p;

    vehicle &veh = *veh_ptr;
    veh.check_falling_or_floating();

    REQUIRE( !veh.is_in_water() );

    veh.tags.insert( "IN_CONTROL_OVERRIDE" );
    veh.engine_on = true;

    const int target_velocity = 400;
    veh.cruise_velocity = target_velocity;
    veh.velocity = target_velocity;
    CHECK( veh.safe_velocity() > 0 );
    int cycles_left = 10;
    const int target_z = use_ramp ? ( up ? 1 : -1 ) : 0;

    while( veh.engine_on && veh.safe_velocity() > 0 && cycles_left > 0 ) {
        cycles_left--;
        g->m.vehmove();
        // If the vehicle starts skidding, the effects become random and test is RUINED
        REQUIRE( !veh.skidding );
        for( const tripoint &pos : veh.get_points() ) {
            REQUIRE( g->m.ter( pos ) );
        }
        for( const vpart_reference &vp : veh.get_all_parts() ) {
            if( vp.info().location != "structure" ) {
                continue;
            }
            const point &pmount = vp.mount();
            const tripoint &ppos = vp.pos();
            if( ppos.x <= transition_x ) {
                CHECK( ppos.z == target_z );
            }
            if( pmount.x == 0 && pmount.y == 0 ) {
                CHECK( dmon.pos() == ppos );
            }
        }
    }
    return 0;
}

static void test_ramp( std::string type )
{
    SECTION( type + " no ramp" ) {
        ramp_transition( vproto_id( type ), false, false );
    }
    SECTION( type + " ramp up" ) {
        ramp_transition( vproto_id( type ), true, true );
    }
    /*    SECTION( type + " ramp down" ) {
            ramp_transition( vproto_id( type ), true, false );
        }*/
}

static std::vector<std::string> ramp_vehs_to_test = {{
        "beetle",
    }
};

/** This isn't a test per se, it executes this code to
 * determine the current state of vehicle efficiency.
 **/
TEST_CASE( "vehicle_ramp_test", "[vehicle][ramp]" )
{
    for( const std::string &veh : ramp_vehs_to_test ) {
        test_ramp( veh );
    }
}
