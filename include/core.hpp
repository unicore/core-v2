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
#include "hosts.hpp"
#include "shares.hpp"
#include "goals.hpp"
#include "voting.hpp"
#include "badges.hpp"
#include "tasks.hpp"
#include "ipfs.hpp"
#include "cms.hpp"


#define QUARKS_IN_QUANTS 1000000
#define PERCENT_PRECISION 10000
#define HUNDR_PERCENT 1000000
#define SYS_PERCENT 100000
#define TOTAL_SEGMENTS 1000000000
#define MAX_CORE_HISTORY_LENGTH 1000

namespace eosio {
   #define DEBUG_ENABLED true

    #ifdef DEBUG_ENABLED
        static const eosio::name _self = "tt.tc"_n;
        static const eosio::name _registrator = "bob.tc"_n;
        static const eosio::name _curator = "bob.tc"_n;
        static constexpr eosio::symbol _SYM     = eosio::symbol(eosio::symbol_code("FLO"), 4);
    #else
        static const eosio::name _self = "core.z"_n;
        static const eosio::name _registrator = "registrator"_n;
        static const eosio::name _curator = "curator"_n;
        static constexpr eosio::symbol _SYM     = eosio::symbol(eosio::symbol_code("UNIT"), 4);

    #endif


    static const uint64_t _DATA_ORDER_EXPIRATION = 86400;
    static const uint64_t _SHARES_VESTING_DURATION = 604800;
    static const uint64_t _TOTAL_VOTES = 7;
    static const uint64_t _MAX_LEVELS = 7;


    struct [[eosio::table]] spiral{
        uint64_t id;
        uint64_t size_of_pool;
        uint64_t quants_precision = 0;
        uint64_t overlap;
        uint64_t profit_growth;
        uint64_t base_rate;
        uint64_t loss_percent;
        uint64_t pool_limit;
        uint64_t pool_timeout;
        uint64_t priority_seconds;

        uint64_t primary_key() const {return id;}

        EOSLIB_SERIALIZE(spiral, (id)(size_of_pool)(quants_precision)(overlap)(profit_growth)(base_rate)(loss_percent)(pool_limit)(pool_timeout)(priority_seconds))
    };

    typedef eosio::multi_index<"spiral"_n, spiral> spiral_index;


    
    struct [[eosio::table]] balance{
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
        std::string pool_color;
        eosio::asset available; 
        eosio::asset purchase_amount;
        eosio::asset if_convert; 
        bool withdrawed = false;
        std::vector<eosio::asset> forecasts;
        eosio::asset ref_amount; 
        eosio::asset dac_amount;
        eosio::asset cfund_amount;
        eosio::asset hfund_amount;
        eosio::asset sys_amount;

        eosio::string meta; 

        uint64_t primary_key() const {return id;}
        
        EOSLIB_SERIALIZE(balance, (id)(host)(chost)(cycle_num)(pool_num)(global_pool_id)(quants_for_sale)(next_quants_for_sale)(last_recalculated_win_pool_id)(win)(pool_color)(available)(purchase_amount)(if_convert)(withdrawed)(forecasts)(ref_amount)(dac_amount)(cfund_amount)(hfund_amount)(sys_amount)(meta))
    
        eosio::name get_ahost() const {
            if (host == chost)
                return host;
            else
                return chost;
        }
    };

    typedef eosio::multi_index<"balance"_n, balance> balance_index;


    struct [[eosio::table]] bwtradegraph{
        uint64_t pool_id;
        uint64_t cycle_num;
        uint64_t pool_num;
        uint64_t open;
        uint64_t high;
        uint64_t low;
        uint64_t close;
        uint64_t primary_key() const {return pool_id;}
        uint64_t bycycle() const {return cycle_num;}

        EOSLIB_SERIALIZE(bwtradegraph, (pool_id)(cycle_num)(pool_num)(open)(high)(low)(close))        
    };

