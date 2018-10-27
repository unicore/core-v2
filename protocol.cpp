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
    if (code == N(eosio.token)){
        if (action == N(transfer)){
            auto op = eosio::unpack_action_data<eosio::currency::transfer>();
            if (op.from != _self){
                auto code = op.memo.substr(0,3);
                auto delimeter = op.memo[3];
                eosio_assert(delimeter == '-', "Wrong subcode format. Right format: code[3]-parameter");
                auto parameter = op.memo.substr(4, op.memo.length());
                uint64_t intcode = atoi(code.c_str());

                //codes:
                //100 - deposit
                //110 - pay for host
                //200 - buyshares
                //300 - goal activate action

                switch (intcode){
                    case 100: {
                        auto host = eosio::string_to_name(parameter.c_str());
                        core().deposit(op.from, host, op.quantity);
                        break;
                    }
                    case 110: {
                        auto host = eosio::string_to_name(parameter.c_str());
                        hosts().pay_for_upgrade(host, op.quantity);
                        break;
                    }
                    case 200: {
                        auto host = eosio::string_to_name(parameter.c_str());
                        shares().buyshares_action(op.from, host, op.quantity);
                        break;
                    }
                    case 300: {
                        auto delimeter2 = parameter.find('-');
                        std::string parameter2 = parameter.substr(delimeter2+1, parameter.length());
                        
                        print("del2: ", delimeter2);
                        print("param2: ", parameter2);
                        auto host = eosio::string_to_name(parameter2.c_str());
                        uint64_t goal_id = atoi(parameter.c_str());                    
                        goal().donate_action(op.from, host, goal_id, op.quantity);
                        break;
                    }
                    //default:
                       // eosio_assert(false, "Subcode is wrong");
                }

            }
        } 
   }  else if (code == _self){
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
                 case N(next): { 
                    core().next_goals(N(alice.tc));
                    break;
                 }
                //VOTING
                 case N(vote): { 
                    voting().vote_action(eosio::unpack_action_data<vote>());
                    break;
                 }
                 // //ASSETS
                 // case N(makeasset): { 
                 //    assets().make_asset_action(eosio::unpack_action_data<makeasset>());
                 //    break;
                 // };
                //MARKETS
                
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
                    hosts().upgrade_action(eosio::unpack_action_data<upgrade>());
                    break;
                };
                case N(cchildhost): {
                    hosts().create_child_host_action(eosio::unpack_action_data<cchildhost>());
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
                case N(syswithdraw): {
                    core().syswithdraw_action(eosio::unpack_action_data<syswithdraw>());
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
                case N(gpriorenter): {
                    core().priority_goal_enter(eosio::unpack_action_data<gpriorenter>());
                    break;
                };
                case N(gwithdraw):{
                    goal().gwithdraw_action(eosio::unpack_action_data<gwithdraw>());
                    break;
                }
            }
            
        }
}

}; 
