#pragma once
#ifndef CATA_SRC_TALKER_NPC_H
#define CATA_SRC_TALKER_NPC_H

#include "talker.h"
#include "talker_character.h"

class Character;
class faction;
class item;
class mission;
class npc;
class player;
class recipe;
struct tripoint;
class vehicle;

/*
 */
class talker_npc : public talker_character
{
    public:
        talker_npc( npc *new_me ): talker_character( new_me ), me_npc( new_me ) {
        }
        ~talker_npc() = default;

        npc *get_npc() override {
            return me_npc;
        }
        npc *get_npc() const override {
            return me_npc;
        }

        // override functions called in condition.cpp
        bool has_activity() const override;
        bool myclass( const npc_class_id &class_to_check ) const override;
        int debt() const override;
        bool has_ai_rule( const std::string &type, const std::string &rule ) const override;
        std::vector<mission *> available_missions() const override;
        std::vector<mission *> assigned_missions() const override;
        mission *selected_mission() const override;
        void select_mission( mission *selected ) override;
        bool is_following() const override;
        bool is_friendly( const Character &guy ) const override;
        bool is_enemy() const override;
        bool is_player_ally()  const override;
        std::vector<skill_id> skills_offered_to( const talker &student ) const override;
        std::string skill_training_text( const talker &, const skill_id & ) const override;
        std::vector<matype_id> styles_offered_to( const talker &student ) const override;
        std::string style_training_text( const talker &, const matype_id & ) const override;
        std::vector<spell_id> spells_offered_to( talker &student ) override;
        std::string spell_training_text( talker &, const spell_id & ) override;
        void store_chosen_training( const skill_id &c_skill, const matype_id &c_style,
                                    const spell_id &c_spell ) override;
        // override getter functions called in npctalk.cpp
        bool will_talk_to_u( const player &u, const bool force ) override;
        std::vector<std::string> get_topics( const bool radio_contact ) override;
        std::string get_job_description() const override;
        std::string evaluation_by( const talker &alpha ) const override;
        std::string opinion_text() const override;
        tripoint global_omt_goal() const override;
        int cash_to_favor( int value ) const override;
        bool buy_from( const int amount ) override;
        bool turned_hostile() const override;
        bool enslave_mind() override;
        int parse_mod( const std::string &attribute, const int factor ) const override;
        int trial_chance_mod( const std::string &trial_type ) const override;
        // override setter functions called in npctalk.cpp
        void say( const std::string &speech ) override;
        void check_missions() override;
        void update_missions( const std::vector<mission *> &missions_assigned,
                              const character_id &charID ) override;
        void set_companion_mission( const std::string &role_id ) override;
        void add_debt( const int cost ) override;
        void set_fac( const faction_id &new_fac_name ) override;
        void set_class( const npc_class_id &new_class ) override;
        void toggle_ai_rule( const std::string &type, const std::string &rule ) override;
        void set_ai_rule( const std::string &type, const std::string &rule ) override;
        void clear_ai_rule( const std::string &type, const std::string &rule ) override;
        std::string give_item_to( const bool to_use ) override;
        void add_mission( const mission_type_id &mission_id ) override;
        void add_opinion( const int trust, const int fear, const int value, const int anger,
                          const int debt ) override;
        void make_angry() override;
        bool check_hostile_response( const int anger ) const override;
        void add_faction_rep( const int rep_change ) override;
    protected:
        mutable npc *me_npc;
};
#endif
