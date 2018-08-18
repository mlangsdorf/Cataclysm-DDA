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

    vehicle *veh_ptr = g->m.add_vehicle( vproto_id( "split_test" ), vehicle_origin, -90 );
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
    tripoint grab_point = test_origin + tripoint( -1, -1, 0 );
    g->u.grab_type = OBJECT_VEHICLE;
    g->u.grab_point = grab_point;

    tripoint break_point = test_origin + tripoint( 3, 1, 0 );
    g->m.destroy( break_point );
    std::vector<int> split_parts0;
    std::vector<int> split_parts1;
    std::vector<int> split_parts2;
    int vehcnt = veh_ptr->find_split_parts( 2, split_parts0, split_parts1, split_parts2 );
    printf( "there are now %d new vehicles\n", vehcnt );

    const auto print_parts = [&]( std::vector<int> vehparts ) {
        printf( "New vehicle consists of parts: ");
        for( auto splitpart: vehparts ) {
            printf( "%2d ", splitpart );
        }
        printf( "\n" );
    };

    print_parts( split_parts0 );
    print_parts( split_parts1 );
    print_parts( split_parts2 );

    veh_ptr->split( split_parts0, split_parts1, split_parts2 );

    g->m.vehmove();
    vehs = g->m.get_vehicles();
    CHECK( vehs.size() == 4 );

    for( auto &vehs_v : vehs ) {
        if( vehs_v.v == veh_ptr ) {
            printf( "original vehicle\n" );
        } else {
            vehicle *new_veh = vehs_v.v;
            printf( "\n\nparts of a vehicle\n" );
        }
        int i = 0;
        for( auto &vpart : vehs_v.v->parts ) {
            tripoint vpart_pos = vehs_v.v->global_part_pos3( vpart );
            printf( "\tpart %d: %s @ %d:%d\n", i++, vpart.info().name().c_str(),
                    vpart_pos.x, vpart_pos.y );
            }
    }
    CHECK( g->u.grab_type == OBJECT_VEHICLE);
    CHECK( g->u.grab_point == grab_point );

    printf( "\n\tvehicle split test completed successfully\n\n" );
}
