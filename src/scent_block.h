#pragma once
#ifndef CATA_SRC_SCENT_BLOCK_H
#define CATA_SRC_SCENT_BLOCK_H

#include <algorithm>
#include <array>

#include "coordinate_conversions.h"
#include "point.h"
#include "scent_map.h"

struct scent_block {
    template<typename T>
    using data_block = std::array < std::array < T, SEEY + 2 >, SEEX + 2 >;

    enum class data_mode : int {
        NONE = 0,
        SET = 1,
        MAX = 2
    };

    struct datum {
        data_mode mode;
        int intensity;
    };
    data_block<datum> assignment;

    tripoint origin;
    scent_map &scents;
    int modification_count;

    scent_block( const tripoint &sub, scent_map &scents )
    // NOLINTNEXTLINE(cata-use-named-point-constants)
        : origin( sm_to_ms_copy( sub ) + point( -1, -1 ) )
        , scents( scents )
        , modification_count( 0 ) {
        for( int x = 0; x < SEEX + 2; ++x ) {
            for( int y = 0; y < SEEY + 2; ++y ) {
                assignment[x][y] = { data_mode::NONE, 0 };
            }
        }
    }

    void commit_modifications() {
        if( modification_count == 0 ) {
            return;
        }
        for( int x = 0; x < SEEX + 2; ++x ) {
            for( int y = 0; y < SEEY + 2; ++y ) {
                switch( assignment[x][y].mode ) {
                    case data_mode::NONE:
                        break;
                    case data_mode::SET: {
                        tripoint p = origin + tripoint( x, y, 0 );
                        if( scents.inbounds( p ) ) {
                            scents.set_unsafe( p, assignment[x][y].intensity );
                        }
                        break;
                    }
                    case data_mode::MAX: {
                        tripoint p = origin + tripoint( x, y, 0 );
                        if( scents.inbounds( p ) ) {
                            scents.set_unsafe( p, std::max( assignment[x][y].intensity, scents.get_unsafe( p ) ) );
                        }
                        break;
                    }
                }
            }
        }
    }

    point index( const tripoint &p ) const {
        return -origin.xy() + p.xy();
    }

    // We should be working entirely within the range, so don't range check here
    void apply_gas( const tripoint &p, const int nintensity = 0 ) {
        const point ndx = index( p );
        assignment[ndx.x][ndx.y].mode = data_mode::SET;
        assignment[ndx.x][ndx.y].intensity = std::max( 0, assignment[ndx.x][ndx.y].intensity - nintensity );
        ++modification_count;
    }
    void apply_slime( const tripoint &p, int intensity ) {
        const point ndx = index( p );
        datum &dat = assignment[ndx.x][ndx.y];
        switch( dat.mode ) {
            case data_mode::NONE: {
                // we don't know what the current intensity is, so we must do a max operation
                dat.mode = data_mode::MAX;
                dat.intensity = intensity;
                break;
            }
            case data_mode::MAX:
            // Already max for some reason, shouldn't occur.
            // If it does we want to grow if possible
            case data_mode::SET: {
                // new intensity is going to be dat.intensity, so we just need to make it larger
                // but cannot change
                dat.intensity = std::max( dat.intensity, intensity );
                break;
            }
        }
        ++modification_count;
    }
};

#endif // CATA_SRC_SCENT_BLOCK_H
