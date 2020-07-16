#include "character_id.h"
#include "dialogue_chatbin.h"
#include "game.h"
#include "mission.h"
#include "skill.h"

void dialogue_chatbin::add_new_mission( mission *miss )
{
    if( miss == nullptr ) {
        return;
    }
    missions.push_back( miss );
}

void dialogue_chatbin::check_missions()
{
    // TODO: or simply fail them? Some missions might only need to be reported.
    auto &ma = missions_assigned;
    const auto last = std::remove_if( ma.begin(), ma.end(), []( class mission const * m ) {
        return !m->is_assigned();
    } );
    std::copy( last, ma.end(), std::back_inserter( missions ) );
    ma.erase( last, ma.end() );
}

void dialogue_chatbin::store_chosen_training( const skill_id &c_skill, const matype_id &c_style,
        const spell_id &c_spell )
{
    if( c_skill == skill_id() && c_style == matype_id() && c_spell == spell_id() ) {
        return;
    }
    clear_training();
    if( c_skill != skill_id() ) {
        skill = c_skill;
    } else if( c_style != matype_id() ) {
        style = c_style;
    } else if( c_spell != spell_id() ) {
        dialogue_spell = c_spell;
    }
}

void dialogue_chatbin::clear_training()
{
    style = matype_id();
    skill = skill_id();
    dialogue_spell = spell_id();
}

void dialogue_chatbin::clear_all()
{
    clear_training();
    missions.clear();
    missions_assigned.clear();
    mission_selected = nullptr;
}

void smart_chatbin::load( const JsonObject &jo, const std::string & )
{
    bool was_loaded = false;
    mandatory( jo, was_loaded, "id", my_id );
    my_npc_id = g->assign_npc_id();
    mandatory( jo, was_loaded, "chat", first_topic );
    std::vector<mission_type_id> tmp_miss_ids;
    optional( jo, was_loaded, "missions", tmp_miss_ids );
    for( const mission_type_id &miss_id : tmp_miss_ids ) {
        add_new_mission( mission::reserve_new( miss_id, my_npc_id ) );
    }
    optional( jo, was_loaded, "traits", my_traits );
    optional( jo, was_loaded, "effects", my_effects );
    optional( jo, was_loaded, "styles", my_styles );
    optional( jo, was_loaded, "spells", my_spells );
    optional( jo, was_loaded, "values", my_values );
    if( jo.has_object( "skills" ) ) {
        JsonObject jo_skill = jo.get_object( "skills" );
        for( JsonMember jm : jo_skill ) {
            const std::string &my_skill = jm.name();
            int my_skill_level = jo_skill.get_int( my_skill );
            my_skills->mod_skill_level( skill_id( my_skill ), my_skill_level );
        }
    }
}

void smart_chatbin::check()
{
}

void smart_chatbin::reset()
{
    clear_all();
    my_traits.clear();
    my_effects.clear();
    my_spells.clear();
    my_values.clear();
}

