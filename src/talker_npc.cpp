#include "avatar.h"
#include "game.h"
#include "game_constants.h"
#include "game_inventory.h"
#include "item.h"
#include "itype.h"
#include "item_location.h"
#include "martialarts.h"
#include "messages.h"
#include "mission.h"
#include "mission_companion.h"
#include "player.h"
#include "npc.h"
#include "npctalk.h"
#include "npctrade.h"
#include "skill.h"
#include "talker_npc.h"
#include "talker_character.h"

class Character;

static const efftype_id effect_lying_down( "lying_down" );
static const efftype_id effect_narcosis( "narcosis" );
static const efftype_id effect_npc_suspend( "npc_suspend" );
static const efftype_id effect_sleep( "sleep" );

static const trait_id trait_DEBUG_MIND_CONTROL( "DEBUG_MIND_CONTROL" );
static const trait_id trait_PROF_FOODP( "PROF_FOODP" );

bool talker_npc::has_activity() const
{
    return !me_npc->activity.is_null();
}

bool talker_npc::myclass( const npc_class_id &class_to_check ) const
{
    return me_npc->myclass == class_to_check;
}

int talker_npc::debt() const
{
    return me_npc->op_of_u.owed;
}

bool talker_npc::has_ai_rule( const std::string &type,
                              const std::string &rule ) const
{
    if( type == "aim_rule" ) {
        auto rule_val = aim_rule_strs.find( rule );
        if( rule_val != aim_rule_strs.end() ) {
            return me_npc->rules.aim == rule_val->second;
        }
    } else if( type == "engagement_rule" ) {
        auto rule_val = combat_engagement_strs.find( rule );
        if( rule_val != combat_engagement_strs.end() ) {
            return me_npc->rules.engagement == rule_val->second;
        }
    } else if( type == "cbm_reserve_rule" ) {
        auto rule_val = cbm_reserve_strs.find( rule );
        if( rule_val != cbm_reserve_strs.end() ) {
            return me_npc->rules.cbm_reserve == rule_val->second;
        }
    } else if( type == "cbm_recharge_rule" ) {
        auto rule_val = cbm_recharge_strs.find( rule );
        if( rule_val != cbm_recharge_strs.end() ) {
            return me_npc->rules.cbm_recharge == rule_val->second;
        }
    } else if( type == "ally_rule" ) {
        auto rule_val = ally_rule_strs.find( rule );
        if( rule_val != ally_rule_strs.end() ) {
            return me_npc->rules.has_flag( rule_val->second.rule );
        }
    } else if( type == "ally_override" ) {
        auto rule_val = ally_rule_strs.find( rule );
        if( rule_val != ally_rule_strs.end() ) {
            return me_npc->rules.has_override_enable( rule_val->second.rule );
        }
    } else if( type == "pickup_rule" ) {
        if( rule == "any" ) {
            return !me_npc->rules.pickup_whitelist->empty();
        }
    }

    return false;
}

std::vector<mission *> talker_npc::available_missions() const
{
    return me_npc->chatbin.missions;
}

std::vector<mission *> talker_npc::assigned_missions() const
{
    return me_npc->chatbin.missions_assigned;
}

mission *talker_npc::selected_mission() const
{
    return me_npc->chatbin.mission_selected;
}

void talker_npc::select_mission( mission *selected )
{
    me_npc->chatbin.mission_selected = selected;
}

bool talker_npc::is_following() const
{
    return me_npc->is_following();
}

bool talker_npc::is_friendly( const Character &guy ) const
{
    return me_npc->is_friendly( guy );
}

bool talker_npc::is_enemy() const
{
    return me_npc->is_enemy();
}

bool talker_npc::is_player_ally()  const
{
    return me_npc->is_player_ally();
}

std::vector<skill_id> talker_npc::skills_offered_to( const talker &student ) const
{
    if( student.get_character() ) {
        return me_npc->skills_offered_to( *student.get_character() );
    } else {
        return {};
    }
}

