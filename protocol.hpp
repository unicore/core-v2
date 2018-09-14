
#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/currency.hpp>
#include <eosiolib/multi_index.hpp>
#include <eosiolib/contract.hpp>

#define LEPTS_PRECISION 1000000
#define PERCENT_PRECISION 10000


namespace eosio {
    static const account_name _self = N(tt.tc);
    static const account_name _dacomfee = N(dacomfee.tc);
    
    static const eosio::symbol_name _SYM = S(4, FLO);
    static const uint64_t _MAX_SUPPLY = 1000000000000000;

    // @abi table account
    struct account{
        account_name username;
        std::string purpose;
        eosio::time_point_sec registered_at;
        bool activated;
        uint64_t primary_key()const { return username; }
        
        EOSLIB_SERIALIZE( account, (username)(purpose)(registered_at)(activated))
    };

    typedef eosio::multi_index <N(account), account> account_index;
    

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
        uint64_t primary_key() const {return id;}

        EOSLIB_SERIALIZE(spiral, (id)(size_of_pool)(overlap)(profit_growth)(base_rate)(loss_percent)(pool_limit)(pool_timeout))
    };

    typedef eosio::multi_index<N(spiral), spiral> spiral_index;


    // @abi table balance i64
    struct balance{
        uint64_t id;
        account_name host;
        uint64_t cycle_num;
        uint64_t pool_num;
        uint64_t global_pool_id;
        uint64_t lept_for_sale;
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
        
        
        uint64_t primary_key() const {return id;}

        EOSLIB_SERIALIZE(balance, (id)(host)(cycle_num)(pool_num)(global_pool_id)(lept_for_sale)(last_recalculated_win_pool_id)(win)(pool_color)(available)(purchase_amount)(date_of_purchase)(withdrawed)(sold_amount)(date_of_sale)(forecasts))
    };

    typedef eosio::multi_index<N(balance), balance> balance_index;

    // @abi table cycle i64
    struct cycle{
        uint64_t id;
        uint64_t start_at_global_pool_id;
        uint64_t finish_at_global_pool_id;
        
        uint64_t primary_key() const {return id;}

        EOSLIB_SERIALIZE(cycle, (id)(start_at_global_pool_id)(finish_at_global_pool_id));
    };

    typedef eosio::multi_index<N(cycle), cycle> cycle_index;
    
    // @abi table dprop i64
    struct dprop{
        uint64_t id;
        uint64_t cycle_start_at_id = 0;
        uint64_t current_pool_id = 0;
        uint64_t current_cycle_num = 1;
        uint64_t current_pool_num = 1;
        
        
        uint64_t primary_key() const {return id;}

        EOSLIB_SERIALIZE(dprop, (id)(cycle_start_at_id)(current_pool_id)(current_cycle_num)(current_pool_num))
    };

    typedef eosio::multi_index<N(dprop), dprop> dprop_index;

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
        uint64_t cycle_num;
        uint64_t pool_num;
        std::string color;
        uint64_t total_lepts; 
        uint64_t creserved_lepts; 
        uint64_t remain_lepts;
        eosio::asset lept_cost;
        eosio::asset total_win_withdraw;
        eosio::asset total_loss_withdraw;
        eosio::time_point_sec pool_expired_at;
        
        uint64_t primary_key() const {return id;}
        uint64_t by_cycle() const {return cycle_num;}
        
        EOSLIB_SERIALIZE(pool,(id)(cycle_num)(pool_num)(color)(total_lepts)(creserved_lepts)(remain_lepts)(lept_cost)(total_win_withdraw)(total_loss_withdraw)(pool_expired_at))
    };

    typedef eosio::multi_index<N(pool), pool> pool_index;
    

    // @abi table rate i64
    struct rate {
        uint64_t pool_id;
        uint64_t total_lepts;
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

        EOSLIB_SERIALIZE(rate, (pool_id)(total_lepts)(buy_rate)(sell_rate)(client_income)(delta)(pool_cost)(total_in_box)(payment_to_wins)(payment_to_loss)(system_income)(live_balance_for_sale))
    };
    typedef eosio::multi_index<N(rate), rate> rate_index;
    


    // @abi table sincome i64
    struct sincome{
        uint64_t id;
        uint64_t pool_num;
        eosio::asset available;
        bool withdrawed = false;
        uint64_t primary_key() const {return id;}
        bool is_empty()const { return !( available.amount ); }
        
        EOSLIB_SERIALIZE(sincome, (id)(pool_num)(available)(withdrawed))

    };
    typedef eosio::multi_index<N(sincome), sincome> sincome_index;
    



    /* ACTIONS */

    // @abi action
    struct tests {
        account_name owner;
        uint64_t c;
        EOSLIB_SERIALIZE( tests, (owner)(c))
    };

    // @abi action
    struct upgrade{
        account_name username;
        std::string purpose;
        EOSLIB_SERIALIZE( upgrade, (username)(purpose))
    };

    
    // @abi action
    struct setparams{
        account_name host;
        uint64_t size_of_pool;
        uint64_t overlap;
        uint64_t profit_growth;
        uint64_t base_rate;
        uint64_t loss_percent;
        uint64_t pool_limit;
        uint64_t pool_timeout;
        EOSLIB_SERIALIZE( setparams, (host)(size_of_pool)(overlap)(profit_growth)(base_rate)(loss_percent)(pool_limit)(pool_timeout))

    };

    // @abi action
    struct start{
        account_name host;
        
        EOSLIB_SERIALIZE( start, (host))

    };

    // @abi action
    struct withdraw{
        account_name username; 
        account_name host;
        uint64_t balance_id;

        
        EOSLIB_SERIALIZE( withdraw, (username)(host)(balance_id))


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
        account_name username;
        account_name host;

        EOSLIB_SERIALIZE( refreshst, (username)(host))
    };

    // @abi action
    struct syswithdraw{
        account_name host;
        uint64_t sbalanceid;
        EOSLIB_SERIALIZE( syswithdraw, (host)(sbalanceid))
    };

    // @abi action
    struct setfee{
        eosio::asset createhost;
        uint64_t systemfee;
        
        EOSLIB_SERIALIZE( setfee, (createhost)(systemfee))
    };

    
    
};






