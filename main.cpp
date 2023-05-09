#include "include/core.hpp"
#include "src/hosts.cpp"
#include "src/ref.cpp"
#include "src/core.cpp"
#include "src/conditions.cpp"
#include "src/balances.cpp"
#include "src/dacs.cpp"
#include "src/helpers.cpp"
#include "src/system.cpp"

using namespace eosio;
extern "C" {

   void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
        if (action == "transfer"_n.value){

            struct transfer{
                eosio::name from;
                eosio::name to;
                eosio::asset quantity;
                std::string memo;
            };

            auto op = eosio::unpack_action_data<transfer>();

            if (op.from != _me){
                
                auto subcode = op.memo.substr(0,3);
                auto delimeter = op.memo[3];

                if (delimeter == '-'){
                    
                    auto parameter = op.memo.substr(4, op.memo.length());
                    uint64_t subintcode = atoll(subcode.c_str());
                    
                    switch (subintcode){

                        case DEPOSIT: {
                            require_auth(op.from);

                            eosio::name host; 
                            std::string message = "";
                            auto delimeter2 = parameter.find('-');
                    
                            if (delimeter2 != -1){
                                auto host_string = op.memo.substr(4, delimeter2);
                                host = name(host_string.c_str());
                                message = parameter.substr(delimeter2+1, parameter.length());
                        
                            } else {
                                auto host_string = op.memo.substr(4, parameter.length());
                                host = name(host_string.c_str());
                            }
                            
                            unicore2::deposit(op.from, host, op.quantity, name(code), message);
                            break;
                        }

                        case DEPOSIT_FOR_SOMEONE: {
                            require_auth(op.from);
                            eosio::name host; 
                            std::string message = "";
                            auto delimeter2 = parameter.find('-');
                            auto host_string = op.memo.substr(4, delimeter2);
                            host = name(host_string.c_str());
                            auto username_string = parameter.substr(delimeter2+1, parameter.length());
                            auto username = name(username_string.c_str());
                            unicore2::deposit(username, host, op.quantity, name(code), message);
                            break;
                        }

                        case PAY_FOR_HOST: {
                            require_auth(op.from);
                            auto host = name(parameter.c_str());
                            unicore2::pay_for_upgrade(host, op.quantity, name(code));
                            break;
                        }
                        
                       
                        case BURN_QUANTS: {
                            require_auth(op.from);
                            auto delimeter2 = parameter.find('-');
                            auto host_string = op.memo.substr(4, delimeter2);
                            eosio::name host = name(host_string.c_str());
                            auto status_string = parameter.substr(delimeter2+1, parameter.length());
                            eosio::name status = name(status_string.c_str());
                            unicore2::burn_action(op.from, host, op.quantity, name(code), status);
                            break;
                        };
                       
                        case SPREAD_TO_REFS: {
                            require_auth(op.from);
                            auto delimeter2 = parameter.find('-');
                            auto host_string = op.memo.substr(4, delimeter2);
                            auto host = name(host_string.c_str());
                            auto username_string = parameter.substr(delimeter2+1, parameter.length());
                            auto username = name(username_string.c_str());
                            unicore2::spread_to_refs(host, username, op.quantity, op.quantity, name(code));
                            break;
                        }
                        case SPREAD_TO_FUNDS: {
                            require_auth(op.from);
                            auto delimeter2 = parameter.find('-');
                            eosio::name host;
                            std::string message = "";
                            
                            if (delimeter2 != -1){
                                auto host_string = op.memo.substr(4, delimeter2);
                                host = name(host_string.c_str());
                                message = parameter.substr(delimeter2+1, parameter.length());
                            } else {
                                auto host_string = op.memo.substr(4, parameter.length());
                                host = name(host_string.c_str());
                            }

                            unicore2::spread_to_funds(name(code), host, op.quantity, message);
                            break;
                        }
                        case SPREAD_TO_DACS: {
                            require_auth(op.from);
                            auto delimeter2 = parameter.find('-');
                            auto host_string = op.memo.substr(4, delimeter2);
                            auto host = name(host_string.c_str());
                            unicore2::spread_to_dacs(host, op.quantity, name(code));
                            break;
                        }
                        
                        case ADD_INTERNAL_BALANCE: {
                            require_auth(op.from);
                            unicore2::add_balance(op.from, op.quantity, eosio::name(code));  
                            break;
                        }
                    }
                }
                
            }
        } else if (code == _me.value){

            switch (action) {

                EOSIO_DISPATCH_HELPER(unicore2, (init)(changemode)(upgrade)(setlevels)
                    (cchildhost)(settiming)
                    (setflows)(setparams)(start)(setstartdate)(refreshbal)(refreshst)
                    (withdraw)(priorenter)(edithost)(setcondition)(setarch)(adddac)
                    (rmdac)(withdrdacinc)
                )


            }
            
        }
}

}; 