std::string talker_npc::skill_training_text( const talker &student,
        const skill_id &skill ) const
{
    const player *pupil = student.get_character();
    if( !pupil ) {
        return "";
    }
    const int cost = me_npc->is_ally( *pupil ) ? 0 : 1000 *
                     ( 1 + pupil->get_skill_level( skill ) ) *
                     ( 1 + pupil->get_skill_level( skill ) );
    SkillLevel skill_level_obj = pupil->get_skill_level_object( skill );
    const int cur_level = skill_level_obj.level();
    const int cur_level_exercise = skill_level_obj.exercise();
    skill_level_obj.train( 100 );
    const int next_level = skill_level_obj.level();
    const int next_level_exercise = skill_level_obj.exercise();

    //~Skill name: current level (exercise) -> next level (exercise) (cost in dollars)
    return string_format( cost > 0 ?  _( "%s: %d (%d%%) -> %d (%d%%) (cost $%d)" ) :
                          _( "%s: %d (%d%%) -> %d (%d%%)" ), skill.obj().name(), cur_level,
                          cur_level_exercise, next_level, next_level_exercise, cost / 100 );
}

std::vector<matype_id> talker_npc::styles_offered_to( const talker &student ) const
{
    if( student.get_character() ) {
        return me_npc->styles_offered_to( *student.get_character() );
    } else {
        return {};
    }
}

std::string talker_npc::style_training_text( const talker &student,
        const matype_id &style ) const
{
    if( !student.get_character() ) {
        return "";
    } else if( me_npc->is_ally( *student.get_character() ) ) {
        return string_format( "%s", style.obj().name );
    } else {
        return string_format( _( "%s ( cost $%d )" ), style.obj().name, 8 );
    }
}

std::vector<spell_id> talker_npc::spells_offered_to( talker &student )
{
    if( student.get_character() ) {
        return me_npc->spells_offered_to( *student.get_character() );
    } else {
        return {};
    }
}

std::string talker_npc::spell_training_text( talker &student, const spell_id &sp )
{
    player *pupil = student.get_character();
    if( !pupil ) {
        return "";
    }
    const spell &temp_spell = me_npc->magic.get_spell( sp );
    const bool knows = pupil->magic.knows_spell( sp );
    const int cost = me_npc->calc_spell_training_cost( knows, temp_spell.get_difficulty(),
                     temp_spell.get_level() );
    std::string text;
    if( knows ) {
        text = string_format( _( "%s: 1 hour lesson (cost %s)" ), temp_spell.name(),
                              format_money( cost ) );
    } else {
        text = string_format( _( "%s: teaching spell knowledge (cost %s)" ),
                              temp_spell.name(), format_money( cost ) );
    }
    return text;
}

void talker_npc::store_chosen_training( const skill_id &c_skill, const matype_id &c_style,
                                        const spell_id &c_spell )
{
    if( c_skill ) {
        me_npc->chatbin.skill = c_skill;
        me_npc->chatbin.style = matype_id::NULL_ID();
        me_npc->chatbin.dialogue_spell = spell_id();
    } else if( c_style ) {
        me_npc->chatbin.style = c_style;
        me_npc->chatbin.skill = skill_id::NULL_ID();
        me_npc->chatbin.dialogue_spell = spell_id();
    } else if( c_spell != spell_id() ) {
        me_npc->chatbin.style = matype_id::NULL_ID();
        me_npc->chatbin.skill = skill_id::NULL_ID();
        me_npc->chatbin.dialogue_spell = c_spell;
    }
}

// functions called in npctalk.cpp
void talker_npc::say( const std::string &speech )
{
    me_npc->say( speech );
}

bool talker_npc::will_talk_to_u( const player &u, bool force )
{
    if( u.is_dead_state() ) {
        me_npc->set_attitude( NPCATT_NULL );
        return false;
    }
    if( g->u.getID() == u.getID() ) {
        if( me_npc->get_faction() ) {
            me_npc->get_faction()->known_by_u = true;
        }
        me_npc->set_known_to_u( true );
    }
    // This is necessary so that we don't bug the player over and over
    if( me_npc->get_attitude() == NPCATT_TALK ) {
        me_npc->set_attitude( NPCATT_NULL );
    } else if( !force && ( me_npc->get_attitude() == NPCATT_FLEE ||
                           me_npc-> get_attitude() == NPCATT_FLEE_TEMP ) ) {
        add_msg( _( "%s is fleeing from you!" ), disp_name() );
        return false;
    } else if( !force && me_npc->get_attitude() == NPCATT_KILL ) {
        add_msg( _( "%s is hostile!" ), disp_name() );
        return false;
    }
    return true;
}

