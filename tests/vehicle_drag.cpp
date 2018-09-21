#include "catch/catch.hpp"

#include "game.h"
#include "map.h"
#include "map_iterator.h"
#include "vehicle.h"
#include "veh_type.h"
#include "vpart_range.h"
#include "vpart_reference.h"
#include "itype.h"
#include "player.h"
#include "cata_utility.h"
#include "options.h"
#include "test_statistics.h"
#include <stdio.h>

typedef statistics<long> efficiency_stat;

const efftype_id effect_blind( "blind" );

void clear_game_drag( const ter_id &terrain )
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
        g->m.ter_set( p, terrain );
        g->m.trap_set( p, trap_id( "tr_null" ) );
        g->m.i_clear( p );
    }

    for( wrapped_vehicle &veh : g->m.get_vehicles( tripoint( 0, 0, 0 ), tripoint( MAPSIZE * SEEX,
            MAPSIZE * SEEY, 0 ) ) ) {
        g->m.destroy_vehicle( veh.v );
    }

    g->m.build_map_cache( 0, true );
}


vehicle *setup_drag_test( const vproto_id &veh_id )
{
    clear_game_drag( ter_id( "t_pavement" ) );

    const tripoint map_starting_point( 60, 60, 0 );
    vehicle *veh_ptr = g->m.add_vehicle( veh_id, map_starting_point, -90, 100, 0 );

    REQUIRE( veh_ptr != nullptr );
    if( veh_ptr == nullptr ) {
        return nullptr;
    }

    // start the engines, close the doors
    veh_ptr->start_engines();
    const auto doors = veh_ptr->get_avail_parts( "OPENABLE" );
    for( const vpart_reference vp :  doors ) {
        const size_t door = vp.part_index();
        veh_ptr->close( door );
    }

    veh_ptr->refresh_insides();
    return veh_ptr;
}

// Algorithm goes as follows:
// Clear map
// Spawn a vehicle
// calculate c_air_drag and c_rolling_resistance
// return whether they're within 5% of expected values
bool test_drag( const vproto_id &veh_id,
                const double expected_c_air, const double expected_c_rr,
                const int expected_safe, const int expected_max )
{
    vehicle *veh_ptr = setup_drag_test( veh_id );
    if( veh_ptr == nullptr ) {
        return false;
    }

    double c_air = veh_ptr->coeff_air_drag();
    double c_rolling = veh_ptr->coeff_rolling_drag();
    int safe_v = veh_ptr->safe_velocity();
    int max_v = veh_ptr->max_velocity();

    const auto d_in_bounds = [&]( double expected, double value ) {
        double expected_high = expected * 1.05;
        double expected_low = expected * 0.95;
        CHECK( value >= expected_low );
        CHECK( value <= expected_high );
        return ( value >= expected_low ) && ( value <= expected_high );
    };
    const auto i_in_bounds = [&]( int expected, int value ) {
        int expected_high = expected * 1.05;
        int expected_low = expected * 0.95;
        CHECK( value >= expected_low );
        CHECK( value <= expected_high );
        return ( value >= expected_low ) && ( value <= expected_high );
    };
    bool valid = true;
    valid &= d_in_bounds( expected_c_air, c_air );
    valid &= d_in_bounds( expected_c_rr, c_rolling );
    valid &= i_in_bounds( expected_safe, safe_v );
    valid &= i_in_bounds( expected_max, max_v );
    return valid;
}

// Behold: power of laziness
void print_drag_test_strings( const std::string &type )
{
    std::ostringstream ss;
    vehicle *veh_ptr = setup_drag_test( vproto_id( type ) );
    if( veh_ptr == nullptr ) {
        return;
    }
    ss << "    test_vehicle_drag( \"" << type << "\", ";
    ss << veh_ptr->coeff_air_drag() << ", ";
    ss << veh_ptr->coeff_rolling_drag() << ", ";
    ss << veh_ptr->safe_velocity() << ", ";
    ss << veh_ptr->max_velocity();
    ss << " );" << std::endl;
    printf( "%s", ss.str().c_str() );
    fflush( stdout );
}

