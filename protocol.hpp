#include <algorithm>
#include <cmath>
#include <eosiolib/currency.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/multi_index.hpp>
#include <eosiolib/contract.hpp>
#include <eosiolib/action.hpp>
#include "eosio.token.hpp"
#include "hosts.hpp"
#include "shares.hpp"
#include "goals.hpp"
#include "voting.hpp"

#define QUANTS_PRECISION 1000000
#define PERCENT_PRECISION 10000


namespace eosio {
    static const account_name _self = N(tt.tc);
    static const account_name _dacomfee = N(dacomfee.tc);
    static const eosio::symbol_name _SYM = S(4, FLO);

    static const eosio::symbol_name _SHARES = S(4, MSHARES);
    static const uint64_t _REGISTRATOR_SUFFIX_LENGHT = 12;
    static const char _REGISTRATOR_STRING_SUFFIX[_REGISTRATOR_SUFFIX_LENGHT+1] = ".goals";
    static const uint64_t _MAX_SUPPLY = 1000000000000000000;
    static const uint64_t _SHARES_VESTING_DURATION = 604800;
    static const uint64_t _TOTAL_VOTES = 7;
    static const uint64_t _MAX_LEVELS = 7;

    // @abi table spiral i64
    struct spiral{
        uint64_t id;
        uint64_t size_of_pool;
        uint64_t overlap;
        uint64_t profit_growth;
        uint64_t base_rate;
        uint64_t loss_percent;
        uint64_t pool_limit;
        uint64_t pool_timeout;
        uint64_t priority_seconds;
        uint64_t primary_key() const {return id;}

        EOSLIB_SERIALIZE(spiral, (id)(size_of_pool)(overlap)(profit_growth)(base_rate)(loss_percent)(pool_limit)(pool_timeout)(priority_seconds))
    };

    typedef eosio::multi_index<N(spiral), spiral> spiral_index;


    // @abi table balance i64
    struct balance{
        uint64_t id;
        account_name host;
        account_name children_host;
        uint64_t cycle_num;
        uint64_t pool_num;
        bool is_goal = false;
        uint64_t goal_id;
        uint64_t global_pool_id;
        uint64_t quants_for_sale;
        uint64_t next_quants_for_sale;
        uint64_t last_recalculated_win_pool_id = 1;
        bool win = false; //true if win, false if lose or nominal
        std::string pool_color;
        eosio::asset available; 
        eosio::asset purchase_amount;
        eosio::time_point_sec date_of_purchase;
        bool withdrawed = false;
        eosio::asset sold_amount;
        eosio::time_point_sec date_of_sale;
        std::vector<eosio::asset> forecasts;
        eosio::asset ref_amount; 
        eosio::asset sys_amount;

        uint64_t primary_key() const {return id;}
        uint64_t by_is_goal() const {return is_goal;} 

        EOSLIB_SERIALIZE(balance, (id)(host)(children_host)(cycle_num)(pool_num)(is_goal)(goal_id)(global_pool_id)(quants_for_sale)(next_quants_for_sale)(last_recalculated_win_pool_id)(win)(pool_color)(available)(purchase_amount)(date_of_purchase)(withdrawed)(sold_amount)(date_of_sale)(forecasts)(ref_amount)(sys_amount))
    
        account_name get_active_host() const {
            if (host == children_host)
                return host;
            else
                return children_host;
        }
    };

    typedef eosio::multi_index<N(balance), balance,
    indexed_by<N(is_goal), const_mem_fun<balance, uint64_t, &balance::by_is_goal>>
    > balance_index;

    // @abi table cycle i64
    struct cycle{
        uint64_t id;
        account_name active_host;
        uint64_t start_at_global_pool_id;
        uint64_t finish_at_global_pool_id;
        
        uint64_t primary_key() const {return id;}

        EOSLIB_SERIALIZE(cycle, (id)(active_host)(start_at_global_pool_id)(finish_at_global_pool_id));
    };

    typedef eosio::multi_index<N(cycle), cycle> cycle_index;
    
   
    // @abi table fee i64
    struct fee{
        uint64_t id;
        eosio::asset createhost;
        uint64_t systemfee;
        
        uint64_t primary_key() const {return id;}
        bool is_empty()const { return !( createhost.amount ); }

        EOSLIB_SERIALIZE(fee, (id)(createhost)(systemfee))
    };

    typedef eosio::multi_index<N(fee), fee> fee_index;
    

    // @abi table pool i64
    struct pool{
        uint64_t id;
        account_name active_host;
        uint64_t cycle_num;
        uint64_t pool_num;
        std::string color;
        uint64_t total_quants; 
        uint64_t creserved_quants; 
        uint64_t remain_quants;
        eosio::asset quant_cost;
        eosio::asset total_win_withdraw;
        eosio::asset total_loss_withdraw;
        eosio::time_point_sec pool_started_at;
        eosio::time_point_sec priority_until;
        eosio::time_point_sec pool_expired_at;
        