std::vector<std::string> talker_npc::get_topics( bool radio_contact )
{
    std::vector<std::string> add_topics;
    // For each active mission we have, let the mission know we talked to this NPC.
    for( auto &mission : g->u.get_active_missions() ) {
        mission->on_talk_with_npc( me_npc->getID() );
    }

    add_topics.push_back( me_npc->chatbin.first_topic );
    if( radio_contact ) {
        add_topics.push_back( "TALK_RADIO" );
    } else if( me_npc->is_leader() ) {
        add_topics.push_back( "TALK_LEADER" );
    } else if( me_npc->is_player_ally() && ( me_npc->is_walking_with() || me_npc->has_activity() ) ) {
        add_topics.push_back( "TALK_FRIEND" );
    } else if( me_npc->get_attitude() == NPCATT_RECOVER_GOODS ) {
        add_topics.push_back( "TALK_STOLE_ITEM" );
    }
    int most_difficult_mission = 0;
    for( auto &mission : me_npc->chatbin.missions ) {
        const auto &type = mission->get_type();
        if( type.urgent && type.difficulty > most_difficult_mission ) {
            add_topics.push_back( "TALK_MISSION_DESCRIBE_URGENT" );
            me_npc->chatbin.mission_selected = mission;
            most_difficult_mission = type.difficulty;
        }
    }
    most_difficult_mission = 0;
    bool chosen_urgent = false;
    for( auto &mission : me_npc->chatbin.missions_assigned ) {
        if( mission->get_assigned_player_id() != g->u.getID() ) {
            // Not assigned to the player that is currently talking to the npc
            continue;
        }
        const auto &type = mission->get_type();
        if( ( type.urgent && !chosen_urgent ) || ( type.difficulty > most_difficult_mission &&
                ( type.urgent || !chosen_urgent ) ) ) {
            chosen_urgent = type.urgent;
            add_topics.push_back( "TALK_MISSION_INQUIRE" );
            me_npc->chatbin.mission_selected = mission;
            most_difficult_mission = type.difficulty;
        }
    }

    // Needs
    if( me_npc->has_effect( effect_npc_suspend ) ) {
        add_topics.push_back( "TALK_REBOOT" );
    }
    if( me_npc->has_effect( effect_sleep ) || me_npc->has_effect( effect_lying_down ) ) {
        if( me_npc->has_effect( effect_narcosis ) ) {
            add_topics.push_back( "TALK_SEDATED" );
        } else {
            add_topics.push_back( "TALK_WAKE_UP" );
        }
    }

    if( add_topics.back() == "TALK_NONE" ) {
        add_topics.back() = me_npc->pick_talk_topic( g->u );
    }
    me_npc->moves -= 100;

    if( g->u.is_deaf() ) {
        if( add_topics.back() == "TALK_MUG" ||
            add_topics.back() == "TALK_STRANGER_AGGRESSIVE" ) {
            me_npc->make_angry();
            add_topics.push_back( "TALK_DEAF_ANGRY" );
        } else {
            add_topics.push_back( "TALK_DEAF" );
        }
    }

    if( me_npc->has_trait( trait_PROF_FOODP ) &&
        !( me_npc->is_wearing( itype_id( "foodperson_mask_on" ) ) ||
           me_npc->is_wearing( itype_id( "foodperson_mask" ) ) ) ) {
        add_topics.push_back( "TALK_NPC_NOFACE" );
    }
    me_npc->decide_needs();

    return add_topics;
}

std::string talker_npc::get_job_description() const
{
    return me_npc->describe_mission();
}

