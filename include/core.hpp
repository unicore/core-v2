#pragma once

#include <algorithm>
#include <cmath>
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/time.hpp>
#include <eosio/multi_index.hpp>
#include <eosio/contract.hpp>
#include <eosio/action.hpp>
#include <eosio/system.hpp>
#include "eosio.token.hpp"
#include <eosio/print.hpp>
#include <eosio/datastream.hpp>


#include "hosts.hpp"
#include "ref.hpp"
#include "shares.hpp"
#include "goals.hpp"
#include "voting.hpp"
#include "badges.hpp"
#include "tasks.hpp"
#include "ipfs.hpp"
#include "cms.hpp"
#include "crypto.hpp"
#include "comments.hpp"
#include "conditions.hpp"
#include "events.hpp"

#include "consts.hpp"

namespace eosio {

class [[eosio::contract]] unicore : public eosio::contract {
    public:
        unicore(eosio::name receiver, eosio::name code, eosio::datastream<const char*> ds ): eosio::contract(receiver, code, ds)
        {}
        void apply(uint64_t receiver, uint64_t code, uint64_t action);
        // uint128_t unicore::combine_ids(const uint64_t &x, const uint64_t &y);
        // uint128_t unicore::combine_ids2(const uint64_t &x, const uint64_t &y);

        
        static eosio::asset convert_to_power(eosio::asset quantity, eosio::name host);
        [[eosio::action]] void convert(eosio::name username, eosio::name host, uint64_t balance_id);

        
        [[eosio::action]] void setparams(eosio::name host, eosio::name chost, uint64_t size_of_pool,
            uint64_t quants_precision, uint64_t overlap, uint64_t profit_growth, uint64_t base_rate,
            uint64_t loss_percent, uint64_t pool_limit, uint64_t pool_timeout, uint64_t priority_seconds);

        [[eosio::action]] void start(eosio::name host, eosio::name chost); 
        [[eosio::action]] void withdraw(eosio::name username, eosio::name host, uint64_t balance_id);
        [[eosio::action]] void priorenter(eosio::name username, eosio::name host, uint64_t balance_id);
        [[eosio::action]] void refreshbal(eosio::name username, uint64_t balance_id, uint64_t partrefresh);
        
        static void pay_for_upgrade(eosio::name username, eosio::asset amount, eosio::name code);
        
        static void refresh_state(eosio::name host);

        [[eosio::action]] void init(uint64_t system_income);

        [[eosio::action]] void refreshst(eosio::name host);

        //BADGES
        [[eosio::action]] void setbadge(uint64_t id, eosio::name host, eosio::string caption, eosio::string description, eosio::string iurl, uint64_t total, uint64_t power);
        // [[eosio::action]] void giftbadge(eosio::name host, eosio::name to, uint64_t badge_id, eosio::string comment);
        [[eosio::action]] void backbadge(eosio::name host, eosio::name from, uint64_t badge_id, eosio::string comment);
        
        static void giftbadge_action(eosio::name host, eosio::name to, uint64_t badge_id, eosio::string comment);
        static void deposit ( eosio::name username, eosio::name host, eosio::asset amount, eosio::name code, std::string message );


        //CMS
        [[eosio::action]] void setcontent(eosio::name username, eosio::name type, eosio::name lang, eosio::string content);
        [[eosio::action]] void rmcontent(eosio::name username, eosio::name type);
        [[eosio::action]] void setcmsconfig(eosio::name username, eosio::string config);

        //GOALS
        [[eosio::action]] void setgoal(eosio::name username, eosio::name benefactor, eosio::name host, std::string title, std::string permlink, std::string description, eosio::asset target, uint64_t expiration);
        [[eosio::action]] void editgoal(uint64_t goal_id, eosio::name username, eosio::name host, eosio::name benefactor, std::string title, std::string description, eosio::asset target);
        [[eosio::action]] void dfundgoal(eosio::name architect, eosio::name host, uint64_t goal_id, eosio::asset amount, std::string comment);
        [[eosio::action]] void delgoal(eosio::name username, eosio::name host, uint64_t goal_id);
        [[eosio::action]] void report(eosio::name username, eosio::name host, uint64_t goal_id, std::string report);
        [[eosio::action]] void check(eosio::name architect, eosio::name host, uint64_t goal_id);
        [[eosio::action]] void gwithdraw(eosio::name username, eosio::name host, uint64_t goal_id);
        [[eosio::action]] void gsponsor(eosio::name hoperator, eosio::name host, eosio::name reciever, uint64_t goal_id, eosio::asset amount);
        [[eosio::action]] void setemi(eosio::name host, uint64_t percent, uint64_t gtop);
        static void donate_action(eosio::name from, eosio::name host, uint64_t goal_id, eosio::asset quantity, eosio::name code);
        static eosio::asset adjust_goals_emission_pool(eosio::name hostname);
        static void fund_emi_pool ( eosio::name username, eosio::name host, eosio::asset amount, eosio::name code );
        static void add_asset_to_fund_action(eosio::name username, eosio::asset quantity, eosio::name code);

