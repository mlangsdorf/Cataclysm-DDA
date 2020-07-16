#include "dialogue_chatbin.h"
#include "item.h"
#include "mission.h"

// identity and location
std::string talker_item::disp_name() const
{
}

character_id talker_item::getID() const
{
}

int talker_item::posx() const
{
}

int talker_item::posy() const
{
}

int talker_item::posz() const
{
}

tripoint talker_item::pos() const
{
}

tripoint talker_item::global_omt_location() const
{
}

// mandatory functions for starting a dialogue
bool talker_item::will_talk_to_u( const player &, bool )
{
}

std::vector<std::string> talker_item::get_topics( bool )
{
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
}

bool talker_item::has_trait( const trait_id &trait_to_test ) const
{
}

void talker_item::set_mutation( const trait_id &add_trait )
{
}

void talker_item::unset_mutation( const trait_id &lose_trait )
{
}

bool talker_item::has_trait_flag( const std::string &trait_flag_to_check ) const
{
}

bool talker_item::crossed_threshold() const
{
}

int talker_item::num_bionics() const
{
}

bool talker_item::has_bionic( const bionic_id &bio_to_test ) const
{
}

bool talker_item::knows_spell( const spell_id &spell_to_test ) const
{
}

std::vector<skill_id> talker_item::skills_offered_to( const talker &student ) const
{
}

std::string talker_item::skill_training_text( const talker &student,
        const skill_id &skill_to_learn ) const
{
}

std::vector<matype_id> talker_item::styles_offered_to( const talker &student ) const
{
}

std::string talker_item::style_training_text( const talker &student,
        const matype_id &style_to_learn ) const
{
}

std::vector<spell_id> talker_item::spells_offered_to( talker &student )
{
}

std::string talker_item::spell_training_text( talker &student, const spell_id &spell_to_learn )
{
}

void talker_item::store_chosen_training( const skill_id &train_skill, const matype_id &train_style,
        const spell_id &train_spell )
{
}

// effects and values
bool talker_item::has_effect( const efftype_id &effect_to_test ) const
{
}

bool talker_item::is_deaf() const
{
}

void talker_item::add_effect( const efftype_id &new_effect, const time_duration &, bool )
{
}

void talker_item::remove_effect( const efftype_id &lose_effect )
{
}

std::string talker_item::get_value( const std::string &val_to_test ) const
{
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
}

std::vector<mission *> talker_item::assigned_missions() const
{
}

mission *talker_item::selected_mission() const
{
}

void talker_item::select_mission( mission *miss )
{
}

void talker_item::add_mission( const mission_type_id &new_miss )
{
}

// factions and alliances
faction *talker_item::get_faction() const
{
}

bool talker_item::is_friendly( const Character &guy ) const
{
}

bool talker_item::is_player_ally() const
{
}

bool talker_item::is_enemy() const
{
}

// other descriptors
std::string talker_item::short_description() const
{
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
}
