#if 0
#include "character.h"
#include "npc.h"
#include "talker_npc.h"

talker_npc::talker_npc( npc *new_me )
{
    me_npc = new_me;
    me_chr = new_me;
}

bool talker_npc::has_activity() const override
{
    return !me_npc->activity.is_null();
}

bool talker_npc::myclass( const npc_class_id &class_to_check ) const override
{
    return me_npc->my_class == class_to_check;
}

int talker_npc::debt() const override
{
    return me_npc->op_o_u.owed;
}

bool talker_npc::has_ai_rule( const std::string &type,
                              const std::string &rule ) const override
{
    if( type == "aim_rule" ) {
        auto rule_val = aim_rule_strs.find( rule );
        if( rule_val != aim_rule_strs.end() ) {
            return me_npc->rules.aim == rule_val->second;
        }
    } else if( type == "engagement_rule" ) {
        auto rule_val = combat_engagement_rule_strs.find( rule );
        if( rule_val != combat_engagement_rule_strs.end() ) {
            return me_npc->rules.engagement == rule_val->second;
        }
    } else if( type == "cbm_reserve_rule" ) {
        auto rule_val = cbm_reserve_rule_strs.find( rule );
        if( rule_val != cbm_reserve_rule_strs.end() ) {
            return me_npc->rules.cbm_reserve == rule_val->second;
        }
    } else if( type == "cbm_recharge_rule" ) {
        auto rule_val = cbm_recharge_rule_strs.find( rule );
        if( rule_val != cbm_recharge_rule_strs.end() ) {
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

std::vector<missions *> talker_npc::available_missions() const override
{
    return me_npc->chatbin.missions;
}

std::vector<mission *> talker_npc::assigned_missions() const override
{
    return me_npc->chatbin.missions_assigned;
}

mission *talker_npc::selected_mission() const override
{
    return me_npc->chatbin.mission_selected;
}

void select_mission( mission *selected ) override
{
    me_npc->chatbin.mission_selected = selected;
}

bool talker_npc::is_following() const override
{
    return me_npc->is_following();
}

bool talker_npc::is_friendly( const Character &guy ) const override
{
    return me_npc->is_friendly( guy );
}

bool talker_npc::is_enemy() const override
{
    return me_npc->is_enemy();
}

bool talker_npc::is_player_ally()  const override
{
    return me_npc->is_player_ally();
}

std::vector<skill_id> skills_offered_to( const talker &student ) const override
{
    if( student->get_character() ) {
        return me_npc->styles_offered_to( *student->get_character() );
    } else {
        return {};
    }
}

std::string npc_talker::skill_training_text( const talker &student,
        const skill_id &skill ) const override
{
    Character *pupil = student.get_character();
    if( !pupil ) {
        return "";
    }
    const int cost = me_npc->is_ally( *pupil ) ? 0 : 1000 *
                     ( 1 + pupil->get_skill_level( skill ) ) * ( 1 + pupil->get_skill_level( skill ) );
    SkillLevel skill_level_obj = pupil.get_skill_level_object( skill );
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

std::vector<matype_id> styles_offered_to( const talker &student ) const override
{
    if( student->get_character() ) {
        return me_npc->styles_offered_to( *student->get_character() );
    } else {
        return {};
    }
}

std::string npc_talker::style_training_text( const talker &student,
        const matype_id &style ) const override
{
    if( !student.get_character() ) {
        return "";
    } else if( me_npc->is_ally( *student.get_character() ) ) {
        return style.obj().name;
    } else {
        return string_format( _( "%s ( cost $%d )" ), style.obj().name, 8 )
    }
}

std::vector<spell_id> spells_offered_to( talker &student ) override
{
    if( student->get_character() ) {
        return me_npc->spells_offered_to( *student->get_character() );
    } else {
        return {};
    }
}

std::string npc_talker::spell_training_text( talker &student, const spell_id &sp ) override
{
    Character *pupil = student.get_character();
    if( !pupil ) {
        return "";
    }
    const spell &temp_spell = me_npc->magic.get_spell( sp );
    const bool knows = pupil.magic.knows_spell( sp );
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
                                        const spell_id &c_spell ) override
{
    if( c_skill ) {
        beta->chatbin.skill = c_skill;
        beta->chatbin.style = matype_id::NULL_ID();
        beta->chatbin.dialogue_spell = spell_id();
    } else if( c_style ) {
        beta->chatbin.style = c_style;
        beta->chatbin.skill = skill_id::NULL_ID();
        beta->chatbin.dialogue_spell = spell_id();
    } else if( c_spell != spell_id() ) {
        beta->chatbin.style = matype_id::NULL_ID();
        beta->chatbin.skill = skill_id::NULL_ID();
        beta->chatbin.dialogue_spell = c_spell;
    }
}

bool talker_npc::has_stolen_item( const talker_character &guy ) const override
{
    if( guy.get_character() ) {
        const character &owner = guy.get_character();
        for( auto &elem : me_npc->inv_dump() ) {
            if( elem->is_old_owner( &guy, true ) ) {
                return true;
            }
        }
    }
    return false;
}

// override functions called in npctalk.cpp
bool talker_npc::will_talk_to_u( const character &u, bool force ) override
{
    if( u.is_dead_state() ) {
        set_attitude( NPCATT_NULL );
        return false;
    }
    // This is necessary so that we don't bug the player over and over
    if( get_attitude() == NPCATT_TALK ) {
        set_attitude( NPCATT_NULL );
    } else if( !force && ( get_attitude() == NPCATT_FLEE || get_attitude() == NPCATT_FLEE_TEMP ) ) {
        add_msg( _( "%s is fleeing from you!" ), name );
        return false;
    } else if( !force && get_attitude() == NPCATT_KILL ) {
        add_msg( _( "%s is hostile!" ), name );
        return false;
    }
    if( g->u == u ) {
        if( get_faction() ) {
            get_faction()->known_by_u = true;
        }
        set_known_to_u( true );
    }
    return true;
}

std::vector<std::string> talker_npc::get_topics( bool radio_contact ) override
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

    if( add_topics.back().id == "TALK_NONE" ) {
        add_topics.back() = talk_topic( pick_talk_topic( g->u ) );
    }
    me_npc->moves -= 100;

    if( g->u.is_deaf() ) {
        if( add_topics.back().id == "TALK_MUG" ||
            add_topics.back().id == "TALK_STRANGER_AGGRESSIVE" ) {
            me_npc->make_angry();
            add_topics_push_back( "TALK_DEAF_ANGRY" );
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

std::string talker_npc::get_job_description() const override
{
    return me_npc->describe_mission();
}

std::string talker_npc::evaluation_by( const talker &alpha ) const override
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
    int str_min = static_cast<int>( me_npc->str_max() / str_range ) * str_range;
    info += string_format( _( "Str %d - %d" ), str_min, str_min + str_range );

    if( ability >= 40 ) {
        int dex_range = static_cast<int>( 160 / ability );
        int dex_min = static_cast<int>( me_npc->dex_max() / dex_range ) * dex_range;
        info += string_format( _( "  Dex %d - %d" ), dex_min, dex_min + dex_range );
    }

    if( ability >= 50 ) {
        int int_range = static_cast<int>( 200 / ability );
        int int_min = static_cast<int>( me_npc->int_max() / int_range ) * int_range;
        info += string_format( _( "  Int %d - %d" ), int_min, int_min + int_range );
    }

    if( ability >= 60 ) {
        int per_range = static_cast<int>( 240 / ability );
        int per_min = static_cast<int>( me_npc->per_max() / per_range ) * per_range;
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

std::string talker_npc::opinion_text() const override
{
    return me_npc->opinion_text();
}

int talker_npc::cash_to_favor( const int value ) override
{
    return npc_trading::cash_to_favor( me, value );
}

bool talker_npc::turned_hostile() const override
{
    return me_npc->turned_hostile();
}

tripoint talker_npc::global_omt_goal() const override
{
    return me_npc->goal;
}

// override functions called in npctalk.cpp
void talker_npc::check_missions() override
{
    chatbin.check_missions();
}

void talker_npc::update_missions( const std::vector<mission *> &missions_assigned,
                                  const character_id &charID ) override
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

void talker_npc::set_companion_mission( const std::string &role_id ) override
{
    return me_npc->companion_mission_role_id = role_id;
    talk_function::companion_mission( me );
}

void talker_npc::add_debt( const int cost ) override
{
    me_npc->op_of_u.owed += cost;
}


bool talker_npc::sell_to( int cost ) override
{
    return npc_trading::pay_npc( me, cost );
}

void talker_npc::set_fac( const faction_id &new_fac_name ) override
{
    me_npc->set_fac( new_fac_name );
}

void talker_npc::set_class( const npc_class_id &new_class ) override
{
    me_npc->my_class = new_class;
}

void talker_npc::add_faction_rep( const int rep_change ) override
{
    if( me_npc->get_faction()-> id != faction_id( "no_faction" ) ) {
        me_npc->get_faction()->likes_u += rep_change;
        me_npc->get_faction()->respects_u += rep_change;
    }
}

void talker_npc::toggle_ai_rule( const std::string &, const std::string &rule ) override
{
    auto toggle = ally_rule_strs.find( rule );
    if( toggle == ally_rule_strs.end() ) {
        return;
    }
    me_npc->rules.toggle_flag( toggle->second.rule );
    me_npc->invalidate_range_cache();
    me_npc->wield_better_weapon();
}

void talker_npc::set_ai_rule( const std::string &type, const std::string &rule ) override;
{
} else if( type == "aim_rule" )
{
    auto rule_val = aim_rule_strs.find( rule );
    if( rule_val != aim_rule_strs.end() ) {
        me_npc->rule.aim = rule_val->second;
        me_npc->invalidate_range_cache();
    }
} else if( type == "engagement_rule" )
{
    auto rule_val = combat_engagement_rule_strs.find( rule );
    if( rule_val != combat_engagement_rule_strs.end() ) {
        me_npc->rules.engagement = rule_val->second;
        me_npc->invalidate_range_cache();
        me_npc->wield_better_weapon();
    }
} else if( type == "cbm_reserve_rule" )
{
    auto rule_val = cbm_reserve_rule_strs.find( rule );
    if( rule_val != cbm_reserve_rule_strs.end() ) {
        me_npc->rules.cbm_reserve = rule_val->second;
    }
} else if( type == "cbm_recharge_rule" )
{
    auto rule_val = cbm_recharge_rule_strs.find( rule );
    if( rule_val != cbm_recharge_rule_strs.end() ) {
        me_npc->rules.cbm_recharge = rule_val->second;
    }
} else if( type == "ally_rule" )
{
    auto toggle = ally_rule_strs.find( rule );
    if( toggle == ally_rule_strs.end() ) {
        return;
    }
    me_npc->rules.set_flag( toggle->second.rule );
    me_npc->invalidate_range_cache();
    me_npc->wield_better_weapon();
}
}

void talker_npc::clear_ai_rule( const std::string &, const std::string &rule ) override
{
    auto toggle = ally_rule_strs.find( rule );
    if( toggle == ally_rule_strs.end() ) {
        return;
    }
    me_npc->rules.clear_flag( toggle->second.rule );
    me_npc->invalidate_range_cache();
    me_npc->wield_better_weapon();
}

void talker_npc::give_item_to( const bool to_use ) override
{
    give_item_to( me, to_use );
}

void talker_npc::add_mission( const mission_type_id &mission_id ) override
{
    mission *miss = mission::reserve_new( mission_id, me_npc->getID() );
    miss->assign( get_avatar() );
    me_npc->chatbin.missions_assigned.push_back( miss );
}

void talker_npc::add_opinion( const int trust, const int fear, const int value,
                              const int anger ) override
{
    me_npc->op_of_u += npc_opinion( trust, fear, value, anger );
}

bool talker_npc::buy_from( const int amount ) override
{
    return npc_trading::pay_npc( me_npc, amount );
}

bool talker_npc::check_hostile_response( const int anger ) const override
{
    return me_npc->op_of_u.anger + anger > me_npc->hostile_anger_level();
}

void talker_npc::make_angry() override
{
    me_npc->make_angry();
}

bool talker_npc::enslave_mind() override
{
    bool not_following = g->get_follower_list().count( me.getID() ) == 0;
    me.companion_mission_role_id.clear();
    talk_function::follow( me );
    return not_following;
}
#endif
