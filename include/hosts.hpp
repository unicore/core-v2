
// #include <eosio/transaction.hpp> 
// #include <eosio.system/native.hpp>
#include "exchange_state.hpp"
#include "../src/exchange_state.cpp"
#include <eosio/crypto.hpp>

namespace eosio {
   uint128_t combine_ids(const uint64_t &x, const uint64_t &y) {
        return (uint128_t{x} << 64) | y;
   };

   struct [[eosio::table, eosio::contract("unicore")]] spiral{
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


    struct [[eosio::table, eosio::contract("unicore")]] rate {
        uint64_t pool_id;
        uint64_t buy_rate=0;
        uint64_t sell_rate=0;
        uint64_t convert_rate=0;
        eosio::asset quant_buy_rate;
        eosio::asset quant_sell_rate;
        eosio::asset quant_convert_rate;
        eosio::asset client_income;
        eosio::asset delta;
        eosio::asset pool_cost;
        eosio::asset total_in_box;
        eosio::asset payment_to_wins;
        eosio::asset payment_to_loss;
        eosio::asset system_income;
        eosio::asset live_balance_for_sale;
        eosio::asset live_balance_for_convert;
        
        
        uint64_t primary_key() const{return pool_id;}

        EOSLIB_SERIALIZE(rate, (pool_id)(buy_rate)(sell_rate)(convert_rate)(quant_buy_rate)(quant_sell_rate)(quant_convert_rate)(client_income)(delta)(pool_cost)(total_in_box)(payment_to_wins)(payment_to_loss)(system_income)(live_balance_for_sale)(live_balance_for_convert))
    };
    typedef eosio::multi_index<"rate"_n, rate> rate_index;
    
    struct [[eosio::table, eosio::contract("unicore")]] hosts{
        eosio::name username;
        eosio::time_point_sec registered_at;
        eosio::name architect;
        eosio::name hoperator;
        uint64_t type = 0;
        uint64_t consensus_percent;
        uint64_t referral_percent;
        uint64_t dacs_percent;
        uint64_t cfund_percent;
        uint64_t hfund_percent;
        uint64_t sys_percent;
        std::vector<uint64_t> levels;
        std::vector<eosio::name> gsponsor_model;
        bool direct_goal_withdraw = false;
        uint64_t dac_mode;
        std::vector<eosio::name> dacs;
        
        eosio::name ahost;
        std::vector<eosio::name> chosts;
        
        bool sale_is_enabled = false;
        uint64_t sale_mode = 0;
        eosio::name sale_token_contract;
        eosio::asset asset_on_sale;
        int64_t sale_shift = 0;
        
        //Метод добавления хоста в фонд
        //Метод включения сейла хостом
        
        bool non_active_chost = false;
        bool need_switch = false;

        uint64_t fhosts_mode;
        std::vector<eosio::name> fhosts;
        

        std::string title;
        std::string purpose;
        bool voting_only_up = false;
        uint64_t power_market_id = 0;
        uint64_t total_shares;
        eosio::asset quote_amount;
        eosio::name quote_token_contract;
        std::string quote_symbol;
        uint64_t quote_precision;
        eosio::name root_token_contract;
        eosio::asset root_token;
        std::string symbol;
        uint64_t precision;
        eosio::asset to_pay;
        bool payed = false;
        
        uint64_t cycle_start_id = 0;
        uint64_t current_pool_id = 0;
        uint64_t current_cycle_num = 1;
        uint64_t current_pool_num = 1;
        bool parameters_setted = false;
        bool activated = false;
        
        bool priority_flag = false;

        uint64_t total_goals = 0;
        uint64_t achieved_goals = 0;
        uint64_t total_tasks = 0;
        uint64_t completed_tasks = 0;
        uint64_t total_reports = 0;
        uint64_t approved_reports = 0;
        

        std::string meta;
        


