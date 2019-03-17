
#include <eosiolib/transaction.hpp> 
#include <eosio.system/native.hpp>
#include <exchange_state.hpp>
#include <exchange_state.cpp>
#include <eosiolib/crypto.h>

namespace eosio {
	
    // @abi table hosts
    struct hosts{
        //TODO add active_chosts или предусмотреть возможность перераспредлеения финансового потока иначе;
        //TODO + current_color?
        //TODO ADD extension
        //TODO оформить порядок и названия

        account_name username;
        account_name hoperator;
        std::vector<account_name> childrens;
        account_name active_host;
        
        bool non_active_child = false;
        bool parameters_setted = false;
        bool need_switch = false;
        bool is_whitelisted = false;
        
        vector <account_name> whitelist;
        
        uint64_t consensus_percent;
        uint64_t goal_validation_percent;
        
        std::string title;
        std::string purpose;
        
        uint64_t total_shares;
        eosio::asset quote_amount;
        eosio::asset root_token;
        std::string symbol;
        uint64_t precision;
        account_name root_token_contract;
        eosio::time_point_sec registered_at;
        uint64_t referral_percent;
        bool activated = false;
        bool payed = false;
        eosio::asset to_pay;
        std::vector<uint64_t> levels;
        std::string meta;
        uint64_t cycle_start_at_id = 0;
        uint64_t current_pool_id = 0;
        uint64_t current_cycle_num = 1;
        uint64_t current_pool_num = 1;
        bool priority_flag = false;

        account_name primary_key()const { return username; }
      
        account_name get_active_host() const {
        	if (active_host == username)
        		return username;
        	else 
        		return active_host; 
        }

        eosio::symbol_name get_root_symbol() const {
        	return root_token.symbol;
        }

        bool is_account_in_whitelist(account_name username) const {
        	auto it = std::find(whitelist.begin(), whitelist.end(), username);
        	
        	if (it == whitelist.end())
        		return false;
        	else 
        		return true;
        }


        EOSLIB_SERIALIZE( hosts, (username)(hoperator)(childrens)(active_host)(non_active_child)
        	(parameters_setted)(need_switch)(is_whitelisted)(whitelist)(consensus_percent)
        	(goal_validation_percent)(title)(purpose)(total_shares)(quote_amount)(root_token)(symbol)(precision)(root_token_contract)
        	(registered_at)(referral_percent)(activated)(payed)(to_pay)(levels)(meta)(cycle_start_at_id)(current_pool_id)
        	(current_cycle_num)(current_pool_num)(priority_flag))
    };

    typedef eosio::multi_index <N(hosts), hosts> account_index;
    
    //@abi table emission
    struct emission{
        account_name host;
        account_name from;
        uint64_t percent;
        eosio::asset emitted;
        eosio::asset remain;
        account_name primary_key() const {return host;}
        
        EOSLIB_SERIALIZE(emission, (host)(from)(percent)(emitted)(remain))
    };

    typedef eosio::multi_index<N(emission), emission> emission_index;

    // @abi action
    struct setemi{
        account_name host;
        account_name from;
        uint64_t percent;
    };

    // @abi action
    struct upgrade{
        account_name username;
        account_name hoperator;
        std::string title;
        std::string purpose;
        uint64_t total_shares;
        eosio::asset quote_amount;
        eosio::asset root_token;
        account_name root_token_contract;
        bool is_whitelisted = false;
       	uint64_t goal_validation_percent; 
        uint64_t referral_percent;
        std::vector<uint64_t> levels;
        std::string meta;

        EOSLIB_SERIALIZE( upgrade, (username)(hoperator)(title)(purpose)(total_shares)(quote_amount)(root_token)(root_token_contract)(is_whitelisted)(goal_validation_percent)(referral_percent)(levels)(meta))
    };

    // @abi action
    struct cchildhost{
    	account_name parent_host;
    	account_name child_host;

    	EOSLIB_SERIALIZE(cchildhost, (parent_host)(child_host))
    };


}