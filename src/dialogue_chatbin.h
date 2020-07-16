#pragma once
#ifndef CATA_SRC_DIALOGUE_CHATBIN_H
#define CATA_SRC_DIALOGUE_CHATBIN_H

#include <unordered_map>
#include <set>
#include <string>
#include <vector>
#include "pimpl.h"
#include "string_id.h"
#include "type_id.h"

class character_id;
class JsonIn;
class JsonObject;
class JsonOut;
class mission;
class SkillLevelMap;

struct dialogue_chatbin {
    /**
     * Add a new mission to the available missions (@ref missions). For compatibility it silently
     * ignores null pointers passed to it.
     */
    void add_new_mission( mission *miss );
    /**
     * Check that assigned missions are still assigned if not move them back to the
     * unassigned vector. This is called directly before talking.
     */
    void check_missions();
    /**
     * Missions that the talker can give out. All missions in this vector should be unassigned,
     * when given out, they should be moved to @ref missions_assigned.
     */
    std::vector<mission *> missions;
    /**
     * Mission that have been assigned by this dialogue to a player character.
     */
    std::vector<mission *> missions_assigned;
    /**
     * The mission (if any) that we talk about right now. Can be null. Should be one of the
     * missions in @ref missions or @ref missions_assigned.
     */
    mission *mission_selected = nullptr;
    /**
     * The skill this dialogue offers to train.
     */
    skill_id skill = skill_id::NULL_ID();
    /**
     * The martial art style this dialogue offers to train.
     */
    matype_id style;
    /**
     * The spell this dialogue offers to train
     */
    spell_id dialogue_spell;
    void store_chosen_training( const skill_id &c_skill, const matype_id &c_style,
                                const spell_id &c_spell );
    void clear_training();
    std::string first_topic = "TALK_NONE";

    dialogue_chatbin() = default;

    void clear_all();
    void serialize( JsonOut &json ) const;
    void deserialize( JsonIn &jsin );
};

/*
 * A dialogue_chatbin with additional private members for skills, spells, styles, traits,
 * effects, and values.  Intended to be used with game entities that don't have any of those
 * members normally but might want them for dialogue purposes.
 */
struct smart_chatbin : public dialogue_chatbin {
    smart_chatbin() = default;
    void load( const JsonObject &jo, const std::string & );
    void check();
    void reset();
    void serialize( JsonOut &json ) const;
    void deserialize( JsonIn &jsin );

    std::string my_id;
    character_id my_npc_id;
    std::set<spell_id> my_spells;
    std::set<matype_id> my_styles;
    std::unordered_map<std::string, std::string> my_values;
    // random entities may not have traits or effects, so these are just sets of ids
    // that the entity would have if it were an NPC
    std::set<trait_id> my_traits;
    std::set<efftype_id> my_effects;
    pimpl<SkillLevelMap> my_skills;
    faction *my_faction;

};

#endif // CATA_SRC_DIALOGUE_CHATBIN_H
