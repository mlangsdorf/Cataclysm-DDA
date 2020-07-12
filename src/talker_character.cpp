#include "game.h"
#include "game_constants.h"
#include "messages.h"
#include "monster.h"
#include "mtype.h"
#include "npc.h"
#include "npctrade.h"
#include "output.h"
#include "player.h"
#include "talker_character.h"
#include "vehicle.h"

static const efftype_id effect_pacified( "pacified" );
static const efftype_id effect_pet( "pet" );

static const trait_id trait_PROF_FOODP( "PROF_FOODP" );

std::string talker_character::disp_name() const
{
    return me_chr->disp_name();
}

bool talker_character::has_trait( const trait_id &trait_to_check ) const
{
    return me_chr->has_trait( trait_to_check );
}

bool talker_character::crossed_threshold() const
{
    return me_chr->crossed_threshold();
}

bool talker_character::has_activity() const
{
    return !me_chr->activity.is_null();
}

bool talker_character::is_mounted() const
{
    return me_chr->is_mounted();
}

int talker_character::str_cur() const
{
    return me_chr->str_cur;
}

int talker_character::dex_cur() const
{
    return me_chr->dex_cur;
}

int talker_character::int_cur() const
{
    return me_chr->int_cur;
}

int talker_character::per_cur() const
{
    return me_chr->per_cur;
}

bool talker_character::is_wearing( const itype_id &item_id ) const
{
    return me_chr->is_wearing( item_id );
}

int talker_character::charges_of( const itype_id &item_id ) const
{
    return me_chr->charges_of( item_id );
}

bool talker_character::has_charges( const itype_id &item_id, int count ) const
{
    return me_chr->has_charges( item_id, count );
}

bool talker_character::has_amount( const itype_id &item_id, int count ) const
{
    return me_chr->has_amount( item_id, count );
}

std::vector<item *> talker_character::items_with( const std::function<bool( const item & )>
        &filter ) const
{
    return me_chr->items_with( filter );
}

int talker_character::num_bionics() const
{
    return me_chr->num_bionics();
}

bool talker_character::has_max_power() const
{
    return me_chr->has_max_power();
}

bool talker_character::has_bionic( const bionic_id &bionics_id ) const
{
    return me_chr->has_bionic( bionics_id );
}

bool talker_character::has_effect( const efftype_id &effect_id ) const
{
    return me_chr->has_effect( effect_id );
}

int talker_character::get_fatigue() const
{
    return me_chr->get_fatigue();
}

int talker_character::get_hunger() const
{
    return me_chr->get_hunger();
}

int talker_character::get_thirst() const
{
    return me_chr->get_thirst();
}

tripoint talker_character::global_omt_location() const
{
    return me_chr->global_omt_location();
}

std::string talker_character:: get_value( const std::string &var_name ) const
{
    return me_chr->get_value( var_name );
}

int talker_character::posx() const
{
    return me_chr->posx();
}

int talker_character::posy() const
{
    return me_chr->posy();
}

int talker_character::posz() const
{
    return me_chr->posz();
}

tripoint talker_character::pos() const
{
    return me_chr->pos();
}

int talker_character::cash() const
{
    return me_chr->cash;
}

bool talker_character::is_male() const
{
    return me_chr->male;
}

bool talker_character::knows_spell( const spell_id &sp ) const
{
    return me_chr->magic.knows_spell( sp );
}

bool talker_character::unarmed_attack() const
{
    return me_chr->unarmed_attack();
}

bool talker_character::in_control( const vehicle &veh ) const
{
    return veh.player_in_control( *me_chr );
}

bool talker_character::can_stash_weapon() const
{
    return me_chr->can_pickVolume( me_chr->weapon );
}

bool talker_character::has_stolen_item( const talker &guy ) const
{
    const player *owner = guy.get_character();
    if( owner ) {
        for( auto &elem : me_chr->inv_dump() ) {
            if( elem->is_old_owner( *owner, true ) ) {
                return true;
            }
        }
    }
    return false;
}