    typedef eosio::multi_index<"bwtradegraph"_n, bwtradegraph,
        indexed_by<"bycycle"_n, const_mem_fun<bwtradegraph, uint64_t, &bwtradegraph::bycycle>>
    > bwtradegraph_index;

    
    struct [[eosio::table]] refbalances{
        uint64_t id;
        eosio::name host;
        eosio::asset amount;
        eosio::name from;
        uint128_t segments;
        uint64_t primary_key() const {return id;}

        EOSLIB_SERIALIZE(refbalances, (id)(host)(amount)(from)(segments))
    };
    typedef eosio::multi_index<"refbalances"_n, refbalances> refbalances_index;



    struct [[eosio::table]] cycle{
        uint64_t id;
        eosio::name ahost;
        uint64_t start_at_global_pool_id;
        uint64_t finish_at_global_pool_id;
        eosio::asset emitted;
        uint64_t primary_key() const {return id;}

        EOSLIB_SERIALIZE(cycle, (id)(ahost)(start_at_global_pool_id)(finish_at_global_pool_id)(emitted));
    };

    typedef eosio::multi_index<"cycle"_n, cycle> cycle_index;
    


   
    struct [[eosio::table]] pool{
        uint64_t id;
        eosio::name ahost;
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
        uint64_t bycycle() const {return cycle_num;}
        
        EOSLIB_SERIALIZE(pool,(id)(ahost)(cycle_num)(pool_num)(color)(total_quants)(creserved_quants)(remain_quants)(quant_cost)(total_win_withdraw)(total_loss_withdraw)(pool_started_at)(priority_until)(pool_expired_at))
    };

    typedef eosio::multi_index<"pool"_n, pool> pool_index;
    

    struct [[eosio::table]] rate {
        uint64_t pool_id;
        uint64_t buy_rate=0;
        uint64_t sell_rate=0;
        eosio::asset quant_buy_rate;
        eosio::asset quant_sell_rate;
        eosio::asset client_income;
        eosio::asset delta;
        eosio::asset pool_cost;
        eosio::asset total_in_box;
        eosio::asset payment_to_wins;
        eosio::asset payment_to_loss;
        eosio::asset system_income;
        eosio::asset live_balance_for_sale;
        
        
        uint64_t primary_key() const{return pool_id;}

        EOSLIB_SERIALIZE(rate, (pool_id)(buy_rate)(sell_rate)(quant_buy_rate)(quant_sell_rate)(client_income)(delta)(pool_cost)(total_in_box)(payment_to_wins)(payment_to_loss)(system_income)(live_balance_for_sale))
    };
    typedef eosio::multi_index<"rate"_n, rate> rate_index;
    
    struct [[eosio::table]] coredhistory{
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
    


    struct [[eosio::table]] sale{
        uint64_t pool_id;
        uint64_t sell_rate;
        eosio::asset solded;

        uint64_t primary_key() const {return pool_id;}

        EOSLIB_SERIALIZE(sale, (pool_id)(sell_rate)(solded));
    };

    typedef eosio::multi_index<"sale"_n, sale> sale_index;
 

    struct [[eosio::table]] sincome{
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
        uint128_t cyclandpool() const { return combine_ids(cycle_num, pool_num); }
        
        EOSLIB_SERIALIZE(sincome, (pool_id)(ahost)(cycle_num)(pool_num)(liquid_power)(max)(total)(paid_to_refs)(paid_to_dacs)(paid_to_cfund)(paid_to_hfund)(paid_to_sys)(hfund_in_segments)(distributed_segments))

    };
    typedef eosio::multi_index<"sincome"_n, sincome,
    indexed_by<"cyclandpool"_n, const_mem_fun<sincome, uint128_t, &sincome::cyclandpool>>
    > sincome_index;
    
    struct [[eosio::table]] currency_stats {
            asset          supply;
            asset          max_supply;
            eosio::name    issuer;

            uint64_t primary_key()const { return supply.symbol.code().raw(); }
         };

    typedef eosio::multi_index<"stat"_n, currency_stats> stats;


    struct [[eosio::table]] userscount
     {
         uint64_t id;
         uint64_t count;
         eosio::string subject = "registered";
         uint64_t primary_key() const {return 0;}

