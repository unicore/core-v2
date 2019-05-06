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
#include "badges.hpp"
#include "tasks.hpp"

#define QUANTS_PRECISION 1000000
#define PERCENT_PRECISION 10000


namespace eosio {
    static const account_name _self = N(core);
    static const account_name _registrator = N(registrator);
    
    static const eosio::symbol_name _SYM = S(4, FLO);

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
        account_name chost;
        uint64_t cycle_num;
        uint64_t pool_num;
        uint64_t global_pool_id;
        uint64_t quants_for_sale;
        uint64_t next_quants_for_sale;
        uint64_t last_recalculated_win_pool_id = 1;
        bool win = false; //true if win, false if lose or nominal
        std::string pool_color;
        eosio::asset available; 
        eosio::asset purchase_amount;
        bool withdrawed = false;
        std::vector<eosio::asset> forecasts;
        eosio::asset ref_amount; 
        eosio::asset sys_amount;
        eosio::string meta; 

        uint64_t primary_key() const {return id;}
        
        EOSLIB_SERIALIZE(balance, (id)(host)(chost)(cycle_num)(pool_num)(global_pool_id)(quants_for_sale)(next_quants_for_sale)(last_recalculated_win_pool_id)(win)(pool_color)(available)(purchase_amount)(withdrawed)(forecasts)(ref_amount)(sys_amount)(meta))
    
        account_name get_ahost() const {
            if (host == chost)
                return host;
            else
                return chost;
        }
    };

    typedef eosio::multi_index<N(balance), balance> balance_index;

    // @abi table cycle i64
    struct cycle{
        uint64_t id;
        account_name ahost;
        uint64_t start_at_global_pool_id;
        uint64_t finish_at_global_pool_id;
        eosio::asset emitted;
        uint64_t primary_key() const {return id;}

        EOSLIB_SERIALIZE(cycle, (id)(ahost)(start_at_global_pool_id)(finish_at_global_pool_id)(emitted));
    };

    typedef eosio::multi_index<N(cycle), cycle> cycle_index;
    
   
    // @abi table pool i64
    struct pool{
        uint64_t id;
        account_name ahost;
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
        
        EOSLIB_SERIALIZE(pool,(id)(ahost)(cycle_num)(pool_num)(color)(total_quants)(creserved_quants)(remain_quants)(quant_cost)(total_win_withdraw)(total_loss_withdraw)(pool_started_at)(priority_until)(pool_expired_at))
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
        account_name ahost;
        uint64_t pool_num;
        eosio::asset total;
        eosio::asset paid_to_refs;
        eosio::asset paid_to_dacs;
        eosio::asset paid_to_fund;
        uint64_t primary_key() const {return id;}
        bool is_empty()const { return !( total.amount ); }
        
        EOSLIB_SERIALIZE(sincome, (id)(ahost)(pool_num)(total)(paid_to_refs)(paid_to_dacs)(paid_to_fund))

    };
    typedef eosio::multi_index<N(sincome), sincome> sincome_index;
    

    // @abi table users i64
    struct users{
        account_name username;
        account_name referer;
        bool rules = true;
        bool is_host = false;
        
        std::string meta;
        account_name primary_key() const{return username;}
        account_name by_secondary_key() const{return referer;}

        EOSLIB_SERIALIZE(users, (username)(referer)(rules)(is_host)(meta))
    };

    typedef eosio::multi_index<N(users), users,
    indexed_by<N(users), const_mem_fun<users, account_name, &users::by_secondary_key>>
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
    struct profupdate {
        account_name username;
        std::string meta;
        
        EOSLIB_SERIALIZE( profupdate, (username)(meta))
    };
 
    // @abi action
    struct setparams{
        account_name host;
        account_name chost;
        uint64_t size_of_pool;
        uint64_t overlap;
        uint64_t profit_growth;
        uint64_t base_rate;
        uint64_t loss_percent;
        uint64_t pool_limit;
        uint64_t pool_timeout;
        uint64_t priority_seconds;
        EOSLIB_SERIALIZE( setparams, (host)(chost)(size_of_pool)(overlap)(profit_growth)(base_rate)(loss_percent)(pool_limit)(pool_timeout)(priority_seconds))

    };

    // @abi action
    struct start{
        account_name host;
        account_name chost;
        EOSLIB_SERIALIZE( start, (host)(chost))

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

    
};






