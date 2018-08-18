#include "catch/catch.hpp"

#include "game.h"
#include "map.h"
#include "vehicle.h"
#include "veh_type.h"
#include "player.h"

#include <stdio.h>

TEST_CASE( "vehicle_split_section" )
{
    tripoint test_origin( 60, 60, 0 );
    g->u.setpos( test_origin );
    tripoint vehicle_origin = test_origin + tripoint( 1, 1, 0 );

    vehicle *veh_ptr = g->m.add_vehicle( vproto_id( "split_test" ), vehicle_origin, -30 );
    REQUIRE( veh_ptr != nullptr );
    VehicleList vehs = g->m.get_vehicles();
    REQUIRE( vehs.size() == 1 );

    for( auto &vehs_v : vehs ) {
        int i = 0;
        for( auto &vpart : vehs_v.v->parts ) {
            tripoint vpart_pos = vehs_v.v->global_part_pos3( vpart );
            printf( "\tpart %d: %s @ %d:%d\n", i++, vpart.info().name().c_str(),
                    vpart_pos.x, vpart_pos.y );
            }
    }
//    tripoint grab_point = test_origin + tripoint( -1, -1, 0 );
//    g->u.grab_type = OBJECT_VEHICLE;
//    g->u.grab_point = grab_point;

    CHECK( veh_ptr->parts.size() == 192 );

    tripoint break_point = test_origin + tripoint( 1, 3, 0 );
    g->m.destroy( break_point );

    g->m.vehmove();
    vehs = g->m.get_vehicles();
    CHECK( vehs.size() == 4 );

    int cnt7 = 0;
    int cnt170 = 0;
    for( auto &vehs_v : vehs ) {
        if( vehs_v.v == veh_ptr ) {
            printf( "original vehicle\n" );
        } else {
            vehicle *new_veh = vehs_v.v;
            printf( "\n\nparts of a vehicle\n" );
        }
        int i = 0;
        CHECK( ( ( vehs_v.v->parts.size() == 7 ) || ( vehs_v.v->parts.size() == 170 ) ) );
        if( vehs_v.v->parts.size() == 7 ) {
            cnt7 += 1;
        } else if ( vehs_v.v->parts.size() == 170 ) {
            cnt170 += 1;
        }
        for( auto &vpart : vehs_v.v->parts ) {
            tripoint vpart_pos = vehs_v.v->global_part_pos3( vpart );
            printf( "\tpart %d: %s @ %d:%d\n", i++, vpart.info().name().c_str(),
                    vpart_pos.x, vpart_pos.y );
            }
    }
    CHECK( cnt7 == 3 );
    CHECK( cnt170 == 1 );

//    CHECK( g->u.grab_type == OBJECT_VEHICLE);
//    CHECK( g->u.grab_point == grab_point );

    printf( "\n\tvehicle split test completed successfully\n\n" );
}
