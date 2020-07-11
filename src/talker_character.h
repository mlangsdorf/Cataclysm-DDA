#pragma once
#ifndef CATA_SRC_TALKER_CHARACTER_H
#define CATA_SRC_TALKER_CHARACTER_H

#include "talker.h"

class Character;
class faction;
class item;
class mission;
class npc;
class recipe;
struct tripoint;
class vehicle;
/*
 */
class talker_character: public talker
{
    public:
        talker_character( Character *new_me ) {
            me_chr = new_me;
        }
        Character *get_character() override {
            return me_chr;
        }

        // override functions called in condition.cpp
        std::string disp_name() const override;
        bool has_trait( const trait_id &trait_to_check ) const override;
        bool crossed_threshold() const override;
        bool has_activity() const override;
        bool is_mounted() const override;
        int str_cur() const override;
        int dex_cur() const override;
        int int_cur() const override;
        int per_cur() const override;
        bool is_wearing( const itype_id &item_id ) const override;
        bool charges_of( const itype_id &item_id ) const override;
        bool has_charges( const itype_id &item_id, const int count ) const override;
        bool has_amount( const itype_id &item_id, const int count ) const override;
        int num_bionics() const override;
        bool has_max_power() const override;
        bool has_bionic( const bionic_id &bionics_id ) const override;
        bool has_effect( const efftype_id &effect_id ) const override;
        bool get_fatigue() const override;
        bool get_hunger() const override;
        bool get_thirst() const override;
        tripoint global_omt_location() const override;
        std::string get_value( const std::string &var_name ) const override;
        int posx() const override;
        int posy() const override;
        int posz() const override;
        tripoint pos() const override;
        int cash() const override;
        int debt() const override;
        bool has_ai_rule( const std::string &type, const std::string &rule ) const override;
        bool is_male() const override;
        bool unarmed_attack() const override;
        bool in_control( const vehicle &veh ) const override;
        bool can_stash_weapon() const override;
        bool has_stolen_item( const talker &guy ) const override;
        int get_skill_level( const skill_id &skill ) const override;
        bool knows_recipe( const recipe &rep ) const override;
        // override functions called in npctalk.cpp
        bool is_deaf() const override;
        std::string short_description() const override;
        character_id getID() const override;
        faction *get_faction() const override;
        // override functions called in npctalk.cpp
        void say( const std::string &speech ) override;
        void shout( const std::string &speech = "", bool order = false ) override;
        void add_effect( const efftype_id &new_effect, const time_duration &dur,
                         const bool permanent ) override;
        void remove_effect( const efftype_id &old_effect ) override;
        void set_mutation( const trait_id &new_trait ) override;
        void unset_mutation( const trait_id &old_trait ) override;
        void set_value( const std::string &var_name, const std::string &value ) override;
        void remove_value( const std::string &var_name ) override;
        void i_add( const item &new_item ) override;
        void use_charges( const itype_id &item_name, const int count ) override;
        void use_amount( const itype_id &item_name, const int count ) override;
        void remove_items_with( const item &it ) override;
        void buy_monster( talker &seller, const mtype_id &mtype, int cost,
                          int count, bool pacified, const translation &name ) override;
        void add_faction_rep( const int rep_change ) override;
        void give_item_to( const bool to_use ) override;
    protected:
        Character *me_chr;
};
#endif