        //POT
        [[eosio::action]] void enablesale(eosio::name host, eosio::name token_contract, eosio::asset asset_on_sale, int64_t sale_shift);
        [[eosio::action]] void addhostofund(uint64_t fund_id, eosio::name host);
        static void createfund(eosio::name token_contract, eosio::asset fund_asset, std::string descriptor);
        static eosio::asset buy_action(eosio::name username, eosio::name host, eosio::asset quantity, eosio::name code, bool transfer);
        [[eosio::action]] void transfromgf(eosio::name to, eosio::name token_contract, eosio::asset quantity);


        //HOST
        [[eosio::action]] void setarch(eosio::name host, eosio::name architect);
        [[eosio::action]] void upgrade(eosio::name username, std::string title, std::string purpose, uint64_t total_shares, eosio::asset quote_amount, eosio::name quote_token_contract, eosio::asset root_token, eosio::name root_token_contract, bool voting_only_up, uint64_t consensus_percent, uint64_t referral_percent, uint64_t dacs_percent, uint64_t cfund_percent, uint64_t hfund_percent, std::vector<uint64_t> levels, uint64_t emission_percent, uint64_t gtop, std::string meta);
        [[eosio::action]] void cchildhost(eosio::name parent_host, eosio::name chost);
        [[eosio::action]] void edithost(eosio::name architect, eosio::name host, eosio::string title, eosio::string purpose, eosio::string manifest, eosio::string meta);
        [[eosio::action]] void fixs(eosio::name host);
        [[eosio::action]] void settype(eosio::name host, eosio::name type);
        static void settype_static(eosio::name host, eosio::name type);

        //DACS
        [[eosio::action]] void adddac(eosio::name username, eosio::name host, uint64_t weight);
        [[eosio::action]] void rmdac(eosio::name username, eosio::name host);
        [[eosio::action]] void withdrdacinc(eosio::name username, eosio::name host);
        [[eosio::action]] void setwebsite(eosio::name host, eosio::name ahostname, eosio::string website, eosio::name type);

        //DATA
        [[eosio::action]] void selldata(eosio::name username, uint64_t id, eosio::string data, eosio::name root_token_contract, eosio::asset amount);
        [[eosio::action]] void dataapprove(eosio::name username, eosio::name owner, eosio::name who, uint64_t order_id, eosio::string message);
        // [[eosio::action]] void datadispute(eosio::name username, eosio::string data, eosio::name root_token_contract, eosio::asset amount);
        // [[eosio::action]] void datarelease(eosio::name username, eosio::string data, eosio::name root_token_contract, eosio::asset amount);
        [[eosio::action]] void setstorage(eosio::name username, uint64_t id, eosio::string name, eosio::string address);
        [[eosio::action]] void removeroute(eosio::name username, uint64_t id);
        [[eosio::action]] void setipfskey (eosio::name username, eosio::string pkey, eosio::string meta);
        
        static void buydata_action(eosio::name owner, uint64_t data_id, eosio::name buyer, eosio::asset quantity, eosio::name code);

        //MARKETS
        [[eosio::action]] void refreshsh (eosio::name owner, uint64_t id);
        [[eosio::action]] void withpbenefit(eosio::name username, eosio::name host);
        [[eosio::action]] void withrsegment(eosio::name username, eosio::name host);

        [[eosio::action]] void withrbenefit(eosio::name username, eosio::name host, std::vector<uint64_t> ids);
        [[eosio::action]] void refreshpu(eosio::name username, eosio::name host);
        [[eosio::action]] void withdrawsh(eosio::name owner, uint64_t id);
        [[eosio::action]] void sellshares(eosio::name username, eosio::name host, uint64_t shares);
        [[eosio::action]] void undelshares(eosio::name from, eosio::name reciever, eosio::name host, uint64_t shares);
        static void buyshares_action ( eosio::name buyer, eosio::name host, eosio::asset amount, eosio::name code );
        static void delegate_shares_action(eosio::name from, eosio::name reciever, eosio::name host, uint64_t shares);
        
