#include "character_id.h"
#include "dialogue_chatbin.h"
#include "item.h"
#include "mission.h"
#include "skill.h"
#include "talker_item.h"

// identity and location
std::string talker_item::disp_name() const
{
	return "";
}

character_id talker_item::getID() const
{
	return me_chat.my_npc_id;
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
	return 0;
}

tripoint talker_item::global_omt_location() const
{
	return tripoint_zero;
}

// mandatory functions for starting a dialogue
bool talker_item::will_talk_to_u( const player &, bool )
{
	return false;
}

std::vector<std::string> talker_item::get_topics( bool )
{
	return "";
}

void talker_item::check_missions()
{
}

void talker_item::update_missions( const std::vector<mission *> &, const character_id & )
{
}

// stats, skills, traits, bionics, and magic
int talker_item::get_skill_level( const skill_id &skill ) const
{
    return me_chat->my_skills.get_skill_level( skill );
}

bool talker_item::has_trait( const trait_id &trait_to_test ) const
{
    return me_chat->my_traits.count( trait_to_test ) > 0;
}

void talker_item::set_mutation( const trait_id &add_trait )
{
}

void talker_item::unset_mutation( const trait_id &lose_trait )
{
}

bool talker_item::has_trait_flag( const std::string &trait_flag_to_check ) const
{
	return false;
}

bool talker_item::crossed_threshold() const
{
	return false;
}

int talker_item::num_bionics() const
{
	return false;
}

bool talker_item::has_bionic( const bionic_id &bio_to_test ) const
{
	return me_chat->my_bionics.count( bio_to_test ) > 0;
}

bool talker_item::knows_spell( const spell_id &spell_to_test ) const
{
	return me_chat->my_spells.count( spell_to_test ) > 0;
}

std::vector<skill_id> talker_item::skills_offered_to( const talker &student ) const
{
	return {};
}

std::string talker_item::skill_training_text( const talker &student,
        const skill_id &skill_to_learn ) const
{
	return "";
}

std::vector<matype_id> talker_item::styles_offered_to( const talker &student ) const
{
	return {};
}

std::string talker_item::style_training_text( const talker &student,
        const matype_id &style_to_learn ) const
{
	return "";
}

std::vector<spell_id> talker_item::spells_offered_to( talker &student )
{
	return {};
}

std::string talker_item::spell_training_text( talker &student, const spell_id &spell_to_learn )
{
	return "";
}

void talker_item::store_chosen_training( const skill_id &train_skill, const matype_id &train_style,
        const spell_id &train_spell )
{
	me_chat->store_chosen_training( train_skill, train_style, train_spell );
}

// effects and values
bool talker_item::has_effect( const efftype_id &effect_to_test ) const
{
	returm me_chat->my_effects.count( effect_to_test ) > 0;
}

bool talker_item::is_deaf() const
{
	return false;
}

void talker_item::add_effect( const efftype_id &new_effect, const time_duration &, bool )
{
}

void talker_item::remove_effect( const efftype_id &lose_effect )
{
}

std::string talker_item::get_value( const std::string &val_to_test ) const
{
	const auto it = me_chat->my_values.find( val_to_test );
	if( it != me_chat->my_values.end() ) {
	    return it->second;
	}
	return "";
}

void talker_item::set_value( const std::string &val_to_set, const std::string &new_val )
{
}

void talker_item::remove_value( const std::string &val_to_lose )
{
}

// inventory, buying, and selling

// missions
std::vector<mission *> talker_item::available_missions() const
{
	return me_chat->missions;
}

std::vector<mission *> talker_item::assigned_missions() const
{
	return me_chat->missions_assigned;
}

mission *talker_item::selected_mission() const
{
	return me_chat->mission_selected;
}

void talker_item::select_mission( mission *miss )
{
	me_chat->mission_selected = miss;
}

void talker_item::add_mission( const mission_type_id &new_miss )
{
}

// factions and alliances
faction *talker_item::get_faction() const
{
	return me_chat->my_faction;
}

bool talker_item::is_friendly( const Character &guy ) const
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
}

void talker_item::shout( const std::string &speech = "", bool order = false )
{
}

// miscellaneous
void talker_item::set_first_topic( const std::string &new_chat )
{
	me_chat->first_topic = new_chat;
}