         EOSLIB_SERIALIZE(userscount, (id)(count)(subject))
     }; 
    typedef eosio::multi_index<"userscount"_n, userscount> userscount_index;


    struct [[eosio::table]] users{
        eosio::name username;
        eosio::name referer;
        uint64_t id;
        bool is_host = false;
        
        std::string meta;
        
        uint64_t primary_key() const{return username.value;}
        uint64_t byreferer() const{return referer.value;}

        EOSLIB_SERIALIZE(users, (username)(referer)(id)(is_host)(meta))
    };

    typedef eosio::multi_index<"users"_n, users,
    indexed_by<"users"_n, const_mem_fun<users, uint64_t, &users::byreferer>>
    > user_index;


    struct  [[eosio::table]] ahosts{
        eosio::name username;
        uint64_t votes;
        std::string title;
        std::string purpose;
        std::string manifest;
        bool comments_is_enabled = false;
        std::string meta;

        uint64_t primary_key() const{return username.value;}
        uint64_t by_votes() const {return votes;}
        EOSLIB_SERIALIZE(ahosts, (username)(votes)(title)(purpose)(manifest)(comments_is_enabled)(meta))
    };

    typedef eosio::multi_index<"ahosts"_n, ahosts,
    indexed_by<"ahosts"_n, const_mem_fun<ahosts, uint64_t, &ahosts::by_votes>>
    > ahosts_index;







    /* ACTIONS */

    struct [[eosio::action]] reg {
        eosio::name username;
        eosio::name referer;
        std::string meta;
        
        EOSLIB_SERIALIZE( reg, (username)(referer)(meta))
    };


    struct [[eosio::action]] fixs {
        eosio::name host;
        // uint64_t pool;
        // std::string meta;
        
        EOSLIB_SERIALIZE( fixs, (host))
    };

    
    struct [[eosio::action]] convert{
        eosio::name username;
        eosio::name host;
        uint64_t balance_id;

        EOSLIB_SERIALIZE( convert, (username)(host)(balance_id))
    };

    
    struct [[eosio::action]] del {
        eosio::name username;
        
        EOSLIB_SERIALIZE( del, (username))
    };

   struct [[eosio::action]] profupdate {
        eosio::name username;
        std::string meta;
        
        EOSLIB_SERIALIZE( profupdate, (username)(meta))
    };
 
    struct [[eosio::action]] setparams{
        eosio::name host;
        eosio::name chost;
        uint64_t size_of_pool;
        uint64_t quants_precision;
        uint64_t overlap;
        uint64_t profit_growth;
        uint64_t base_rate;
        uint64_t loss_percent;
        uint64_t pool_limit;
        uint64_t pool_timeout;
        uint64_t priority_seconds;
        EOSLIB_SERIALIZE( setparams, (host)(chost)(size_of_pool)(quants_precision)(overlap)(profit_growth)(base_rate)(loss_percent)(pool_limit)(pool_timeout)(priority_seconds))

    };

    
    struct [[eosio::action]] start{
        eosio::name host;
        eosio::name chost;
        EOSLIB_SERIALIZE( start, (host)(chost))

    };

    
    struct [[eosio::action]] withdraw{
        eosio::name username; 
        eosio::name host;
        uint64_t balance_id;

        
        EOSLIB_SERIALIZE( withdraw, (username)(host)(balance_id))


    };

    
    struct [[eosio::action]] priorenter{
        eosio::name username; 
        eosio::name host;
        uint64_t balance_id;

        
        EOSLIB_SERIALIZE( priorenter, (username)(host)(balance_id))


    };
    
    struct [[eosio::action]] refreshbal{
        eosio::name username;
        uint64_t balance_id;
        uint64_t partrefresh;
        EOSLIB_SERIALIZE( refreshbal, (username)(balance_id)(partrefresh))
    };

    
    struct [[eosio::action]] refreshst{
        eosio::name host;

        EOSLIB_SERIALIZE( refreshst, (host))
    };

    
    struct [[eosio::action]] mstartcycle{
        eosio::name host;
        EOSLIB_SERIALIZE( mstartcycle, (host))
    };


    
};