std::string talker_npc::evaluation_by( const talker &alpha ) const
{
    ///\EFFECT_PER affects whether player can size up NPCs

    ///\EFFECT_INT slightly affects whether player can size up NPCs
    int ability = alpha.per_cur() * 3 + alpha.int_cur();
    if( ability <= 10 ) {
        return _( "&You can't make anything out." );
    }

    if( is_player_ally() || ability > 100 ) {
        ability = 100;
    }

    std::string info = "&";
    int str_range = static_cast<int>( 100 / ability );
    int str_min = static_cast<int>( me_npc->str_max / str_range ) * str_range;
    info += string_format( _( "Str %d - %d" ), str_min, str_min + str_range );

    if( ability >= 40 ) {
        int dex_range = static_cast<int>( 160 / ability );
        int dex_min = static_cast<int>( me_npc->dex_max / dex_range ) * dex_range;
        info += string_format( _( "  Dex %d - %d" ), dex_min, dex_min + dex_range );
    }

    if( ability >= 50 ) {
        int int_range = static_cast<int>( 200 / ability );
        int int_min = static_cast<int>( me_npc->int_max / int_range ) * int_range;
        info += string_format( _( "  Int %d - %d" ), int_min, int_min + int_range );
    }

    if( ability >= 60 ) {
        int per_range = static_cast<int>( 240 / ability );
        int per_min = static_cast<int>( me_npc->per_max / per_range ) * per_range;
        info += string_format( _( "  Per %d - %d" ), per_min, per_min + per_range );
    }
    needs_rates rates = me_npc->calc_needs_rates();
    if( ability >= 100 - ( get_fatigue() / 10 ) ) {
        std::string how_tired;
        if( get_fatigue() > fatigue_levels::EXHAUSTED ) {
            how_tired = _( "Exhausted" );
        } else if( get_fatigue() > fatigue_levels::DEAD_TIRED ) {
            how_tired = _( "Dead tired" );
        } else if( get_fatigue() > fatigue_levels::TIRED ) {
            how_tired = _( "Tired" );
        } else {
            how_tired = _( "Not tired" );
            if( ability >= 100 ) {
                time_duration sleep_at = 5_minutes * ( fatigue_levels::TIRED -
                                                       get_fatigue() ) / rates.fatigue;
                how_tired += _( ".  Will need sleep in " ) + to_string_approx( sleep_at );
            }
        }
        info += "\n" + how_tired;
    }
    if( ability >= 100 ) {
        if( get_thirst() < 100 ) {
            time_duration thirst_at = 5_minutes * ( 100 - get_thirst() ) / rates.thirst;
            if( thirst_at > 1_hours ) {
                info += _( "\nWill need water in " ) + to_string_approx( thirst_at );
            }
        } else {
            info += _( "\nThirsty" );
        }
        if( get_hunger() < 100 ) {
            time_duration hunger_at = 5_minutes * ( 100 - get_hunger() ) / rates.hunger;
            if( hunger_at > 1_hours ) {
                info += _( "\nWill need food in " ) + to_string_approx( hunger_at );
            }
        } else {
            info += _( "\nHungry" );
        }
    }
    return info;

}

std::string talker_npc::opinion_text() const
{
    return me_npc->opinion_text();
}

int talker_npc::cash_to_favor( const int value ) const
{
    return npc_trading::cash_to_favor( *me_npc, value );
}

bool talker_npc::turned_hostile() const
{
    return me_npc->turned_hostile();
}

tripoint talker_npc::global_omt_goal() const
{
    return me_npc->goal;
}

// functions called in npctalk.cpp
void talker_npc::check_missions()
{
    me_npc->chatbin.check_missions();
}

void talker_npc::update_missions( const std::vector<mission *> &missions_assigned,
                                  const character_id &charID )
{
    if( me_npc->chatbin.mission_selected != nullptr ) {
        if( me_npc->chatbin.mission_selected->get_assigned_player_id() != charID ) {
            // Don't talk about a mission that is assigned to someone else.
            me_npc->chatbin.mission_selected = nullptr;
        }
    }
    if( me_npc->chatbin.mission_selected == nullptr ) {
        // if possible, select a mission to talk about
        if( !me_npc->chatbin.missions.empty() ) {
            me_npc->chatbin.mission_selected = me_npc->chatbin.missions.front();
        } else if( !missions_assigned.empty() ) {
            me_npc->chatbin.mission_selected = missions_assigned.front();
        }
    }
}

void talker_npc::set_companion_mission( const std::string &role_id )
{
    me_npc->companion_mission_role_id = role_id;
    talk_function::companion_mission( *me_npc );
}

void talker_npc::add_debt( const int cost )
{
    me_npc->op_of_u.owed += cost;
}