void test_vehicle_drag( std::string type,
                        const double expected_c_air, const double expected_c_rr,
                        const int expected_safe, const int expected_max )
{
    SECTION( type ) {
        test_drag( vproto_id( type ), expected_c_air, expected_c_rr, expected_safe, expected_max );
    }
}

std::vector<std::string> vehs_to_test_drag = {
    {
        "bicycle",
        "bicycle_electric",
        "motorcycle",
        "motorcycle_sidecart",
        "quad_bike",
        "scooter",
        "scooter_electric",
        "superbike",
        "tandem",
        "unicycle",
        "beetle",
        "bubble_car",
        "car",
        "car_mini",
        "car_sports",
        "car_sports_atomic",
        "car_sports_electric",
        "electric_car",
        "rara_x",
        "suv",
        "suv_electric",
        "golf_cart",
        "golf_cart_4seat",
        "hearse",
        "pickup_technical",
        "ambulance",
        "car_fbi",
        "fire_engine",
        "fire_truck",
        "policecar",
        "policesuv",
        "truck_swat",
        "oldtractor",
        "autotractor",
        "tractor_plow",
        "tractor_reaper",
        "tractor_seed",
        "aapc-mg",
        "apc",
        "humvee",
        "military_cargo_truck",
        "flatbed_truck",
        "pickup",
        "semi_truck",
        "truck_trailer",
        "tatra_truck",
        "animalctrl",
        "autosweeper",
        "excavator",
        "road_roller",
        "forklift",
        "trencher",
        "armored_car",
        "cube_van",
        "cube_van_cheap",
        "hippie_van",
        "icecream_truck",
        "lux_rv",
        "meth_lab",
        "rv",
        "schoolbus",
        "security_van",
        "wienermobile",
    }
};

/** This is even less of a test. It generates C++ lines for the actual test below */
TEST_CASE( "vehicle_make_drag_case", "[.]" )
{
    for( const std::string &veh : vehs_to_test_drag ) {
        print_drag_test_strings( veh );
    }
}

