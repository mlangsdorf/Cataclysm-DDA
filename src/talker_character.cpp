#include "character.h"
#include "talker_character.h"

std::string talker_character::disp_name() const override
{
    return me_chr->disp_name();
}

bool talker_character::has_trait( const &trait_id trait_to_check ) const override
{
    return me_chr->has_trait( trait_to_check );
}

bool talker_character::crossed_threshold() const override
{
    return me_chr->crossed_threshold();
}

bool talker_character::has_activity() const override
{
    return !me_chr->activity.is_null();
}

bool talker_character::is_mounted() const override
{
    return me_chr->is_mounted();
}

int talker_character::str_cur() const override
{
    return me_chr->str_cur;
}

int talker_character::dex_cur() const override
{
    return me_chr->dex_cur;
}

int talker_character::int_cur() const override
{
    return me_chr->int_cur;
}

int talker_character::per_cur() const override
{
    return me_chr->per_cur;
}

bool talker_character::is_wearing( const itype_id &item_id ) const override
{
    return me_chr->is_wearing( item_id );
}

bool talker_character::charges_of( const itype_id &item_id ) const override
{
    return me_chr->charges_of( item_id );
}

bool talker_character::has_charges( const itype_id &item_id, int count ) const override
{
    return me_chr->has_charges( item_id, count );
}

bool talker_character::has_amount( const itype_id &item_id, int count ) const override
{
    return me_chr->has_amount( item_id, count );
}

std::vector<item *> talker_character::items_with( const item_category_id &category_id ) const
override
{
    const auto items_with = me_chr->items_with( [category_id]( const item & it ) {
        return it.get_category().get_id() == category_id;
    } );
    return items_with( category_id );
}

int talker_character::num_bionics() const override
{
    return me_chr->num_bionics();
}

bool talker_character::has_max_power() const override
{
    return me_chr->has_max_power();
}

bool talker_character::has_bionic( const bionic_id &bionics_id ) override
{
    return me_chr->has_bionic( bionics_id );
}

bool talker_character::has_effect( const efftype_id &effect_id ) override
{
    return me_chr->has_effect( effect_id );
}

bool talker_character::get_fatigue() const override
{
    return me_chr->get_fatigue();
}

bool talker_character::get_hunger() const override
{
    return me_chr->get_hunger();
}

bool talker_character::get_thirst() const override
{
    return me_chr->get_thirst();
}

tripoint talker_character::global_omt_location() const override
{
    return me_chr->global_omt_location();
}

std::string talker_character:: get_value( const std::string &var_name ) const override
{
    return me_chr->get_value( var_name );
}

int talker_character::posx() const override
{
    return me_chr->posx();
}

int talker_character::posy() const override
{
    return me_chr->posy();
}

int talker_character::posz() const override
{
    return me_chr->posz();
}

tripoint talker_character::pos() const override
{
    return me_chr->pos();
}

int talker_character::cash() const override
{
    return me_chr->cash;
}

bool talker_character::is_male() const override
{
    return me_chr->is_male;
}

bool talker_character::is_following() const override
{
    return me_chr->is_following();
}

bool talker_character::is_friendly() const override
{
    return me_chr->is_friendly( get_player_character() );
}

bool talker_character::is_enemy() const override
{
    return me_chr->is_enemy();
}

bool talker_character::unarmed_attack() const override
{
    return me_chr->unarmed_attack();
}

bool talker_character::in_control( const vehicle &veh ) const override
{
    return veh.player_in_control( veh );
}

bool talker_character::can_stash_weapon() const override
{
    return me_chr->can_pickVolume( me_chr->weapon );
}

bool talker_character::has_stolen_item( const talker &guy ) const override
{
    if( guy.get_character() ) {
        const character *owner = guy.get_character();
        for( auto &elem : me_chr->inv_dump() ) {
            if( elem->is_old_owner( guy, true ) ) {
                return true;
            }
        }
    }
    return false;
}

int talker_character::get_skill_level( const &skill_id skill ) const override
{
    return me_chr->get_skill_level( skill );
}

bool talker_character::knows_recipe( const recipe &rep ) const override
{
    return me_chr->knows_recipe( rep );
}

// override functions called in npctalk.cpp
std::vector<std::string> talker_character::get_topics( bool ) override
{
    std::vector<std::string> add_topics;
    if( has_trait( trait_PROF_FOODP ) && !( is_wearing( itype_id( "foodperson_mask" ) ) ||
                                            is_wearing( itype_id( "foodperson_mask_on" ) ) ) ) {
        add_topics.push_back( "TALK_NOFACE" );
    }
    return add_topics;
}


character_id talker_character::getID() const override
{
    return me_chr->getID();
}

faction *talker_character::get_faction() const override
{
    return me_chr->get_faction();
}

std::string talker_character::short_description() const override
{
    return me_chr->short_description();
}

bool talker_character::is_deaf() const override
{
    return me_chr->is_deaf();
}

// override functions called in npctalk.cpp
void talker_character::say( const std::string &speech ) override
{
    me_chr->say( speech );
}

void talker_character::shout( const std::string &speech, bool order ) override
{
    me_chr->shout( speech, order );
}

void talker_character::add_effect( const efftype_id &new_effect, const time_duration &dur,
                                   bool permanent ) override
{
    me_chr->add_effect( new_effect, dur, num_bp, permanent );
}

void talker_character::remove_effect( const efftype_id &old_effect ) override
{
    me_chr->remove_effect( old_effect, num_bp );
}

void talker_character::set_mutation( const trait_id &new_trait ) override
{
    me_chr->set_mutation( new_trait );
}

void talker_character::unset_mutation( const trait_id &old_trait ) override
{
    me_chr->unset_mutation( old_trait );
}

void talker_character::set_value( const std::string &var_name, const std::string &value ) override
{
    me_chr->set_value( var_name, value );
}

void talker_character::remove_value( const std::string &var_name ) override
{
    me_chr->remove_value( var_name );
}

void talker_character::i_add( const item &new_item ) override
{
    me_chr->i_add( new_item );
}

void talker_character::add_debt( const int cost ) override
{
    me_chr->op_of_u.owed += cost;
}

void talker_character::use_charges( const itype_id &item_name, const int count ) override
{
    me_chr->use_charges( item_name, count );
}

void talker_character::use_amount( const itype_id &item_name, const int count ) override
{
    me_chr->use_amount( item_name, count );
}

void talker_character::remove_items_with( const item &it ) override
{
    me_chr->remove_items_with( [item_id]( const item & it ) {
        return it.typeId() == item_id;
    } );
}

void talker_character::buy_monster( talker &seller, const mtype_id &mtype, int cost,
                                    int count, bool pacified, const translation &name ) override
{
    npc *seller_guy = seller.get_npc();
    if( !seller_guy ) {
        popup( _( "%s can't sell you any %s", seller.disp_name(), mtype.obj().nname( 2 ) ) );
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

        if( !name_chr->empty() ) {
            tmp.unique_name = name_chr->translated();
        }

    }

    if( name_chr->empty() ) {
        popup( _( "%1$s gives you %2$d %3$s." ), seller_guy->name, count, mtype.obj().nname( count ) );
    } else {
        popup( _( "%1$s gives you %2$s." ), seller_guy->name, name );
    }
}

void talker_character::learn_recipe( const &recipe r ) override
{
    me_chr->learn_recipe( r );
    popup( _( "You learn how to craft %s." ), r.result_name() );
}