void talker_npc::set_fac( const faction_id &new_fac_name )
{
    me_npc->set_fac( new_fac_name );
}

void talker_npc::set_class( const npc_class_id &new_class )
{
    me_npc->myclass = new_class;
}

void talker_npc::add_faction_rep( const int rep_change )
{
    if( me_npc->get_faction()-> id != faction_id( "no_faction" ) ) {
        me_npc->get_faction()->likes_u += rep_change;
        me_npc->get_faction()->respects_u += rep_change;
    }
}

void talker_npc::toggle_ai_rule( const std::string &, const std::string &rule )
{
    auto toggle = ally_rule_strs.find( rule );
    if( toggle == ally_rule_strs.end() ) {
        return;
    }
    me_npc->rules.toggle_flag( toggle->second.rule );
    me_npc->invalidate_range_cache();
    me_npc->wield_better_weapon();
}

void talker_npc::set_ai_rule( const std::string &type, const std::string &rule )
{
    if( type == "aim_rule" ) {
        auto rule_val = aim_rule_strs.find( rule );
        if( rule_val != aim_rule_strs.end() ) {
            me_npc->rules.aim = rule_val->second;
            me_npc->invalidate_range_cache();
        }
    } else if( type == "engagement_rule" ) {
        auto rule_val = combat_engagement_strs.find( rule );
        if( rule_val != combat_engagement_strs.end() ) {
            me_npc->rules.engagement = rule_val->second;
            me_npc->invalidate_range_cache();
            me_npc->wield_better_weapon();
        }
    } else if( type == "cbm_reserve_rule" ) {
        auto rule_val = cbm_reserve_strs.find( rule );
        if( rule_val != cbm_reserve_strs.end() ) {
            me_npc->rules.cbm_reserve = rule_val->second;
        }
    } else if( type == "cbm_recharge_rule" ) {
        auto rule_val = cbm_recharge_strs.find( rule );
        if( rule_val != cbm_recharge_strs.end() ) {
            me_npc->rules.cbm_recharge = rule_val->second;
        }
    } else if( type == "ally_rule" ) {
        auto toggle = ally_rule_strs.find( rule );
        if( toggle == ally_rule_strs.end() ) {
            return;
        }
        me_npc->rules.set_flag( toggle->second.rule );
        me_npc->invalidate_range_cache();
        me_npc->wield_better_weapon();
    }
}

void talker_npc::clear_ai_rule( const std::string &, const std::string &rule )
{
    auto toggle = ally_rule_strs.find( rule );
    if( toggle == ally_rule_strs.end() ) {
        return;
    }
    me_npc->rules.clear_flag( toggle->second.rule );
    me_npc->invalidate_range_cache();
    me_npc->wield_better_weapon();
}

enum consumption_result {
    REFUSED = 0,
    CONSUMED_SOME, // Consumption didn't fail, but don't delete the item
    CONSUMED_ALL   // Consumption succeeded, delete the item
};

// Returns true if we destroyed the item through consumption
// does not try to consume contents
static consumption_result try_consume( npc &p, item &it, std::string &reason )
{
    // TODO: Unify this with 'player::consume_item()'
    item &to_eat = it;
    if( to_eat.is_null() ) {
        debugmsg( "Null item to try_consume." );
        return REFUSED;
    }
    const auto &comest = to_eat.get_comestible();
    if( !comest ) {
        // Don't inform the player that we don't want to eat the lighter
        return REFUSED;
    }

    if( !p.will_accept_from_player( it ) ) {
        reason = _( "I don't <swear> trust you enough to eat THIS…" );
        return REFUSED;
    }

    // TODO: Make it not a copy+paste from player::consume_item
    int amount_used = 1;
    if( to_eat.is_food() ) {
        if( !p.can_consume( to_eat ) ) {
            reason = _( "It doesn't look like a good idea to consume this…" );
            return REFUSED;
        } else {
            const time_duration &consume_time = p.get_consume_time( to_eat );
            p.moves -= to_moves<int>( consume_time );
            p.consume( to_eat );
            reason = _( "Thanks, that hit the spot." );

        }
    } else if( to_eat.is_medication() ) {
        if( !comest->tool.is_null() ) {
            bool has = p.has_amount( comest->tool, 1 );
            if( item::count_by_charges( comest->tool ) ) {
                has = p.has_charges( comest->tool, 1 );
            }
            if( !has ) {
                reason = string_format( _( "I need a %s to consume that!" ),
                                        item::nname( comest->tool ) );
                return REFUSED;
            }
            p.use_charges( comest->tool, 1 );
            reason = _( "Thanks, I feel better already." );
        }
        if( to_eat.type->has_use() ) {
            amount_used = to_eat.type->invoke( p, to_eat, p.pos() );
            if( amount_used <= 0 ) {
                reason = _( "It doesn't look like a good idea to consume this…" );
                return REFUSED;
            }
            reason = _( "Thanks, I used it." );
        }

        to_eat.charges -= amount_used;
        p.consume_effects( to_eat );
        p.moves -= 250;
    } else {
        debugmsg( "Unknown comestible type of item: %s\n", to_eat.tname() );
    }

    if( to_eat.charges > 0 ) {
        return CONSUMED_SOME;
    }

    // If not consuming contents and charge <= 0, we just ate the last charge from the stack
    return CONSUMED_ALL;
}