int talker_character::get_skill_level( const skill_id &skill ) const
{
    return me_chr->get_skill_level( skill );
}

// functions called in npctalk.cpp
std::vector<std::string> talker_character::get_topics( bool )
{
    std::vector<std::string> add_topics;
    if( has_trait( trait_PROF_FOODP ) && !( is_wearing( itype_id( "foodperson_mask" ) ) ||
                                            is_wearing( itype_id( "foodperson_mask_on" ) ) ) ) {
        add_topics.push_back( "TALK_NOFACE" );
    }
    return add_topics;
}


character_id talker_character::getID() const
{
    return me_chr->getID();
}

faction *talker_character::get_faction() const
{
    return me_chr->get_faction();
}

std::string talker_character::short_description() const
{
    return me_chr->short_description();
}

bool talker_character::is_deaf() const
{
    return me_chr->is_deaf();
}

std::vector<std::string> talker_character::get_grammatical_genders() const
{
    return me_chr->get_grammatical_genders();
}

// functions called in npctalk.cpp
void talker_character::shout( const std::string &speech, bool order )
{
    me_chr->shout( speech, order );
}

void talker_character::add_effect( const efftype_id &new_effect, const time_duration &dur,
                                   bool permanent )
{
    me_chr->add_effect( new_effect, dur, num_bp, permanent );
}

void talker_character::remove_effect( const efftype_id &old_effect )
{
    me_chr->remove_effect( old_effect, num_bp );
}

void talker_character::set_mutation( const trait_id &new_trait )
{
    me_chr->set_mutation( new_trait );
}

void talker_character::unset_mutation( const trait_id &old_trait )
{
    me_chr->unset_mutation( old_trait );
}

void talker_character::set_value( const std::string &var_name, const std::string &value )
{
    me_chr->set_value( var_name, value );
}

void talker_character::remove_value( const std::string &var_name )
{
    me_chr->remove_value( var_name );
}

void talker_character::i_add( const item &new_item )
{
    me_chr->i_add( new_item );
}

std::list<item> talker_character::use_charges( const itype_id &item_name, const int count )
{
    return me_chr->use_charges( item_name, count );
}

std::list<item> talker_character::use_amount( const itype_id &item_name, const int count )
{
    return me_chr->use_amount( item_name, count );
}

void talker_character::remove_items_with( const std::function<bool( const item & )> &filter )
{
    me_chr->remove_items_with( filter );
}

void talker_character::buy_monster( talker &seller, const mtype_id &mtype, int cost,
                                    int count, bool pacified, const translation &name )
{
    npc *seller_guy = seller.get_npc();
    if( !seller_guy ) {
        popup( _( "%s can't sell you any %s" ), seller.disp_name(), mtype.obj().nname( 2 ) );
        return;
    }
    if( !npc_trading::pay_npc( *seller_guy, cost ) ) {
        popup( _( "You can't afford it!" ) );
        return;
    }

    for( int i = 0; i < count; i++ ) {
        monster *const mon_ptr = g->place_critter_around( mtype, me_chr->pos(), 3 );
        if( !mon_ptr ) {
            add_msg( m_debug, "Cannot place u_buy_monster, no valid placement locations." );
            break;
        }
        monster &tmp = *mon_ptr;
        // Our monster is always a pet.
        tmp.friendly = -1;
        tmp.add_effect( effect_pet, 1_turns, num_bp, true );

        if( pacified ) {
            tmp.add_effect( effect_pacified, 1_turns, num_bp, true );
        }

        if( !name.empty() ) {
            tmp.unique_name = name.translated();
        }

    }

    if( name.empty() ) {
        popup( _( "%1$s gives you %2$d %3$s." ), seller_guy->name,
               count, mtype.obj().nname( count ) );
    } else {
        popup( _( "%1$s gives you %2$s." ), seller_guy->name, name );
    }
}
