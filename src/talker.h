#pragma once
#ifndef CATA_SRC_TALKER_H
#define CATA_SRC_TALKER_H

class Character;
class faction;
class item;
class mission;
class npc;
class recipe;
struct tripoint;
class vehicle;
/*
 * Talker is an entity independent way of providing a participant in a dialogue.
 * Talker is a virtual abstract class and should never really be used.  Instead,
 * entity specific talker child classes such as character_talker should be used.
 */
class talker
{
    public:
        virtual ~talker();
        // virtual member accessor functions
        virtual Character *get_character() {
            return nullptr;
        }
        virtual npc *get_npc() {
            return nullptr;
        }

        // virtual functions called in condition.cpp
        virtual std::string disp_name() const {
            return "";
        }
        virtual bool has_trait( const trait_id & ) const {
            return false;
        }
        virtual bool has_trait_flag( const std::string & ) const {
            return false;
        }
        virtual bool crossed_threshold() const {
            return false;
        }
        virtual bool has_activity() const {
            return false;
        }
        virtual bool is_mounted() const {
            return false;
        }
        virtual bool myclass( const npc_class_id & ) const {
            return false;
        }
        virtual int str_cur() const {
            return 0;
        }
        virtual int dex_cur() const {
            return 0;
        }
        virtual int int_cur() const {
            return 0;
        }
        virtual int per_cur() const {
            return 0;
        }
        virtual bool is_wearing( const itype_id & ) const {
            return false;
        }
        virtual bool charges_of( const itype_id & ) const {
            return false;
        }
        virtual bool has_charges( const itype_id &, const int ) const {
            return false;
        }
        virtual bool has_amount( const itype_id &, const int ) const {
            return false;
        }
        virtual int num_bionics() const {
            return 0;
        }
        virtual bool has_max_power() const {
            return false;
        }
        virtual bool has_bionic( const bionic_id & ) const {
            return false;
        }
        virtual bool has_effect( const efftype_id & ) const {
            return false;
        }
        virtual bool get_fatigue() const {
            return 0;
        }
        virtual bool get_hunger() const {
            return 0;
        }
        virtual bool get_thirst() const {
            return 0;
        }
        virtual tripoint global_omt_location() const {
            return tripoint_zero;
        }
        virtual std::string get_value( const std::string & ) const {
            return "";
        }
        virtual int posx() const {
            return 0;
        }
        virtual int posy() const {
            return 0;
        }
        virtual int posz() const {
            return 0;
        }
        virtual tripoint pos() const {
            return tripoint_zero;
        }
        virtual int cash() const {
            return 0;
        }
        virtual int debt() const {
            return 0;
        }
        virtual bool has_ai_rule( const std::string &, const std::string & ) const {
            return false;
        }
        virtual bool is_male() const {
            return false;
        }
        virtual std::vector<mission *> available_missions() const {
            return {};
        }
        virtual std::vector<mission *> assigned_missions() const {
            return {};
        }
        virtual mission *selected_mission() const {
            return nullptr;
        }
        virtual bool is_following() const {
            return false;
        }
        virtual bool is_friendly( const Character & )  const {
            return false;
        }
        virtual bool is_enemy() const {
            return false;
        }
        virtual std::vector<skill_id> skills_offered_to( const talker & ) const {
            return {};
        }
        virtual std::vector<matype_id> styles_offered_to( const talker & ) const {
            return {};
        }
        virtual std::vector<spell_id> spells_offered_to( const talker & ) const {
            return {};
        }
        virtual bool unarmed_attack() const {
            return false;
        }
        virtual bool can_stash_weapon() const {
            return false;
        }
        virtual bool in_control( const vehicle & ) const {
            return false;
        }
        virtual bool has_stolen_item( const talker & ) const {
            return false;
        }
        virtual int get_skill_level( const skill_id & ) const {
            return false;
        }
        virtual bool knows_recipe( const recipe & ) const {
            return false;
        }
        // virtual getter functions called in npctalk.cpp
        virtual bool will_talk_to_u( const Character &, const bool ) {
            return false;
        }
        virtual std::vector<std::string> get_topics( const bool ) {
            return {};
        }
        virtual std::string get_job_description() const {
            return "";
        }
        virtual std::string evaluation_by( const talker & ) const {
            return "";
        }
        virtual std::string short_description() const {
            return "";
        }
        virtual std::string opinion_text() const {
            return "";
        }
        virtual tripoint global_omt_goal() const {
            return tripoint_zero;
        }
        virtual int cash_to_favor( const int ) const {
            return 0;
        }
        virtual faction *get_faction() const {
            return nullptr;
        }
        virtual bool turned_hostile() const {
            return false;
        }
        virtual bool buy_from( const int ) {
            return false;
        }
        virtual character_id getID() const {
            return character_id( 0 );
        }
        virtual bool is_deaf() const {
            return false;
        }
        virtual bool enslave_mind() {
            return false;
        }
        // virtual setter functions called in npctalk.cpp
        virtual void say( const std::string & ) {}
        virtual void shout( const std::string &, bool ) {}
        virtual void set_companion_mission( const std::string & ) {}
        virtual void check_missions() {}
        virtual void update_missions( const std::vector<mission *> &, const character_id & ) {}
        virtual void add_effect( const efftype_id &, const time_duration &, const bool ) {}
        virtual void remove_effect( const efftype_id & ) {}
        virtual void set_mutation( const trait_id & ) {}
        virtual void unset_mutation( const trait_id & ) {}
        virtual void set_value( const std::string &, const std::string & ) {}
        virtual void remove_value( const std::string & ) {}
        virtual void i_add( const item & ) {}
        virtual void add_debt( const int ) {}
        virtual void use_charges( const itype_id &, const int ) {}
        virtual void use_amount( const itype_id &, const int ) {}
        virtual void remove_items_with( const item & ) {}
        virtual void sell_to( talker & ) {}
        virtual void set_fac( const faction_id & ) {}
        virtual void set_class( const npc_class_id & ) {}
        virtual void add_faction_rep( const int ) {}
        virtual void toggle_ai_rule( const std::string &, const std::string & ) {}
        virtual void set_ai_rule( const std::string &, const std::string & ) {}
        virtual void clear_ai_rule( const std::string &, const std::string & ) {}
        virtual void give_item_to( const bool ) {}
        virtual void add_mission( const std::string & ) {}
        virtual void buy_monster( talker &, const mtype_id &, const int, const int, const bool,
                                  const translation & ) {}
        virtual void learn_recipe( const recipe & ) {}
        virtual void add_opinion( const int, const int, const int, const int ) {}
        virtual void make_angry() {}
};
#endif
