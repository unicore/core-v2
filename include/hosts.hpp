
// #include <eosio/transaction.hpp> 
// #include <eosio.system/native.hpp>
#include <exchange_state.hpp>
#include <../src/exchange_state.cpp>
#include <eosio/crypto.hpp>

namespace eosio {
    uint128_t combine_ids(const uint64_t &x, const uint64_t &y) {
        return (uint128_t{x} << 64) | y;
    }

    struct [[eosio::table]] hosts{
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
        bool can_architect_fund_goals = true;
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
        std::string meta;


        EOSLIB_SERIALIZE( hosts, (username)(registered_at)(architect)(hoperator)(type)(consensus_percent)(referral_percent)
            (dacs_percent)(cfund_percent)(hfund_percent)(sys_percent)(levels)(dac_mode)(dacs)(ahost)(chosts)
            (sale_is_enabled)(sale_mode)(sale_token_contract)(asset_on_sale)(sale_shift)
            (non_active_chost)(need_switch)(fhosts_mode)(fhosts)
            (title)(purpose)(voting_only_up)(can_architect_fund_goals)(power_market_id)(total_shares)(quote_amount)(quote_token_contract)(quote_symbol)(quote_precision)(root_token_contract)(root_token)(symbol)(precision)
            (to_pay)(payed)(cycle_start_id)(current_pool_id)
            (current_cycle_num)(current_pool_num)(parameters_setted)(activated)(priority_flag)(meta))

        uint64_t primary_key()const { return username.value; }
      
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
    
    struct [[eosio::table]] emission{
        eosio::name host;
        uint64_t percent;
        uint64_t gtop;
        eosio::asset fund;

        uint64_t primary_key() const {return host.value;}
        
        EOSLIB_SERIALIZE(emission, (host)(percent)(gtop)(fund))
    };

    typedef eosio::multi_index<"emission"_n, emission> emission_index;


    struct [[eosio::table]] funds{
        uint64_t id;
        eosio::name issuer;
        eosio::name token_contract;
        eosio::asset fund;
        std::string descriptor;
        
        uint64_t primary_key()const { return id; }
        uint64_t byissuer()const { return issuer.value; }
        uint128_t codeandsmbl() const {return combine_ids(token_contract.value, fund.symbol.code().raw());}
        
        EOSLIB_SERIALIZE(funds, (id)(issuer)(token_contract)(fund)(descriptor))
    };

    
    typedef eosio::multi_index<"funds"_n, funds,
      indexed_by<"codeandsmbl"_n, const_mem_fun<funds, uint128_t, 
                              &funds::codeandsmbl>>,

      indexed_by<"byissuer"_n, const_mem_fun<funds, uint64_t, 
                              &funds::byissuer>>
    > funds_index;

    struct [[eosio::table]] hostsonfunds{
        uint64_t id;
        uint64_t fund_id;
        eosio::name host;
        
        uint64_t primary_key()const { return id; }
        
        uint128_t fundandhost() const {return combine_ids(fund_id, host.value);}
        EOSLIB_SERIALIZE(hostsonfunds, (id)(fund_id)(host))
    };

    
    typedef eosio::multi_index<"hostsonfunds"_n, hostsonfunds,
      indexed_by<"fundandhost"_n, const_mem_fun<hostsonfunds, uint128_t, 
                              &hostsonfunds::fundandhost>>
    > hostsonfunds_index;


    struct [[eosio::action]] enablesale{
        eosio::name host;
        eosio::name token_contract;
        eosio::asset asset_on_sale;
        int64_t sale_shift;
        EOSLIB_SERIALIZE( enablesale, (host)(token_contract)(asset_on_sale)(sale_shift))
    };

    struct [[eosio::action]] addhostofund{
        uint64_t fund_id;
        eosio::name host;                
        
        EOSLIB_SERIALIZE( addhostofund, (fund_id)(host))
    };


    struct [[eosio::action]] createfund{
        eosio::name token_contract;
        eosio::asset fund_asset;
        std::string descriptor;

        EOSLIB_SERIALIZE( createfund, (token_contract)(fund_asset)(descriptor))
    };


    struct [[eosio::action]] setemi{
        eosio::name host;
        uint64_t percent;
        uint64_t gtop;
        EOSLIB_SERIALIZE( setemi, (host)(percent)(gtop))
   
    };
    
    struct [[eosio::action]] setarch{
        eosio::name host;
        eosio::name architect;
        EOSLIB_SERIALIZE( setarch, (host)(architect))
    };


    struct [[eosio::action]] upgrade{
        eosio::name username;
        std::string title;
        std::string purpose;
        uint64_t total_shares;
        eosio::asset quote_amount;
        eosio::name quote_token_contract;
        eosio::asset root_token;
        eosio::name root_token_contract;
        bool voting_only_up = true;
        uint64_t consensus_percent; 
        uint64_t referral_percent;
        uint64_t dacs_percent;
        uint64_t cfund_percent;
        uint64_t hfund_percent;
        std::vector<uint64_t> levels;
        uint64_t emission_percent;
        uint64_t gtop;
        
        std::string meta;

        EOSLIB_SERIALIZE( upgrade, (username)(title)(purpose)(total_shares)(quote_amount)(quote_token_contract)(root_token)(root_token_contract)(voting_only_up)(consensus_percent)(referral_percent)(dacs_percent)(cfund_percent)(hfund_percent)(levels)(emission_percent)(gtop)(meta))
    };

    struct [[eosio::action]] cchildhost{
    	eosio::name parent_host;
    	eosio::name chost;

    	EOSLIB_SERIALIZE(cchildhost, (parent_host)(chost))
    };

    
    struct [[eosio::action]] edithost
    {
        eosio::name architect;
        eosio::name host;
        eosio::string title;
        eosio::string purpose;
        eosio::string manifest;
        eosio::string meta;

        EOSLIB_SERIALIZE(edithost, (architect)(host)(title)(purpose)(manifest)(meta))
    };

    
    struct [[eosio::action]] ehosttime
    {
        eosio::name architect;
        eosio::name host;
        uint64_t pool_timeout;
        uint64_t priority_seconds;

    };

    struct [[eosio::action]] deactivate{
        eosio::name architect;
        eosio::name host;

        EOSLIB_SERIALIZE(deactivate, (architect)(host))
    };


}