        //TASKS
        [[eosio::action]] void settask(eosio::name host, eosio::name username, uint64_t goal_id, eosio::string title, eosio::string data, eosio::asset requested, bool is_public, eosio::asset for_each, bool with_badge, uint64_t badge_id, uint64_t expiration);
        [[eosio::action]] void fundtask(eosio::name host, uint64_t task_id, eosio::asset amount, eosio::string comment);
        [[eosio::action]] void tactivate(eosio::name host, uint64_t task_id);
        [[eosio::action]] void tdeactivate(eosio::name host, uint64_t task_id);
        [[eosio::action]] void setreport(eosio::name host, eosio::name username, uint64_t task_id, eosio::string data);
        [[eosio::action]] void editreport(eosio::name host, eosio::name username, uint64_t report_id, eosio::string data);
        [[eosio::action]] void approver(eosio::name host, uint64_t report_id, eosio::string comment); 
        [[eosio::action]] void disapprover(eosio::name host, uint64_t report_id, eosio::string comment);


        //VOTING
        [[eosio::action]] void vote(eosio::name voter, eosio::name host, uint64_t goal_id, bool up);
        static void propagate_votes_changes(eosio::name host, eosio::name voter, uint64_t old_power, uint64_t new_power);
        
        //CONDITIONS
        [[eosio::action]] void setcondition(eosio::name host, eosio::string key, uint64_t value);
        [[eosio::action]] void rmcondition(eosio::name host, uint64_t key); 
        

        // //EVENTS
        [[eosio::action]] void addevent(eosio::name host, eosio::name creator, eosio::name lang, eosio::string title, eosio::string permlink, eosio::string descriptor, eosio::time_point_sec start_at, eosio::time_point_sec finish_at, uint64_t category_id, bool is_regular, bool activated, uint64_t location_id, eosio::string geoposition, eosio::string location, eosio::name token_contract, eosio::asset price, eosio::string meta);
        [[eosio::action]] void editevent(eosio::name host, eosio::name creator, uint64_t id, eosio::name lang, eosio::string title, eosio::string descriptor, eosio::time_point_sec start_at, eosio::time_point_sec finish_at, uint64_t category_id, bool is_regular, bool activated, uint64_t location_id, eosio::string geoposition, eosio::string location, eosio::name token_contract, eosio::asset price, eosio::string meta);
        [[eosio::action]] void rmevent(eosio::name host, eosio::name creator, uint64_t id);
        [[eosio::action]] void joinevent(eosio::name host, eosio::name username, uint64_t id);

        [[eosio::action]] void addlocation(eosio::name host, eosio::name coordinator, eosio::string title, eosio::string geoposition, eosio::string meta);
        [[eosio::action]] void rmlocation(eosio::name host, uint64_t id);
        [[eosio::action]] void addcategory(eosio::name host, eosio::name lang, eosio::string title);
        [[eosio::action]] void rmcategory(eosio::name host, uint64_t id);



        static void rmfromhostwl(eosio::name host, eosio::name username);
        static void addtohostwl(eosio::name host, eosio::name username);
        static bool checkcondition(eosio::name host, eosio::string key, uint64_t value);
        static void checkminpwr(eosio::name host, eosio::name username);

        std::string symbol_to_string(eosio::asset val) const;
        static void change_bw_trade_graph(eosio::name host, uint64_t pool_id, uint64_t cycle_num, uint64_t pool_num, uint64_t buy_rate, uint64_t next_buy_rate, uint64_t total_quants, uint64_t remain_quants, std::string color);
        
        static void add_coredhistory(eosio::name host, eosio::name username, uint64_t pool_id, eosio::asset amount, std::string action, std::string message);

        static void create_bancor_market(std::string name, uint64_t id, eosio::name host, uint64_t total_shares, eosio::asset quote_amount, eosio::name quote_token_contract, uint64_t vesting_seconds);
        static std::vector <eosio::asset> calculate_forecast(eosio::name username, eosio::name host, uint64_t quants, uint64_t pool_num, eosio::asset purchase_amount, bool calculate_first, bool calculate_zero);
        static void fill_pool(eosio::name username, eosio::name host, uint64_t quants, eosio::asset amount, uint64_t filled_pool_id);
        static void check_and_modify_sale_fund(eosio::asset amount, hosts acc);
        static void give_shares_with_badge_action (eosio::name host, eosio::name reciever, uint64_t shares);
        static void back_shares_with_badge_action (eosio::name host, eosio::name from, uint64_t shares);
        static void add_sale_history(hosts acc, rate rate, spiral sp, eosio::asset amount);

