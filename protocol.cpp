#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/print.hpp>
#include "protocol.hpp"
#include "shares.cpp"
#include "hosts.cpp"

#include "core.cpp"
#include "goals.cpp"

#include "voting.cpp"

using namespace eosio;


extern "C" {
   
   void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
        if (action == N(transfer)){
            auto op = eosio::unpack_action_data<eosio::currency::transfer>();
            if (op.from != _self){
                auto subcode = op.memo.substr(0,3);
                auto delimeter = op.memo[3];
                eosio_assert(delimeter == '-', "Wrong subcode format. Right format: code[3]-parameter");
                auto parameter = op.memo.substr(4, op.memo.length());
                uint64_t subintcode = atoi(subcode.c_str());

                //codes:
                //100 - deposit
                //110 - pay for host
                //200 - buyshares
                //300 - goal activate action
                
                //TODO CHECK FOR CODE!!! if (code == N(eosio.token)){

                switch (subintcode){
                    case 100: {
                        //check for code inside
                        //auto cd = eosio::string_to_name(code.c_str());
                        //Deposit in the Core
                        auto host = eosio::string_to_name(parameter.c_str());
                        core().deposit(op.from, host, op.quantity, code);
                        break;
                    }
                    case 110: {
                        //check for code outside
                        //auto cd = eosio::string_to_name(code.c_str());
                        //Pay for upgration
                        eosio_assert(code == N(eosio.token), "Only eosio.token contract can be used for upgrade");
                        auto host = eosio::string_to_name(parameter.c_str());
                        hosts_struct().pay_for_upgrade(host, op.quantity);
                        break;
                    }
                    case 200: {
                        //check for code outside
                        //auto cd = eosio::string_to_name(code.c_str());
                        //Buy Shares
                        eosio_assert(code == N(eosio.token), "Only eosio.token contract can be used for buy power");
                        auto host = eosio::string_to_name(parameter.c_str());
                        shares().buyshares_action(op.from, host, op.quantity);
                        break;
                    }
                    case 300: {
                        //check for code inside
                        //Donation for goal
                        auto delimeter2 = parameter.find('-');
                        std::string parameter2 = parameter.substr(delimeter2+1, parameter.length());
                        
                        auto host = eosio::string_to_name(parameter2.c_str());
                        uint64_t goal_id = atoi(parameter.c_str());    

                        goal().donate_action(op.from, host, goal_id, op.quantity, code);
                        break;
                    }
                    // default:
                       //eosio_assert(false, "Subcode is wrong");
                }

            }
        } else if (code == _self){
            switch (action){
                //GOALS
                 case N(setgoal): {
                    goal().set_goal_action(eosio::unpack_action_data<setgoal>());
                    break;
                 }
                 case N(editgoal): {
                    goal().edit_goal_action(eosio::unpack_action_data<editgoal>());
                    break;
                 }
                 case N(delgoal): {
                    goal().del_goal_action(eosio::unpack_action_data<delgoal>());
                    break;
                 }
                 case N(report): {
                    goal().report_action(eosio::unpack_action_data<report>());
                    break;
                 }
                //VOTING
                 case N(vote): { 
                    voting().vote_action(eosio::unpack_action_data<vote>());
                    break;
                 }
                //SHARES
                case N(sellshares): {
                    shares().sellshares_action(eosio::unpack_action_data<sellshares>());
                    break;
                };
                case N(delshares): {
                    shares().delegate_shares_action(eosio::unpack_action_data<delshares>());
                    break;
                };
                case N(undelshares): {
                    shares().undelegate_shares_action(eosio::unpack_action_data<undelshares>());
                    break;
                };
                case N(refreshsh): {
                    shares().refresh_action(eosio::unpack_action_data<refreshsh>());
                    break;
                };
                case N(withdrawsh): {
                    shares().withdraw_action(eosio::unpack_action_data<withdrawsh>());
                    break;
                };
                //HOSTS
                case N(upgrade): {
                    hosts_struct().upgrade_action(eosio::unpack_action_data<upgrade>());
                    break;
                };
                case N(cchildhost): {
                    hosts_struct().create_child_host_action(eosio::unpack_action_data<cchildhost>());
                    break;
                };

                //CORE
                case N(setparams): {
                    core().setparams_action(eosio::unpack_action_data<setparams>());
                    break;
                };
                case N(start): {
                   core().start_action(eosio::unpack_action_data<start>());
                   break;
                };
                case N(refreshbal): {
                    core().refresh_balance_action(eosio::unpack_action_data<refreshbal>());
                    break;
                };
                case N(refreshst): {
                    auto op = eosio::unpack_action_data<refreshst>();
                    core().refresh_state(op.host);
                    break;
                };
                case N(withdraw): {
                    core().withdraw_action(eosio::unpack_action_data<withdraw>());
                    break;
                };
                case N(reg): {
                    core().reg_action(eosio::unpack_action_data<reg>());
                    break;
                };
                case N(profupdate): {
                    core().profupdate_action(eosio::unpack_action_data<profupdate>());
                    break;
                };
                case N(setfee): {
                    core().setfee_action(eosio::unpack_action_data<setfee>());
                    break;
                };
                case N(priorenter): {
                    core().priority_enter(eosio::unpack_action_data<priorenter>());   
                    break;
                };
                case N(gwithdraw):{
                    goal().gwithdraw_action(eosio::unpack_action_data<gwithdraw>());
                    break;
                };
                case N(setemi):{
                    hosts_struct().create_emission_action(eosio::unpack_action_data<setemi>());
                    break;
                }
            }
            
        }
}

}; 
