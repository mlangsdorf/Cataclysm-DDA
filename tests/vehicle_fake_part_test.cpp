#include <memory>
#include <vector>

#include "avatar.h"
#include "catch/catch.hpp"
#include "damage.h"
#include "enums.h"
#include "game.h"
#include "item.h"
#include "map.h"
#include "map_helpers.h"
#include "optional.h"
#include "point.h"
#include "type_id.h"
#include "vehicle.h"
#include "vpart_position.h"
#include "vpart_range.h"
#include "veh_type.h"

static void really_clear_map()
{
    clear_map();
    build_test_map( ter_id( "t_pavement" ) );
}

static void validate_part_count( const vehicle &veh, const int target_velocity,
                                 const int face_dir, const int real_parts, const int fake_parts,
                                 const int active_fakes )
{
    if( target_velocity > 0 && veh.velocity <= 200 ) {
        std::cout << veh.disp_name() << " at dir " << face_dir;
        std::cout <<  " speed " << veh.velocity << std::endl;
    }
    if( veh.face.dir() != face_dir ) {
        std::cout << veh.disp_name() << " at dir " << face_dir;
        std::cout <<  " face " << veh.face.dir() << std::endl;
    }
    if( veh.num_true_parts() != real_parts ) {
        std::cout << veh.disp_name() << " at dir " << face_dir;
        std::cout <<  " real parts " << veh.num_true_parts() << std::endl;
    }
    if( veh.num_fake_parts() != fake_parts ) {
        std::cout << veh.disp_name() << " at dir " << face_dir;
        std::cout <<  " fake parts " << veh.num_fake_parts() << std::endl;
    }
    if( veh.num_active_fake_parts() != active_fakes ) {
        std::cout << veh.disp_name() << " at dir " << face_dir;
        std::cout <<  " active fakes " << veh.num_active_fake_parts() << std::endl;
    }


    if( target_velocity > 0 ) {
        REQUIRE( veh.velocity > 200 );
    }
    REQUIRE( veh.face.dir() == face_dir );
    CHECK( veh.num_true_parts() == real_parts );
    CHECK( veh.num_fake_parts() == fake_parts );
    CHECK( veh.num_active_fake_parts() == active_fakes );
}

TEST_CASE( "ensure_fake_parts_enable_on_turn", "[vehicle] [vehicle_fake]" )
{
    const int original_parts = 120;
    const int fake_parts = 18;
    std::vector<int> active_fakes_by_angle = { 0, 3, 8, 15, 6, 1 };
    GIVEN( "A vehicle with a known number of parts" ) {
        really_clear_map();
        map &here = get_map();
        const tripoint test_origin( 30, 30, 0 );
        vehicle *veh = here.add_vehicle( vproto_id( "test_van" ), test_origin, 0, 100, 0 );
        REQUIRE( veh != nullptr );

        /* since we want all the doors closed anyway, go ahead and test that opening
         * and closing the real part also changes the fake part
         */
        for( const vpart_reference vp : veh->get_avail_parts( "OPENABLE" ) ) {
            REQUIRE( !vp.part().is_fake );
            veh->open( vp.part_index() );
        }
        // include inactive fakes since the vehicle isn't rotated
        bool tested_a_fake = false;
        for( const vpart_reference vp : veh->get_all_parts_with_fakes( true ) ) {
            if( vp.info().has_flag( "OPENABLE" ) ) {
                tested_a_fake |= vp.part().is_fake;
                CHECK( veh->is_open( vp.part_index() ) );
            }
        }
        REQUIRE( tested_a_fake );
        for( const vpart_reference vp : veh->get_avail_parts( "OPENABLE" ) ) {
            veh->close( vp.part_index() );
        }
        for( const vpart_reference vp : veh->get_all_parts_with_fakes( true ) ) {
            if( vp.info().has_flag( "OPENABLE" ) ) {
                CHECK( !veh->is_open( vp.part_index() ) );
            }
        }

        veh->tags.insert( "IN_CONTROL_OVERRIDE" );
        veh->engine_on = true;
        const int target_velocity = 12 * 100;
        veh->cruise_velocity = target_velocity;
        veh->velocity = veh->cruise_velocity;
        veh->cruise_on = true;

        for( int quadrant = 0; quadrant < 4; quadrant += 1 ) {
            for( int sub_angle = 0; sub_angle < 6; sub_angle += 1 )  {
                const int angle = quadrant * 90 + sub_angle * 15;
                here.vehmove();
                REQUIRE( veh->cruise_on );
                validate_part_count( *veh, target_velocity, angle, original_parts, fake_parts,
                                     active_fakes_by_angle.at( sub_angle ) );
                veh->turn( 15 );
                veh->velocity = veh->cruise_velocity;
            }
        }
        here.vehmove();
        veh->idle( true );
        validate_part_count( *veh, target_velocity, 0, original_parts, fake_parts,
                             active_fakes_by_angle.at( 0 ) );
    }
}