// format is vehicle, coeff_air_drag, coeff_rolling_drag, safe speed, max speed
// coeffs are dimensionless, speeds are 100ths of mph, so 6101 is 61.01 mph
TEST_CASE( "vehicle_drag", "[vehicle] [engine]" )
{
    test_vehicle_drag( "bicycle", 0.609525, 0.0169566, 2356, 3078 );
    test_vehicle_drag( "bicycle_electric", 0.609525, 0.0274778, 3067, 3528 );
    test_vehicle_drag( "motorcycle", 0.609525, 0.603851, 7204, 8620 );
    test_vehicle_drag( "motorcycle_sidecart", 0.880425, 0.902852, 6341, 7597 );
    test_vehicle_drag( "quad_bike", 1.15133, 1.15222, 5780, 6931 );
    test_vehicle_drag( "scooter", 0.609525, 0.199444, 6098, 7302 );
    test_vehicle_drag( "scooter_electric", 0.609525, 0.161612, 4928, 5106 );
    test_vehicle_drag( "superbike", 0.609525, 0.879941, 9860, 11758 );
    test_vehicle_drag( "tandem", 0.609525, 0.0213436, 2353, 3076 );
    test_vehicle_drag( "unicycle", 0.690795, 0.00239997, 2266, 2958 );
    test_vehicle_drag( "beetle", 1.32064, 1.82622, 7528, 9011 );
    test_vehicle_drag( "bubble_car", 0.846562, 1.54169, 9373, 9720 );
    test_vehicle_drag( "car", 0.507938, 2.49932, 10024, 12078 );
    test_vehicle_drag( "car_mini", 0.507938, 1.75356, 10199, 12244 );
    test_vehicle_drag( "car_sports", 0.507938, 2.54032, 17538, 20928 );
    test_vehicle_drag( "car_sports_atomic", 0.507938, 3.15377, 17451, 18103 );
    test_vehicle_drag( "car_sports_electric", 0.507938, 2.46264, 17587, 18239 );
    test_vehicle_drag( "electric_car", 0.507938, 2.12512, 13892, 14411 );
    test_vehicle_drag( "rara_x", 0.903, 1.92071, 8443, 8759 );
    test_vehicle_drag( "suv", 0.507938, 3.09861, 11771, 14157 );
    test_vehicle_drag( "suv_electric", 0.5418, 2.33541, 13570, 14078 );
    test_vehicle_drag( "golf_cart", 0.943313, 1.25185, 7161, 7427 );
    test_vehicle_drag( "golf_cart_4seat", 0.943313, 1.21965, 7166, 7432 );
    test_vehicle_drag( "hearse", 0.67725, 3.14893, 9062, 10934 );
    test_vehicle_drag( "pickup_technical", 2.33651, 3.01756, 7309, 8736 );
    test_vehicle_drag( "ambulance", 2.27556, 2.41808, 8753, 10437 );
    test_vehicle_drag( "car_fbi", 1.03619, 2.77324, 11243, 13438 );
    test_vehicle_drag( "fire_engine", 2.30588, 4.21247, 8612, 10295 );
    test_vehicle_drag( "fire_truck", 2.27556, 8.09649, 8425, 10130 );
    test_vehicle_drag( "policecar", 1.03619, 2.7686, 11244, 13438 );
    test_vehicle_drag( "policesuv", 1.03619, 3.23795, 11190, 13388 );
    test_vehicle_drag( "truck_swat", 1.78794, 7.69292, 7819, 9178 );
    test_vehicle_drag( "oldtractor", 1.15133, 1.0534, 9610, 11150 );
    test_vehicle_drag( "autotractor", 1.568, 1.69294, 7103, 7366 );
    test_vehicle_drag( "tractor_plow", 1.69313, 0.969211, 8476, 9829 );
    test_vehicle_drag( "tractor_reaper", 1.69313, 0.854986, 8485, 9838 );
    test_vehicle_drag( "tractor_seed", 1.69313, 0.854986, 8485, 9838 );
    test_vehicle_drag( "aapc-mg", 4.74075, 9.17644, 5756, 6735 );
    test_vehicle_drag( "apc", 4.74075, 9.06788, 5760, 6739 );
    test_vehicle_drag( "humvee", 1.66894, 7.39436, 9583, 11197 );
    test_vehicle_drag( "military_cargo_truck", 1.52381, 9.68775, 9674, 11347 );
    test_vehicle_drag( "flatbed_truck", 1.37869, 4.61079, 8470, 10191 );
    test_vehicle_drag( "pickup", 0.914288, 3.22121, 9780, 11748 );
    test_vehicle_drag( "semi_truck", 1.71248, 10.2967, 9305, 10915 );
    test_vehicle_drag( "truck_trailer", 1.19647, 12.8338, 0, 0 );
    test_vehicle_drag( "tatra_truck", 0.846562, 7.95577, 14639, 17105 );
    test_vehicle_drag( "animalctrl", 0.67725, 2.85216, 10792, 12965 );
    test_vehicle_drag( "autosweeper", 1.568, 1.5759, 6048, 6273 );
    test_vehicle_drag( "excavator", 1.42545, 1.88181, 10232, 11869 );
    test_vehicle_drag( "road_roller", 2.20106, 2.78305, 8826, 10245 );
    test_vehicle_drag( "forklift", 0.943313, 1.29502, 7154, 7421 );
    test_vehicle_drag( "trencher", 1.42545, 1.71732, 6220, 6453 );
    test_vehicle_drag( "armored_car", 1.66894, 7.11003, 9603, 11217 );
    test_vehicle_drag( "cube_van", 1.1739, 2.78861, 9121, 10919 );
    test_vehicle_drag( "cube_van_cheap", 1.11101, 2.72477, 7844, 9424 );
    test_vehicle_drag( "hippie_van", 0.711113, 2.91252, 8971, 10810 );
    test_vehicle_drag( "icecream_truck", 2.11302, 3.84609, 7470, 8957 );
    test_vehicle_drag( "lux_rv", 3.28692, 3.74837, 6683, 7776 );
    test_vehicle_drag( "meth_lab", 1.1739, 3.03962, 9067, 10874 );
    test_vehicle_drag( "rv", 1.1739, 3.09593, 9061, 10868 );
    test_vehicle_drag( "schoolbus", 0.846562, 3.21262, 10313, 12036 );
    test_vehicle_drag( "security_van", 1.1739, 7.74538, 8839, 10406 );
    test_vehicle_drag( "wienermobile", 3.28692, 2.39884, 7766, 9255 );
}
