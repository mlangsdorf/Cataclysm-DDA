#include "avatar.h"
#include "character.h"
#include "character_id.h"
#include "dialogue_chatbin.h"
#include "item.h"
#include "messages.h"
#include "mission.h"
#include "mutation.h"
#include "skill.h"
#include "talker_item.h"

static const efftype_id effect_deaf( "deaf" );

static const trait_id trait_DEAF( "DEAF" );

talker_item::talker_item( item *new_me )
{
    me_item = new_me;
}
// identity and location
std::string talker_item::disp_name() const
{
    return "";
}

character_id talker_item::getID() const
{
    return me_chat().my_npc_id;
}

int talker_item::posx() const
{
    return 0;
}

int talker_item::posy() const
{
    return 0;
}

int talker_item::posz() const
{
    return 0;
}

tripoint talker_item::pos() const
{
    return tripoint_zero;
}

tripoint_abs_omt talker_item::global_omt_location() const
{
    return tripoint_abs_omt( 0, 0, 0 );
}

// mandatory functions for starting a dialogue
bool talker_item::will_talk_to_u( const player &, bool )
{
    return false;
}

std::vector<std::string> talker_item::get_topics( bool )
{
    std::vector<std::string> all_topics;
    all_topics.push_back( me_chat().first_topic );
    return all_topics;
}

void talker_item::check_missions()
{
    me_chat().check_missions();
}

void talker_item::update_missions( const std::vector<mission *> &missions_assigned,
                                   const character_id &avatar_id )
{
    if( me_chat().mission_selected != nullptr ) {
        if( me_chat().mission_selected->get_assigned_player_id() != avatar_id ) {
            // Don't talk about a mission that is assigned to someone else.
            me_chat().mission_selected = nullptr;
        }
    }
    if( me_chat().mission_selected == nullptr ) {
        // if possible, select a mission to talk about
        if( !me_chat().missions.empty() ) {
            me_chat().mission_selected = me_chat().missions.front();
        } else if( !missions_assigned.empty() ) {
            me_chat().mission_selected = missions_assigned.front();
        }
    }
}

// stats, skills, traits, bionics, and magic
int talker_item::get_skill_level( const skill_id &skill ) const
{
    return me_chat().my_skills->get_skill_level( skill );
}

bool talker_item::has_trait( const trait_id &trait_to_test ) const
{
    return me_chat().my_traits.count( trait_to_test ) > 0;
}

void talker_item::set_mutation( const trait_id &add_trait )
{
    me_chat().my_traits.insert( add_trait );
}

void talker_item::unset_mutation( const trait_id &lose_trait )
{
    const auto it = me_chat().my_traits.find( lose_trait );
    if( it != me_chat().my_traits.end() ) {
        me_chat().my_traits.erase( it );
    }
}

bool talker_item::has_trait_flag( const std::string &trait_flag_to_check ) const
{
    // UGLY, SLOW, should be cached as my_mutation_flags or something
    for( const trait_id &mut : me_chat().my_traits ) {
        const mutation_branch &mut_data = mut.obj();
        if( mut_data.flags.count( trait_flag_to_check ) > 0 ) {
            return true;
        }
    }

    return false;
}

bool talker_item::crossed_threshold() const
{
    for( const trait_id &mut : me_chat().my_traits ) {
        if( mut->threshold ) {
            return true;
        }
    }
    return false;
}

int talker_item::num_bionics() const
{
    return me_chat().my_bionics.size();
}

bool talker_item::has_bionic( const bionic_id &bio_to_test ) const
{
    return me_chat().my_bionics.count( bio_to_test ) > 0;
}

bool talker_item::knows_spell( const spell_id &spell_to_test ) const
{
    return me_chat().my_spells.count( spell_to_test ) > 0;
}

std::vector<skill_id> talker_item::skills_offered_to( const talker &student ) const
{
    player *pupil = student.get_character();
    std::vector<skill_id> ret;
    if( pupil ) {
        for( const auto &pair : *me_chat().my_skills ) {
            const skill_id &id = pair.first;
            if( pupil->get_skill_level( id ) < pair.second.level() ) {
                ret.push_back( id );
            }
        }
    }
    return ret;
}

std::string talker_item::skill_training_text( const talker &student,
        const skill_id &skill_to_learn ) const
{
    player *pupil = student.get_character();
    if( !pupil ) {
        return "";
    }
    SkillLevel skill_level_obj = pupil->get_skill_level_object( skill_to_learn );
    const int cur_level = skill_level_obj.level();
    const int cost = is_friendly( *pupil ) ? 0 : 10 * ( 1 + cur_level ) * ( 1 + cur_level );
    const int cur_level_exercise = skill_level_obj.exercise();
    skill_level_obj.train( 100 );
    const int next_level = skill_level_obj.level();
    const int next_level_exercise = skill_level_obj.exercise();

    //~Skill name: current level (exercise) -> next level (exercise) (cost in dollars)
    return string_format( cost > 0 ?  _( "%s: %d (%d%%) -> %d (%d%%) (cost $%d)" ) :
                          _( "%s: %d (%d%%) -> %d (%d%%)" ), skill_to_learn.obj().name(),
                          cur_level, cur_level_exercise, next_level, next_level_exercise,
                          cost );
}

std::vector<matype_id> talker_item::styles_offered_to( const talker &student ) const
{
    std::vector<matype_id> teachable;
    player *pupil = student.get_character();
    if( pupil ) {
        for( const matype_id &teacher_style : me_chat().my_styles ) {
            if( !pupil->martial_arts_data.knows_style( teacher_style ) ) {
                teachable.push_back( teacher_style );
            }
        }
    }
    return teachable;
}