std::string talker_npc::give_item_to( const bool to_use )
{
    avatar &u = get_avatar();
    if( me_npc->is_hallucination() ) {
        return _( "No thanks, I'm good." );
    }
    item_location loc = game_menus::inv::titled_menu( g->u, _( "Offer what?" ),
                        _( "You have no items to offer." ) );
    if( !loc ) {
        return _( "Changed your mind?" );
    }
    item &given = *loc;

    if( ( &given == &u.weapon && given.has_flag( "NO_UNWIELD" ) ) ||
        ( u.is_worn( given ) && given.has_flag( "NO_TAKEOFF" ) ) ) {
        // Bionic weapon or shackles
        return _( "How?" );
    }

    if( given.is_dangerous() && !u.has_trait( trait_DEBUG_MIND_CONTROL ) ) {
        return _( "Are you <swear> insane!?" );
    }

    bool taken = false;
    std::string reason = _( "Nope." );
    int our_ammo = me_npc->ammo_count_for( me_npc->weapon );
    int new_ammo = me_npc->ammo_count_for( given );
    const double new_weapon_value = me_npc->weapon_value( given, new_ammo );
    const double cur_weapon_value = me_npc->weapon_value( me_npc->weapon, our_ammo );
    add_msg( m_debug, "NPC evaluates own %s (%d ammo): %0.1f",
             me_npc->weapon.typeId().str(), our_ammo, cur_weapon_value );
    add_msg( m_debug, "NPC evaluates your %s (%d ammo): %0.1f",
             given.typeId().str(), new_ammo, new_weapon_value );
    if( to_use ) {
        // Eating first, to avoid evaluating bread as a weapon
        const consumption_result consume_res = try_consume( *me_npc, given, reason );
        if( consume_res != REFUSED ) {
            if( consume_res == CONSUMED_ALL ) {
                u.i_rem( &given );
            }
            u.moves -= 100;
            if( given.is_container() ) {
                given.on_contents_changed();
            }
        }// wield it if its a weapon
        else if( new_weapon_value > cur_weapon_value ) {
            me_npc->wield( given );
            reason = _( "Thanks, I'll wield that now." );
            taken = true;
        }// HACK: is_gun here is a hack to prevent NPCs wearing guns if they don't want to use them
        else if( !given.is_gun() && given.is_armor() ) {
            //if it is impossible to wear return why
            ret_val<bool> can_wear = me_npc->can_wear( given, true );
            if( !can_wear.success() ) {
                reason = can_wear.str();
            } else {
                //if we can wear it with equip changes prompt first
                can_wear = me_npc->can_wear( given );
                if( ( can_wear.success() ||
                      query_yn( can_wear.str() + _( " Should I take something off?" ) ) )
                    && me_npc->wear_if_wanted( given, reason ) ) {
                    taken = true;
                } else {
                    reason = can_wear.str();
                }
            }
        } else {
            reason += string_format( _( "My current weapon is better than this.\n"
                                        "(new weapon value: %.1f vs %.1f)." ), new_weapon_value,
                                     cur_weapon_value );
        }
    } else {//allow_use is false so try to carry instead
        if( me_npc->can_pickVolume( given ) && me_npc->can_pickWeight( given ) ) {
            reason = _( "Thanks, I'll carry that now." );
            taken = true;
            me_npc->i_add( given );
        } else {
            if( !me_npc->can_pickVolume( given ) ) {
                const units::volume free_space = me_npc->volume_capacity() -
                                                 me_npc->volume_carried();
                reason += "\n" + std::string( _( "I have no space to store it." ) ) + "\n";
                if( free_space > 0_ml ) {
                    reason += string_format( _( "I can only store %s %s more." ),
                                             format_volume( free_space ), volume_units_long() );
                } else {
                    reason += _( "…or to store anything else for that matter." );
                }
            }
            if( !me_npc->can_pickWeight( given ) ) {
                reason += std::string( "\n" ) + _( "It is too heavy for me to carry." );
            }
        }
    }


    if( taken ) {
        u.i_rem( &given );
        u.moves -= 100;
        me_npc->has_new_items = true;
    }

    return reason;
}

