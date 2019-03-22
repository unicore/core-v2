
#include <eosiolib/transaction.hpp> 
#include <eosio.system/native.hpp>
#include <exchange_state.hpp>
#include <exchange_state.cpp>
#include <eosiolib/crypto.h>

namespace eosio {
	
    // @abi table hosts
    struct hosts{
        account_name username;
        eosio::time_point_sec registered_at;
        account_name architect;
        account_name hoperator;
        uint64_t consensus_percent;
        uint64_t referral_percent;
        std::vector<uint64_t> levels;
        
        uint64_t dac_mode;
        std::vector<account_name> dacs;
        
        account_name ahost;
        std::vector<account_name> chosts;
        
        bool non_active_chost = false;
        bool need_switch = false;

        uint64_t fhosts_mode;
        std::vector<account_name> fhosts;
        

        std::string title;
        std::string purpose;
        
        uint64_t total_shares;
        eosio::asset quote_amount;
        account_name root_token_contract;
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


        EOSLIB_SERIALIZE( hosts, (username)(registered_at)(architect)(hoperator)(consensus_percent)(referral_percent)
            (levels)(dac_mode)(dacs)(ahost)(chosts)(non_active_chost)(need_switch)(fhosts_mode)(fhosts)
            (title)(purpose)(total_shares)(quote_amount)(root_token_contract)(root_token)(symbol)(precision)
            (to_pay)(payed)(cycle_start_id)(current_pool_id)
            (current_cycle_num)(current_pool_num)(parameters_setted)(activated)(priority_flag)(meta))

        account_name primary_key()const { return username; }
      
        account_name get_ahost() const {
        	if (ahost == username)
        		return username;
        	else 
        		return ahost; 
        }

        eosio::symbol_name get_root_symbol() const {
        	return root_token.symbol;
        }

        // bool is_account_in_whitelist(account_name username) const {
        // 	auto it = std::find(whitelist.begin(), whitelist.end(), username);
        	
        // 	if (it == whitelist.end())
        // 		return false;
        // 	else 
        // 		return true;
        // }



    };

    typedef eosio::multi_index <N(hosts), hosts> account_index;
    
    //@abi table emission
    struct emission{
        account_name host;
        uint64_t percent;
        uint64_t gtop;
        uint64_t model = 1;
        bool auto_fill = true;
        eosio::asset fund;

        account_name primary_key() const {return host;}
        
        EOSLIB_SERIALIZE(emission, (host)(percent)(gtop)(model)(auto_fill)(fund))
    };

    typedef eosio::multi_index<N(emission), emission> emission_index;

    // @abi action
    struct cremi{
        account_name host;
        uint64_t percent;
    };

    // @abi action
    struct setemi{
        account_name host;
        uint64_t percent;
        uint64_t gtop;
    };
    
    // @abi action
    struct upgrade{
        account_name username;
        std::string title;
        std::string purpose;
        uint64_t total_shares;
        eosio::asset quote_amount;
        eosio::asset root_token;
        account_name root_token_contract;
        uint64_t consensus_percent; 
        uint64_t referral_percent;
        std::vector<uint64_t> levels;
        std::string meta;

        EOSLIB_SERIALIZE( upgrade, (username)(title)(purpose)(total_shares)(quote_amount)(root_token)(root_token_contract)(consensus_percent)(referral_percent)(levels)(meta))
    };

    // @abi action
    struct cchildhost{
    	account_name parent_host;
    	account_name chost;

    	EOSLIB_SERIALIZE(cchildhost, (parent_host)(chost))
    };


}