        EOSLIB_SERIALIZE( hosts, (username)(registered_at)(architect)(hoperator)(type)(consensus_percent)(referral_percent)
            (dacs_percent)(cfund_percent)(hfund_percent)(sys_percent)(levels)(gsponsor_model)(direct_goal_withdraw)(dac_mode)(dacs)(ahost)(chosts)
            (sale_is_enabled)(sale_mode)(sale_token_contract)(asset_on_sale)(sale_shift)
            (non_active_chost)(need_switch)(fhosts_mode)(fhosts)
            (title)(purpose)(voting_only_up)(power_market_id)(total_shares)(quote_amount)(quote_token_contract)(quote_symbol)(quote_precision)(root_token_contract)(root_token)(symbol)(precision)
            (to_pay)(payed)(cycle_start_id)(current_pool_id)
            (current_cycle_num)(current_pool_num)(parameters_setted)(activated)(priority_flag)(total_goals)(achieved_goals)(total_tasks)(completed_tasks)(total_reports)(approved_reports)(meta))

        uint64_t primary_key()const { return username.value; }
      
        // eosoi::name gsponsor_model_is_exist(eosio::name model) const {
        //  auto it = std::find(gsponsor_model.begin(), gsponsor_model.end(), model);
            
        //  if (it == gsponsor_model.end())
        //      return false;
        //  else 
        //      return true;
        // }

        eosio::name get_ahost() const {
        	if (ahost == username)
        		return username;
        	else 
        		return ahost; 
        }

        eosio::symbol get_root_symbol() const {
        	return root_token.symbol;
        }

        // bool is_account_in_whitelist(eosio::name username) const {
        // 	auto it = std::find(whitelist.begin(), whitelist.end(), username);
        	
        // 	if (it == whitelist.end())
        // 		return false;
        // 	else 
        // 		return true;
        // }



    };

    typedef eosio::multi_index <"hosts"_n, hosts> account_index;
    
    struct [[eosio::table, eosio::contract("unicore")]] emission{
        eosio::name host;
        uint64_t percent;
        uint64_t gtop;
        eosio::asset fund;

        uint64_t primary_key() const {return host.value;}
        
        EOSLIB_SERIALIZE(emission, (host)(percent)(gtop)(fund))
    };

    typedef eosio::multi_index<"emission"_n, emission> emission_index;


    struct [[eosio::table, eosio::contract("unicore")]] funds{
        uint64_t id;
        eosio::name issuer;
        eosio::name token_contract;
        eosio::asset fund;
        std::string descriptor;
        
        uint64_t primary_key()const { return id; }
        uint64_t byissuer()const { return issuer.value; }
        uint128_t codeandsmbl() const {return eosio::combine_ids(token_contract.value, fund.symbol.code().raw());}
        
        EOSLIB_SERIALIZE(funds, (id)(issuer)(token_contract)(fund)(descriptor))
    };

    
    typedef eosio::multi_index<"funds"_n, funds,
      eosio::indexed_by<"codeandsmbl"_n, eosio::const_mem_fun<funds, uint128_t, 
                              &funds::codeandsmbl>>,

      eosio::indexed_by<"byissuer"_n, eosio::const_mem_fun<funds, uint64_t, 
                              &funds::byissuer>>
    > funds_index;

    struct [[eosio::table, eosio::contract("unicore")]] hostsonfunds{
        uint64_t id;
        uint64_t fund_id;
        eosio::name host;
        
        uint64_t primary_key()const { return id; }
        
        uint128_t fundandhost() const {return eosio::combine_ids(fund_id, host.value);}
        EOSLIB_SERIALIZE(hostsonfunds, (id)(fund_id)(host))
    };

    
    typedef eosio::multi_index<"hostsonfunds"_n, hostsonfunds,
      eosio::indexed_by<"fundandhost"_n, eosio::const_mem_fun<hostsonfunds, uint128_t, 
                              &hostsonfunds::fundandhost>>
    > hostsonfunds_index;



    struct  [[eosio::table, eosio::contract("unicore")]] partners {
        eosio::name partner;
        uint64_t level = 0;

        uint64_t primary_key() const{return partner.value;}
        EOSLIB_SERIALIZE(partners, (partner)(level))
    };

    typedef eosio::multi_index<"partners"_n, partners> partners_index;


};