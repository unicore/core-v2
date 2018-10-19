
#include <eosiolib/transaction.hpp> 
#include <eosio.system/native.hpp>
#include <exchange_state.hpp>
#include <exchange_state.cpp>
#include <eosiolib/crypto.h>

namespace eosio {
	
    // @abi table account
    struct account{
        account_name username;
        account_name hoperator;
        std::vector<account_name> childrens;
        account_name active_host;
        bool need_switch = false;
        std::string title;
        std::string purpose;
        uint64_t total_shares;
        eosio::asset quote_amount;
        eosio::asset root_token;
        eosio::time_point_sec registered_at;
        bool activated = false;
        bool payed = false;
        eosio::asset to_pay;
        std::string meta;
        
        account_name primary_key()const { return username; }
        
        // account_name find_children_host( account_name children) const {
        // 	auto last_children_host = childrens.rbegin();
        // 	auto child =  std::find(childrens.rbegin(), childrens.rend(), children);
        	
        // 	eosio_assert(child != childrens.end(), "Child host not founded");

        // 	return child;
        // }

        eosio::symbol_name get_root_symbol() const {
        	return root_token.symbol;
        }


        EOSLIB_SERIALIZE( account, (username)(hoperator)(childrens)(active_host)(need_switch)(title)(purpose)(total_shares)(quote_amount)(root_token)(registered_at)(activated)(payed)(to_pay)(meta))
    };

    typedef eosio::multi_index <N(account), account> account_index;
    

    // @abi action
    struct upgrade{
        account_name username;
        account_name child_host;
        account_name hoperator;
        std::string title;
        std::string purpose;
        uint64_t total_shares;
        eosio::asset quote_amount;
        eosio::asset root_token;
        std::string meta;
        EOSLIB_SERIALIZE( upgrade, (username)(child_host)(hoperator)(title)(purpose)(total_shares)(quote_amount)(root_token)(meta))
    };

    // @abi action
    struct cchildhost{
    	account_name parent_host;
    	account_name child_host;

    	EOSLIB_SERIALIZE(cchildhost, (parent_host)(child_host))
    };

    struct wait_weight {
	  uint32_t wait_sec;
	  weight_type weight;
	};

	struct authority {
	  uint32_t threshold;
	  vector<eosiosystem::key_weight> keys;
	  vector<eosiosystem::permission_level_weight> accounts;
	  vector<wait_weight> waits;
	};

	// eosiosystem::native::newaccount Doesn't seem to want to take authorities.
	struct call {
	  struct eosio {
	    void newaccount(account_name creator, account_name name,
	                    authority owner, authority active);
	  };
	};

	asset buyrambytes(uint32_t bytes) {
	  rammarket market(N(eosio), N(eosio));
	  auto itr = market.find(S(4,RAMCORE));
	  eosio_assert(itr != market.end(), "RAMCORE market not found");
	  
	  auto tmp = *itr;

	  return tmp.convert(asset(bytes, S(0, RAM)), CORE_SYMBOL);
}

}