        static void spread_to_dacs(eosio::name host, eosio::asset amount);
};


    struct [[eosio::table, eosio::contract("unicore")]] gpercents {
        eosio::name key;
        uint64_t value;
        uint64_t primary_key() const {return key.value;}

        EOSLIB_SERIALIZE(gpercents, (key)(value))
    };

    typedef eosio::multi_index<"gpercents"_n, gpercents> gpercents_index;

    
    struct [[eosio::table, eosio::contract("unicore")]] balance{
        uint64_t id;
        eosio::name host;
        eosio::name chost;
        uint64_t cycle_num;
        uint64_t pool_num;
        uint64_t global_pool_id;
        uint64_t quants_for_sale;
        uint64_t next_quants_for_sale;
        uint64_t last_recalculated_win_pool_id = 1;
        bool win = false; //true if win, false if lose or nominal
        int64_t root_percent;
        int64_t convert_percent;
        std::string pool_color;
        eosio::asset available; 
        eosio::asset purchase_amount;
        eosio::asset start_convert_amount;
        eosio::asset if_convert; 
        eosio::asset if_convert_to_power;
        bool withdrawed = false;
        std::vector<eosio::asset> forecasts;
        eosio::asset ref_amount; 
        eosio::asset dac_amount;
        eosio::asset cfund_amount;
        eosio::asset hfund_amount;
        eosio::asset sys_amount;

        eosio::string meta; 

        uint64_t primary_key() const {return id;}
        uint64_t byhost() const {return host.value;}

        EOSLIB_SERIALIZE(balance, (id)(host)(chost)(cycle_num)(pool_num)(global_pool_id)(quants_for_sale)(next_quants_for_sale)(last_recalculated_win_pool_id)(win)(root_percent)(convert_percent)(pool_color)(available)(purchase_amount)(start_convert_amount)(if_convert)(if_convert_to_power)(withdrawed)(forecasts)(ref_amount)(dac_amount)(cfund_amount)(hfund_amount)(sys_amount)(meta))
    
        eosio::name get_ahost() const {
            if (host == chost)
                return host;
            else
                return chost;
        }
    };

    typedef eosio::multi_index<"balance"_n, balance,
        eosio::indexed_by<"byhost"_n, eosio::const_mem_fun<balance, uint64_t, &balance::byhost>>
    > balance_index;


    struct [[eosio::table, eosio::contract("unicore")]] bwtradegraph{
        uint64_t pool_id;
        uint64_t cycle_num;
        uint64_t pool_num;
        uint64_t open;
        uint64_t high;
        uint64_t low;
        uint64_t close;
        bool is_white;
        uint64_t primary_key() const {return pool_id;}
        uint64_t bycycle() const {return cycle_num;}

        EOSLIB_SERIALIZE(bwtradegraph, (pool_id)(cycle_num)(pool_num)(open)(high)(low)(close)(is_white))        
    };

    typedef eosio::multi_index<"bwtradegraph"_n, bwtradegraph,
        eosio::indexed_by<"bycycle"_n, eosio::const_mem_fun<bwtradegraph, uint64_t, &bwtradegraph::bycycle>>
    > bwtradegraph_index;

    



    struct [[eosio::table, eosio::contract("unicore")]] cycle{
        uint64_t id;
        eosio::name ahost;
        uint64_t start_at_global_pool_id;
        uint64_t finish_at_global_pool_id;
        eosio::asset emitted;
        uint64_t primary_key() const {return id;}

        EOSLIB_SERIALIZE(cycle, (id)(ahost)(start_at_global_pool_id)(finish_at_global_pool_id)(emitted));
    };

    typedef eosio::multi_index<"cycle"_n, cycle> cycle_index;
    


    struct [[eosio::table, eosio::contract("unicore")]] pool{
        uint64_t id;
        eosio::name ahost;
        uint64_t cycle_num;
        uint64_t pool_num;
        std::string color;
        uint64_t total_quants; 
        uint64_t remain_quants;
        uint64_t creserved_quants; 
        eosio::asset filled;
        eosio::asset remain;
        uint64_t filled_percent;
        eosio::asset pool_cost;
        eosio::asset quant_cost;
        eosio::asset total_win_withdraw;
        eosio::asset total_loss_withdraw;
        eosio::time_point_sec pool_started_at;
        eosio::time_point_sec priority_until;
        eosio::time_point_sec pool_expired_at;
        
        uint64_t primary_key() const {return id;}
        uint64_t bycycle() const {return cycle_num;}
        