std::string talker_item::style_training_text( const talker &student,
        const matype_id &style_to_learn ) const
{
    player *pupil = student.get_character();
    if( !pupil ) {
        return "";
    } else if( is_friendly( *pupil ) ) {
        return string_format( "%s", style_to_learn.obj().name );
    } else {
        return string_format( _( "%s ( cost $%d )" ), style_to_learn.obj().name, 8 );
    }

}

std::vector<spell_id> talker_item::spells_offered_to( talker &student )
{
    std::vector<spell_id> teachable;
    player *pupil = student.get_character();
    if( pupil ) {
        for( const auto &sp_data : me_chat().my_spells ) {
            const spell_id &teacher_spell = sp_data.first;
            const int teacher_level = sp_data.second;
            if( pupil->magic.can_learn_spell( *pupil, teacher_spell ) ) {
                if( pupil->magic.knows_spell( teacher_spell ) ) {
                    const spell &student_spell = pupil->magic.get_spell( teacher_spell );
                    if( student_spell.is_max_level() ||
                        student_spell.get_level() >= teacher_level ) {
                        continue;
                    }
                }
                teachable.emplace_back( teacher_spell );
            }
        }
    }
    return teachable;

}

std::string talker_item::spell_training_text( talker &student, const spell_id &spell_to_learn )
{
    player *pupil = student.get_character();
    if( !pupil || me_chat().my_spells.find( spell_to_learn ) == me_chat().my_spells.end() ) {
        return "";
    }
    const spell &temp_spell = pupil->magic.get_spell( spell_to_learn );
    const bool knows = pupil->magic.knows_spell( spell_to_learn );
    const int cost = is_friendly( *pupil ) ? 0 : std::max( 1, temp_spell.get_difficulty() ) *
                     std::max( 1, me_chat().my_spells[spell_to_learn] ) * 100;
    std::string text;
    if( knows ) {
        text = string_format( _( "%s: 1 hour lesson (cost %s)" ), temp_spell.name(),
                              format_money( cost ) );
    } else {
        text = string_format( _( "%s: teaching spell knowledge (cost %s)" ),
                              temp_spell.name(), format_money( 2 * cost ) );
    }
    return text;
}

void talker_item::store_chosen_training( const skill_id &train_skill, const matype_id &train_style,
        const spell_id &train_spell )
{
    if( train_skill != skill_id() || train_style != matype_id() || train_spell != spell_id() ) {
        me_chat().store_chosen_training( train_skill, train_style, train_spell );
    }
}

// effects and values
bool talker_item::has_effect( const efftype_id &effect_to_test ) const
{
    return me_chat().my_effects.count( effect_to_test ) > 0;
}

bool talker_item::is_deaf() const
{
    return has_effect( effect_deaf ) || has_trait( trait_DEAF );
}

void talker_item::add_effect( const efftype_id &new_effect, const time_duration &, bool )
{
    me_chat().my_effects.insert( new_effect );
}

void talker_item::remove_effect( const efftype_id &lose_effect )
{
    const auto it = me_chat().my_effects.find( lose_effect );
    if( it != me_chat().my_effects.end() ) {
        me_chat().my_effects.erase( it );
    }
}

std::string talker_item::get_value( const std::string &val_to_test ) const
{
    const auto it = me_chat().my_values.find( val_to_test );
    if( it != me_chat().my_values.end() ) {
        return it->second;
    }
    return "";
}

void talker_item::set_value( const std::string &val_to_set, const std::string &new_val )
{
    const auto it = me_chat().my_values.find( val_to_set );
    if( it == me_chat().my_values.end() ) {
        me_chat().my_values[ val_to_set ] = new_val;
    } else {
        it->second = new_val;
    }
}

void talker_item::remove_value( const std::string &val_to_lose )
{
    const auto it = me_chat().my_values.find( val_to_lose );
    if( it != me_chat().my_values.end() ) {
        me_chat().my_values.erase( val_to_lose );
    }
}

// inventory, buying, and selling

// missions
std::vector<mission *> talker_item::available_missions() const
{
    return me_chat().missions;
}

std::vector<mission *> talker_item::assigned_missions() const
{
    return me_chat().missions_assigned;
}

mission *talker_item::selected_mission() const
{
    return me_chat().mission_selected;
}

void talker_item::select_mission( mission *miss )
{
    me_chat().mission_selected = miss;
}

void talker_item::add_mission( const mission_type_id &new_miss )
{
    mission *miss = mission::reserve_new( new_miss, me_chat().my_npc_id );
    miss->assign( get_avatar() );
    me_chat().missions_assigned.push_back( miss );
}

// factions and alliances
faction *talker_item::get_faction() const
{
    return me_chat().my_faction;
}

bool talker_item::is_friendly( const Character & ) const
{
    return false;
}

bool talker_item::is_player_ally() const
{
    return false;
}

bool talker_item::is_enemy() const
{
    return false;
}

// other descriptors
std::string talker_item::short_description() const
{
    return "";
}

// speaking
void talker_item::say( const std::string &speech )
{
    add_msg( string_format( _( "%s says %s" ), disp_name(), speech ) );
}

void talker_item::shout( const std::string &speech, const bool order )
{
    if( order ) {
        add_msg( string_format( _( "%s shouts %s" ), disp_name(), speech ) );
    }
}

// miscellaneous
void talker_item::set_first_topic( const std::string &new_chat )
{
    me_chat().first_topic = new_chat;
}