void talker_npc::add_mission( const mission_type_id &mission_id )
{
    mission *miss = mission::reserve_new( mission_id, me_npc->getID() );
    miss->assign( get_avatar() );
    me_npc->chatbin.missions_assigned.push_back( miss );
}

void talker_npc::add_opinion( const int trust, const int fear, const int value,
                              const int anger, const int debt )
{
    me_npc->op_of_u += npc_opinion( trust, fear, value, anger, debt );
}

bool talker_npc::buy_from( const int amount )
{
    return npc_trading::pay_npc( *me_npc, amount );
}

bool talker_npc::check_hostile_response( const int anger ) const
{
    return me_npc->op_of_u.anger + anger > me_npc->hostile_anger_level();
}

void talker_npc::make_angry()
{
    me_npc->make_angry();
}

bool talker_npc::enslave_mind()
{
    bool not_following = g->get_follower_list().count( me_npc->getID() ) == 0;
    me_npc->companion_mission_role_id.clear();
    talk_function::follow( *me_npc );
    return not_following;
}

// Every OWED_VAL that the NPC owes you counts as +1 towards convincing
static constexpr int OWED_VAL = 1000;
int talker_npc::parse_mod( const std::string &attribute, const int factor ) const
{
    int modifier = 0;
    if( attribute == "ANGER" ) {
        modifier = me_npc->op_of_u.anger;
    } else if( attribute == "FEAR" ) {
        modifier = me_npc->op_of_u.fear;
    } else if( attribute == "TRUST" ) {
        modifier = me_npc->op_of_u.trust;
    } else if( attribute == "VALUE" ) {
        modifier = me_npc->op_of_u.value;
    } else if( attribute == "POS_FEAR" ) {
        modifier = std::max( 0, me_npc->op_of_u.fear );
    } else if( attribute == "AGGRESSION" ) {
        modifier = me_npc->personality.aggression;
    } else if( attribute == "ALTRUISM" ) {
        modifier = me_npc->personality.altruism;
    } else if( attribute == "BRAVERY" ) {
        modifier = me_npc->personality.bravery;
    } else if( attribute == "COLLECTOR" ) {
        modifier = me_npc->personality.collector;
    } else if( attribute == "MISSIONS" ) {
        modifier = me_npc->assigned_missions_value() / OWED_VAL;
    } else if( attribute == "NPC_INTIMIDATE" ) {
        modifier = me_npc->intimidation();
    }
    modifier *= factor;
    return modifier;
}

int talker_npc::trial_chance_mod( const std::string &trial_type ) const
{
    int chance = 0;
    if( trial_type == "lie" ) {
        chance += - me_npc->talk_skill() + me_npc->op_of_u.trust * 3;
    } else if( trial_type == "persuade" ) {
        chance += - static_cast<int>( me_npc->talk_skill() * 0.5 ) +
                  me_npc->op_of_u.trust * 2 + me_npc->op_of_u.value;
    } else if( trial_type == "intimidate" ) {
        chance += - me_npc->intimidation() + me_npc->op_of_u.fear * 2 -
                  me_npc->personality.bravery * 2;
    }
    return chance;
}