        EOSLIB_SERIALIZE(pool,(id)(ahost)(cycle_num)(pool_num)(color)(total_quants)(remain_quants)(creserved_quants)(filled)(remain)(filled_percent)(pool_cost)(quant_cost)(total_win_withdraw)(total_loss_withdraw)(pool_started_at)(priority_until)(pool_expired_at))
    };

    typedef eosio::multi_index<"pool"_n, pool> pool_index;
    


    struct [[eosio::table, eosio::contract("unicore")]] coredhistory{
        uint64_t id;
        uint64_t pool_id;
        eosio::name username;
        std::string action;
        std::string message;
        eosio::asset amount;
        uint64_t primary_key() const {return id;}

        EOSLIB_SERIALIZE(coredhistory, (id)(pool_id)(username)(action)(message)(amount));
    
    };
    
    typedef eosio::multi_index<"coredhistory"_n, coredhistory> coredhistory_index;
    


    struct [[eosio::table, eosio::contract("unicore")]] sale{
        uint64_t pool_id;
        uint64_t sell_rate;
        eosio::asset solded;
        eosio::asset total_solded;
        uint64_t primary_key() const {return pool_id;}

        EOSLIB_SERIALIZE(sale, (pool_id)(sell_rate)(solded)(total_solded));
    };

    typedef eosio::multi_index<"sale"_n, sale> sale_index;
 

    struct [[eosio::table, eosio::contract("unicore")]] sincome{
        uint64_t pool_id;
        eosio::name ahost;
        uint64_t cycle_num;
        uint64_t pool_num;
        uint64_t liquid_power;
        eosio::asset max;
        eosio::asset total;
        eosio::asset paid_to_refs;
        eosio::asset paid_to_dacs;
        eosio::asset paid_to_cfund;
        eosio::asset paid_to_hfund;
        eosio::asset paid_to_sys;
        uint128_t hfund_in_segments;
        uint128_t distributed_segments;

        uint64_t primary_key() const {return pool_id;}
        uint128_t cyclandpool() const { return eosio::combine_ids(cycle_num, pool_num); }
        
        EOSLIB_SERIALIZE(sincome, (pool_id)(ahost)(cycle_num)(pool_num)(liquid_power)(max)(total)(paid_to_refs)(paid_to_dacs)(paid_to_cfund)(paid_to_hfund)(paid_to_sys)(hfund_in_segments)(distributed_segments))

    };
    typedef eosio::multi_index<"sincome"_n, sincome,
    eosio::indexed_by<"cyclandpool"_n, eosio::const_mem_fun<sincome, uint128_t, &sincome::cyclandpool>>
    > sincome_index;
    
    struct [[eosio::table, eosio::contract("unicore")]] currency_stats {
            asset          supply;
            asset          max_supply;
            eosio::name    issuer;

            uint64_t primary_key()const { return supply.symbol.code().raw(); }
         };

    typedef eosio::multi_index<"stat"_n, currency_stats> stats;


    struct [[eosio::table, eosio::contract("unicore")]] userscount
     {
         uint64_t id;
         uint64_t count;
         eosio::string subject = "registered";
         uint64_t primary_key() const {return id;}

         EOSLIB_SERIALIZE(userscount, (id)(count)(subject))
     }; 
    typedef eosio::multi_index<"userscount"_n, userscount> userscount_index;



    struct  [[eosio::table, eosio::contract("unicore")]] ahosts{
        eosio::name username;
        eosio::string website;
        checksum256 hash;
        bool is_host = false;

        eosio::name type;
        uint64_t votes;
        std::string title;
        std::string purpose;
        std::string manifest;
        bool comments_is_enabled = false;
        std::string meta;

        uint64_t primary_key() const{return username.value;}
        uint64_t byvotes() const {return votes;}

        checksum256 byuuid() const { return hash; }
        
        checksum256 hashit(std::string str) const
        {
            return sha256(const_cast<char*>(str.c_str()), str.size());
        }
          

        EOSLIB_SERIALIZE(ahosts, (username)(website)(hash)(is_host)(type)(votes)(title)(purpose)(manifest)(comments_is_enabled)(meta))
    };

    typedef eosio::multi_index<"ahosts"_n, ahosts,
        eosio::indexed_by<"byvotes"_n, eosio::const_mem_fun<ahosts, uint64_t, &ahosts::byvotes>>,
        eosio::indexed_by<"byuuid"_n, eosio::const_mem_fun<ahosts, checksum256, &ahosts::byuuid>>
    > ahosts_index;


}