TEST_CASE( "ensure_vehicle_weight_is_constant", "[vehicle] [vehicle_fake]" )
{
    really_clear_map();
    const tripoint test_origin( 30, 30, 0 );
    map &here = get_map();
    vehicle *veh = here.add_vehicle( vproto_id( "suv" ), test_origin, 0, 0, 0 );
    REQUIRE( veh != nullptr );

    veh->tags.insert( "IN_CONTROL_OVERRIDE" );
    veh->engine_on = true;
    const int target_velocity = 10 * 100;
    veh->cruise_velocity = target_velocity;
    veh->velocity = veh->cruise_velocity;

    GIVEN( "A vehicle with a known weight" ) {
        units::mass initial_weight = veh->total_mass();
        WHEN( "The vehicle turns such that it is not perpendicular to a cardinal axis" ) {
            veh->turn( 45 );
            here.vehmove();
            THEN( "The vehicle weight is constant" ) {
                units::mass turned_weight = veh->total_mass();
                CHECK( initial_weight == turned_weight );
            }
        }
    }
}

TEST_CASE( "vehicle_collision_applies_damage_to_fake_parent", "[vehicle] [vehicle_fake]" )
{
    really_clear_map();
    map &here = get_map();
    GIVEN( "A moving vehicle traveling at a 45 degree angle to the X axis" ) {
        const tripoint test_origin( 30, 30, 0 );
        vehicle *veh = here.add_vehicle( vproto_id( "suv" ), test_origin, 0, 100, 0 );
        REQUIRE( veh != nullptr );

        veh->tags.insert( "IN_CONTROL_OVERRIDE" );
        veh->engine_on = true;
        const int target_velocity = 50 * 100;
        veh->cruise_velocity = target_velocity;
        veh->velocity = veh->cruise_velocity;
        veh->turn( 45 );
        here.vehmove();

        WHEN( "A bashable object is placed in the vehicle's path such that it will hit a fake part" ) {
            // we know the mount point of the front right headlight is 2,2
            // that places it's fake mirror at 2,3
            const point fake_r_hl( 2, 3 );
            tripoint fake_front_right_headlight = veh->mount_to_tripoint( fake_r_hl );
            // we're travelling south east, so placing it SE of the fake headlight mirror
            // will impact it on next move
            tripoint obstacle_point = fake_front_right_headlight + tripoint_south_east;
            here.furn_set( obstacle_point.xy(), furn_id( "f_boulder_large" ) );

            THEN( "The collision damage is applied to the fake's parent" ) {
                here.vehmove();
                std::vector<int> damaged_parts;
                std::vector<int> damaged_fake_parts;
                // hitting the boulder should have slowed the vehicle down
                REQUIRE( veh->velocity < target_velocity );

                for( int rel : veh->parts_at_relative( point( 2, 2 ), true, false ) ) {
                    vehicle_part &vp = veh->part( rel );
                    if( vp.info().durability > vp.hp() ) {
                        damaged_parts.push_back( rel );
                    }
                }
                for( int rel : veh->parts_at_relative( point( 2, 3 ), true, false ) ) {
                    vehicle_part &vp = veh->part( rel );
                    if( vp.info().durability > vp.hp() ) {
                        damaged_fake_parts.push_back( rel );
                    }
                }
                CHECK( !damaged_parts.empty() );
                CHECK( damaged_fake_parts.empty() );
            }
        }
    }
}

TEST_CASE( "ensure_vehicle_with_no_obstacles_has_no_fake_parts", "[vehicle] [vehicle_fake]" )
{
    really_clear_map();
    map &here = get_map();
    GIVEN( "A vehicle with no parts that block movement" ) {
        const tripoint test_origin( 30, 30, 0 );
        vehicle *veh = here.add_vehicle( vproto_id( "bicycle" ), test_origin, 45, 100, 0 );
        REQUIRE( veh != nullptr );
        WHEN( "The vehicle is placed in the world" ) {
            THEN( "There are no fake parts added" ) {
                validate_part_count( *veh, 0, 45, veh->num_parts(), 0, 0 );
            }
        }
    }
}