        uint64_t primary_key() const {return id;}
        uint64_t by_cycle() const {return cycle_num;}
        
        EOSLIB_SERIALIZE(pool,(id)(active_host)(cycle_num)(pool_num)(color)(total_quants)(creserved_quants)(remain_quants)(quant_cost)(total_win_withdraw)(total_loss_withdraw)(pool_started_at)(priority_until)(pool_expired_at))
    };

    typedef eosio::multi_index<N(pool), pool> pool_index;
    

    // @abi table rate i64
    struct rate {
        uint64_t pool_id;
        uint64_t total_quants;
        uint64_t buy_rate=0;
        uint64_t sell_rate=0;
        eosio::asset client_income;
        eosio::asset delta;
        eosio::asset pool_cost;
        eosio::asset total_in_box;
        eosio::asset payment_to_wins;
        eosio::asset payment_to_loss;
        eosio::asset system_income;
        eosio::asset live_balance_for_sale;
        
        
        uint64_t primary_key() const{return pool_id;}

        EOSLIB_SERIALIZE(rate, (pool_id)(total_quants)(buy_rate)(sell_rate)(client_income)(delta)(pool_cost)(total_in_box)(payment_to_wins)(payment_to_loss)(system_income)(live_balance_for_sale))
    };
    typedef eosio::multi_index<N(rate), rate> rate_index;
    

    // @abi table sincome i64
    struct sincome{
        uint64_t id;
        account_name active_host;
        uint64_t pool_num;
        eosio::asset available;
        eosio::asset paid_to_refs;
        eosio::asset paid_to_host;
        uint64_t primary_key() const {return id;}
        bool is_empty()const { return !( available.amount ); }
        
        EOSLIB_SERIALIZE(sincome, (id)(active_host)(pool_num)(available)(paid_to_refs)(paid_to_host))

    };
    typedef eosio::multi_index<N(sincome), sincome> sincome_index;
    

    // @abi table users i64
    struct users{
        account_name username;
        account_name referer;
        bool rules = true;
        eosio::time_point_sec registered_at;
        uint64_t time;
        std::string meta;
        
        account_name primary_key() const{return username;}
        account_name by_secondary_key() const{return referer;}
        uint64_t by_time() const{return -time;}

        EOSLIB_SERIALIZE(users, (username)(referer)(rules)(registered_at)(time)(meta))
    };

    typedef eosio::multi_index<N(users), users,
    indexed_by<N(users), const_mem_fun<users, account_name, &users::by_secondary_key>>,
    indexed_by<N(users), const_mem_fun<users, uint64_t, &users::by_time>>
    > user_index;


    /* ACTIONS */

    // @abi action
    struct reg {
        account_name username;
        account_name referer;
        std::string meta;
        
        EOSLIB_SERIALIZE( reg, (username)(referer)(meta))
    };

    
    // @abi action
    struct setparams{
        account_name host;
        account_name child_host;
        uint64_t size_of_pool;
        uint64_t overlap;
        uint64_t profit_growth;
        uint64_t base_rate;
        uint64_t loss_percent;
        uint64_t pool_limit;
        uint64_t pool_timeout;
        uint64_t priority_seconds;
        EOSLIB_SERIALIZE( setparams, (host)(child_host)(size_of_pool)(overlap)(profit_growth)(base_rate)(loss_percent)(pool_limit)(pool_timeout)(priority_seconds))

    };

    // @abi action
    struct start{
        account_name host;
        account_name child_host;
        EOSLIB_SERIALIZE( start, (host)(child_host))

    };

    // @abi action
    struct withdraw{
        account_name username; 
        account_name host;
        uint64_t balance_id;

        
        EOSLIB_SERIALIZE( withdraw, (username)(host)(balance_id))


    };

    // @abi action
    struct priorenter{
        account_name username; 
        account_name host;
        uint64_t balance_id;

        
        EOSLIB_SERIALIZE( priorenter, (username)(host)(balance_id))


    };

// @abi action
    struct gpriorenter{
        account_name username; 
        account_name host;
        uint64_t quants_for_each_pool; 
        uint64_t goal_id;
        EOSLIB_SERIALIZE( gpriorenter, (username)(host)(quants_for_each_pool)(goal_id))


    };

    // @abi action
    struct refreshbal{
        account_name username;
        uint64_t balance_id;
        bool partrefresh = false;
        EOSLIB_SERIALIZE( refreshbal, (username)(balance_id)(partrefresh))
    };

      // @abi action
    struct refreshst{
        account_name host;

        EOSLIB_SERIALIZE( refreshst, (host))
    };

    // @abi action
    struct syswithdraw{
        account_name username;
        account_name host;
        uint64_t sbalanceid;
        EOSLIB_SERIALIZE( syswithdraw, (username)(host)(sbalanceid))
    };

    // @abi action
    struct setfee{
        eosio::asset createhost;
        uint64_t systemfee;
        
        EOSLIB_SERIALIZE( setfee, (createhost)(systemfee))
    };
    
    
};






