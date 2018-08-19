#include "catch/catch.hpp"

#include "game.h"
#include "map.h"
#include "map_iterator.h"
#include "vehicle.h"
#include "veh_type.h"
#include "itype.h"
#include "player.h"
#include "cata_utility.h"
#include "options.h"

#include <stdio.h>

const efftype_id effect_blind( "blind" );

void clear_game()
{
    // Set to turn 0 to prevent solars from producing power
    calendar::turn = 0;
    for( monster &critter : g->all_monsters() ) {
        g->remove_zombie( critter );
    }

    g->unload_npcs();

    // Move player somewhere safe
    g->u.setpos( tripoint( 0, 0, 0 ) );
    // Blind the player to avoid needless drawing-related overhead
    g->u.add_effect( effect_blind, 1_turns, num_bp, true );

    for( const tripoint &p : g->m.points_in_rectangle( tripoint( 0, 0, 0 ),
            tripoint( MAPSIZE * SEEX, MAPSIZE * SEEY, 0 ) ) ) {
        g->m.furn_set( p, furn_id( "f_null" ) );
        g->m.ter_set( p, ter_id( "t_pavement" ) );
        g->m.trap_set( p, trap_id( "tr_null" ) );
        g->m.i_clear( p );
    }

    for( wrapped_vehicle &veh : g->m.get_vehicles( tripoint( 0, 0, 0 ), tripoint( MAPSIZE * SEEX,
            MAPSIZE * SEEY, 0 ) ) ) {
        g->m.destroy_vehicle( veh.v );
    }

    g->m.build_map_cache( 0, true );
}

TEST_CASE( "vehicle_split_section" )
{
    tripoint test_origin( 30, 30, 0 );
    g->u.setpos( test_origin );
    int test_cnt = 0;
    for( int dir = 0; dir < 30; dir += 30 ) {
        test_cnt += 1;
        clear_game();
        tripoint vehicle_origin = test_origin + tripoint( 1, 1, 0 );

        vehicle *veh_ptr = g->m.add_vehicle( vproto_id( "split_test" ), vehicle_origin, dir );
        REQUIRE( veh_ptr != nullptr );
        VehicleList vehs = g->m.get_vehicles();
        REQUIRE( vehs.size() == 1 );

#define PRINTEST
#ifdef PRINTEST
        printf( "\n\nstarting split test at angle %d\n", dir );
        for( auto &vehs_v : vehs ) {
            int i = 0;
            for( auto &vpart : vehs_v.v->parts ) {
                tripoint vpart_pos = vehs_v.v->global_part_pos3( vpart );
                printf( "\tpart %d: %s @ %d:%d\n", i++, vpart.info().name().c_str(),
                        vpart_pos.x, vpart_pos.y );
            }
        }
#endif
    //    tripoint grab_point = test_origin + tripoint( -1, -1, 0 );
    //    g->u.grab_type = OBJECT_VEHICLE;
    //    g->u.grab_point = grab_point;

        REQUIRE( veh_ptr->parts.size() == 192 );

        tripoint break_point = veh_ptr->global_part_pos3( 2 );
#ifdef PRINTEST
        printf( "breaking at %d:%d\n", break_point.x, break_point.y );
#endif
        g->m.destroy( break_point );

        g->m.vehmove();
        vehs = g->m.get_vehicles();
        CHECK( vehs.size() == 4 );

        int cnt7 = 0;
        int cnt170 = 0;
        for( auto &vehs_v : vehs ) {
#ifdef PRINTEST
            if( vehs_v.v == veh_ptr ) {
                printf( "original vehicle\n" );
            } else {
                vehicle *new_veh = vehs_v.v;
                printf( "\n\nparts of a vehicle\n" );
            }
#endif
            int i = 0;
            //CHECK( ( ( vehs_v.v->parts.size() == 7 ) || ( vehs_v.v->parts.size() == 170 ) ) );
            if( vehs_v.v->parts.size() == 7 ) {
                cnt7 += 1;
            } else if ( vehs_v.v->parts.size() == 170 ) {
                cnt170 += 1;
            }
#ifdef PRINTEST
            for( auto &vpart : vehs_v.v->parts ) {
                tripoint vpart_pos = vehs_v.v->global_part_pos3( vpart );
                printf( "\tpart %d: %s @ %d:%d\n", i++, vpart.info().name().c_str(),
                        vpart_pos.x, vpart_pos.y );
            }
#endif
        }
        CHECK( cnt7 == 3 );
        CHECK( cnt170 == 1 );
        if( cnt7 == 3 && cnt170 == 1 )
            printf(" successful test at dir %d\n", dir );
        else
            printf(" FAILED test at dir %d\n", dir );
            

    //    CHECK( g->u.grab_type == OBJECT_VEHICLE);
    //    CHECK( g->u.grab_point == grab_point );

    //    printf( "\n\tvehicle split test completed successfully\n\n" );

    printf(" %d test cases\n", test_cnt );
    }
}
