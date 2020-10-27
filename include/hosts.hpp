
// #include <eosio/transaction.hpp> 
// #include <eosio.system/native.hpp>
#include "exchange_state.hpp"
#include "../src/exchange_state.cpp"
#include <eosio/crypto.hpp>

namespace eosio {
   
    struct [[eosio::table, eosio::contract("unicore")]] hosts{
        eosio::name username;
        eosio::time_point_sec registered_at;
        eosio::name architect;
        eosio::name hoperator;
        eosio::name type;
        eosio::name chat_mode;
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
        uint64_t total_dacs_weight;
        
        eosio::name ahost;
        std::vector<eosio::name> chosts;
        
        bool sale_is_enabled = false;
        uint64_t sale_mode = 0;
        eosio::name sale_token_contract;
        eosio::asset asset_on_sale;
        uint64_t asset_on_sale_precision;
        std::string asset_on_sale_symbol;
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
        


        EOSLIB_SERIALIZE( hosts, (username)(registered_at)(architect)(hoperator)(type)(chat_mode)(consensus_percent)(referral_percent)
            (dacs_percent)(cfund_percent)(hfund_percent)(sys_percent)(levels)(gsponsor_model)(direct_goal_withdraw)(dac_mode)(total_dacs_weight)(ahost)(chosts)
            (sale_is_enabled)(sale_mode)(sale_token_contract)(asset_on_sale)(asset_on_sale_precision)(asset_on_sale_symbol)(sale_shift)
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
    

    // struct [[eosio::table, eosio::contract("unicore")]] dacs {
    //     eosio::name dac;
    //     uint64_t weight;
    //     eosio::asset income;
    //     uint128_t income_in_segments;
    //     eosio::asset withdrawed;
    //     eosio::asset income_limit;
    //     uint64_t primary_key() const {return dac.value;}  

    //     EOSLIB_SERIALIZE(dacs, (dac)(weight)(income)(income_in_segments)(withdrawed)(income_limit))      
    // };

    // typedef eosio::multi_index <"dacs"_n, dacs> dacs_index;


    // struct [[eosio::table, eosio::contract("unicore")]] emission {
    //     eosio::name host;
    //     uint64_t percent;
    //     uint64_t gtop;
    //     eosio::asset fund;

    //     uint64_t primary_key() const {return host.value;}
        
    //     EOSLIB_SERIALIZE(emission, (host)(percent)(gtop)(fund))
    // };

    // typedef eosio::multi_index<"emission"_n, emission> emission_index;


    struct [[eosio::table, eosio::contract("unicore")]] funds {
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



};