#pragma once
#ifndef CATA_SRC_TALKER_ITEM_H
#define CATA_SRC_TALKER_ITEM_H

class faction;
class item;
class mission;
class npc;
class player;
class recipe;
class smart_chatbin;
struct tripoint;
class vehicle;

/*
 * Talker is an entity independent way of providing a participant in a dialogue.
 * Talker is a virtual abstract class and should never really be used.  Instead,
 * entity specific talker child classes such as character_talker should be used.
 */
class talker_item : public talker
{
    public:
        talker_item( item *new_me );
        virtual ~talker_item() = default;
        // member accessor functions

        // identity and location
        std::string disp_name() const override;
        character_id getID() const override;
        int posx() const override;
        int posy() const override;
        int posz() const override;
        tripoint pos() const override;
        tripoint_abs_omt global_omt_location() const override;

        // mandatory functions for starting a dialogue
        bool will_talk_to_u( const player &, bool ) override;
        std::vector<std::string> get_topics( bool ) override;
        void check_missions() override;
        void update_missions( const std::vector<mission *> &, const character_id & ) override;

        // stats, skills, traits, bionics, and magic
        int get_skill_level( const skill_id &skill ) const override;
        bool has_trait( const trait_id &trait_to_test ) const override;
        void set_mutation( const trait_id &add_trait ) override;
        void unset_mutation( const trait_id &lose_trait ) override;
        bool has_trait_flag( const std::string &trait_flag_to_check ) const override;
        bool crossed_threshold() const override;
        int num_bionics() const override;
        bool has_bionic( const bionic_id &bio_to_test ) const override;
        bool knows_spell( const spell_id &spell_to_test ) const override;
        std::vector<skill_id> skills_offered_to( const talker &student ) const override;
        std::string skill_training_text( const talker &student,
                                         const skill_id &skill_to_learn ) const override;
        std::vector<matype_id> styles_offered_to( const talker &student ) const override;
        std::string style_training_text( const talker &student,
                                         const matype_id &style_to_learn ) const override;
        std::vector<spell_id> spells_offered_to( talker &student ) override;
        std::string spell_training_text( talker &student, const spell_id &spell_to_learn ) override;
        void store_chosen_training( const skill_id &train_skill, const matype_id &train_style,
                                    const spell_id &train_spell ) override;

        // effects and values
        bool has_effect( const efftype_id &effect_to_test ) const override;
        bool is_deaf() const override;
        void add_effect( const efftype_id &new_effect, const time_duration &, bool ) override;
        void remove_effect( const efftype_id &lose_effect ) override;
        std::string get_value( const std::string &val_to_test ) const override;
        void set_value( const std::string &val_to_set, const std::string &new_val ) override;
        void remove_value( const std::string &val_to_lose ) override;

        // missions
        std::vector<mission *> available_missions() const override;
        std::vector<mission *> assigned_missions() const override;
        mission *selected_mission() const override;
        void select_mission( mission *miss ) override;
        void add_mission( const mission_type_id &new_miss ) override;

        // factions and alliances
        faction *get_faction() const override;
        bool is_friendly( const Character & ) const override;
        bool is_player_ally() const override;
        bool is_enemy() const override;

        // other descriptors
        std::string short_description() const override;

        // speaking
        void say( const std::string &speech ) override;
        void shout( const std::string &speech = "", bool order = false ) override;

        // miscellaneous
        void set_first_topic( const std::string &new_chat ) override;
    protected:
        item *me_item;
    private:
        smart_chatbin &me_chat() {
            return *me_item->get_chatbin();
        }
        smart_chatbin &me_chat() const {
            return *me_item->get_chatbin();
        }
};
#endif // CATA_SRC_TALKER_ITEM_H
